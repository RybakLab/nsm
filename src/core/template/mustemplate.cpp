///////////////////////////////////////////////////////////////////////
// mustemplate.cpp
//---------------------------------------------------------------------
#include "precompile.h"

#if defined (__MECHANICS_2D__)

#include "mustemplate.h"
#include "walker.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

const char *__MuscleNames[__id_MAX_MUSCLES] = {
	"Generic",
	"Iliopsoas",
	"Biceps femoris anterior",
	"Sartorius",
	"Rectus femoris",
	"Biceps femoris posterior",
	"Vastus",
	"Gastrocnemius",
	"Tibialis anterior",
	"Soleus",
	};

struct muscle_data{
		double LT0;
		double PT0;
		double Alpha;
		double LT0perL0;
		double M;
		double Shift;
		double TKE;
		double TKL;
		double PKE;
		double PKL;
		double AV;
		double Omega;
		double Fmax;
		double RH0;
		double Vmax;
		double Beta;
		double Amax;
		double F1;
		double F2;
		double Phi1;
		double Phi2;
		double A1;
		double A2;
		double R1;
		double R2;
		double Ladd;
		double Ltr;
		double TdL1;	// new
		double TF1;	// new
		double TKlo;	// new
		double TKsh;	// new
		double CF0;	// new
		double CFtr;	// new
		double CVtr;	// new
};

static muscle_data DD[__id_MAX_MUSCLES] = {
//		   LT0		PT0		Alpha		LT0L0  M	  Shift   TKE   TKL  PKE   PKL     AV       Omega Fmax  RH0	 Vmax	   Beta      Amax      F1  F2  Phi1      Phi2     A1     A2    R1    R2  Ladd  Ltr   TdL1       TF1        TKlo     TKsh     CF0  CFtr  CVtr
		{  0.0000,  0.0000, 0.0000000,	0.00,  0.0000,  0.0,   0.0, 0.0,  0.0, 0.0000, 0.00000,  0.0,  0.0, 0.0, 0.000000, 0.000000, 0.000000,  0,  0, 0.00000,  0.00000, 0.00,  0.0,  0.0,  0.0, 0.0, 0.00, 0.0000000, 0.0000000,  0.0000, 0.00000, 0.0, 0.0,  0.0000000 },
/*IP*/	{ 16.0000, 95.1724, 0.0000000,	0.25, 28.6255, -0.2,  50.0, 2.6, 30.0, 0.0018, 1.28205,  0.55, 1.8, 4.0, 0.547450, 0.992043, 0.205064,  1,  0, 0.00000,  0.00000, 54.9,  7.0,  5.0,  0.0, 0.0, 1.00, 0.0240013, 0.1232330, 24.3532, 4.69996, 1.0, 0.78, 0.0602195 }, /*IP*/
/*BFA*/	{ 31.0000, 52.5963, 0.2443460,	1.00, 7.89725, -0.2,  50.0, 2.6, 30.0, 0.0018, 1.28205,  0.55, 1.8, 4.0, 0.333358, 0.464178, 0.957494, -1,  0, 2.46091,  0.00000, 10.0, 50.0,  5.0,  0.0, 0.0, 1.00, 0.0240013, 0.1232330, 24.3532, 4.69996, 1.0, 0.78, 0.0366693 }, /*BFA*/
/*SART*/{ 29.0000, 10.0500, 0.0733038,	0.25, 5.49358, -0.2,  50.0, 2.6, 30.0, 0.0018, 1.28205,  0.55, 1.8, 4.0, 0.992253, 0.992987, 0.250156,  1, -1, 0.00000,  0.00000, 16.0, 20.0,  5.0,  6.0, 0.0, 1.00, 0.0100000, 0.0113306, 9.06752, 8.00000, 1.0, 0.78, 0.1091480 }, /*SART*/
/*RF*/	{ 89.9972, 61.0026, 0.1221730,	4.00, 6.49875, -0.2,  50.0, 2.6, 30.0, 0.0018, 1.28205,  0.55, 1.8, 4.0, 0.214452, 0.521572, 0.661831,  1,  1, 0.00000,  2.53073, 16.0, 10.0,  5.0, 10.0, 0.0, 1.00, 0.0100000, 0.0113306, 9.06752, 8.00000, 1.0, 0.78, 0.0235897 }, /*RF*/
/*BFP*/	{ 57.0000, 94.9003, 0.1221730,	1.00, 25.6127, -0.2,  50.0, 2.6, 30.0, 0.0018, 1.28205,  0.55, 1.8, 4.0, 0.612948, 0.770385, 0.267400, -1, -1, 2.46091,  0.00000, 10.0, 20.0,  5.0,  6.0, 0.0, 1.00, 0.0100000, 0.0113306, 9.06752, 8.00000, 1.0, 0.78, 0.0674243 }, /*BFP*/
/*VA*/	{ 82.3831, 202.134, 0.2391100,	3.00, 26.8506, -0.2,  50.0, 2.6, 30.0, 0.0018, 1.28205,  0.55, 1.8, 4.0, 0.181208, 0.841158, 0.636886,  1,  0, 0.00000,  2.53073, 97.3, 10.0, 10.0,  0.0, 0.0, 1.00, 0.0240013, 0.1232330, 24.3532, 4.69996, 1.0, 0.78, 0.0199329 }, /*VA*/
/*GA*/	{ 81.0000, 96.8500, 0.3316130,	3.00, 12.9974, -0.1,  50.0, 2.6, 30.0, 0.3400, 1.28205,  0.55, 1.8, 4.0, 0.259000, 0.400819, 0.211974, -1, -1, 0.00000, -3.03687,  6.0, 16.0,  6.0,  5.0, 0.0, 1.00, 0.0100000, 0.0113306, 9.06752, 8.00000, 1.0, 0.78, 0.0284900 }, /*GA*/
/*TA*/	{ 57.0000, 40.4036, 0.1221730,	0.70, 15.5779,  0.0,  50.0, 2.6, 30.0, 0.3400, 1.28205,  0.55, 1.8, 4.0, 0.805987, 0.446370, 0.243204,  1,  0, 0.00000,  0.00000, 15.0, 15.0,  5.0,  0.0, 85., 1.00, 0.0240013, 0.1232330, 24.3532, 4.69996, 1.0, 0.78, 0.0886586 }, /*TA*/
/*SOL*/	{ 68.9999, 10.4000, 0.1221730,	2.00, 1.69889, -0.1,  50.0, 2.6, 30.0, 0.3400, 1.28205,  0.55, 1.8, 4.0, 0.176000, 0.453350, 0.696734, -1,  0, 0.00000, -3.03687, 99.0, 16.0,  5.0,  0.0, 0.0, 1.00, 0.0240013, 0.1232330, 24.3532, 4.69996, 1.0, 0.78, 0.0193600 }  /*SOL*/
};																					   

static int get_linkid( string &name, string &scheme )
{
	if( name == "Trunk" ){
		return TRUNK_;
	}
	else if( name == "Pelvis" ){
		return PELVIS_;
	}
	else if( name == "Thigh" ){
		if( scheme == "Left" ){
			return THIGH_L;
		}
		else if( scheme == "Right" ){
			return THIGH_R;
		}
	}
	else if( name == "Shank" ){
		if( scheme == "Left" ){
			return SHANK_L;
		}
		else if( scheme == "Right" ){
			return SHANK_R;
		}
	}
	else if( name == "Foot" ){
		if( scheme == "Left" ){
			return FOOT_L;
		}
		else if( scheme == "Right" ){
			return FOOT_R;
		}
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////////////
// t_muscle 
t_muscle::t_muscle( uni_template *parent, const char *lscheme, int is_active )
		: uni_template( parent, is_active, ( char **)__MuscleNames, __id_MAX_MUSCLES ), 
		LScheme( lscheme ), JScheme( "One joint" ),
		MnNames( "None" ), Mn( "None" ), Amax( 1.0 ),
		ULink( "None" ), MLink( "None" ), LLink( "None" )
{
//	IsShowType = true;
	TypeName = "Muscle";
	TypeNameS = "zMuscle";
	// attachment scheme
	DList.insert( make_pair( "Attachment\tOrigin\tDistance\nA1", &A1 ));
	DList.insert( make_pair( "Attachment\tOrigin\tMinimal arm\nR1", &R1 ));
	DList.insert( make_pair( "Attachment\tOrigin\tAngle\nPhi1", &Phi1 ));
	DList.insert( make_pair( "Attachment\tOrigin\tDirection [ccwise(1)/cwise(-1)]\nF1", &F1 ));
	DList.insert( make_pair( "Attachment\tInsertion\tDistance\nA2", &A2 ));
	DList.insert( make_pair( "Attachment\tInsertion\tMinimal arm\nR2", &R2 ));
	DList.insert( make_pair( "Attachment\tInsertion\tAngle\nPhi2", &Phi2 ));
	DList.insert( make_pair( "Attachment\tInsertion\tDirection [ccwise(1)/cwise(-1)]\nF2", &F2 ));

	// Muscle properties
	DList.insert( make_pair( "Muscle properties\tLtr\nLtr", &Ltr ));
	DList.insert( make_pair( "Muscle properties\tLength of tendon\nLT0", &LT0 ));
	DList.insert( make_pair( "Muscle properties\tMaximum activation\nAmax", &Amax ));
	DList.insert( make_pair( "Muscle properties\tMaximum tendon force\nPT0", &PT0 ));
	DList.insert( make_pair( "Muscle properties\tAlpha\nAlpha", &Alpha ));
	DList.insert( make_pair( "Muscle properties\tLT0perL0\nLT0perL0", &LT0perL0 ));
	DList.insert( make_pair( "Muscle properties\tMuscle mass\nM", &M ));
	DList.insert( make_pair( "Muscle properties\tShift\nShift", &Shift ));
	DList.insert( make_pair( "Muscle properties\tLadd\nLadd", &Ladd ));

	// tendom element
	DList.insert( make_pair( "Tendom element\tKE\nTKE", &TKE ));
	DList.insert( make_pair( "Tendom element\tKL\nTKL", &TKL ));

	// passive element
	DList.insert( make_pair( "Passive element\tKE\nPKE", &PKE ));
	DList.insert( make_pair( "Passive element\tKL\nPKL", &PKL ));

	// contractile element
	DList.insert( make_pair( "Contractile element\tAV\nAV", &AV ));
	DList.insert( make_pair( "Contractile element\tBeta\nBeta", &Beta ));
	DList.insert( make_pair( "Contractile element\tFmax\nFmax", &Fmax ));
	DList.insert( make_pair( "Contractile element\tOmega\nOmega", &Omega ));
	DList.insert( make_pair( "Contractile element\tRH0\nRH0", &RH0 ));
	DList.insert( make_pair( "Contractile element\tVmax\nVmax", &Vmax ));

#ifdef __NEW_MUSCLE__
	DList.insert( make_pair( "Tendom element\tdL1\nTdL1", &TdL1 ));
	DList.insert( make_pair( "Tendom element\tF1\nTF1", &TF1 ));
	DList.insert( make_pair( "Tendom element\tKlo\nTKlo", &TKlo ));
	DList.insert( make_pair( "Tendom element\tKsh\nTKsh", &TKsh ));
	DList.insert( make_pair( "Contractile element\tF0\nCF0", &CF0 ));
	DList.insert( make_pair( "Contractile element\tFtr\nCFtr", &CFtr ));
	DList.insert( make_pair( "Contractile element\tVtr\nCVtr", &CVtr ));
#endif // __NEW_MUSCLE__
	UnitId = __id_generic_M;
	Name = __MuscleNames[UnitId];
	IsSchDef = false;
	if( LScheme == "Left" || LScheme == "Right" ){
		IsSchDef = true;
	}
}

t_muscle::t_muscle( const t_muscle &m )
		: uni_template( m ), LScheme( m.LScheme ), JScheme( m.JScheme ), IsSchDef( m.IsSchDef ), 
		MnNames( m.MnNames ), Mn( m.Mn ), Amax( m.Amax ),
		ULink( m.ULink ), MLink( m.MLink ), LLink( m.LLink ),
		F1( m.F1 ), F2( m.F2 ), Phi1( m.Phi1 ), Phi2( m.Phi2 ),
		A1( m.A1 ), A2( m.A2 ), R1( m.R1 ), R2( m.R2 ),
		Ltr( m.Ltr ), LT0( m.LT0 ), PT0( m.PT0 ), Alpha( m.Alpha ), LT0perL0( m.LT0perL0 ),
		M( m.M ), Shift( m.Shift ), Ladd( m.Ladd ), TKE( m.TKE ),
		TKL( m.TKL ), PKE( m.PKE ), PKL( m.PKL ), AV( m.AV ),
		Beta( m.Beta ), Fmax( m.Fmax ), Omega( m.Omega ), RH0( m.RH0 ), Vmax( m.Vmax )
#ifdef __NEW_MUSCLE__
		, TdL1( m.TdL1 ), TF1( m.TF1 ), TKlo( m.TKlo ), TKsh( m.TKsh ), 
		CF0( m.CF0 ), CFtr( m.CFtr ), CVtr( m.CVtr )
#endif // __NEW_MUSCLE__
{
	// attachment scheme
	DList.insert( make_pair( "Attachment\tOrigin\tDistance\nA1", &A1 ));
	DList.insert( make_pair( "Attachment\tOrigin\tMinimal arm\nR1", &R1 ));
	DList.insert( make_pair( "Attachment\tOrigin\tAngle\nPhi1", &Phi1 ));
	DList.insert( make_pair( "Attachment\tOrigin\tDirection [ccwise(1)/cwise(-1)]\nF1", &F1 ));
	DList.insert( make_pair( "Attachment\tInsertion\tDistance\nA2", &A2 ));
	DList.insert( make_pair( "Attachment\tInsertion\tMinimal arm\nR2", &R2 ));
	DList.insert( make_pair( "Attachment\tInsertion\tAngle\nPhi2", &Phi2 ));
	DList.insert( make_pair( "Attachment\tInsertion\tDirection [ccwise(1)/cwise(-1)]\nF2", &F2 ));

	// Muscle properties
	DList.insert( make_pair( "Muscle properties\tLtr\nLtr", &Ltr ));
	DList.insert( make_pair( "Muscle properties\tLength of tendon\nLT0", &LT0 ));
	DList.insert( make_pair( "Muscle properties\tMaximum activation\nAmax", &Amax ));
	DList.insert( make_pair( "Muscle properties\tMaximum tendon force\nPT0", &PT0 ));
	DList.insert( make_pair( "Muscle properties\tAlpha\nAlpha", &Alpha ));
	DList.insert( make_pair( "Muscle properties\tLT0perL0\nLT0perL0", &LT0perL0 ));
	DList.insert( make_pair( "Muscle properties\tMuscle mass\nM", &M ));
	DList.insert( make_pair( "Muscle properties\tShift\nShift", &Shift ));
	DList.insert( make_pair( "Muscle properties\tLadd\nLadd", &Ladd ));

	// tendom element
	DList.insert( make_pair( "Tendom element\tKE\nTKE", &TKE ));
	DList.insert( make_pair( "Tendom element\tKL\nTKL", &TKL ));

	// passive element
	DList.insert( make_pair( "Passive element\tKE\nPKE", &PKE ));
	DList.insert( make_pair( "Passive element\tKL\nPKL", &PKL ));

	// contractile element
	DList.insert( make_pair( "Contractile element\tAV\nAV", &AV ));
	DList.insert( make_pair( "Contractile element\tBeta\nBeta", &Beta ));
	DList.insert( make_pair( "Contractile element\tFmax\nFmax", &Fmax ));
	DList.insert( make_pair( "Contractile element\tOmega\nOmega", &Omega ));
	DList.insert( make_pair( "Contractile element\tRH0\nRH0", &RH0 ));
	DList.insert( make_pair( "Contractile element\tVmax\nVmax", &Vmax ));
#ifdef __NEW_MUSCLE__
	DList.insert( make_pair( "Tendom element\tdL1\nTdL1", &TdL1 ));
	DList.insert( make_pair( "Tendom element\tF1\nTF1", &TF1 ));
	DList.insert( make_pair( "Tendom element\tKlo\nTKlo", &TKlo ));
	DList.insert( make_pair( "Tendom element\tKsh\nTKsh", &TKsh ));
	DList.insert( make_pair( "Contractile element\tF0\nCF0", &CF0 ));
	DList.insert( make_pair( "Contractile element\tFtr\nCFtr", &CFtr ));
	DList.insert( make_pair( "Contractile element\tVtr\nCVtr", &CVtr ));
#endif // __NEW_MUSCLE__
}

t_muscle::~t_muscle( void )
{
}

t_muscle &t_muscle::operator = ( const t_muscle &m )
{
	uni_template::operator = ( m );

	F1 = m.F1;
	F2 = m.F2;
	Phi1 = m.Phi1;
	Phi2 = m.Phi2;
	A1 = m.A1;
	A2 = m.A2;
	R1 = m.R1;
	R2 = m.R2;

	Ltr = m.Ltr;
	LT0 = m.LT0;
	PT0 = m.PT0;
	Alpha = m.Alpha;
	LT0perL0 = m.LT0perL0;
	Amax = m.Amax;
	M = m.M;
	Shift = m.Shift;
	Ladd = m.Ladd;

	TKE = m.TKE;
	TKL = m.TKL;

	PKE = m.PKE;
	PKL = m.PKL;

	AV = m.AV;
	Beta = m.Beta;
	Fmax = m.Fmax;
	Omega = m.Omega;
	RH0 = m.RH0;
	Vmax = m.Vmax;

	IsSchDef = m.IsSchDef;
	MnNames = m.MnNames;
	Mn = m.Mn;
	LScheme = m.LScheme;
	JScheme = m.JScheme;
	ULink = m.ULink;
	MLink = m.MLink;
	LLink = m.LLink;

#ifdef __NEW_MUSCLE__
	TdL1 = m.TdL1;
	TF1 = m.TF1;
	TKlo = m.TKlo;
	TKsh = m.TKsh;
	CF0 = m.CF0;
	CFtr = m.CFtr;
	CVtr = m.CVtr;
#endif // __NEW_MUSCLE__
	return *this;
}

void t_muscle::upd_outlist( vector<string> &out_list )
{
	MnNames.clear();
	add_substr( MnNames, "None" );
	for( size_t i = 0; i < out_list.size(); ++i ){
		add_substr( MnNames, out_list[i].c_str() );
	}
}

void t_muscle::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_muscle( *this );
	( *unit )->set_parent( parent );
}

void t_muscle::copy_to( zmuscle **m )
{
	if( JScheme != "One joint" ){
		j2scheme link;
		link.Upper = get_linkid( ULink, LScheme );
		link.Lower = get_linkid( LLink, LScheme );
		link.Middle = get_linkid( MLink, LScheme );
		*m = zcreate( link );
		( double( F2 ) < 0.0 )? (( j2muscle *)( *m ))->F2 = -1:(( j2muscle *)( *m ))->F2 = 1;
		(( j2muscle *)( *m ))->R2 = double( R2 );
	}
	else{
		j1scheme link;
		link.Upper = get_linkid( ULink, LScheme );
		link.Lower = get_linkid( LLink, LScheme );
		*m = zcreate( link );
	}
	( *m )->A1 = double( A1 );
	( *m )->A2 = double( A2 );
	( *m )->R1 = double( R1 );
	( *m )->Phi1 = double( Phi1 )/180.*PI;
	( *m )->Phi2 = double( Phi2 )/180.*PI;
	( double( F1 ) < 0.0 )? ( *m )->F1 = -1:( *m )->F1 = 1;	
	( *m )->LT0 = double( LT0 );
	( *m )->PT0 = double( PT0 );
	( *m )->Alpha0 = double( Alpha )/180.*PI;
	( *m )->LT0perL0 = double( LT0perL0 );
	( *m )->M = double( M );
	( *m )->Shift = double( Shift );
	( *m )->Ltr = double( Ltr );
	( *m )->Ladd = double( Ladd );
	( *m )->TE.KE = double( TKE );
	( *m )->TE.KL = double( TKL );
	( *m )->PE.KE = double( PKE );
	( *m )->PE.KL = double( PKL );
	( *m )->CE.AV = double( AV );
	( *m )->CE.Beta = double( Beta );
	( *m )->CE.Fmax = double( Fmax );
	( *m )->CE.Omega = double( Omega );
	( *m )->CE.RHO = double( RH0 );
	( *m )->CE.Vmax = double( Vmax );
	( *m )->Amax = double( Amax );
	( *m )->MnName = Mn;

#ifdef __NEW_MUSCLE__
	( *m )->TE.dL1 = double( TdL1 );
	( *m )->TE.F1 = double( TF1 );
	( *m )->TE.Klo = double( TKlo );
	( *m )->TE.Ksh = double( TKsh );
	( *m )->CE.F0 = double( CF0 );
	( *m )->CE.Ftr = double( CFtr );
	( *m )->CE.Vtr = double( CVtr );
#endif // __NEW_MUSCLE__
}

void t_muscle::front( string &str, const char *name )
{
	string::size_type pos = str.find( name );
	if( pos != string::npos ){
		del_substr( str, name, pos );
	}
	string tmp("");
	swap( tmp, str );
	add_substr( str, name );
	str += "\t";
	str += tmp;
}

void t_muscle::add_substr( string &str, const char *name )
{
	if( str.empty() ){
		str += name;
	}
	else{
		str += "\t";
		str += name;
	}
}

void t_muscle::del_substr( string &str, const char *name, string::size_type pos )
{
	char delim = '\t';
	string::size_type pos1 = str.find( delim, pos+1 );
	if( pos1 != string::npos ){
		str.erase( str.begin()+pos, str.begin()+pos1+1 );
	}
	else{
		pos = ( pos ==  0 )? pos: pos-1;
		str.erase( str.begin()+pos, str.end() );
	}
}

void t_muscle::make_linkcombo( string &name, string &element )
{
	element = name;
	if( name != "Trunk" ){
		element += "\tTrunk";
	}
	if( name != "Pelvis" ){
		element += "\tPelvis";
	}
	if( name != "Thigh" ){
		element += "\tThigh";
	}
	if( name != "Shank" ){
		element += "\tShank";
	}
	if( name != "Foot" ){
		element += "\tFoot";
	}
	if( name != "None" ){
		element += "\tNone";
	}
}

bool t_muscle::get_addparlist( vector<string> &parlist )
{
	parlist.push_back( "Name" );
	parlist.push_back( "MN input" );
	parlist.push_back( "Leg scheme" );
	parlist.push_back( "Joint scheme" );
	parlist.push_back( "Upper link" );
	parlist.push_back( "Middle link" );
	parlist.push_back( "Lower link" );
	return true;
}

int t_muscle::get_datattr( const char *path )
{
	string p_path = path;
	if( p_path == "Attachment\tRadius of lower joint block" || p_path == "Attachment\tCounterclockwise|clockwise lower muscle 1|-1" ){
		if( JScheme != "One joint" ){
			return GRID_DOUBLE;
		}
		else{
			return GRID_READONLY;
		}
	}
	else if( p_path == "Joint scheme" ){
		return GRID_COMBO|GRID_PROCESS;
	}
	else if( p_path == "Leg scheme" && IsSchDef ){
		return GRID_READONLY;
	}
	else if( p_path == "Middle link" ){
		if( JScheme != "One joint" ){
			return GRID_COMBO;
		}
		return GRID_READONLY;
	}
	return uni_template::get_datattr( path );
}

bool t_muscle::get_addpar( const char *path, string &name, string &element, int &type )
{
	if( path ){
		string p_path = path;
		if( p_path == "Leg scheme" && !IsSchDef ){
			name = "Data";
			element = LScheme;
			if( LScheme != "None" ){
				element += "\tNone";
			}
			if( LScheme != "Left" ){
				element += "\tLeft";
			}
			if( LScheme != "Right" ){
				element += "\tRight";
			}
			type = GRID_COMBO;
			return true;
		}
		else if( p_path == "Leg scheme" && IsSchDef ){
			name = "Data";
			element = LScheme;
			type = GRID_STRING;
			return true;
		}
		else if( p_path == "MN input" ){
			name = "Data";
			element = MnNames;
			type = GRID_COMBO;
			return true;
		}
		else if( p_path == "Joint scheme" ){
			name = "Data";
			element = JScheme;
			if( JScheme != "One joint" ){
				element += "\tOne joint";
			}
			if( JScheme != "Two joint" ){
				element += "\tTwo joint";
			}
			type = GRID_COMBO;
			return true;
		}
		else if( p_path == "Upper link" ){
			name = "Data";
			make_linkcombo( ULink, element );
			type = GRID_COMBO;
			return true;
		}
		else if( p_path == "Middle link" ){
			name = "Data";
			make_linkcombo( MLink, element );
			type = GRID_COMBO;
			return true;
		}
		else if( p_path == "Lower link" ){
			name = "Data";
			make_linkcombo( LLink, element );
			type = GRID_COMBO;
			return true;
		}
		else if( p_path == "Name" ){
			name = "Data";
			element = Name;
			type = GRID_STRING;
			return true;
		}
	}
	return false;
}

bool t_muscle::upd_addpar( const char *path, const char *name, string &element, bool save )
{
	if( path && name ){
		string p_path = path;
		string p_name = name;
		if( p_path == "Name" && p_name == "Data" ){
			if( save ){
				Name = element;
			}
			else{
				element = Name;
			}
			return true;
		}
		else if( p_path == "Leg scheme" && p_name == "Data" ){
			if( save ){
				LScheme = element;
			}
			else{
				element = LScheme;
			}
			return true;
		}
		else if( p_path == "MN input" && p_name == "Data" ){
			if( save ){
				Mn = element;
				front( MnNames, Mn.c_str() );
			}
			else{
				element = Mn;
				front( MnNames, Mn.c_str() );
			}
			return true;
		}
		else if( p_path == "Joint scheme" && p_name == "Data" ){
			if( save ){
				JScheme = element;
			}
			else{
				element = JScheme;
			}
			return true;
		}
		else if( p_path == "Upper link" && p_name == "Data" ){
			if( save ){
				ULink = element;
			}
			else{
				element = ULink;
			}
			return true;
		}
		else if( p_path == "Middle link" && p_name == "Data" ){
			if( save ){
				MLink = element;
			}
			else{
				element = MLink;
			}
			return true;
		}
		else if( p_path == "Lower link" && p_name == "Data" ){
			if( save ){
				LLink = element;
			}
			else{
				element = LLink;
			}
			return true;
		}
	}
	return false;
}

bool t_muscle::load_addpar( string str, istream &file )
{
	if( str == "MnName"){
		file >> str >> ws;
		getline( file, Mn );
		front( MnNames, Mn.c_str() );
		return true;
	}
	else if( str == "LScheme"){
		file >> str >> ws;
		getline( file, LScheme );
		return true;
	}
	else if( str == "JScheme"){
		file >> str >> ws;
		getline( file, JScheme );
		return true;
	}
	else if( str == "ULink"){
		file >> str >> ws;
		getline( file, ULink );
		return true;
	}
	else if( str == "MLink"){
		file >> str >> ws;
		getline( file, MLink );
		return true;
	}
	else if( str == "LLink"){
		file >> str >> ws;
		getline( file, LLink );
		return true;
	}
	return false;
}

void t_muscle::save_addpar( ostream &file )
{
	file << "MnName = " << Mn << endl;
	file << "LScheme = " << LScheme << endl;
	file << "JScheme = " << JScheme << endl;
	file << "ULink = " << ULink << endl;
	file << "MLink = " << MLink << endl;
	file << "LLink = " << LLink << endl;
}

void t_muscle::load_dummy( void )
{
//	uni_template::load_dummy();
}

void t_muscle::set_default_data( void )
{
	LT0 = DD[UnitId].LT0;
	PT0 = DD[UnitId].PT0;
	Alpha = DD[UnitId].Alpha;
	LT0perL0 = DD[UnitId].LT0perL0;
	M = DD[UnitId].M;
	Shift = DD[UnitId].Shift;
	TKE = DD[UnitId].TKE;
	TKL = DD[UnitId].TKL;
	PKE = DD[UnitId].PKE;
	PKL = DD[UnitId].PKL;
	AV = DD[UnitId].AV;
	Omega = DD[UnitId].Omega;
	Fmax = DD[UnitId].Fmax;
	RH0 = DD[UnitId].RH0;
	Vmax = DD[UnitId].Vmax;
	Beta = DD[UnitId].Beta;
	Amax = DD[UnitId].Amax;
	F1 = DD[UnitId].F1;
	F2 = DD[UnitId].F2;
	Phi1 = DD[UnitId].Phi1;
	Phi2 = DD[UnitId].Phi2;
	A1 = DD[UnitId].A1;
	A2 = DD[UnitId].A2;
	R1 = DD[UnitId].R1;
	R2 = DD[UnitId].R2;
	Ladd = DD[UnitId].Ladd;
	Ltr = DD[UnitId].Ltr;
#ifdef __NEW_MUSCLE__
	TdL1 = DD[UnitId].TdL1;
	TF1 = DD[UnitId].TF1;
	TKlo = DD[UnitId].TKlo;
	TKsh = DD[UnitId].TKsh;
	CF0 = DD[UnitId].CF0;
	CFtr = DD[UnitId].CFtr;
	CVtr = DD[UnitId].CVtr;
#endif // __NEW_MUSCLE__
}

////////////////////////////////////////////////////////////////////////
// class t_ip
t_ip::t_ip( uni_template *parent, const char *lscheme )
	: t_muscle( parent, lscheme  )
{
	UnitId = __id_IP;
	Name = __MuscleNames[UnitId];
	JScheme = "One joint";
	ULink = "Pelvis";
	LLink = "Thigh";
	set_default_data();
}

void t_ip::load_dummy( void )
{
	t_muscle::load_dummy();
}

////////////////////////////////////////////////////////////////////////
// class t_bfa
t_bfa::t_bfa( uni_template *parent, const char *lscheme )
	: t_muscle( parent, lscheme  )
{
	UnitId = __id_BFA;
	Name = __MuscleNames[UnitId];
	JScheme = "One joint";
	ULink = "Pelvis";
	LLink = "Thigh";
	set_default_data();
}

void t_bfa::load_dummy( void )
{
	t_muscle::load_dummy();
}

////////////////////////////////////////////////////////////////////////
// class t_sart
t_sart::t_sart( uni_template *parent, const char *lscheme )
	: t_muscle( parent, lscheme  )
{
	UnitId = __id_SART;
	Name = __MuscleNames[UnitId];
	JScheme = "Two joint";
	ULink = "Pelvis";
	MLink = "Thigh";
	LLink = "Shank";
	set_default_data();
}

void t_sart::load_dummy( void )
{
	t_muscle::load_dummy();
}

////////////////////////////////////////////////////////////////////////
// class t_rf
t_rf::t_rf( uni_template *parent, const char *lscheme )
	: t_muscle( parent, lscheme  )
{
	UnitId = __id_RF;
	Name = __MuscleNames[UnitId];
	JScheme = "Two joint";
	ULink = "Pelvis";
	MLink = "Thigh";
	LLink = "Shank";
	set_default_data();
}

void t_rf::load_dummy( void )
{
	t_muscle::load_dummy();
}

////////////////////////////////////////////////////////////////////////
// class t_bfp
/*BFP*/	
t_bfp::t_bfp( uni_template *parent, const char *lscheme )
	: t_muscle( parent, lscheme  )
{
	UnitId = __id_BFP;
	Name = __MuscleNames[UnitId];
	JScheme = "Two joint";
	ULink = "Pelvis";
	MLink = "Thigh";
	LLink = "Shank";
	set_default_data();
}

void t_bfp::load_dummy( void )
{
	t_muscle::load_dummy();
}

////////////////////////////////////////////////////////////////////////
// class t_va
t_va::t_va( uni_template *parent, const char *lscheme )
	: t_muscle( parent, lscheme  )
{
	UnitId = __id_VA;
	Name = __MuscleNames[UnitId];
	JScheme = "One joint";
	ULink = "Thigh";
	MLink = "None";
	LLink = "Shank";
	set_default_data();
}

void t_va::load_dummy( void )
{
	t_muscle::load_dummy();
}

////////////////////////////////////////////////////////////////////////
// class t_ga
t_ga::t_ga( uni_template *parent, const char *lscheme )
	: t_muscle( parent, lscheme  )
{
	UnitId = __id_GA;
	Name = __MuscleNames[UnitId];
	JScheme = "Two joint";
	ULink = "Thigh";
	MLink = "Shank";
	LLink = "Foot";
	set_default_data();
}

void t_ga::load_dummy( void )
{
	t_muscle::load_dummy();
}

////////////////////////////////////////////////////////////////////////
// class t_ta
t_ta::t_ta( uni_template *parent, const char *lscheme )
	: t_muscle( parent, lscheme  )
{
	UnitId = __id_TA;
	Name = __MuscleNames[UnitId];
	JScheme = "One joint";
	ULink = "Shank";
	MLink = "None";
	LLink = "Foot";
	set_default_data();
}

void t_ta::load_dummy( void )
{
	t_muscle::load_dummy();
}

////////////////////////////////////////////////////////////////////////
// class t_sol
t_sol::t_sol( uni_template *parent, const char *lscheme )
	: t_muscle( parent, lscheme  )
{
	UnitId = __id_SOL;
	Name = __MuscleNames[UnitId];
	JScheme = "One joint";
	ULink = "Shank";
	MLink = "None";
	LLink = "Foot";
	set_default_data();
}

void t_sol::load_dummy( void )
{
	t_muscle::load_dummy();
}

#elif defined (__MECHANICS_3D__)
// TODO implementation 3d model
#endif //__MECHANICS_2D__
