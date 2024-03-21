////////////////////////////////////////////////////////////////////////
// jointemplate.h
#include "precompile.h"

#if defined (__MECHANICS_2D__)

#include "jointemplate.h"
#include "walker.h"

const char *__JointNames[__id_MAX_JOINTS] = {
	"generic joint",
	"Loins",
	"Hip",
	"Knee",
	"Ankle",
};

////////////////////////////////////////////////////////////////////////
// class t_joint
t_joint::t_joint( uni_template *parent, int is_active )
	: uni_template( parent, is_active, ( char **)__JointNames, __id_MAX_JOINTS ), 
	IsFixed( false ), A( 90 ), V( 0 ), MinA( 0 ), MaxA( 180 )
{
	TypeName = TypeNameS = "Joint";
	// IsShowType = true;
	DList.insert( make_pair( "Angle\nA", &A ));
	DList.insert( make_pair( "Velocity\nV", &V ));
	DList.insert( make_pair( "Min angle\nMinA", &MinA ));
	DList.insert( make_pair( "Max angle\nMaxA", &MaxA ));
	UnitId = __id_generic_J;
	Name = __JointNames[UnitId];
}

t_joint::t_joint( const t_joint &j )
	: uni_template( j ),
	IsFixed( j.IsFixed ), A( j.A ), V( j.V ), MinA( j.MinA ), MaxA( j.MaxA )
{
	DList.insert( make_pair( "Angle\nA", &A ));
	DList.insert( make_pair( "Velosity\nV", &V ));
	DList.insert( make_pair( "Min angle\nMinA", &MinA ));
	DList.insert( make_pair( "Max angle\nMaxA", &MaxA ));
}

t_joint::~t_joint( void )
{
}

t_joint &t_joint::operator = ( const t_joint &j )
{
	uni_template::operator = ( j );
	IsFixed = j.IsFixed;
	A = j.A;
	V = j.V;
	MinA = j.MinA;
	MaxA = j.MaxA;
	return *this;
}

void t_joint::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_joint( *this );
	( *unit )->set_parent( parent );
}

int t_joint::get_datattr( const char *path )
{
 	string p_path = path;
	if( IsFixed &&( p_path == "Min angle" || p_path == "Max angle" )){
		return GRID_READONLY;
	}
	if( p_path == "Fixed joint" ){
		return GRID_BOOL|GRID_PROCESS;
	}
	return uni_template::get_datattr( path );
}

bool t_joint::get_addparlist( vector<string> &parlist )
{
	parlist.push_back( "Name" );
	parlist.push_back( "Fixed joint" );
	return true;
}

bool t_joint::get_addpar( const char *path, string &name, string &element, int &type )
{
	if( path ){
		string p_path = path;
		if( p_path == "Fixed joint" ){
			name = "Data";
			element = ( IsFixed? "true": "false" );
			type = GRID_BOOL;
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

bool t_joint::upd_addpar( const char *path, const char *name, string &element, bool save )
{
	if( path && name ){
		string p_path = path;
		string p_name = name;
		if( p_path == "Name" && p_name == "Data" ){
			if( save )
				Name = element;
			else
				element = Name;
			return true;
		}
		else if( p_path == "Fixed joint" && p_name == "Data" ){
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

bool t_joint::load_addpar( string str, istream &file )
{
	if( str == "IsFixed"){
		file >> str >> str;
		IsFixed = ( str == "true" )? true: false;
		return true;
	}
	return false;
}

void t_joint::save_addpar( ostream &file )
{
	file << "IsFixed = " << ( IsFixed? "true": "false" ) << endl;
}

////////////////////////////////////////////////////////////////////////
// class t_loins
t_loins::t_loins( uni_template *parent, int is_active )
	: t_joint( parent, is_active )
{
	IsFixed = true; A = 127.3; V = 0.0;
	MinA = MaxA = 127.3;
	UnitId = __id_LOINS;
	Name = __JointNames[UnitId];
}

void t_loins::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_loins( *this );
	( *unit )->set_parent( parent );
}

void t_loins::copy_to( walker *w, int /*side*/ )
{
	w->Joint[A_L].A = deg2rad( double( A ));
	w->Joint[A_L].dA = deg2rad( double( V ));
	if( IsFixed ){
		w->RGJ[A_L].Min = w->RGJ[A_L].Max = w->Joint[A_L].A;
	}
	else{
		w->RGJ[A_L].Min = deg2rad( double( MinA ));
		w->RGJ[A_L].Max = deg2rad( double( MaxA ));
	}
	w->RGJ[A_L].UppSegment = A_R;
	w->RGJ[A_L].LowSegment = A_P;
}

////////////////////////////////////////////////////////////////////////
// class t_hip
t_hip::t_hip( uni_template *parent, int is_active )
	: t_joint( parent, is_active )
{
	IsFixed = false; A = 100.0; V = 0.0; MinA = 50.0; MaxA = 180.0;
	UnitId = __id_HIP;
	Name = __JointNames[UnitId];
}

void t_hip::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_hip( *this );
	( *unit )->set_parent( parent );
}

void t_hip::copy_to( walker *w, int side )
{
	switch( side ){
		case 0:	// left
			w->Joint[A_HL].A = deg2rad( double( A ));
			w->Joint[A_HL].dA = deg2rad( double( V ));
			if( IsFixed ){
				w->RGJ[A_HL].Min = w->RGJ[A_HL].Max = w->Joint[A_HL].A;
			}
			else{
				w->RGJ[A_HL].Min = deg2rad( double( MinA ));
				w->RGJ[A_HL].Max = deg2rad( double( MaxA ));
			}
			w->RGJ[A_HL].UppSegment = A_P;
			w->RGJ[A_HL].LowSegment = A_TL;
			break;
		case 1:	// right
			w->Joint[A_HR].A = deg2rad( double( A ));
			w->Joint[A_HR].dA = deg2rad( double( V ));
			if( IsFixed ){
				w->RGJ[A_HR].Min = w->RGJ[A_HR].Max = w->Joint[A_HR].A;
			}
			else{
				w->RGJ[A_HR].Min = deg2rad( double( MinA ));
				w->RGJ[A_HR].Max = deg2rad( double( MaxA ));
			}
			w->RGJ[A_HR].UppSegment = A_P;
			w->RGJ[A_HR].LowSegment = A_TR;
			break;
	}
}

////////////////////////////////////////////////////////////////////////
// class t_knee
t_knee::t_knee( uni_template *parent, int is_active )
	: t_joint( parent, is_active )
{
	IsFixed = false; A = 140.0; V = 0.0; MinA = 45.0; MaxA = 175.0;
	UnitId = __id_KNEE;
	Name = __JointNames[UnitId];
}

void t_knee::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_knee( *this );
	( *unit )->set_parent( parent );
}

void t_knee::copy_to( walker *w, int side )
{
	switch( side ){
		case 0:	// left
			w->Joint[A_KL].A = 2*PI-deg2rad( double( A ));
			w->Joint[A_KL].dA = deg2rad( double( V ));
			if( IsFixed ){
				w->RGJ[A_KL].Min = w->RGJ[A_KL].Max = w->Joint[A_KL].A;
			}
			else{
				w->RGJ[A_KL].Min = 2*PI-deg2rad( double( MaxA ));
				w->RGJ[A_KL].Max = 2*PI-deg2rad( double( MinA ));
			}
			w->RGJ[A_KL].UppSegment = A_TL;
			w->RGJ[A_KL].LowSegment = A_SL;
			break;
		case 1:	// right
			w->Joint[A_KR].A = 2*PI-deg2rad( double( A ));
			w->Joint[A_KR].dA = deg2rad( double( V ));
			if( IsFixed ){
				w->RGJ[A_KR].Min = w->RGJ[A_KR].Max = w->Joint[A_KR].A;
			}
			else{
				w->RGJ[A_KR].Min = 2*PI-deg2rad( double( MaxA ));
				w->RGJ[A_KR].Max = 2*PI-deg2rad( double( MinA ));
			}
			w->RGJ[A_KR].UppSegment = A_TR;
			w->RGJ[A_KR].LowSegment = A_SR;
			break;
	}
}

////////////////////////////////////////////////////////////////////////
// class t_ankle
t_ankle::t_ankle( uni_template *parent, int is_active )
	: t_joint( parent, is_active )
{
	IsFixed = false; A = 120.0; V = 0.0; MinA = 45.0; MaxA = 160.0;
	UnitId = __id_ANKLE;
	Name = __JointNames[UnitId];
}

void t_ankle::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_ankle( *this );
	( *unit )->set_parent( parent );
}

void t_ankle::copy_to( walker *w, int side )
{
	switch( side ){
		case 0:	// left
			w->Joint[A_AL].A = deg2rad( double( A ));
			w->Joint[A_AL].dA = deg2rad( double( V ));
			if( IsFixed ){
				w->RGJ[A_AL].Min = w->RGJ[A_AL].Max = w->Joint[A_AL].A;
			}
			else{
				w->RGJ[A_AL].Min = deg2rad( double( MinA ));
				w->RGJ[A_AL].Max = deg2rad( double( MaxA ));
			}
			w->RGJ[A_AL].UppSegment = A_SL;
			w->RGJ[A_AL].LowSegment = A_FL;
			break;
		case 1:	// right
			w->Joint[A_AR].A = deg2rad( double( A ));
			w->Joint[A_AR].dA = deg2rad( double( V ));
			if( IsFixed ){
				w->RGJ[A_AR].Min = w->RGJ[A_AR].Max = w->Joint[A_AR].A;
			}
			else{
				w->RGJ[A_AR].Min = deg2rad( double( MinA ));
				w->RGJ[A_AR].Max = deg2rad( double( MaxA ));
			}
			w->RGJ[A_AR].UppSegment = A_SR;
			w->RGJ[A_AR].LowSegment = A_FR;
			break;
	}
}

#elif defined (__MECHANICS_3D__)
// TODO implementation 3d model
#endif //__MECHANICS_2D__
