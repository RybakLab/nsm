////////////////////////////////////////////////////////////////////////
// linktemplate.h
//---------------------------------------------------------------------
#include "precompile.h"

#ifdef __MECHANICS__

#include "linktemplate.h"
#include "walker.h"

const char *__LinkNames[__id_MAX_LINKS] = {
	"generic link",
	"Forepart",
	"Trunk",
	"Pelvis",
	"Thigh",
	"Shank",
	"Foot",
};

struct link_data{
	double CM;
	double L;
	double M;
	double I;
};

static link_data DD[__id_MAX_LINKS] = {
	{   0.0,   0.0,    0.0, 0.0 },					// generic link
	{   0.0,   0.0,    0.0, 0.0 },					// forepart
	{ 122.5, 245.0, 1448.8, 7249930.0 },			// trunk
	{  27.5,  54.9,  324.6,   81524.0 },			// pelvis
	{  43.1,  97.3,  149.5,  140063.0 },			// thigh
	{  43.4, 102.5,   63.5,   59201.0 },			// shank
	{  33.5,  68.9,   21.7,    9147.0 }				// foot
//  no digits
};

////////////////////////////////////////////////////////////////////////
// class t_link
t_link::t_link( uni_template *parent, int is_active )
	: uni_template( parent, is_active, ( char **)__LinkNames, __id_MAX_LINKS ), 
	IsRod( true )
{
	DList.insert( make_pair( "Center of mass\nCM", &CM ));
	DList.insert( make_pair( "Length\nL", &L ));
	DList.insert( make_pair( "Mass\nM", &M ));
	DList.insert( make_pair( "Inertia\nI", &I ));
	UnitId = __id_generic_L;
	Name = __LinkNames[UnitId];
	TypeName = "Link";
	TypeNameS = "Link";
}

t_link::t_link( const t_link &l )
	: uni_template( l ), IsRod( l.IsRod ), 
	L( l.L ), M( l.M ), CM( l.CM ), I( l.I )
{
	DList.insert( make_pair( "Length\nL", &L ));
	DList.insert( make_pair( "Mass\nM", &M ));
	DList.insert( make_pair( "Center of mass\nCM", &CM ));
	DList.insert( make_pair( "Inertia\nI", &I ));
}

t_link::~t_link( void )
{
}

t_link &t_link::operator = ( const t_link &l )
{
	uni_template::operator = ( l );
	IsRod = l.IsRod;
	L = l.L;
	M = l.M;
	CM = l.CM;
	I = l.I;
	return *this;
}

void t_link::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_link( *this );
	( *unit )->set_parent( parent );
}

int t_link::get_datattr( const char *path )
{
	string p_path = path;
	if( IsRod &&( p_path == "Center of mass" || p_path == "Inertia" )){
		return GRID_READONLY;
	}
	if( p_path == "Rod" ){
		return GRID_BOOL|GRID_PROCESS;
	}
	return uni_template::get_datattr( path );
}

bool t_link::get_addparlist( vector<string> &parlist )
{
	parlist.push_back( "Name" );
	parlist.push_back( "Rod" );
	return true;
}

bool t_link::get_addpar( const char *path, string &name, string &element, int &type )
{
	if( path ){
		string p_path = path;
		if( p_path == "Rod" ){
			name = "Data";
			element = ( IsRod? "true": "false" );
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

bool t_link::upd_addpar( const char *path, const char *name, string &element, bool save )
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
		else if( p_path == "Rod" && p_name == "Data" ){
			if( save ){
				IsRod = ( element == "true" )? true: false;
			}
			else{
				element = ( IsRod? "true": "false" );
			}
			return true;
		}
	}
	return false;
}

bool t_link::load_addpar( string str, istream &file )
{
	if( str == "IsRod"){
		file >> str >> str;
		IsRod = ( str == "true" )? true: false;
		return true;
	}
	return false;
}

void t_link::save_addpar( ostream &file )
{
	file << "IsRod = " << ( IsRod? "true": "false" ) << endl;
}

void t_link::load_dummy( void )
{
	set_default_data();
}

void t_link::set_default_data( void )
{
	CM = DD[UnitId].CM;
	I = DD[UnitId].I;
	L = DD[UnitId].L;
	M = DD[UnitId].M;
}

////////////////////////////////////////////////////////////////////////
// class t_forepart
t_forepart::t_forepart( uni_template *parent, int is_active )
	: t_link( parent, is_active ),
	IsFixedX( false ), IsFixedY( true )
{
	IsRod = false; L = 0.0; M = 843.4; CM = 0.0; I = 0.0;
	UnitId = __id_FOREPART;
	Name = __LinkNames[UnitId];
	set_default_data();
}

t_forepart::t_forepart( const t_forepart &l )
	: t_link( l ), IsFixedX( l.IsFixedX ), IsFixedY( l.IsFixedY )
{
}

t_forepart &t_forepart::operator = ( const t_forepart &l )
{
	t_link::operator = ( l );
	IsFixedX = l.IsFixedX;
	IsFixedY = l.IsFixedY;
	return *this;
}

void t_forepart::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_forepart( *this );
	( *unit )->set_parent( parent );
}

void t_forepart::copy_to( walker *w, int /*side*/, double *m, double *cm, double *i )
{
	w->IsTrXFixed = IsFixedX;
	w->IsTrYFixed = IsFixedY;
	w->L[HEAD] = double( L );
	m[HEAD] = double( M );
	cm[HEAD] = double( CM );
	i[HEAD] = double( I );
}

bool t_forepart::load_addpar( string str, istream &file )
{
	if( str == "IsFixedX"){
		file >> str >> str;
		IsFixedX = ( str == "true" )? true: false;
		return true;
	}
	else if( str == "IsFixedY"){
		file >> str >> str;
		IsFixedY = ( str == "true" )? true: false;
		return true;
	}
	return t_link::load_addpar( str, file );
}

void t_forepart::save_addpar( ostream &file )
{
	file << "IsFixedX = " << ( IsFixedX? "true": "false" ) << endl;
	file << "IsFixedY = " << ( IsFixedY? "true": "false" ) << endl;
	t_link::save_addpar( file );
}

void t_forepart::load_dummy( void )
{
	t_link::load_dummy();
	set_default_data();
}

////////////////////////////////////////////////////////////////////////
// class t_trunk
t_trunk::t_trunk( uni_template *parent, int is_active )
	: t_link( parent, is_active ), IsFixed( false )
{
	IsRod = false; L = 247.0; M = 1248.5; CM = 123.5; I = 6347641.0;
	UnitId = __id_TRUNK;
	Name = __LinkNames[UnitId];
	set_default_data();
}

t_trunk &t_trunk::operator = ( const t_trunk &l )
{
	t_link::operator = ( l );
	IsFixed = l.IsFixed;
	return *this;
}

void t_trunk::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_trunk( *this );
	( *unit )->set_parent( parent );
}

void t_trunk::copy_to( walker *w, int /*side*/, double *m, double *cm, double *i )
{
	w->IsTrQFixed = IsFixed;
	w->L[TRUNK] = double( L );
	w->Link[TRUNK_].Type = TRUNK; 
	w->Link[TRUNK_].In = A_R;
	m[TRUNK] = double( M );
	cm[TRUNK] = double( CM );
	i[TRUNK] = double( I );
}

bool t_trunk::load_addpar( string str, istream &file )
{
	if( str == "IsFixed"){
		file >> str >> str;
		IsFixed = ( str == "true" )? true: false;
		return true;
	}
	return t_link::load_addpar( str, file );
}

void t_trunk::save_addpar( ostream &file )
{
	file << "IsFixed = " << ( IsFixed? "true": "false" ) << endl;
	t_link::save_addpar( file );
}

void t_trunk::load_dummy( void )
{
	t_link::load_dummy();
	set_default_data();
}

////////////////////////////////////////////////////////////////////////
// class t_pelvis
t_pelvis::t_pelvis( uni_template *parent, int is_active )
	: t_link( parent, is_active ), IsFixed( false )
{
	IsRod = false; L = 56.0; M = 283.1; CM = 28.0; I = 73975.0;
	UnitId = __id_PELVIS;
	Name = __LinkNames[UnitId];
	set_default_data();
}

t_pelvis::t_pelvis( const t_pelvis &l )
	: t_link( l ), IsFixed( l.IsFixed )
{
}

t_pelvis &t_pelvis::operator = ( const t_pelvis &l )
{
	t_link::operator = ( l );
	IsFixed = l.IsFixed;
	return *this;
}

void t_pelvis::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_pelvis( *this );
	( *unit )->set_parent( parent );
}

void t_pelvis::copy_to( walker *w, int /*side*/, double *m, double *cm, double *i )
{
	w->L[PELVIS] = double( L );
	w->Link[PELVIS_].Type = PELVIS;
	w->Link[PELVIS_].In = A_P;
	w->IsPlvFixed = IsFixed;
	m[PELVIS] = double( M );
	cm[PELVIS] = double( CM );
	i[PELVIS] = double( I );
}

bool t_pelvis::load_addpar( string str, istream &file )
{
	if( str == "IsFixed"){
		file >> str >> str;
		IsFixed = ( str == "true" )? true: false;
		return true;
	}
	return t_link::load_addpar( str, file );
}

void t_pelvis::save_addpar( ostream &file )
{
	file << "IsFixed = " << ( IsFixed? "true": "false" ) << endl;
	t_link::save_addpar( file );
}

void t_pelvis::load_dummy( void )
{
	t_link::load_dummy();
	set_default_data();
}

////////////////////////////////////////////////////////////////////////
// class t_thigh
t_thigh::t_thigh( uni_template *parent, int is_active )
	: t_link( parent, is_active )
{
	IsRod = false; L = 97.0; M = 126.9; CM = 43.0; I = 107208.0;
	UnitId = __id_THIGH;
	Name = __LinkNames[UnitId];
	set_default_data();
}

void t_thigh::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_thigh( *this );
	( *unit )->set_parent( parent );
}

void t_thigh::copy_to( walker *w, int side, double *m, double *cm, double *i )
{
	w->L[THIGH] = double( L );
	switch( side ){
		case 0:	// left
			w->Link[THIGH_L].Type = THIGH;
			w->Link[THIGH_L].In = A_TL;
			break;
		case 1:	// right
			w->Link[THIGH_R].Type = THIGH;
			w->Link[THIGH_R].In = A_TR;
			break;
	}
	m[THIGH] = double( M );
	cm[THIGH] = double( CM );
	i[THIGH] = double( I );
}

void t_thigh::load_dummy( void )
{
	t_link::load_dummy();
	set_default_data();
}

////////////////////////////////////////////////////////////////////////
// class t_shank
t_shank::t_shank( uni_template *parent, int is_active )
	: t_link( parent, is_active )
{
	IsRod = false; L = 102.0; M = 60.5; CM = 43.1; I = 56273.0;
	UnitId = __id_SHANK;
	Name = __LinkNames[UnitId];
	set_default_data();
}

void t_shank::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_shank( *this );
	( *unit )->set_parent( parent );
}

void t_shank::copy_to( walker *w, int side, double *m, double *cm, double *i )
{
	w->L[SHANK] = double( L );
	switch( side ){
		case 0:	// left
			w->Link[SHANK_L].Type = SHANK;
			w->Link[SHANK_L].In = A_SL;
			break;
		case 1:	// right
			w->Link[SHANK_R].Type = SHANK;
			w->Link[SHANK_R].In = A_SR;
			break;
	}
	m[SHANK] = double( M );
	cm[SHANK] = double( CM );
	i[SHANK] = double( I );
}

void t_shank::load_dummy( void )
{
	t_link::load_dummy();
	set_default_data();
}

////////////////////////////////////////////////////////////////////////
// class t_foot
t_foot::t_foot( uni_template *parent, int is_active )
	: t_link( parent, is_active )
{
	IsRod = false; L = 76.5; M = 20.9; CM = 37.2; I = 10248.0;
	UnitId = __id_FOOT;
	Name = __LinkNames[UnitId];
	set_default_data();
}

void t_foot::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_foot( *this );
	( *unit )->set_parent( parent );
}

void t_foot::copy_to( walker *w, int side, double *m, double *cm, double *i )
{
	w->L[FOOT] = double( L );
	switch( side ){
		case 0:	// left
			w->Link[FOOT_L].Type = FOOT;
			w->Link[FOOT_L].In = A_FL;
			break;
		case 1:	// right
			w->Link[FOOT_R].Type = FOOT;
			w->Link[FOOT_R].In = A_FR;
			break;
	}
	m[FOOT] = double( M );
	cm[FOOT] = double( CM );
	i[FOOT] = double( I );
}

void t_foot::load_dummy( void )
{
	t_link::load_dummy();
	set_default_data();
}

#endif //__MECHANICS__
