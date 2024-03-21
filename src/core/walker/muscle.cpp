////////////////////////////////////////////////////////////////////////
// muscle.cpp
//---------------------------------------------------------------------
#include "precompile.h"

#if defined (__MECHANICS_2D__)

#include "hhnoutput.h"
#include "walker.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

static int get_jointid( int ulink, int llink )
{
	if( ulink == TRUNK_ && llink == PELVIS_ )
		return A_L;
	else if( ulink == PELVIS_ && llink == THIGH_L )
		return A_HL;
	else if( ulink == PELVIS_ && llink == THIGH_R )
		return A_HR;
	else if( ulink == THIGH_L && llink == SHANK_L )
		return A_KL;
	else if( ulink == THIGH_R && llink == SHANK_R )
		return A_KR;
	else if( ulink == SHANK_L && llink == FOOT_L )
		return A_AL;
	else if( ulink == SHANK_R && llink == FOOT_R )
		return A_AR;
	return 0;
}

static int get_typelink( int link )
{
	switch( link ){
		case TRUNK_:
			return TRUNK;
		case PELVIS_:
			return PELVIS;
		case THIGH_L:
		case THIGH_R:
			return THIGH;
		case SHANK_L:
		case SHANK_R:
			return SHANK;
		case FOOT_L:
		case FOOT_R:
			return FOOT;
		default:
			return -1;
	}
}

/////////////////////////////////////////////////////////////////////////////
// class zelastic
//---- constructors/destructor
zelastic::zelastic( double kl, double ke ) :
	KL( kl ), KE( ke )
{	
}

zelastic::zelastic( const zelastic &ee ) :
	KL( ee.KL ), KE( ee.KE )
{
}

zelastic::~zelastic( void )
{
}

//---- operators
zelastic &zelastic::operator = ( const zelastic &ee )
{
	KL = ee.KL; KE = ee.KE;
	return *this;
}

void zelastic::preproc( void )
{
	KLbyke = KL/KE;
}

/////////////////////////////////////////////////////////////////////////////
// class zcontrac
//---- constructors/destructor
zcontrac::zcontrac( void ) :
	AV( 3.7 ), Beta( 1.55 ), Fmax( 1.8 ),
	Omega( 0.45 ), RHO( 3.0 ), Vmax( 0.578 )
{
}

zcontrac::zcontrac( const zcontrac &ce ) :
	AV( ce.AV ), Beta( ce.Beta ), Fmax( ce.Fmax ),
	Omega( ce.Omega ), RHO( ce.RHO ), Vmax( ce.Vmax )
{
}

zcontrac::~zcontrac( void )
{
}

//---- operators
zcontrac &zcontrac::operator = ( const zcontrac &ce )
{
	AV = ce.AV;
	Beta = ce.Beta;
	Fmax = ce.Fmax;
	Omega = ce.Omega;
	RHO = ce.RHO;
	Vmax = ce.Vmax;
	return *this;
}

//---- methods
void zcontrac::preproc( void )
{
	B0 = Vmax*( Fmax-1 )/( AV+1 );
	B1 = ( Fmax*( Vmax+B0 )-B0 )/Vmax;
}

/////////////////////////////////////////////////////////////////////////////
// zmuscle class
double zmuscle::_MN = 0;
//---- constructors/destructor
zmuscle::zmuscle() :
	TE( 2.6, 50. ), PE( .2, 4.5 ), Ladd( 0.0 ), Ltr( 0.0 ),
	MnName( "" ), Amax( 1.0 ), MN( &_MN )
{
}

zmuscle::zmuscle( const zmuscle &muscle ) :
	TE( muscle.TE ), PE( muscle.PE ), CE( muscle.CE ), M( muscle.M ), Ladd( muscle.Ladd ), 
	A1( muscle.A1 ), A2( muscle.A2 ), R1( muscle.R1 ), Shift( muscle.Shift ),
	LT0( muscle.LT0 ), PT0( muscle.PT0 ), Alpha0( muscle.Alpha0 ), LT0perL0( muscle.LT0perL0 ),
	Ltr( muscle.Ltr ), F1( muscle.F1 ), Phi1( muscle.Phi1 ), Phi2( muscle.Phi2 ),
	MnName( muscle.MnName ), Amax( muscle.Amax ), MN( &_MN )
{
}

//---- operators
zmuscle &zmuscle::operator = ( const zmuscle &muscle )
{
	PE = muscle.PE;
	TE = muscle.TE;
	CE = muscle.CE;

	M = muscle.M;
	A1 = muscle.A1;
	A2 = muscle.A2;
	F1 = muscle.F1;
	R1 = muscle.R1;
	Ladd = muscle.Ladd;

	Phi1 = muscle.Phi1;
	Phi2 = muscle.Phi2;
	Shift = muscle.Shift;

	LT0 = muscle.LT0;
	PT0 = muscle.PT0;
	Alpha0 = muscle.Alpha0;
	LT0perL0 = muscle.LT0perL0;
	Ltr = muscle.Ltr;

	MnName = muscle.MnName;
	Amax = muscle.Amax;
	MN = &_MN;
	return *this;
}

//---- methods
void zmuscle::preproc( double *llink )
{
	TE.preproc();
	PE.preproc();
	CE.preproc();

	A1A1 = A1*A1;
	A2A2 = A2*A2;
	A1A2 = A1*A2;
	R1R1 = R1*R1;
	( A1 < R1 )? X1 = Beta1 = 0.0: X1 = sqrt( A1A1-R1R1 ), Beta1 = atan2( X1, R1 );

	L0 = LT0/LT0perL0;
	P0 = PT0/cos( Alpha0 );
	W = L0*sin( Alpha0 );
	W2 = W*W;
}

bool zmuscle::init( void )
{
#ifdef __NEW_MUSCLE__
	VMp = Wnet = 0.0;
#endif // __NEW_MUSCLE__
	LM = F = VM = 0.0;
	VT = V/2.0;
	( LT0 > L )? LT0 = L: 0;

	double x0 = LT0;
	double x1 = 1.01*x0;
	if( get_at( x1, VT ) > 0.0 ){
		for( x1 += 0.01*x0; get_at( x1, VT ) > 0.0 && x1 < L; x1 += 0.01*x0 );
	}
	for( int i = 0; i < 50; ++i ){
		LT = ( x0+x1 )/2.0;
		if( get_at( LT, VT ) > 0.0 ){
			x0 = LT;
		}
		else{
			x1 = LT;
		}
	}
	return true;
}

const void *zmuscle::select( unit_code *view )const
{
	switch( view->Param ){
		case _id_MuscleForce:
			return(( double *)&F );
		case _id_TotalLength:
			return(( double *)&L );
		case _id_TotalVelocity:
			return(( double *)&V );
		case _id_TendonLength:
			return(( double *)&LT );
		case _id_MuscleLength:
			return(( double *)&LM );
		case _id_MuscleVelocity:
			return(( double *)&VM );
		case _id_VW_None:
		default:;
	}
	return NULL;
}

#define __IS_BLOCK( t, h, r, gamma )	\
	( fabs( gamma ) > PI || (( t ) > 0. && ( t ) < 1. && fabs( h ) < ( r )) )
///////////////////////////////////////////////////////////////////////////////
//// j1muscle class
//---- constructors/destructor
j1muscle::j1muscle( const j1scheme &link ) :
	zmuscle(), Link( link )
{
	Link.Joint = get_jointid( Link.Upper, Link.Lower );
}

j1muscle::j1muscle( const j1muscle &muscle ) :
	zmuscle( muscle ), Link( muscle.Link )
{
}

//---- operators
j1muscle &j1muscle ::operator = ( const j1muscle &muscle )
{
	zmuscle::operator = ( muscle );
	Link = muscle.Link;
	return *this;
}

//---- methods
const char *j1muscle::get_name( void ) const
{
	if( Link.Upper == PELVIS_ && Link.Lower == THIGH_L ){		// Left hip muscles
		if( F1 == 1 ){				// IP
			return "IP (L)";
		}
		else if( F1 == -1 ){			// BFA
			return "BFA (L)";
		}
	}
	else if( Link.Upper == PELVIS_ && Link.Lower == THIGH_R ){	// Right hip muscles
		if( F1 == 1 ){				// IP
			return "IP (R)";
		}
		else if( F1 == -1 ){			// BFA
			return "BFA (R)";
		}
	}
	else if( Link.Upper == SHANK_L && Link.Lower == FOOT_L ){	// Left ankle muscles
		if( F1 == 1 ){				// TA
			return "TA (L)";
		}
		else if( F1 == -1 ){			// SOL
			return "SOL (L)";
		}
	}
	else if( Link.Upper == SHANK_R && Link.Lower == FOOT_R ){	// Right ankle muscles
		if( F1 == 1 ){				// TA
			return "TA (R)";
		}
		else if( F1 == -1 ){			// SOL
			return "SOL (R)";
		}
	}
	else if( Link.Upper == THIGH_L && Link.Lower == SHANK_L ){	// Left knee muscle (VA only)
		if( F1 == 1 ){				// VA
			return "VA (L)";
		}
	}
	else if( Link.Upper == THIGH_R && Link.Lower == SHANK_R ){	// Right knee muscle (VA only)
		if( F1 == 1 ){				// VA
			return "VA (R)";
		}
	}
	return NULL;
}

void j1muscle::preproc( double *llink )
{
	zmuscle::preproc( llink );
	( A2 <= R1 )? X2 = Beta2 = 0.0: X2 = sqrt( A2A2-R1R1 ), Beta2 = atan2( X2, R1 );
}

void j1muscle::proc( wjoint *joints )
{
	double alpha = joints[Link.Joint].A;
	double dalpha = joints[Link.Joint].dA;

	double gamma1 = ( F1 == -1 )? alpha+Phi1-Phi2-2*PI: alpha+Phi1-Phi2;
	double a1a2cos = A1A2*cos( gamma1 );
	double ll = A1A1+A2A2-2*a1a2cos;
	double t = ( A1A1-a1a2cos )/ll;

	L = sqrt( ll );
	H1 = A1A2*sin( gamma1 )/L;
	if( __IS_BLOCK( t, H1, R1, gamma1 )){
		H1 = R1*sign( gamma1 );
		L = X1+X2+R1*( fabs( gamma1 )-Beta1-Beta2 );
	}
	L += Ladd;
	V = H1*dalpha;
}

///////////////////////////////////////////////////////////////////////////////
//// j2muscle class
//---- constructors/destructor
j2muscle::j2muscle( const j2scheme &link ) :
	zmuscle(), Link( link )
{
/*
	Link.Joint = get_jointid( Link.Upper, Link.Lower );
	Link.LowerJoint = get_jointid( Link.Middle, Link.Lower );
*/
	Link.Joint = get_jointid( Link.Upper, Link.Middle );
	Link.LowerJoint = get_jointid( Link.Middle, Link.Lower );
}

j2muscle::j2muscle( const j2muscle &muscle ) :
	zmuscle( muscle ), Link( muscle.Link ),
	F2( muscle.F2 ), R2( muscle.R2 ), Lm( muscle.Lm )
{
}

//---- operators
j2muscle & j2muscle ::operator = ( const j2muscle & muscle )
{
	zmuscle::operator = ( muscle );
	Link = muscle.Link;

	F2 = muscle.F2;
	R2 = muscle.R2;
	Lm = muscle.Lm; 
	return *this;
}

//---- methods
const char *j2muscle::get_name( void ) const
{
	if( Link.Upper == PELVIS_ && Link.Middle == THIGH_L && Link.Lower == SHANK_L ){ // Left hip/knee muscles
		if( F1 == 1 && F2 == 1){		// RF
			return "RF (L)";
		}
		else if( F1 == -1 && F2 == -1){		// BFP
			return "BFP (L)";
		}
		else if( F1 == 1 && F2 == -1){		// SART
			return "SART (L)";
		}
	}
	else if( Link.Upper == PELVIS_ && Link.Middle == THIGH_R && Link.Lower == SHANK_R ){ // Right hip/knee muscles
		if( F1 == 1 && F2 == 1){		// RF
			return "RF (R)";
		}
		else if( F1 == -1 && F2 == -1){		// BFP
			return "BFP (R)";
		}
		else if( F1 == 1 && F2 == -1){		// SART
			return "SART (R)";
		}
	}
	if( Link.Upper == THIGH_L && Link.Middle == SHANK_L && Link.Lower == FOOT_L ){ // Left knee/ankle muscles (GA only)
		if( F1 == -1 && F2 == -1){		// GA
			return "GA (L)";
		}
	}
	else if( Link.Upper == THIGH_R && Link.Middle == SHANK_R && Link.Lower == FOOT_R ){ // Right knee/ankle muscles (GA only)
		if( F1 == -1 && F2 == -1){		// GA
			return "GA (R)";
		}
	}
	return NULL;
}

void j2muscle::preproc( double *llink )
{
	zmuscle::preproc( llink );
	Lm = llink[get_typelink( Link.Middle )];
	LmLm = Lm*Lm;
	LmA1 = Lm*A1;
	LmA2 = Lm*A2;
	LmR1 = Lm*R1;
	R1A2 = R1*A2;
	R2A1 = R2*A1;
	R2R2 = R2*R2;
	( A2 <= R2 )? X2 = Beta2 = 0: X2 = sqrt( A2A2-R2R2 ), Beta2 = atan2( X2, R2 );

	double r12 = R1-R2;
	ZP = sqrt( LmLm-r12*r12 );
	ZP -= R1*atan2( ZP, r12 )+R2*atan2( ZP, -r12 );

	r12 = R1+R2;
	ZM = sqrt( LmLm-r12*r12 );
	ZM -= ( R1+R2 )*atan2( ZM, r12 );
}

void j2muscle::proc( wjoint *joints )
{
	double dalpha1 = joints[Link.Joint].dA;
	double dalpha2 = joints[Link.LowerJoint].dA;

	double gamma1 = ( F1 == -1 )? joints[Link.Joint].A+Phi1-2*PI: joints[Link.Joint].A+Phi1;
	double gamma2 = ( F2 == -1 )? joints[Link.LowerJoint].A-Phi2-2*PI: joints[Link.LowerJoint].A-Phi2;

	double cos1 = cos( gamma1 );
	double cos2 = cos( gamma2 );
	double sin1 = sin( gamma1 );
	double sin2 = sin( gamma2 );

	double lm_a1_cos1 = LmA1*cos1;
	double lm_a2_cos2 = LmA2*cos2;
	double lm_a1_sin1 = LmA1*sin1;
	double lm_a2_sin2 = LmA2*sin2;
	double a1_a2_sin12 = A1A2*( sin1*cos2+cos1*sin2);
	double a1_a2_cos12 = A1A2*( cos1*cos2-sin1*sin2 );
	double psi1 = atan2( lm_a2_sin2, LmLm-lm_a2_cos2 );
	double psi2 = atan2( lm_a1_sin1, LmLm-lm_a1_cos1 );
	double delta1 = gamma1-psi1;
	double delta2 = gamma2-psi2;
	double ll = A1A1+A2A2+LmLm+2*( a1_a2_cos12-lm_a1_cos1-lm_a2_cos2 );
	double t = ( A1A1-lm_a1_cos1+a1_a2_cos12 )/ll;

	L = sqrt( ll );
	H1 = ( lm_a1_sin1-a1_a2_sin12 )/L;
	if(__IS_BLOCK( t, H1, R1, delta1 )){		// upper block
		double zz = LmLm-2*lm_a2_cos2+A2A2-R1R1;
		double z = sqrt( zz );
		double beta = atan2(z, R1);
		double phi = gamma2+beta*sign( delta1 )+psi1;
		t = ( A2A2-lm_a2_cos2+R1A2*cos( phi ))/zz;

		L = z+X1+R1*( fabs( delta1 )-Beta1-beta );
		H1 = R1*sign( delta1 );
		H2 = ( lm_a2_sin2-R1A2*sin( phi ))/z;
		if( __IS_BLOCK( t, H2, R2, delta2 )){	// two blocks
			H2 = R2*sign( delta2 );		
			L = X1+X2+R1*( fabs( gamma1 )-Beta1 )+R2*( fabs( gamma2 )-Beta2 );
			L += ( H1*H2 > 0 )? ZP: ZM;
		}
	}
	else{						// check if lower block
		t =( A2A2-lm_a2_cos2+a1_a2_cos12 )/ll;

		H2 = ( lm_a2_sin2-a1_a2_sin12 )/L;
		if(__IS_BLOCK( t, H2, R2, delta2 )){	// lower block
			double zz = LmLm-2*lm_a1_cos1+A1A1-R2R2;
			double z = sqrt( zz );
			double beta = atan2( z, R2 );
			double phi = gamma1+beta*sign( delta2 )+psi2;
			t = ( A1A1-lm_a1_cos1+R2A1*cos( phi ))/zz;

			L = z+X2+R2*( fabs( delta2 )-beta-Beta2 );
			H1 = ( lm_a1_sin1-R2A1*sin( phi ))/z;
			H2 = R2*sign( delta2 );
			if(__IS_BLOCK( t, H1, R1, delta1 )){		// two blocks
				H1 = R1*sign( delta1 );
				L = X1+X2+R1*( fabs( gamma1 )-Beta1 )+R2*( fabs( gamma2 )-Beta2 );
				L += ( H1*H2 > 0 )? ZP: ZM;
			}
		}
	}	
	L += Ladd;
	V = H1*dalpha1+H2*dalpha2;
}
#undef __IS_BLOCK
#elif defined (__MECHANICS_3D__)
// TODO implementation 3d model
#endif // __MECHANICS_2D__
