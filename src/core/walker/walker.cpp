//////////////////////////////////////////////////////////////////////
// walker.cpp
#include "precompile.h"

#if defined (__MECHANICS_2D__)

#include "walker.h"
#include "hhnnetwork.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

static alignas( 16 ) double _Q_ini[NUM_INDEPEND] = {
	-1.75997231338469,
	-2.76907231338469,
	-1.96772231338469,
	-1.02280958550262,
	-2.11940534194854,
	-1.01213956708777,
	-553.8125,
	237.6625,
	0.860825242,
	-0.0586138013023647,
};

static alignas( 16 ) double _P_ini[NUM_INDEPEND] = {
	0.003571666667,
	-0.004845833333,
	0.000527500000000001,
	-0.00163515470863359,
	-0.00272920487084002,
	-0.00145133457225693,
	0.5686875,
	-0.0475625,
	-0.000454866667,
	0.000307038836962111,
};

const char *get_vname( size_t ver )
{
	switch( ver ){
		case TRUNK_H:
			return "vertex: High point of Trunk";
		case LOINS:
			return "vertex: Loins";
		case HIP:
			return "vertex: Hip";
		case KNEE_L:
			return "vertex: Left Knee";
		case KNEE_R:
			return "vertex: Right Knee";
		case ANKLE_L:
			return "vertex: Left Ankle";
		case ANKLE_R:
			return "vertex: Right Ankle";
		case TOE__L:
			return "vertex: Left Toe";
		case TOE__R:
			return "vertex: Right Toe";
		default:;
	}
	return NULL;
}

const char *get_lname( size_t link )
{
	switch( link ){
		case TRUNK_:
			return "link: Trunk";
		case PELVIS_:
			return "link: Pelvis";
		case THIGH_L:
			return "link: Left Thigh";
		case THIGH_R:
			return "link: Right Thigh";
		case SHANK_L:
			return "link: Left Shank";
		case SHANK_R:
			return "link: Right Shank";
		case FOOT_L:
			return "link: Left Foot";
		case FOOT_R:
			return "link: Right Foot";
		default:;
	}
	return NULL;
}

const char *get_jname( size_t joint )
{
	switch( joint ){
		case A_L:
			return "joint: Loins";
		case A_HL:
			return "joint: Left Hip";
		case A_HR:
			return "joint: Right Hip";
		case A_KL:
			return "joint: Left Knee";
		case A_KR:
			return "joint: Right Knee";
		case A_AL:
			return "joint: Left Ankle";
		case A_AR:
			return "joint: Right Ankle";
		default:;
	}
	return NULL;
}

const char *get_tname( size_t tg )
{
	switch( tg ){
		case HEEL_L:
			return "grf: Left Heel";
		case HEEL_R:
			return "grf: Right Heel";
		case TOE_L:
			return "grf: Left Toe";
		case TOE_R:
			return "grf: Right Toe";
	}
	return NULL;
}

wjoint rad2deg( wjoint &rad,  size_t jid )
{
	wjoint deg; deg.A = deg.dA = -1;
	switch( jid ){
		case A_L:
		case A_HL:
		case A_HR:
		case A_AL:
		case A_AR:
			deg.A = rad.A*180./PI;
			deg.dA = rad.dA*180./PI;
			break;
		case A_KL:
		case A_KR:
			deg.A = 360.-rad.A*180./PI;
			deg.dA = 360.-rad.dA*180./PI;
			break;
		default:; 
	}
	return deg;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
walker::walker( void ) :
	Gravity( ::Gravity ), BJ( 0.0 ), GL( 0.0 ),X0( 0. ), IsTouched( false ),
	TanSl( 0.0 ), SinSl( 0.0 ), CosSl( 1.0 ),
	Ground( 3. , 350. ), Forepart( 6., 35. ), TrunkLim( 3000., 18000. ),
	LoinsLim( 2000., 20000. ), PelvisLim( 3000., 18000. ), JointsLim( 3000., 18000. ), 
	IsTrQFixed( false ), IsTrXFixed( false ), IsTrYFixed( true )
{
	RGJ[A_L].UppSegment = A_R; RGJ[A_L].LowSegment = A_P;
	RGJ[A_HL].UppSegment = A_P; RGJ[A_HL].LowSegment = A_TL;
	RGJ[A_HR].UppSegment = A_P; RGJ[A_HR].LowSegment = A_TR;
	RGJ[A_KL].UppSegment = A_TL; RGJ[A_KL].LowSegment = A_SL;
	RGJ[A_AL].UppSegment = A_SL; RGJ[A_AL].LowSegment = A_FL;
	RGJ[A_KR].UppSegment = A_TR; RGJ[A_KR].LowSegment = A_SR;
	RGJ[A_AR].UppSegment = A_SR; RGJ[A_AR].LowSegment = A_FR;

	Link[TRUNK_].Type = TRUNK; Link[TRUNK_].In = A_R;
	Link[PELVIS_].Type = PELVIS; Link[PELVIS_].In = A_P;
	Link[THIGH_L].Type = THIGH; Link[THIGH_L].In = A_TL;
	Link[THIGH_R].Type = THIGH; Link[THIGH_R].In = A_TR;
	Link[SHANK_L].Type = SHANK; Link[SHANK_L].In = A_SL;
	Link[SHANK_R].Type = SHANK; Link[SHANK_R].In = A_SR;
	Link[FOOT_L].Type = FOOT; Link[FOOT_L].In = A_FL;
	Link[FOOT_R].Type = FOOT; Link[FOOT_R].In = A_FR;
	
}

walker::~walker( void )
{
	for( size_t i = 0; i < Muscles.size(); ++i ){
		delete Muscles[i];
	}
}

const void *walker::select( unit_code *view )const
{
	switch( view->UnitId ){
		case _id_Vertex:
			if(!( view->NNIndex >= 0 && view->NNIndex < NUM_VERTEX )) break;
			switch( view->Param ){
				case _id_Coordinate_X:
					return ( double *)&( Ver[view->NNIndex].X );
				case _id_Coordinate_Y:
					return ( double *)&( Ver[view->NNIndex].Y );
				case _id_Velocity_X:
					return ( double *)&( Ver[view->NNIndex].dX );
				case _id_Velocity_Y:
					return ( double *)&( Ver[view->NNIndex].dY );
				}
			break;
		case _id_Link:
			if(!( view->NNIndex >= 0 && view->NNIndex < NUM_LINK )) break;
/*
			switch( view->Param ){
				case _id_Angle:
					return ( double *)&( Link[view->NNIndex].A );
				case _id_Angle_Velocity:
					return ( double *)&( Link[view->NNIndex].dA );
				}
*/
			break;
		case _id_Joint:
			if(!( view->NNIndex >= 0 && view->NNIndex < NUM_JOINT )) break;
			switch( view->Param ){
				case _id_Angle:
					return ( double *)&( JointDeg[view->NNIndex].A );
				case _id_Angle_Velocity:
					return ( double *)&( JointDeg[view->NNIndex].dA );
				}
			break;
		case _id_Muscle:
			if( view->NNIndex >= 0 && view->NNIndex < Muscles.size())
				return ( Muscles[view->NNIndex] )->select( view );
			break;
		case _id_Ground:
			if(!( view->NNIndex >= 0 && view->NNIndex < NUM_TOUCH )) break;
			switch( view->Param ){
				case _id_GroundForce_X:
					return ( double *)&( TG[view->NNIndex].Fx );
				case _id_GroundForce_Y:
					return ( double *)&( TG[view->NNIndex].Fy );
				}
			break;
		case _id_Body:
		default:;
	}
	return NULL;
}

void *walker::select( CHhnControlled *ctrl )
{
	int ctrl_par = ctrl->get_par().Param;
	switch( ctrl_par){
		case _id_Ground:
			return &GL;
		case _id_TreadmillL:
			return &GV_L;
		case _id_TreadmillR:
			return &GV_R;
		default:
			return NULL;
	}
}

double walker::ground( double x )
{
	if( IsTouched ){
		GL = -TanSl*( x-X0 );
		return GL;
	}
	return 0.;
}

void walker::set_slope( double slope )
{
	TanSl = tan(( PI*slope )/180.0 );
	SinSl = sin(( PI*slope )/180.0 );
	CosSl = cos(( PI*slope )/180.0 );
}

const string &walker::get_name( const unit_code &code ) const
{
	static string name;
	switch( code.UnitId ){
		case _id_Body:
			name = "Walker";
			break;
		case _id_Vertex:
			name = get_vname( code.NNIndex );
			break;
		case _id_Link:
			name = get_lname( code.NNIndex );
			break;
		case _id_Joint:
			name = get_jname( code.NNIndex );
			break;
		case _id_Ground:
			name = get_tname( code.NNIndex );
			break;
		case _id_Muscle:
			if( code.NNIndex < Muscles.size()){
				name = Muscles[code.NNIndex]->get_name();
				break;
			}
		default:
			name = "";
		}
	return name;
}

void walker::reg_unit( runman *man )		// new code
{
	::reg_unit( this, walker::calc_biomech, _id_PASS_WALKER, -1, man ); 
}

void walker::calc_biomech( double step )	// new code
{
	const alignas( 16 ) int DN_MUS = 20;
	alignas( 16 ) double qc[NUM_INDEPEND] = {0.}, pc[NUM_INDEPEND] = {0.};
	alignas( 16 ) double fq0[NUM_INDEPEND] = {0.}, fq1[NUM_INDEPEND] = {0.}, fq2[NUM_INDEPEND] = {0.}, fq3[NUM_INDEPEND] = {0.};
	alignas( 16 ) double lt0[DN_MUS] = {0.}, vt0[DN_MUS] = {0.}, ltc[DN_MUS] = {0.}, vtc[DN_MUS] = {0.};
	alignas( 16 ) double flt0[DN_MUS] = {0.}, flt1[DN_MUS] = {0.}, flt2[DN_MUS] = {0.}, flt3[DN_MUS] = {0.};
	size_t m_size = Muscles.size();
	for( size_t i = 0; i < m_size; ++i ){
		zmuscle *m = Muscles[i];
		lt0[i] = m->LT;
		vt0[i] = m->VT;
	}
	get_f( TG, Q, P, lt0, vt0, fq0, flt0 );
	for( size_t i = 0; i < NUM_INDEPEND; ++i ){
		fq0[i] *= step;
		qc[i] = Q[i]+0.5*step*P[i];
		pc[i] = P[i]+0.5*fq0[i];
	}
	for( size_t i = 0; i < m_size; ++i ){
		flt0[i] *= step;
		ltc[i] = lt0[i]+0.5*step*vt0[i];
		vtc[i] = vt0[i]+0.5*flt0[i];
	}
	get_f( TG, qc, pc, ltc, vtc, fq1, flt1 );
	for( size_t i = 0; i < NUM_INDEPEND; ++i ){
		fq1[i] *= step;
		qc[i] = Q[i]+0.5*step*( P[i]+0.5*fq0[i] );
		pc[i] = P[i]+0.5*fq1[i];
	}
	for( size_t i = 0; i < m_size; ++i ){
		flt1[i] *= step;
		ltc[i] += 0.25*step*flt0[i];
		vtc[i] = vt0[i]+0.5*flt1[i];
	}
	get_f( TG, qc, pc, ltc, vtc, fq2, flt2 );
	for( size_t i = 0; i < NUM_INDEPEND; ++i ){
		fq2[i] *= step;
		qc[i] = Q[i]+step*( P[i]+0.5*fq1[i] );
		pc[i] = P[i]+fq2[i];
	}
	for( size_t i = 0; i < m_size; ++i ){
		flt2[i] *= step;
		ltc[i] = lt0[i]+step*( vt0[i]+0.5*flt1[i] );
		vtc[i] = vt0[i]+flt2[i];
	}
	get_f( TG, qc, pc, ltc, vtc, fq3, flt3 );
	for( size_t i = 0; i < NUM_INDEPEND; ++i ){
		fq3[i] *= step;
		Q[i] += step*( P[i]+( fq0[i]+fq1[i]+fq2[i] )/6. ); 
		P[i] += ( fq0[i]+2*( fq1[i]+fq2[i] )+fq3[i] )/6.;
	}
	for( size_t i = 0; i < m_size; ++i ){
		zmuscle *m = Muscles[i];
		flt3[i] *= step;
		m->LT += step*( vt0[i]+( flt0[i]+flt1[i]+flt2[i] )/6.);
		m->VT += ( flt0[i]+2*( flt1[i]+flt2[i] )+flt3[i] )/6.;
		if( m->LT <= m->LT0 ){
			m->VT = 0.0;
			m->LT = m->LT0;
		}
		double blt = 1./( 1.+EXP( 20000.0*( m->LT0-m->LT )));
		m->VT = m->VT*blt;
		m->LT = m->LT0+( m->LT-m->LT0 )*blt;
	}
	for( size_t i = 0; i < m_size; ++i ){
		zmuscle *m = Muscles[i];
		m->proc( Joint );
		m->calc_lvm();
	}
	for( size_t i = 0; i < NUM_JOINT; ++i ){
		JointDeg[i] = rad2deg( Joint[i], i );
	}
	TG[HEEL_L].X0 += step*GV_L*CosSl;
	TG[HEEL_L].Y0 -= step*GV_L*SinSl;
	TG[TOE_L].X0 += step*GV_L*CosSl;
	TG[TOE_L].Y0 -= step*GV_L*SinSl;
	TG[HEEL_R].X0 += step*GV_R*CosSl;
	TG[HEEL_R].Y0 -= step*GV_R*SinSl;
	TG[TOE_R].X0 += step*GV_R*CosSl;
	TG[TOE_R].Y0 -= step*GV_R*SinSl;
}

void walker::attach( CHhnNetwork *net )
{
	for( size_t i = 0; i < Muscles.size(); ++i ){
		zmuscle *m = Muscles[i];
		nn_unit *mn = net->get_nnunit( m->MnName );
		if( mn ){
			(( hhn_output *)mn )->attach( m );
		}
	}
	for( size_t i = 0; i < net->size_fbk(); ++i ){
		string mech_name = net->get_fbk( i ).MechName;
		zmuscle *m = get_muscle( mech_name.c_str() );
		touch *grf = get_ground( mech_name.c_str() );
		net->get_fbk( i ).attach( m, grf );
	}
}

void walker::init( double *m, double *cm, double *i, double x, double y )
{
	alignas( 16 ) double cosq[NUM_INDEPEND], sinq[NUM_INDEPEND];
	init_qp(); sincos( Q, cosq, sinq );
	X0 = 0.0; IsTouched = false;
	AB[0] = m[THIGH]*cm[THIGH]+( m[SHANK]+m[FOOT] )*L[THIGH];
	AB[1] = m[SHANK]*cm[SHANK]+m[FOOT]*L[SHANK];
	AB[2] = m[FOOT]*cm[FOOT];
	AB[3] = m[PELVIS]*cm[PELVIS]+( m[TRUNK]+m[HEAD] )*L[PELVIS];
	AB[4] = m[TRUNK]*cm[TRUNK]+m[HEAD]*L[TRUNK];
	AB[5] = AB[4]*L[PELVIS];
	A0[0][0] = i[THIGH]+m[THIGH]*cm[THIGH]*cm[THIGH]+( m[SHANK]+m[FOOT] )*L[THIGH]*L[THIGH];
	A0[0][1] = L[THIGH]*AB[1]; 
	A0[0][2] = L[THIGH]*AB[2];
	A0[1][0] = i[FOOT]+AB[2]*cm[FOOT]; //A(3,3)
	A0[1][1] = i[SHANK]+m[SHANK]*cm[SHANK]*cm[SHANK]+m[FOOT]*L[SHANK]*L[SHANK];
	A0[1][2] = L[SHANK]*AB[2];
	A0[2][0] = m[TRUNK]+m[PELVIS]+m[HEAD]+2*( m[THIGH]+m[SHANK]+m[FOOT] );
	A0[2][1] = i[PELVIS]+m[PELVIS]*cm[PELVIS]*cm[PELVIS]+( m[TRUNK]+m[HEAD] )*L[PELVIS]*L[PELVIS];
	A0[2][2] = i[TRUNK]+m[TRUNK]*cm[TRUNK]*cm[TRUNK]+m[HEAD]*L[TRUNK]*L[TRUNK];
	vfill( Q, cosq, sinq, P );
	jfill( Q, P );
	Q_Trunk = Q[A_R];
	Q_Plvs = Q[A_P];
	X_Trunk = x+L[PELVIS]*cosq[A_P]+L[TRUNK]*cosq[A_R];
	Y_Trunk = y+L[PELVIS]*sinq[A_P]+L[TRUNK]*sinq[A_R]-ground( X_Trunk );
	for( size_t i = 0; i < NUM_TOUCH; ++i ){ 
		TG[i].Underground = false;
		TG[i].X0 = 0.0;		// coordinates of touch point
		TG[i].Y0 = 0.0;
		TG[i].Fx = 0.0;		// force of ground reaction
		TG[i].Fy = 0.0;
	}
	for( size_t i = 0; i < Muscles.size(); ++i ){
		zmuscle *m = Muscles[i];
		m->BM = BM;
		m->BT = BT;
		m->preproc( L );
		m->proc( Joint );
		m->init();
	}
	for( size_t i = 0; i < NUM_JOINT; ++i ){
		JointDeg[i] = rad2deg( Joint[i], i );
	}
}

void walker::vfill( double *q, double *cosq, double *sinq, double *p )
{
	// coordinates of Hip
	alignas( 16 ) double cosl = L[PELVIS]*cosq[A_P];
	alignas( 16 ) double sinl = L[PELVIS]*sinq[A_P];
	vertex *ver = Ver;
	ver[HIP].X  = q[X_H];
	ver[HIP].Y  = q[Y_H];
	ver[HIP].dX = p[X_H];
	ver[HIP].dY = p[Y_H];
	ver[LOINS].X  = ver[HIP].X+cosl;
	ver[LOINS].Y  = ver[HIP].Y+sinl;
	ver[LOINS].dX = ver[HIP].dX-sinl*p[A_P];
	ver[LOINS].dY = ver[HIP].dY+cosl*p[A_P];
	// coordinates of trunk high point
	cosl = L[TRUNK]*cosq[A_R];
	sinl = L[TRUNK]*sinq[A_R];
	ver[TRUNK_H].X  = ver[LOINS].X+cosl;
	ver[TRUNK_H].Y  = ver[LOINS].Y+sinl;
	ver[TRUNK_H].dX = ver[LOINS].dX-sinl*p[A_R];
	ver[TRUNK_H].dY = ver[LOINS].dY+cosl*p[A_R];
	// coordinates of knees
	cosl = L[THIGH]*cosq[A_TL];			//left
	sinl = L[THIGH]*sinq[A_TL];
	ver[KNEE_L].X  = ver[HIP].X+cosl;
	ver[KNEE_L].Y  = ver[HIP].Y+sinl;
	ver[KNEE_L].dX = ver[HIP].dX-sinl*p[A_TL];
	ver[KNEE_L].dY = ver[HIP].dY+cosl*p[A_TL];
	cosl = L[THIGH]*cosq[A_TR];			//right
	sinl = L[THIGH]*sinq[A_TR];
	ver[KNEE_R].X  = ver[HIP].X+cosl;
	ver[KNEE_R].Y  = ver[HIP].Y+sinl;
	ver[KNEE_R].dX = ver[HIP].dX-sinl*p[A_TR];
	ver[KNEE_R].dY = ver[HIP].dY+cosl*p[A_TR];
	// coordinates and velocity of ankles
	cosl = L[SHANK]*cosq[A_SL];			//left
	sinl = L[SHANK]*sinq[A_SL];
	ver[ANKLE_L].X  = ver[KNEE_L].X+cosl;
	ver[ANKLE_L].Y  = ver[KNEE_L].Y+sinl;
	ver[ANKLE_L].dX = ver[KNEE_L].dX-sinl*p[A_SL];
	ver[ANKLE_L].dY = ver[KNEE_L].dY+cosl*p[A_SL];
	cosl = L[SHANK]*cosq[A_SR];			//right
	sinl = L[SHANK]*sinq[A_SR];
	ver[ANKLE_R].X  = ver[KNEE_R].X+cosl;
	ver[ANKLE_R].Y  = ver[KNEE_R].Y+sinl;
	ver[ANKLE_R].dX = ver[KNEE_R].dX-sinl*p[A_SR];
	ver[ANKLE_R].dY = ver[KNEE_R].dY+cosl*p[A_SR];
	// coordinates and velocity of toes
	cosl = L[FOOT]*cosq[A_FL];			//left
	sinl = L[FOOT]*sinq[A_FL];
	ver[TOE__L].X  = ver[ANKLE_L].X+cosl;
	ver[TOE__L].Y  = ver[ANKLE_L].Y+sinl;
	ver[TOE__L].dX = ver[ANKLE_L].dX-sinl*p[A_FL];
	ver[TOE__L].dY = ver[ANKLE_L].dY+cosl*p[A_FL];
	cosl = L[FOOT]*cosq[A_FR];			//right
	sinl = L[FOOT]*sinq[A_FR];
	ver[TOE__R].X  = ver[ANKLE_R].X+cosl;
	ver[TOE__R].Y  = ver[ANKLE_R].Y+sinl;
	ver[TOE__R].dX = ver[ANKLE_R].dX-sinl*p[A_FR];
	ver[TOE__R].dY = ver[ANKLE_R].dY+cosl*p[A_FR];
}

void walker::jfill( double *q, double *p )
{
	alignas( 16 ) wjoint *joint = Joint;
	joint[A_L].A = PI+q[A_R]-q[A_P];
	joint[A_HL].A = -q[A_TL]+q[A_P]; 
	joint[A_HR].A = -q[A_TR]+q[A_P];
	joint[A_KL].A = PI+q[A_TL]-q[A_SL];
	joint[A_KR].A = PI+q[A_TR]-q[A_SR];
	joint[A_AL].A = PI+q[A_SL]-q[A_FL];
	joint[A_AR].A = PI+q[A_SR]-q[A_FR];

	joint[A_L].dA =  -p[A_P]+p[A_R];
	joint[A_HL].dA = -p[A_TL]+p[A_P];
	joint[A_HR].dA = -p[A_TR]+p[A_P];
	joint[A_KL].dA = p[A_TL]-p[A_SL]; 
	joint[A_KR].dA = p[A_TR]-p[A_SR]; 
	joint[A_AL].dA =  -p[A_FL]+p[A_SL];
	joint[A_AR].dA =  -p[A_FR]+p[A_SR];
}

void walker::bcvfill( double *q, double *cosq, double *sinq, double *p, double *b )
{
#define __SINAB( a, b )		\
	(( sinq[a] )*( cosq[b] )-( cosq[a] )*( sinq[b] ))

	alignas( 16 ) double p2[NUM_INDEPEND] = {0.f};
	double m1, m2, m3;
	for( size_t i = 0; i < NUM_INDEPEND; p2[i] = p[i]*p[i], ++i ); 
	m1 = __SINAB( A_SL, A_TL )*A0[0][1];
	m2 = __SINAB( A_FL, A_TL )*A0[0][2];
	m3 = __SINAB( A_FL, A_SL )*A0[1][2];
 	b[A_TL] = m1*p2[A_SL]+m2*p2[A_FL];
	b[A_SL] = -m1*p2[A_TL]+m3*p2[A_FL];
	b[A_FL] = -m2*p2[A_TL]-m3*p2[A_SL];
	m1 = __SINAB( A_SR, A_TR )*A0[0][1];
	m2 = __SINAB( A_FR, A_TR )*A0[0][2];
	m3 = __SINAB( A_FR, A_SR )*A0[1][2];
	b[A_TR] = m1*p2[A_SR]+m2*p2[A_FR];
	b[A_SR] = -m1*p2[A_TR]+m3*p2[A_FR];
	b[A_FR] = -m2*p2[A_TR]-m3*p2[A_SR];
	b[X_H] = 0.0;
	for( size_t i = 0; i < A_TR; b[X_H] += AB[i]*( cosq[i]*p2[i]+cosq[i+A_TR]*p2[i+A_TR] ), ++i );
	for( size_t i = 0; i < 2; b[X_H] += AB[i+A_TR]*cosq[i+A_P]*p2[i+A_P], ++i );
	b[Y_H] = 0.0;
	for( size_t i = 0; i < A_TR; b[Y_H] += AB[i]*( sinq[i]*p2[i]+sinq[i+A_TR]*p2[i+A_TR] ), ++i );
	for( size_t i = 0; i < 2; b[Y_H] += AB[i+A_TR]*sinq[i+A_P]*p2[i+A_P], ++i );
	m1 = __SINAB( A_P, A_R )*AB[5];	
	b[A_P] = -m1*p2[A_R];
	b[A_R] = m1*p2[A_P];
	for( size_t i = 0; i < A_TR; b[i] += -Gravity*AB[i]*cosq[i], ++i );
	for( size_t i = A_TR; i < X_H; b[i] += -Gravity*AB[i-A_TR]*cosq[i], ++i );
	b[Y_H] += -Gravity*A0[2][0];
	for( size_t i = A_P; i < NUM_INDEPEND; b[i] += -Gravity*AB[i-A_FR]*cosq[i], ++i );
	for( size_t i = 0; i < NUM_JOINT; ++i ){
		double vf = BJ*Joint[i].dA; 
		b[RGJ[i].UppSegment] -= vf;
		b[RGJ[i].LowSegment] += vf;
	}
#undef __SINAB
}

void walker::gfill( double *q, double *cosq, double *sinq, touch *tg, double *b )
{
	if( !IsTouched ){
		bool touch_l = false;
		bool touch_r = false;
		double x0_l = 0;
		double x0_r = 0;
		X0 = 0.;
		if( Ver[TOE__L].Y <= 0. ){
			X0 = x0_l = Ver[TOE__L].X;
			IsTouched = touch_l = true;
		}
		if( Ver[TOE__R].Y <= 0. ){
			X0 = x0_r = Ver[TOE__R].X;
			IsTouched = touch_r = true;
			}
		if( touch_l && touch_r ){
			if( TanSl < 0. ){
				X0 = min( x0_l, x0_r );
			}
			else if( TanSl > 0. ){
				X0 = max( x0_l, x0_r );
			}
		}
	}
	for( size_t i = 0; i < NUM_TOUCH; ++i ){
		double yp_toe = y2yp( 0, Ver[ANKLE_L+i].Y-( ground( Ver[ANKLE_L+i].X )));
		if( yp_toe < 0.0 && !tg[i].Underground ){
			tg[i].Y0 = ground( Ver[ANKLE_L+i].X );
			tg[i].X0 = Ver[ANKLE_L+i].X;
		}
		tg[i].Underground = ( yp_toe < 0.0 );
		double xt = x2xt( tg[i].X0-Ver[ANKLE_L+i].X, tg[i].Y0-Ver[ANKLE_L+i].Y );
		double yp = y2yp( tg[i].X0-Ver[ANKLE_L+i].X, tg[i].Y0-Ver[ANKLE_L+i].Y );
		double vt = x2xt( Ver[ANKLE_L+i].dX, Ver[ANKLE_L+i].dY );
		double vp = y2yp( Ver[ANKLE_L+i].dX, Ver[ANKLE_L+i].dY );
		vp = vp/( 1+EXP( 4000000.0*vp ));
		double kgr = 1./( 1+EXP( 40000.0*yp_toe ));
		double ft = kgr*Ground.force( xt, vt );
		double fp = kgr*Ground.force( yp, vp );
		tg[i].Fx = xt2x( ft, fp );
		tg[i].Fy = yp2y( ft, fp );
	}
	// Heels
	if( tg[HEEL_L].Fx != 0. ){
		b[A_TL] -= tg[HEEL_L].Fx*L[THIGH]*sinq[A_TL];
		b[A_SL] -= tg[HEEL_L].Fx*L[SHANK]*sinq[A_SL];
		b[X_H] += tg[HEEL_L].Fx;
	}
	if( tg[HEEL_L].Fy != 0. ){
		b[A_TL] += tg[HEEL_L].Fy*L[THIGH]*cosq[A_TL];
		b[A_SL] += tg[HEEL_L].Fy*L[SHANK]*cosq[A_SL];
		b[Y_H] += tg[HEEL_L].Fy;
	}
	if( tg[HEEL_R].Fx != 0. ){
		b[A_TR] -= tg[HEEL_R].Fx*L[THIGH]*sinq[A_TR];
		b[A_SR] -= tg[HEEL_R].Fx*L[SHANK]*sinq[A_SR];
		b[X_H] += tg[HEEL_R].Fx;
	}
	if( tg[HEEL_R].Fy != 0. ){
		b[A_TR] += tg[HEEL_R].Fy*L[THIGH]*cosq[A_TR];
		b[A_SR] += tg[HEEL_R].Fy*L[SHANK]*cosq[A_SR];
		b[Y_H] += tg[HEEL_R].Fy;
	}
	//Toes
	if( tg[TOE_L].Fx != 0. ){
		b[A_TL] -= tg[TOE_L].Fx*L[THIGH]*sinq[A_TL];
		b[A_SL] -= tg[TOE_L].Fx*L[SHANK]*sinq[A_SL];
		b[A_FL] -= tg[TOE_L].Fx*L[FOOT]*sinq[A_FL];
		b[X_H] += tg[TOE_L].Fx;
	}
	if( tg[TOE_L].Fy != 0. ){
		b[A_TL] += tg[TOE_L].Fy*L[THIGH]*cosq[A_TL];
		b[A_SL] += tg[TOE_L].Fy*L[SHANK]*cosq[A_SL];
		b[A_FL] += tg[TOE_L].Fy*L[FOOT]*cosq[A_FL];
		b[Y_H] += tg[TOE_L].Fy;
	}
	if( tg[TOE_R].Fx != 0. ){
		b[A_TR] -= tg[TOE_R].Fx*L[THIGH]*sinq[A_TR];
		b[A_SR] -= tg[TOE_R].Fx*L[SHANK]*sinq[A_SR];
		b[A_FR] -= tg[TOE_R].Fx*L[FOOT]*sinq[A_FR];
		b[X_H] += tg[TOE_R].Fx;
	}
	if( tg[TOE_R].Fy != 0. ){
		b[A_TR] += tg[TOE_R].Fy*L[THIGH]*cosq[A_TR];
		b[A_SR] += tg[TOE_R].Fy*L[SHANK]*cosq[A_SR];
		b[A_FR] += tg[TOE_R].Fy*L[FOOT]*cosq[A_FR];
		b[Y_H] += tg[TOE_R].Fy;
	}
}

void walker::restrictions( double *cosq, double *sinq, double *b )
{
/*
//	In current model forepart of the cat model supported at constant height 
//	and at constant horizontal velocity. As if trunk is suspend by visco-elastic 
//	link to cart moving at constant speed. Method 'restrictions' calculates contribution 
//	to generalized forces from reaction forces of this cart.
//	Solution: 
//	All influence of the forepart of the cat in method restrictions.
//	If you want to leave ability to maintain forepart of trunk at some height
//	but exclude maintaining constant horizontal velocity you can set to zero
//	xh (force acting in horizontal direction) variable in restrictions method.

	double V_Cat;	// horizontal velocity of the cart.

	double xh = KCat*( Ver[TRUNK_H].x-( X_Cat+V_Cat*time ))+BCat*( Ver[TRUNK_H].dx-V_Cat );
	double yh = KCat*( Ver[TRUNK_H].y-( Y_Cat+ground( Ver[TRUNK_H].x )))+BCat*Ver[TRUNK_H].dy;

	b[X_H] -= xh;
	b[Y_H] -= yh;
	b[A_R] -= L[TRUNK]*( yh*cosq[A_R]-xh*sinq[A_R] );
	b[A_P] -= L[PELVIS]*( yh*cosq[A_P]-xh*sinq[A_P] );
*/
	if( Joint[A_L].A < RGJ[A_L].Min ){
		double vf = LoinsLim.force( RGJ[A_L].Min-Joint[A_L].A, Joint[A_L].dA );
		b[RGJ[A_L].UppSegment] += vf;
		b[RGJ[A_L].LowSegment] -= vf;
	}
	else if ( Joint[A_L].A > RGJ[A_L].Max ){
		double vf = LoinsLim.force( RGJ[A_L].Max-Joint[A_L].A, Joint[A_L].dA );
		b[RGJ[A_L].UppSegment] += vf;
		b[RGJ[A_L].LowSegment] -= vf;
	}
	for( size_t i = A_HL; i < NUM_JOINT; ++i ){
		if( Joint[i].A < RGJ[i].Min ){
			double vf = JointsLim.force( RGJ[i].Min-Joint[i].A, Joint[i].dA );
			b[RGJ[i].UppSegment] += vf;
			b[RGJ[i].LowSegment] -= vf;
		}
		else if( Joint[i].A > RGJ[i].Max ){
			double vf = JointsLim.force( RGJ[i].Max-Joint[i].A, Joint[i].dA );
			b[RGJ[i].UppSegment] += vf;
			b[RGJ[i].LowSegment] -= vf;
		}
	}
	double xh = 0.0, yh = 0.0;
	if( IsTrXFixed ){
		xh -= Forepart.force( X_Trunk-Ver[TRUNK_H].X, Ver[TRUNK_H].dX ); //( KCat*( X_Trunk-Ver[TRUNK_H].X )-BCat*Ver[TRUNK_H].dX );
	}
	if( IsTrYFixed ){
		yh -= Forepart.force( Y_Trunk-Ver[TRUNK_H].Y+ground( Ver[TRUNK_H].X ), Ver[TRUNK_H].dY );// KCat*(( Y_Trunk+ground( Ver[TRUNK_H].X ))-Ver[TRUNK_H].Y )-BCat*Ver[TRUNK_H].dY;
	}
	if( IsTrQFixed ){
		b[A_R] += TrunkLim.force( Q_Trunk-Q[A_R], P[A_R] );
	}
	if( IsPlvFixed ){
		b[A_P] += PelvisLim.force( Q_Plvs-Q[A_P], P[A_P] );
	}
	b[X_H] -= xh; b[Y_H] -= yh;
	b[A_R] -= L[TRUNK]*( yh*cosq[A_R]-xh*sinq[A_R] );
	b[A_P] -= L[PELVIS]*( yh*cosq[A_P]-xh*sinq[A_P] );
}

void walker::add_fm( double *b )
{
	alignas( 16 ) double f[NUM_INDEPEND];		// Generalized forces
	for( size_t i = 0; i < NUM_INDEPEND; f[i] = 0.0, ++i );
	for( size_t i = 0; i < Muscles.size(); ++i ){
		Muscles[i]->add_f( f, Link );			//avoid virtual functions. overhead is too big
	}
	for( size_t i = 0; i < A_R; b[i] += f[i], ++i );
}

void walker::solve( double *q, double *cosq, double *sinq, double *b, double *result )
{
#define COSAB( a, b )		\
	(( cosq[a] )*( cosq[b] )+( sinq[a] )*( sinq[b] ))

	const size_t max_inx = 5;	// Setting Dimension of System
	alignas( 16 ) double y[max_inx] = {0.};
	alignas( 16 ) double ai[NUM_INDEPEND][NUM_INDEPEND] = {0.};
	alignas( 16 ) double ar[3] = {0.};
	alignas( 16 ) double ap[2] = {0.};
	alignas( 16 ) double at[4] = {0.};
	alignas( 16 ) double as[3] = {0.};
	alignas( 16 ) double af[2] = {0.};
	ai[X_H][Y_H] = ai[Y_H][X_H] = 0.0;
	ai[X_H][X_H] = ai[Y_H][Y_H] = A0[2][0];
	ai[A_TL][A_TL] = A0[0][0];
	ai[A_TR][A_TR] = A0[0][0];
	ai[A_FL][A_FL] = A0[1][0];
	ai[A_FR][A_FR] = A0[1][0];
	ai[A_SL][A_SL] = A0[1][1];
	ai[A_SR][A_SR] = A0[1][1];
	ai[A_P][A_P]   = A0[2][1];
	ai[A_R][A_R]   = A0[2][2];
	ai[A_SL][A_TL] = ai[A_TL][A_SL] = COSAB( A_SL, A_TL )*A0[0][1];
	ai[A_SR][A_TR] = ai[A_TR][A_SR] = COSAB( A_SR, A_TR )*A0[0][1];
	ai[A_FL][A_TL] = ai[A_TL][A_FL] = COSAB( A_FL, A_TL )*A0[0][2];
	ai[A_FR][A_TR] = ai[A_TR][A_FR] = COSAB( A_FR, A_TR )*A0[0][2];
	ai[A_FL][A_SL] = ai[A_SL][A_FL] = COSAB( A_FL, A_SL )*A0[1][2];
	ai[A_FR][A_SR] = ai[A_SR][A_FR] = COSAB( A_FR, A_SR )*A0[1][2];
	ai[X_H][A_TL] = ai[A_TL][X_H] = -AB[0]*sinq[A_TL];
	ai[Y_H][A_TL] = ai[A_TL][Y_H] =  AB[0]*cosq[A_TL];
	ai[X_H][A_TR] = ai[A_TR][X_H] = -AB[0]*sinq[A_TR];
	ai[Y_H][A_TR] = ai[A_TR][Y_H] =  AB[0]*cosq[A_TR];
	ai[X_H][A_SL] = ai[A_SL][X_H] = -AB[1]*sinq[A_SL];
	ai[Y_H][A_SL] = ai[A_SL][Y_H] =  AB[1]*cosq[A_SL];
	ai[X_H][A_SR] = ai[A_SR][X_H] = -AB[1]*sinq[A_SR];
	ai[Y_H][A_SR] = ai[A_SR][Y_H] =  AB[1]*cosq[A_SR];
	ai[X_H][A_FL] = ai[A_FL][X_H] = -AB[2]*sinq[A_FL];
	ai[Y_H][A_FL] = ai[A_FL][Y_H] =  AB[2]*cosq[A_FL];
	ai[X_H][A_FR] = ai[A_FR][X_H] = -AB[2]*sinq[A_FR];
	ai[Y_H][A_FR] = ai[A_FR][Y_H] =  AB[2]*cosq[A_FR];
	ai[A_P][X_H]  = ai[X_H][A_P]  = -AB[3]*sinq[A_P];
	ai[A_P][Y_H]  = ai[Y_H][A_P]  =  AB[3]*cosq[A_P];
	ai[A_R][X_H]  = ai[X_H][A_R]  = -AB[4]*sinq[A_R];
	ai[A_R][Y_H]  = ai[Y_H][A_R]  =  AB[4]*cosq[A_R];
	ai[A_P][A_R]  = ai[A_R][A_P]  =  AB[5]*COSAB( A_P, A_R );
	// A_R Exclusion 9
	double bR = -b[A_R]/ai[A_R][A_R];
	ar[0] = -ai[A_R][X_H]/ai[A_R][A_R];
	ar[1] = -ai[A_R][Y_H]/ai[A_R][A_R];
	ar[2] = -ai[A_R][A_P]/ai[A_R][A_R];
	ai[A_P][X_H] += ai[A_P][A_R]*ar[0];
	ai[A_P][Y_H] += ai[A_P][A_R]*ar[1];
	ai[A_P][A_P] += ai[A_P][A_R]*ar[2];
	b[A_P] += ai[A_P][A_R]*bR;
	// A_P Exclusion 8 & A_TL Exclusion 0
	double bP = -b[A_P]/ai[A_P][A_P];
	double bT = -b[A_TL]/ai[A_TL][A_TL];
	ap[0] = -ai[A_P][X_H]/ai[A_P][A_P];
	ap[1] = -ai[A_P][Y_H]/ai[A_P][A_P];
	at[0] = -ai[A_TL][A_SL]/ai[A_TL][A_TL];
	at[1] = -ai[A_TL][A_FL]/ai[A_TL][A_TL];
	at[2] = -ai[A_TL][X_H]/ai[A_TL][A_TL];
	at[3] = -ai[A_TL][Y_H]/ai[A_TL][A_TL];
	ai[A_SL][A_SL] += ai[A_SL][A_TL]*at[0];
	ai[A_FL][A_SL] += ai[A_FL][A_TL]*at[0];
	ai[A_SL][A_FL] += ai[A_SL][A_TL]*at[1];
	ai[A_FL][A_FL] += ai[A_FL][A_TL]*at[1];
	ai[A_SL][X_H]  += ai[A_SL][A_TL]*at[2];
	ai[A_FL][X_H]  += ai[A_FL][A_TL]*at[2];
	ai[A_SL][Y_H]  += ai[A_SL][A_TL]*at[3];
	ai[A_FL][Y_H]  += ai[A_FL][A_TL]*at[3];
	b[A_SL] += ai[A_SL][A_TL]*bT;
	b[A_FL] += ai[A_FL][A_TL]*bT;
	// A_SL Exclusion 1
	double bS = -b[A_SL]/ai[A_SL][A_SL];
	as[0] = -ai[A_SL][A_FL]/ai[A_SL][A_SL];
	as[1] = -ai[A_SL][X_H]/ai[A_SL][A_SL];
	as[2] = -ai[A_SL][Y_H]/ai[A_SL][A_SL];
	ai[A_FL][A_FL] += ai[A_FL][A_SL]*as[0];
	ai[A_FL][X_H]  += ai[A_FL][A_SL]*as[1];
	ai[A_FL][Y_H]  += ai[A_FL][A_SL]*as[2];
	b[A_FL] += ai[A_FL][A_SL]*bS;
	// A_FL Exclusion 2
	double bF = -b[A_FL]/ai[A_FL][A_FL];
	af[0] = -ai[A_FL][X_H]/ai[A_FL][A_FL];
	af[1] = -ai[A_FL][Y_H]/ai[A_FL][A_FL];
	ai[X_H][A_P]  += ai[X_H][A_R]*ar[2];
	ai[Y_H][A_P]  += ai[Y_H][A_R]*ar[2];
	ai[X_H][A_SL] += ai[X_H][A_TL]*at[0];
	ai[Y_H][A_SL] += ai[Y_H][A_TL]*at[0];
	ai[X_H][A_FL] += ai[X_H][A_TL]*at[1]+ai[X_H][A_SL]*as[0];
	ai[Y_H][A_FL] += ai[Y_H][A_TL]*at[1]+ai[Y_H][A_SL]*as[0];
	ai[X_H][X_H]  += ai[X_H][A_P]*ap[0]+ai[X_H][A_TL]*at[2]+ai[X_H][A_SL]*as[1]+ai[X_H][A_R]*ar[0]+ai[X_H][A_FL]*af[0];
	ai[Y_H][X_H]  += ai[Y_H][A_P]*ap[0]+ai[Y_H][A_TL]*at[2]+ai[Y_H][A_SL]*as[1]+ai[Y_H][A_R]*ar[0]+ai[Y_H][A_FL]*af[0];
	ai[X_H][Y_H]  += ai[X_H][A_P]*ap[1]+ai[X_H][A_TL]*at[3]+ai[X_H][A_SL]*as[2]+ai[X_H][A_R]*ar[1]+ai[X_H][A_FL]*af[1];
	ai[Y_H][Y_H]  += ai[Y_H][A_P]*ap[1]+ai[Y_H][A_TL]*at[3]+ai[Y_H][A_SL]*as[2]+ai[Y_H][A_R]*ar[1]+ai[Y_H][A_FL]*af[1];
	b[X_H] += ai[X_H][A_R]*bR+ai[X_H][A_SL]*bS+ai[X_H][A_FL]*bF+ai[X_H][A_TL]*bT+ai[X_H][A_P]*bP;
	b[Y_H] += ai[Y_H][A_R]*bR+ai[Y_H][A_SL]*bS+ai[Y_H][A_FL]*bF+ai[Y_H][A_TL]*bT+ai[Y_H][A_P]*bP;
	//Shifting Maxtix A and vector b  
	memcpy( ai[0], ai[3]+3, 5*sizeof( double ));
	memcpy( ai[1], ai[4]+3, 5*sizeof( double ));
	memcpy( ai[2], ai[5]+3, 5*sizeof( double ));
	memcpy( ai[3], ai[6]+3, 5*sizeof( double ));
	memcpy( ai[4], ai[7]+3, 5*sizeof( double ));
	memcpy( b, b+3, 5*sizeof( double ));
	//LU Decomposition of Matrix A
	for( size_t k = 1; k < max_inx; ++k ){
		for( size_t km = k-1, j = k; j < max_inx; ++j ){
			double ss = 0;
			for( size_t m = 0; m < km; ss += ai[km][m]*ai[m][j], ++m );
			ai[km][j] = ( ai[km][j]-ss )/ai[km][km];
			ss = 0;
			for( size_t m = 0; m < k; ss += ai[j][m]*ai[m][k], ++m );
			ai[j][k] -= ss;
		}
	}
	//Back Substitusion of Lower part of Matrix
	y[A_TL] = b[A_TL]/ai[A_TL][A_TL];
	y[A_SL] = ( b[A_SL]-ai[A_SL][A_TL]*y[A_TL] )/ai[A_SL][A_SL];
	y[A_FL] = ( b[A_FL]-ai[A_FL][A_TL]*y[A_TL]-ai[A_FL][A_SL]*y[A_SL] )/ai[A_FL][A_FL];
	y[A_TR] = ( b[A_TR]-ai[A_TR][A_TL]*y[A_TL]-ai[A_TR][A_SL]*y[A_SL]-ai[A_TR][A_FL]*y[A_FL] )/ai[A_TR][A_TR];
	y[A_SR] = ( b[A_SR]-ai[A_SR][A_TL]*y[A_TL]-ai[A_SR][A_SL]*y[A_SL]-ai[A_SR][A_FL]*y[A_FL]-ai[A_SR][A_TR]*y[A_TR] )/ai[A_SR][A_SR];
	//Back Substitusion of Upper part of Matrix and getting the solution of system
	result[Y_H] = result[A_SR] = y[A_SR];
	result[X_H] = result[A_TR] = y[A_TR]-ai[A_TR][A_SR]*result[A_SR];
	result[A_FR] = result[A_FL] = y[A_FL]-ai[A_FL][A_SR]*result[A_SR]-ai[A_FL][A_TR]*result[A_TR];
	result[A_SL] = y[A_SL]-ai[A_SL][A_SR]*result[A_SR]-ai[A_SL][A_TR]*result[A_TR]-ai[A_SL][A_FL]*result[A_FL];
	result[A_TL] = y[A_TL]-ai[A_TL][A_SR]*result[A_SR]-ai[A_TL][A_TR]*result[A_TR]-ai[A_TL][A_FL]*result[A_FL]-ai[A_TL][A_SL]*result[A_SL];
	result[A_SR] = result[A_SL];	result[A_TR] = result[A_TL];
	//Back Substitusion of excluded variables
	result[A_FL] = result[X_H]*af[0]+result[Y_H]*af[1]-bF;
	result[A_SL] = result[A_FL]*as[0]+result[X_H]*as[1]+result[Y_H]*as[2]-bS;
	result[A_TL] = result[A_SL]*at[0]+result[A_FL]*at[1]+result[X_H]*at[2]+result[Y_H]*at[3]-bT;
	result[A_P] = result[X_H]*ap[0]+result[Y_H]*ap[1]-bP;
	result[A_R] = result[X_H]*ar[0]+result[Y_H]*ar[1]+result[A_P]*ar[2]-bR;
#undef COSAB
}

void walker::mfill( double *q, double *p, double *fq, touch *tg )
{
	alignas( 16 ) double b[NUM_INDEPEND] = {0.};
	alignas( 16 ) double cosq[NUM_INDEPEND], sinq[NUM_INDEPEND] = {0.};
	sincos( q, cosq, sinq );		// 30
	bcvfill( q, cosq, sinq, p, b );	// < 1
	vfill( q, cosq, sinq, p );		// < 1
	gfill( q, cosq, sinq, tg, b );// 17
	restrictions( cosq, sinq, b );		// < 1
	add_fm( b );				// < 1 + 16 overhead to call virtual function
	solve( q, cosq, sinq, b, fq );	// 16
}

void walker::get_f( touch *tg, double *q, double *p, double *lt, double *vt, double *fq, double *flt )
{
	jfill( q, p );				// < 1
	for( size_t i = 0; i < Muscles.size(); ++i ){ // avoid virtual functions. overhead is too big
		zmuscle *m = Muscles[i];
		m->proc( Joint );			// 157
		// add restriction
		flt[i] = m->get_at( lt[i], vt[i] ); // 375
	}
	mfill( q, p, fq, tg );	// 94
}

void walker::sincos( double *q, double *cosq, double *sinq )
{
	for( size_t i = 0; i < NUM_INDEPEND; cosq[i] = cos( q[i] ), sinq[i] = sin( q[i] ), ++i );
}

zmuscle *walker::get_muscle( const char *name )
{
	string mname = name;
	for( size_t i = 0; i < Muscles.size(); ++i ){
		zmuscle *m = Muscles[i];
		if( mname == m->get_name() ){
			return m;
		}
	}
	return NULL;
}

touch *walker::get_ground( const char *name )
{
	string tname = name;
	if( tname == "grf: Left Heel" ){
		return TG+HEEL_L;
	}
	if( tname == "grf: Right Heel" ){
		return TG+HEEL_R;
	}
	if( tname == "grf: Left Toe" ){
		return TG+TOE_L;
	}
	if( tname == "grf: Right Toe" ){
		return TG+TOE_R;
	}
	return NULL;
}

void walker::init_qp( void )
{
	// Coordinates
	Q[A_R]  = Q[A_P]+Joint[A_L].A-PI;
	Q[A_TL] = Q[A_P]-Joint[A_HL].A;
	Q[A_SL] = Q[A_TL]-Joint[A_KL].A+PI;
	Q[A_FL] = Q[A_SL]-Joint[A_AL].A+PI;
	Q[A_TR] = Q[A_P]-Joint[A_HR].A;
	Q[A_SR] = Q[A_TR]-Joint[A_KR].A+PI;
	Q[A_FR] = Q[A_SR]-Joint[A_AR].A+PI;
	// Velocities
	P[A_R]  = P[A_P]+Joint[A_L].dA;
	P[A_TL] = P[A_P]-Joint[A_HL].dA;
	P[A_SL] = P[A_TL]-Joint[A_KL].dA;
	P[A_FL] = P[A_SL]-Joint[A_AL].dA;
	P[A_TR] = P[A_P]-Joint[A_HR].dA;
	P[A_SR] = P[A_TR]-Joint[A_KR].dA;
	P[A_FR] = P[A_SR]-Joint[A_AR].dA;
}

#elif defined (__MECHANICS_3D__)
// TODO implementation 3d model
#endif // __MECHANICS_2D__
