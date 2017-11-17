////////////////////////////////////////////////////////////////////////
// linktemplate.h
//---------------------------------------------------------------------
// default parameters
//---------------------------------------------------------------------
// Name		Trunk	Pelvis	Thigh	Shank	Foot	Digits	ForePart									
// C		122.5	27.5	43.1	43.4	33.5	15.3	0
// L		245		54.9	97.3	102.5	68.9	30.6	0
// M		1448.8	324.6	149.5	63.5	21.7	7.8		0
// I		7249930	81524	140063	59201	9147	612		0
//---------------------------------------------------------------------
#ifndef __LINK_TEMPLATE_H
#define __LINK_TEMPLATE_H

#include "sptypes.h"

#ifdef __MECHANICS__

#include "unitemplate.h"

enum __ID_Link{
	__id_generic_L	= 0,
	__id_FOREPART,
	__id_TRUNK,
	__id_PELVIS,
	__id_THIGH,
	__id_SHANK,
	__id_FOOT,
	__id_MAX_LINKS,
	};

extern const char *__LinkNames[__id_MAX_LINKS];

class walker;

////////////////////////////////////////////////////////////////////////
// class t_link
class t_link : public uni_template{
	public:
		t_link( uni_template *parent, int is_active = GRID_COLLAPSE );
		t_link( const t_link &l );
virtual	~t_link( void );
	public:
		t_link &operator = ( const t_link &l );
	public:
virtual	void copy_to( uni_template **unit, uni_template *parent );
virtual	void copy_to( walker *w, int side, double *m, double *cm, double *i ){};
	protected:
virtual	int get_datattr( const char *path );
virtual	bool get_addparlist( vector<string> &parlist );
virtual	bool get_addpar( const char *path, string &name, string &element, int &type );
virtual	bool upd_addpar( const char *path, const char *name, string &element, bool save );
		//--- Load/save parameters 
virtual	bool load_addpar( string str, istream &file );
virtual	void save_addpar( ostream &file );
virtual	void load_dummy( void );
	protected:
		void set_default_data( void );
	protected:
		bool IsRod;
		t_data CM;
		t_data L;
		t_data M;
		t_data I;
};

////////////////////////////////////////////////////////////////////////
// class t_forepart
class t_forepart : public t_link{
	public:
		t_forepart( uni_template *parent, int is_active = GRID_COLLAPSE );
		t_forepart( const t_forepart &l );
		~t_forepart( void ) {};
	public:
		t_forepart &operator = ( const t_forepart &l );
	public:
		void copy_to( uni_template **unit, uni_template *parent );
		void copy_to( walker *w, int /*side*/, double *m, double *cm, double *i );
	protected:
		int get_datattr( const char *path )
		{
			string p_path = path;
			if( p_path == "Name" || p_path == "Rod" ){
				return GRID_READONLY;
			}
			if( p_path == "Fixed link X" ){
				return GRID_BOOL|GRID_PROCESS;
			}
			if( p_path == "Fixed link Y" ){
				return GRID_BOOL|GRID_PROCESS;
			}
			return t_link::get_datattr( path );
		};
		bool get_addparlist( vector<string> &parlist )
		{
			if( t_link::get_addparlist( parlist )){
				parlist.push_back( "Fixed link X" );
				parlist.push_back( "Fixed link Y" );
				return true;
				}
			return false;
		};
		bool get_addpar( const char *path, string &name, string &element, int &type )
		{
			if( path ){
				string p_path = path;
				if( p_path == "Rod" ){
					name = "Data";
					element = ( IsRod? "true": "false" );
					type = GRID_STRING;
					return true;
				}
				else if( p_path == "Fixed link X" ){
					name = "Data";
					element = ( IsFixedX? "true": "false" );
					type = GRID_BOOL;
					return true;
				}
				else if( p_path == "Fixed link Y" ){
					name = "Data";
					element = ( IsFixedY? "true": "false" );
					type = GRID_BOOL;
					return true;
				}
			}
			return t_link::get_addpar( path, name, element, type );
		};
		bool upd_addpar( const char *path, const char *name, string &element, bool save )
		{
			if( path && name ){
				string p_path = path;
				string p_name = name;
				if( p_path == "Fixed link X" && p_name == "Data" ){
					if( save ){
						IsFixedX = ( element == "true" )? true: false;
					}
					else{
						element = ( IsFixedX? "true": "false" );
					}
					return true;
				}
				else if( p_path == "Fixed link Y" && p_name == "Data" ){
					if( save ){
						IsFixedY = ( element == "true" )? true: false;
					}
					else{
						element = ( IsFixedY? "true": "false" );
					}
					return true;
				}
			}
			return t_link::upd_addpar( path, name, element, save );
		}
		//--- Load/save parameters 
		bool load_addpar( string str, istream &file );
		void save_addpar( ostream &file );
		void load_dummy( void );
	public:
		bool IsFixedX;
		bool IsFixedY;
};

////////////////////////////////////////////////////////////////////////
// class t_trunk
class t_trunk : public t_link{
	public:
		t_trunk( uni_template *parent, int is_active = GRID_COLLAPSE );
		t_trunk( const t_trunk &l ) : t_link( l ), IsFixed( l.IsFixed ){};
		~t_trunk( void ){};
	public:
		t_trunk &operator = ( const t_trunk &l );
	public:
		void copy_to( uni_template **unit, uni_template *parent );
		void copy_to( walker *w, int /*side*/, double *m, double *cm, double *i );
	protected:
		int get_datattr( const char *path )
		{
			string p_path = path;
			if( p_path == "Name" || p_path == "Rod" ){
				return GRID_READONLY;
			}
			if( p_path == "Fixed link" ){
				return GRID_BOOL|GRID_PROCESS;
			}
			return t_link::get_datattr( path );
		};
		bool get_addparlist( vector<string> &parlist )
		{
			if( t_link::get_addparlist( parlist )){
				parlist.push_back( "Fixed link" );
				return true;
				}
			return false;
		};
		bool get_addpar( const char *path, string &name, string &element, int &type )
		{
			if( path ){
				string p_path = path;
				if( p_path == "Rod" ){
					name = "Data";
					element = ( IsRod? "true": "false" );
					type = GRID_STRING;
					return true;
				}
				else if( p_path == "Fixed link" ){
					name = "Data";
					element = ( IsFixed? "true": "false" );
					type = GRID_BOOL;
					return true;
				}
			}
			return t_link::get_addpar( path, name, element, type );
		};
		bool upd_addpar( const char *path, const char *name, string &element, bool save )
		{
			if( path && name ){
				string p_path = path;
				string p_name = name;
				if( p_path == "Fixed link" && p_name == "Data" ){
					if( save ){
						IsFixed = ( element == "true" )? true: false;
					}
					else{
						element = ( IsFixed? "true": "false" );
					}
					return true;
				}
			}
			return t_link::upd_addpar( path, name, element, save );
		};
		//--- Load/save parameters 
		bool load_addpar( string str, istream &file );
		void save_addpar( ostream &file );
		void load_dummy( void );
	public:
		bool IsFixed;
};

////////////////////////////////////////////////////////////////////////
// class t_pelvis
class t_pelvis : public t_link{
	public:
		t_pelvis( uni_template *parent, int is_active = GRID_COLLAPSE );
		t_pelvis( const t_pelvis &l );
		~t_pelvis( void ){};
	public:
		t_pelvis &operator = ( const t_pelvis &l );
	public:
		void copy_to( uni_template **unit, uni_template *parent );
		void copy_to( walker *w, int /*side*/, double *m, double *cm, double *i );
	protected:
		int get_datattr( const char *path )
		{
			string p_path = path;
			if( p_path == "Name" || p_path == "Rod" ){
				return GRID_READONLY;
			}
			if( p_path == "Fixed link" ){
				return GRID_BOOL|GRID_PROCESS;
			}
			return t_link::get_datattr( path );
		};
		bool get_addparlist( vector<string> &parlist )
		{
			if( t_link::get_addparlist( parlist )){
				parlist.push_back( "Fixed link" );
				return true;
				}
			return false;
		};
		bool get_addpar( const char *path, string &name, string &element, int &type )
		{
			if( path ){
				string p_path = path;
				if( p_path == "Rod" ){
					name = "Data";
					element = ( IsRod? "true": "false" );
					type = GRID_STRING;
					return true;
				}
				else if( p_path == "Fixed link" ){
					name = "Data";
					element = ( IsFixed? "true": "false" );
					type = GRID_BOOL;
					return true;
				}
			}
			return t_link::get_addpar( path, name, element, type );
		};
		bool upd_addpar( const char *path, const char *name, string &element, bool save )
		{
			if( path && name ){
				string p_path = path;
				string p_name = name;
				if( p_path == "Fixed link" && p_name == "Data" ){
					if( save ){
						IsFixed = ( element == "true" )? true: false;
					}
					else{
						element = ( IsFixed? "true": "false" );
					}
					return true;
				}
			}
			return t_link::upd_addpar( path, name, element, save );
		}
		//--- Load/save parameters 
		bool load_addpar( string str, istream &file );
		void save_addpar( ostream &file );
		void load_dummy( void );
	public:
		bool IsFixed;
};

////////////////////////////////////////////////////////////////////////
// class t_thigh
class t_thigh : public t_link{
	public:
		t_thigh( uni_template *parent, int is_active = GRID_COLLAPSE );
		t_thigh( const t_thigh &l ) : t_link( l ){};
		~t_thigh( void ){};
	public:
		void copy_to( uni_template **unit, uni_template *parent );
		void copy_to( walker *w, int side, double *m, double *cm, double *i );
	protected:
		int get_datattr( const char *path )
		{
			string p_path = path;
			if( p_path == "Name" || p_path == "Rod" ){
				return GRID_READONLY;
			}
			return t_link::get_datattr( path );
		};
		bool get_addpar( const char *path, string &name, string &element, int &type )
		{
			if( path ){
				string p_path = path;
				if( p_path == "Rod" ){
					name = "Data";
					element = ( IsRod? "true": "false" );
					type = GRID_STRING;
					return true;
				}
			}
			return t_link::get_addpar( path, name, element, type );
		};
		void load_dummy( void );
};

////////////////////////////////////////////////////////////////////////
// class t_shank
class t_shank : public t_link{
	public:
		t_shank( uni_template *parent, int is_active = GRID_COLLAPSE );
		t_shank( const t_shank &l ) : t_link( l ){};
		~t_shank( void ){};
	public:
		void copy_to( uni_template **unit, uni_template *parent );
		void copy_to( walker *w, int side, double *m, double *cm, double *i );
	protected:
		int get_datattr( const char *path )
		{
			string p_path = path;
			if( p_path == "Name" || p_path == "Rod" ){
				return GRID_READONLY;
			}
			return t_link::get_datattr( path );
		};
		bool get_addpar( const char *path, string &name, string &element, int &type )
		{
			if( path ){
				string p_path = path;
				if( p_path == "Rod" ){
					name = "Data";
					element = ( IsRod? "true": "false" );
					type = GRID_STRING;
					return true;
				}
			}
			return t_link::get_addpar( path, name, element, type );
		};
		void load_dummy( void );
};

////////////////////////////////////////////////////////////////////////
// class t_foot
class t_foot : public t_link{
	public:
		t_foot( uni_template *parent, int is_active = GRID_COLLAPSE );
		t_foot( const t_foot &l ) : t_link( l ){};
		~t_foot( void ){};
	public:
		void copy_to( uni_template **unit, uni_template *parent );
		void copy_to( walker *w, int side, double *m, double *cm, double *i );
	protected:
		int get_datattr( const char *path )
		{
			string p_path = path;
			if( p_path == "Name" || p_path == "Rod" ){
				return GRID_READONLY;
			}
			return t_link::get_datattr( path );
		};
		bool get_addpar( const char *path, string &name, string &element, int &type )
		{
			if( path ){
				string p_path = path;
				if( p_path == "Rod" ){
					name = "Data";
					element = ( IsRod? "true": "false" );
					type = GRID_STRING;
					return true;
				}
			}
			return t_link::get_addpar( path, name, element, type );
		};
		void load_dummy( void );
};

#endif //__MECHANICS__
#endif //__LINK_TEMPLATE_H
