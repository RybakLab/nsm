#include "precompile.h"

#ifdef __MECHANICS__

#include "biomtemplate.h"
#include "walker.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

static void new_link( int id, uni_template **unit, uni_template *parent )
{
	switch( id ){
		case __id_FOREPART:
			*unit = new t_forepart( parent );
			break;
		case __id_TRUNK:
			*unit = new t_trunk( parent );
			break;
		case __id_PELVIS:
			*unit = new t_pelvis( parent );
			break;
		case __id_THIGH:
			*unit = new t_thigh( parent );
			break;
		case __id_SHANK:
			*unit = new t_shank( parent );
			break;
		case __id_FOOT:
			*unit = new t_foot( parent );
			break;
		case __id_generic_L:
			*unit = new t_link( parent, GRID_COLLAPSE );
			break;
		default:;
	}
}

static void new_joint( int id, uni_template **unit, uni_template *parent )
{
	switch( id ){
		case __id_LOINS:
			*unit = new t_loins( parent );
			break;
		case __id_HIP:
			*unit = new t_hip( parent );
			break;
		case __id_KNEE:
			*unit = new t_knee( parent );
			break;
		case __id_ANKLE:
			*unit = new t_ankle( parent );
			break;
		case __id_generic_J:
			*unit = new t_link( parent, GRID_COLLAPSE );
			break;
		default:;
	}
}

static void new_muscle( int id, uni_template **unit, uni_template *parent, const char *lscheme = "None" )
{
	switch( id ){
		case __id_IP:
			*unit = new t_ip( parent, lscheme );
			break;
		case __id_BFA:
			*unit = new t_bfa( parent, lscheme );
			break;
		case __id_SART:
			*unit = new t_sart( parent, lscheme );
			break;
		case __id_RF:
			*unit = new t_rf( parent, lscheme );
			break;
		case __id_BFP:
			*unit = new t_bfp( parent, lscheme );
			break;
		case __id_VA:
			*unit = new t_va( parent, lscheme );
			break;
		case __id_GA:
			*unit = new t_ga( parent, lscheme );
			break;
		case __id_TA:
			*unit = new t_ta( parent, lscheme );
			break;
		case __id_SOL:
			*unit = new t_sol( parent, lscheme );
			break;
		case __id_generic_M:
			*unit = new t_muscle( parent, lscheme, GRID_COLLAPSE );
			break;
		default:;
	}
}

/////////////////////////////////////////////////////////////////////////////
// t_limb 
t_muscle t_limb::DefaultMuscle( NULL );
t_link t_limb::DefaultLink( NULL );
t_joint t_limb::DefaultJoint( NULL );

t_limb::t_limb( uni_template *parent, const char *name )
		: uni_template( parent, GRID_NONE )
{
	TypeNameS = TypeName = "Hindlimb";
	Name = name;
	IsShowType = true;
	UnitId = 0;
	DefaultChildren.push_back( &DefaultLink );
	DefaultChildren.push_back( &DefaultJoint );
	DefaultChildren.push_back( &DefaultMuscle );
	for( int i = __id_THIGH; i <= __id_FOOT; ++i ){
		add_child( "Link", i );
	}
	for( int i = __id_HIP; i <= __id_ANKLE; ++i ){
		add_child( "Joint", i );
	}
	for( int i = __id_IP; i < __id_MAX_MUSCLES; ++i ){
		add_child( "Muscle", i );
	}
}

t_limb::t_limb( const t_limb &limb )
		: uni_template( limb )
{
}

t_limb::~t_limb( void )
{
}

t_limb &t_limb::operator = ( const t_limb &limb )
{
	uni_template::operator = ( limb );
	return *this;
}

void t_limb::get_fblist( vector<string> &fb_list ) const
{
	for( size_t i = 0; i < Children.size(); ++i ){
		string ctype = Children[i]->get_type();
		if( ctype == "Muscle" ){
			zmuscle *m = NULL;
			(( t_muscle *)Children[i] )->copy_to( &m );
			fb_list.push_back( m->get_name() );
			delete m;
		} 
	}
}

void t_limb::get_muscleslist( vector<string> &m_list ) const
{
	for( size_t i = 0; i < Children.size(); ++i ){
		string ctype = Children[i]->get_type();
		if( ctype == "Muscle" ){
			zmuscle *m = NULL;
			(( t_muscle *)Children[i] )->copy_to( &m );
			m_list.push_back( m->get_name() );
			delete m;
		} 
	}
}

void t_limb::upd_outlist( vector<string> &out_list )
{
	for( size_t i = 0; i < Children.size(); ++i ){
		string ctype = Children[i]->get_type();
		if( ctype == "Muscle" ){
			(( t_muscle *)Children[i] )->upd_outlist( out_list );
		} 
	}
}

void t_limb::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_limb( *this );
	( *unit )->set_parent( parent );
}

void t_limb::copy_to( walker *w, double *m, double *cm, double *I )
{
	int side = -1;
	if( Name == "Left" ){
		side = 0;
	}
	else if( Name == "Right" ){
		side = 1;
	}
	if( side < 0 ) return;
	for( size_t k = 0; k < Children.size(); ++k ){
		string ctype = Children[k]->get_type();
		if( ctype == "Joint" ){
			(( t_joint *)Children[k] )->copy_to( w, side );
		}
		else if( ctype == "Link" ){
			(( t_link *)Children[k] )->copy_to( w, side, m, cm, I );
		} 
		else if( ctype == "Muscle" ){
			zmuscle *m = NULL;
			(( t_muscle *)Children[k] )->copy_to( &m );
			w->Muscles.push_back( m );
		} 
	}
}

bool t_limb::add_child( const char *type, const char *name )
{
	string stype = type;
	string sname = name;
	if( stype == "Link" ){
		for( int id = __id_FOREPART; id < __id_MAX_LINKS; ++id ){
			if( sname == __LinkNames[id] )
				return add_child( type, id );
		}
		return add_child( type, __id_generic_L, name );
	}
	else if( stype == "Joint" ){
		for( int id = __id_LOINS; id < __id_MAX_JOINTS; ++id ){
			if( sname == __JointNames[id] )
				return add_child( type, id );
		}
		return add_child( type, __id_generic_J, name );
	}
	else if( stype == "Muscle" ){
		for( int id = __id_IP; id < __id_MAX_MUSCLES; ++id ){
			if( sname == __MuscleNames[id] )
				return add_child( type, id );
		}
		return add_child( type, __id_generic_M, name );
	}
	return false;
}

bool t_limb::add_child( const char *type, int id, const char *name )
{
	string s_type = type;
	uni_template *unit = NULL;
	if( s_type == "Muscle" ){
		new_muscle( id, &unit, this, Name.c_str() );
	}
	else if( s_type == "Link" ){
		new_link( id, &unit, this );
	}
	else if( s_type == "Joint" ){
		new_joint( id, &unit, this );
		switch( id ){
			case __id_HIP:
				if( Name == "Left" ){
					(( t_joint *)unit )->A = 85.0;
				}
				if( Name == "Right" ){
					(( t_joint *)unit )->A = 130.0;
				}
			break;
			case __id_KNEE:
				if( Name == "Left" ){
					(( t_joint *)unit )->A = 150.0;
				}
				if( Name == "Right" ){
					(( t_joint *)unit )->A = 140.0;
				}
			break;
			case __id_ANKLE:
				if( Name == "Left" ){
					(( t_joint *)unit )->A = 140.0;
				}
				if( Name == "Right" ){
					(( t_joint *)unit )->A = 110.0;
				}
			break;
		}
	}
	if( unit ){
		if( id == 0 && name ){
			unit->set_name( name );
		}
		uni_template::add_child( unit );
		delete unit;
		return true;
	}
	return false;
}

bool t_limb::load_addpar( string str, istream &file )
{
	if( str == "<zMuscle" ){
		file >> ws;
		getline( file, str, '>') ;
		if( !get_child( "Muscle", str.c_str() )&& !add_child( "Muscle", str.c_str() ))
			return false;
		return get_child( "Muscle", str.c_str() )->load( file );
	}
	if( str == "<Link" ){
		file >> ws;
		getline( file, str, '>') ;
		if( !get_child( "Link", str.c_str() )&& !add_child( "Link", str.c_str() ))
			return false;
		return get_child( "Link", str.c_str() )->load( file );
	}
	if( str == "<Joint" ){
		file >> ws;
		getline( file, str, '>') ;
		if( !get_child( "Joint", str.c_str() )&& !add_child( "Joint", str.c_str() ))
			return false;
		return get_child( "Joint", str.c_str() )->load( file );
	}
	return false;
}

void t_limb::save_addpar( ostream &file )
{
	for( size_t i = 0; i < Children.size(); i++ ){
		Children[i]->save( file );
	}
}

/////////////////////////////////////////////////////////////////////////////
// t_body 
t_link t_body::DefaultLink( NULL );
t_joint t_body::DefaultJoint( NULL );

t_body::t_body( uni_template *parent, const char *name )
		: uni_template( parent, GRID_NONE )
{
	add_child( "Joint", __id_LOINS );
	for( int i = __id_FOREPART; i < __id_THIGH; ++i ){
		add_child( "Link", i );
	}
	Name = name;
	UnitId = 1;
	TypeName = "Body";
	TypeNameS = "Body";
	DefaultChildren.push_back( &DefaultLink );
	DefaultChildren.push_back( &DefaultJoint );
}

t_body::t_body( const t_body &body )
		: uni_template( body )
{
}

t_body::~t_body( void )
{
}

t_body &t_body::operator = ( const t_body &body )
{
	uni_template::operator = ( body );
	return *this;
}

void t_body::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_body( *this );
	( *unit )->set_parent( parent );
}

void t_body::copy_to( walker *w, double *m, double *cm, double *I )
{
	for( size_t k = 0; k < Children.size(); ++k ){
		string ctype = Children[k]->get_type();
		if( ctype == "Joint" ){	// loins only
			(( t_joint *)Children[k] )->copy_to( w, -1 );
		}
		else if( ctype == "Link" ){ //  forepart, trunk, pelvis
			(( t_link *)Children[k] )->copy_to( w, -1, m, cm, I );
		} 
	}
}

bool t_body::add_child( const char *type, int id, const char *name )
{
	string s_type = type;
	uni_template *unit = NULL;
	if( s_type == "Link" ){
		new_link( id, &unit, this );
	}
	else if( s_type == "Joint" ){
		new_joint( id, &unit, this );
	}
	if( unit ){
		if( id == 0 && name ){
			unit->set_name( name );
		}
		uni_template::add_child( unit );
		delete unit;
		return true;
	}
	return false;
}

bool t_body::add_child( const char *type, const char *name )
{
	string stype = type;
	string sname = name;
	if( stype == "Link" ){
		for( int id = __id_FOREPART; id < __id_MAX_LINKS; ++id ){
			if( sname == __LinkNames[id] )
				return add_child( type, id );
		}
		return add_child( type, __id_generic_L, name );
	}
	else if( stype == "Joint" ){
		for( int id = __id_LOINS; id < __id_MAX_JOINTS; ++id ){
			if( sname == __JointNames[id] )
				return add_child( type, id );
		}
		return add_child( type, __id_generic_J, name );
	}
	return false;
}

bool t_body::load_addpar( string str, istream &file )
{
	if( str == "<Link" ){
		file >> ws;
		getline( file, str, '>') ;
		if( !get_child( str.c_str() )&& !add_child( "Link", str.c_str() ))
			return false;
		return get_child( str.c_str() )->load( file );
	}
	if( str == "<Joint" ){
		file >> ws;
		getline( file, str, '>') ;
		if( !get_child( str.c_str() )&& !add_child( "Joint", str.c_str() ))
			return false;
		return get_child( str.c_str() )->load( file );
	}
	return false;
}

void t_body::save_addpar( ostream &file )
{
	for( unsigned int i = 0; i < Children.size(); i++ ){
		Children[i]->save( file );
	}
}

/////////////////////////////////////////////////////////////////////////////
// t_biomech 
t_biomech::t_biomech( uni_template *parent, int is_active )
	: uni_template( parent, is_active ), IsFixed( false ), G( ::Gravity ), Vel( 0.0 ),
	MusBM( 0.02 ), MusBT( 0.02 ), JointVis( 0.2 ),
	TrunkK( 5000.0 ), TrunkB( 20000.0 ),
	PelvisK( 5000.0 ), PelvisB( 20000.0 ),
	ForepartK( 6.0 ), ForepartB( 35.0 ),	// ForepartK( 5000.0 ), ForepartB( 50000.0 ),
	LoinsK( 3000.0 ), LoinsB( 20000.0 ),	// LoinsK( 500000.0 ), LoinsB( 5000000.0 )
	JointK( 6000.0 ), JointB( 100000.0 ),
	GroundK( 3.0 ), GroundB( 350.0 ),
	GroundLevel( 0.0 ), X( -560.0 ), Y( 240.0 ), A( 50.0 ),
	Xmin( -560.0 ), Ymin( 240.0 ),
	dX( 0.5 ), dY( 0.0 ), dA( 0.0 )
{
	TypeNameS = TypeName = "Biomechanics";
	Name = "Biomechanics";
	UnitId = 0;

	add_child( "Body", "Body" );
	add_child( "Hindlimb", "Left" );
	add_child( "Hindlimb", "Right" );

	DList.insert( make_pair( "Common parameters\tTreadmil\tLevel (mm)\nGL", &GroundLevel ));
	DList.insert( make_pair( "Common parameters\tTreadmil\tVelocity (mm/msec)\nVel", &Vel ));
	DList.insert( make_pair( "Common parameters\tTreadmil\tGround reaction\tElasticity\nKG", &GroundK ));
	DList.insert( make_pair( "Common parameters\tTreadmil\tGround reaction\tViscosity\nBG", &GroundB ));
	DList.insert( make_pair( "Common parameters\tMechanics\tInitial conditions\tHip\tPosition (mm)\tX component\nX", &X ));
	DList.insert( make_pair( "Common parameters\tMechanics\tInitial conditions\tHip\tPosition (mm)\tY component\nY", &Y ));
	DList.insert( make_pair( "Common parameters\tMechanics\tInitial conditions\tHip\tVelocity (mm/msec)\tX component\ndX", &dX ));
	DList.insert( make_pair( "Common parameters\tMechanics\tInitial conditions\tHip\tVelocity (mm/msec)\tY component\ndY", &dY ));
	DList.insert( make_pair( "Common parameters\tMechanics\tInitial conditions\tPelvis\tAngle (deg)\nA", &A ));
	DList.insert( make_pair( "Common parameters\tMechanics\tInitial conditions\tPelvis\tVelocity (deg/msec)\ndA", &dA ));
	DList.insert( make_pair( "Common parameters\tMechanics\tJoint\tViscosity\nB", &JointVis ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tLinks\tForepart\tPosition (mm)\tX component for hip\nXmin", &Xmin ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tLinks\tForepart\tPosition (mm)\tY component for hip\nYmin", &Ymin ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tLinks\tForepart\tElasticity\nKCat", &ForepartK ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tLinks\tForepart\tViscosity\nBCat", &ForepartB ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tLinks\tTrunk\tViscosity\nBR", &TrunkB ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tLinks\tTrunk\tElasticity\nKR", &TrunkK ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tLinks\tPelvis\tViscosity\nBP", &PelvisB ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tLinks\tPelvis\tElasticity\nKP", &PelvisK ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tJoints\tLoins\tViscosity\nBL", &LoinsB ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tJoints\tLoins\tElasticity\nKL", &LoinsK ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tJoints\tAll others\tViscosity\nBA", &JointB ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tJoints\tAll others\tElasticity\nKA", &JointK ));
	DList.insert( make_pair( "Common parameters\tMuscles\tFiber viscosity\nBM", &MusBM ));
	DList.insert( make_pair( "Common parameters\tMuscles\tTendon viscosity\nBT", &MusBT ));
	DList.insert( make_pair( "Common parameters\tUniverse\tGravity (mm/msec^2)\nG", &G ));
}

t_biomech::t_biomech( const t_biomech &biomech )
	: uni_template( biomech ), IsFixed( biomech.IsFixed ), G( biomech.G ), Vel( biomech.Vel ),
	MusBM( biomech.MusBM ), MusBT( biomech.MusBT ),	JointVis( biomech.JointVis ),
	PelvisK( biomech.PelvisK ), PelvisB( biomech.PelvisB ),
	TrunkK( biomech.TrunkK ), TrunkB( biomech.TrunkB ),
	ForepartK( biomech.ForepartK ), ForepartB( biomech.ForepartB ),
	LoinsK( biomech.LoinsK ), LoinsB( biomech.LoinsB ),
	JointK( biomech.JointK ), JointB( biomech.JointB ),
	GroundK( biomech.GroundK ), GroundB( biomech.GroundB ),
	GroundLevel( biomech.GroundLevel ), 
	X( biomech.X ), Y( biomech.Y ), A( biomech.A ),
	Xmin( biomech.Xmin ), Ymin( biomech.Ymin ),
	dX( biomech.dX ), dY( biomech.dY ), dA( biomech.dA )
{
	DList.insert( make_pair( "Common parameters\tTreadmil\tLevel (mm)\nGL", &GroundLevel ));
	DList.insert( make_pair( "Common parameters\tTreadmil\tVelocity (mm/msec)\nVel", &Vel ));
	DList.insert( make_pair( "Common parameters\tTreadmil\tGround reaction\tElasticity\nKG", &GroundK ));
	DList.insert( make_pair( "Common parameters\tTreadmil\tGround reaction\tViscosity\nBG", &GroundB ));
	DList.insert( make_pair( "Common parameters\tMechanics\tInitial conditions\tHip\tPosition (mm)\tX component\nX", &X ));
	DList.insert( make_pair( "Common parameters\tMechanics\tInitial conditions\tHip\tPosition (mm)\tY component\nY", &Y ));
	DList.insert( make_pair( "Common parameters\tMechanics\tInitial conditions\tHip\tVelocity (mm/msec)\tX component\ndX", &dX ));
	DList.insert( make_pair( "Common parameters\tMechanics\tInitial conditions\tHip\tVelocity (mm/msec)\tY component\ndY", &dY ));
	DList.insert( make_pair( "Common parameters\tMechanics\tInitial conditions\tPelvis\tAngle (deg)\nA", &A ));
	DList.insert( make_pair( "Common parameters\tMechanics\tInitial conditions\tPelvis\tVelocity (deg/msec)\ndA", &dA ));
	DList.insert( make_pair( "Common parameters\tMechanics\tJoint\tViscosity\nB", &JointVis ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tLinks\tForepart\tPosition (mm)\tX component for hip\nXmin", &Xmin ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tLinks\tForepart\tPosition (mm)\tY component for hip\nYmin", &Ymin ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tLinks\tForepart\tElasticity\nKCat", &ForepartK ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tLinks\tForepart\tViscosity\nBCat", &ForepartB ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tLinks\tTrunk\tViscosity\nBR", &TrunkB ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tLinks\tTrunk\tElasticity\nKR", &TrunkK ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tLinks\tPelvis\tViscosity\nBP", &PelvisB ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tLinks\tPelvis\tElasticity\nKP", &PelvisK ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tJoints\tLoins\tViscosity\nBL", &LoinsB ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tJoints\tLoins\tElasticity\nKL", &LoinsK ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tJoints\tAll others\tViscosity\nBA", &JointB ));
	DList.insert( make_pair( "Common parameters\tMechanics\tRestrictions\tJoints\tAll others\tElasticity\nKA", &JointK ));
	DList.insert( make_pair( "Common parameters\tMuscles\tFiber viscosity\nBM", &MusBM ));
	DList.insert( make_pair( "Common parameters\tMuscles\tTendon viscosity\nBT", &MusBT ));
	DList.insert( make_pair( "Common parameters\tUniverse\tGravity (mm/msec^2)\nG", &G ));
}

t_biomech::~t_biomech( void )
{
}

void t_biomech::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_biomech( *this );
	( *unit )->set_parent( parent );
}

void t_biomech::copy_to( walker **w )
{
	*w = new walker();
	double *m = new double[NUM_TYPELINK];
	double *cm = new double[NUM_TYPELINK];
	double *I = new double[NUM_TYPELINK];
	for( size_t i = 0; i < Children.size(); ++i ){
		string ctype = Children[i]->get_type();
		if( ctype == "Body" ){
			(( t_body *)Children[i] )->copy_to( *w, m, cm, I );
		}
		else if( ctype == "Hindlimb" ){
			(( t_limb *)Children[i] )->copy_to( *w, m, cm, I );
		} 
	}
	( *w )->BM = double( MusBM ); ( *w )->BT = double( MusBT );

	( *w )->GV = double( Vel ); 
	( *w )->GL = double( GroundLevel ); 
	( *w )->Gravity = double( G );
	( *w )->BJ = double( JointVis );

	( *w )->JointsLim( double( JointK ), double( JointB ));
	( *w )->Forepart( double( ForepartK ), double( ForepartB ));
	( *w )->TrunkLim( double( TrunkK ), double( TrunkB ));
	( *w )->PelvisLim( double( PelvisK ), double( PelvisB ));
	( *w )->LoinsLim( double( LoinsK ), double( LoinsB ));
	( *w )->Ground( double( GroundK ), double( GroundB ));
	( *w )->Q[X_H] = double( X );
	( *w )->Q[Y_H] = double( Y );
	( *w )->Q[A_P] = deg2rad( double( A ));
	( *w )->P[X_H] = double( dX );
	( *w )->P[Y_H] = double( dY );
	( *w )->P[A_P] = deg2rad( double( dA ));
	( *w )->init( m, cm, I, double( Xmin ), double( Ymin ));
	if( IsFixed ){
		( *w )->IsTrXFixed = true;
		( *w )->IsTrYFixed = true;
		( *w )->IsTrQFixed = true;
		( *w )->IsPlvFixed = true;
		( *w )->Forepart( 5000.0, 50000.0 );
		( *w )->TrunkLim( 50000.0, 500000.0 );
		( *w )->LoinsLim( 50000.0, 500000.0 );
		( *w )->PelvisLim( 50000.0, 500000.0 );
	}
	delete I;
	delete cm;
	delete m;
}

t_biomech &t_biomech::operator = ( const t_biomech &biomech )
{
	uni_template::operator = ( biomech );
	IsFixed = biomech.IsFixed; G = biomech.G; Vel = biomech.Vel; 
	MusBM = biomech.MusBM; MusBT = biomech.MusBT; JointVis = biomech.JointVis;
	ForepartK = biomech.ForepartK; ForepartB = biomech.ForepartB;
	TrunkK = biomech.TrunkK; TrunkB = biomech.TrunkB;
	PelvisK = biomech.PelvisK; PelvisB = biomech.PelvisB;
	LoinsK = biomech.LoinsK; LoinsB = biomech.LoinsB;
	JointK = biomech.JointK; JointB = biomech.JointB;
	GroundK = biomech.GroundK; GroundB = biomech.GroundB;
	GroundLevel = biomech.GroundLevel; 
	X = biomech.X; Y = biomech.Y; A = biomech.A;
	Xmin = biomech.Xmin; Ymin = biomech.Ymin;
	dX = biomech.dX; dY = biomech.dY; dA = biomech.dA;
	return *this;
}

hhn_pair<int> t_biomech::get_uid( const string &name ) const
{
	if( name == "Walker" ){
		return hhn_pair<int>( _id_Body, 0 );
	}
	for( size_t i = 0; i < NUM_VERTEX; i++ )
		if( name == string( get_vname( i ))){
			return hhn_pair<int>( _id_Vertex, i );
		}
	for( size_t i = 0; i < NUM_LINK; i++ )
		if( name == get_lname( i ) ){
			return hhn_pair<int>( _id_Link, i );
		}
	for( size_t i = 0; i < NUM_JOINT; i++ )
		if( name == get_jname( i ) ){
			return hhn_pair<int>( _id_Joint, i );
		}
	for( size_t i = 0; i < NUM_TOUCH; i++ )
		if( name == string( get_tname( i ))){
			return hhn_pair<int>( _id_Ground, i );
		}
	vector<string> m_list;
	get_muscleslist( m_list );
	for( size_t i = 0; i < m_list.size(); i++ )
		if( name == m_list[i] ){
			return hhn_pair<int>( _id_Muscle, i );
		}
	return hhn_pair<int>( -1, -1 );
}

void t_biomech::get_fblist( vector<string> &fb_list ) const
{
	fb_list.clear();
	for( size_t i = 0; i < NUM_TOUCH; ++i ){
		fb_list.push_back( get_tname( i ));
	}
	for( size_t i = 0; i < Children.size(); ++i ){
		string ctype = Children[i]->get_type();
		if( ctype == "Body" ){
			(( t_body *)Children[i] )->get_fblist( fb_list );
		}
		else if( ctype == "Hindlimb" ){
			(( t_limb *)Children[i] )->get_fblist( fb_list );
		} 
	}
}

void t_biomech::upd_outlist( vector<string> &out_list )
{
	for( size_t i = 0; i < Children.size(); ++i ){
		string ctype = Children[i]->get_type();
		if( ctype == "Hindlimb" ){
			(( t_limb *)Children[i] )->upd_outlist( out_list );
		} 
	}
}

void t_biomech::get_muscleslist( vector<string> &m_list ) const
{
	m_list.clear();
	for( size_t i = 0; i < Children.size(); ++i ){
		string ctype = Children[i]->get_type();
		if( ctype == "Hindlimb" ){
			(( t_limb *)Children[i] )->get_muscleslist( m_list );
		} 
	}
}

bool t_biomech::add_child( const char *type, const char *name )
{
	string s_type = type;
	uni_template *unit = NULL;
	if( s_type == "Body" ){
		unit = new t_body( this, name );
	}
	else if( s_type == "Hindlimb" ){
		unit = new t_limb( this, name );
	}
	if( unit ){
		uni_template::add_child( unit );
		delete unit;
		return true;
	}
	return false;
}

int t_biomech::get_datattr( const char *path )
{
	string p_path = path;
	if( p_path == "Common parameters\tMechanics\tRestrictions\tNail the beast" ){
		return GRID_BOOL|GRID_PROCESS;
	}
	return uni_template::get_datattr( path );
}

bool t_biomech::get_addparlist( vector<string> &parlist )
{
	parlist.push_back( "Common parameters\tMechanics\tRestrictions\tNail the beast" );
	return true;
}

bool t_biomech::get_addpar( const char *path, string &name, string &element, int &type )
{
	if( path ){
		string p_path = path;
		if( p_path == "Common parameters\tMechanics\tRestrictions\tNail the beast" ){
			name = "Data";
			element = ( IsFixed? "true": "false" );
			type = GRID_BOOL;
			return true;
		}
	}
	return false;
}

bool t_biomech::upd_addpar( const char *path, const char *name, string &element, bool save )
{
	if( path && name ){
		string p_path = path;
		string p_name = name;
		if( p_path == "Common parameters\tMechanics\tRestrictions\tNail the beast" && p_name == "Data" ){
			if( save ){
				IsFixed = ( element == "true" )? true: false;
			}
			else{
				element = ( IsFixed? "true": "false" );
			}
			return true;
		}
	}
	return false;
}

bool t_biomech::load_addpar( string str, istream &file )
{
	if( str == "<Body" ){
		file >> ws;
		getline( file, str, '>') ;
		if( !get_child( "Body", str.c_str() )&& !add_child( "Body", str.c_str() ))
			return false;
		return get_child( "Body", str.c_str() )->load( file );
	}
	else if( str == "<Hindlimb" ){
		file >> ws;
		getline( file, str, '>') ;
		if( !get_child( "Hindlimb", str.c_str() )&& !add_child( "Hindlimb", str.c_str() ))
			return false;
		return get_child( "Hindlimb", str.c_str() )->load( file );
	}
	else if( str == "IsFixed"){
		file >> str >> str;
		IsFixed = ( str == "true" )? true: false;
		return true;
	}
	return false;
}

void t_biomech::save_addpar( ostream &file )
{
	file << "IsFixed = " << ( IsFixed? "true": "false" ) << endl;
	for( unsigned int i = 0; i < Children.size(); i++ ){
		Children[i]->save( file );
	}
}

/*
void t_biomech::nail( void )
{
	if( IsFixed ){
		old_PelvisK = PelvisK;
		old_PelvisB = PelvisB;
		old_ForepartK = ForepartK;
		old_ForepartB = ForepartB;
		old_LoinsK = LoinsK;
		old_LoinsB = LoinsB;
	}
	else{
		PelvisK = old_PelvisK;
		PelvisB = old_PelvisB;
		ForepartK = old_ForepartK;
		ForepartB = old_ForepartB;
		LoinsK = old_LoinsK;
		LoinsB = old_LoinsB ),
	}
}
*/
#endif //__MECHANICS__
