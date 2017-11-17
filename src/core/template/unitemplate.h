/******************************************************************************
 *     file unitemplate.h                                                     *
 *----------------------------------------------------------------------------*
 * Sergey Markin, 2005 University of Manitoba                                 *
 ******************************************************************************/
#pragma warning( disable : 4129 )

#ifndef _UNIT_TEMPLATE_H_
#define _UNIT_TEMPLATE_H_

#include "hhnstat.h"
#include "sptypes.h"

class hhn_process;
///////////////////////////////////////////////////////////////////////////////
// class uni_template
//
// basic class for all templates of neuronal network elements such as 
// ionic channels, compartments, neurons, populations etc.
// it provides file operations (load/save) for each network element,
// implements basic operations like add/delete child unit, gets access to
// all parameters of the unit and their names.
class uni_template{
	protected:
		class _all_names{
			public:
				_all_names( char **names = NULL, unsigned int size = 0 )
				{
					Size = size;
					if( Size ){
						Names = new char *[Size];
						for( size_t i = 0; i < Size; ++i )
							Names[i] = names[i];
					}
					else{
						Names = NULL;
					}
				};
				_all_names( const _all_names &names )
				{
					Size = names.Size;
					if( Size ){
						Names = new char *[Size];
						for( size_t i = 0; i < Size; ++i )
						Names[i] = names.Names[i];
					}
					else{
						Names = NULL;
					}
				};
				virtual ~_all_names( void )
				{
					if( Names )
						delete[] Names;
				};
			public:
				_all_names &operator = ( const _all_names &names )
				{
					if( Names )
						delete Names;
					Size = names.Size;
					if( Size ){
						Names = new char *[Size];
						for( size_t i = 0; i < Size; ++i )
							Names[i] = names.Names[i];
					}
					else{
						Names = NULL;
					}
					return *this;
				};
			public:
				unsigned int Size;
				char **Names;
		};
	public:
		uni_template( uni_template *parent, int is_active, char **names = NULL, int size = 0, bool showtype = false );
		uni_template( const uni_template &unit );
virtual	~uni_template( void );
	public:
		uni_template &operator = ( const uni_template &unit );
	public:
		bool add_unit( const char *full_path );
		bool del_unit( const char *path, vector<string> &garbage_path, string &parent_path );
		bool reconfig( const char *path, vector<string> &add_paths, vector<string> &del_paths, string &parent_path );
		uni_template *get_unit( const char *full_path );
		uni_template *get_parent( void ){ return Parent; };
		const char *get_name( void ) const{	return Name.c_str(); };
		void set_name( const char *name ){ Name = name; };
		const char *get_type( void ) const{ return TypeName.c_str(); };
		const char *get_types( void ) const{ return TypeNameS.c_str(); };
		int uid( void ) const{ return UnitId; };
		void set_parent( uni_template *parent ){ Parent = parent; }
		void get_properties( const char *start_path, pair<string,int> &prop )
		{
			prop.first = start_path;
			prop.second = IsActive;
		}
		void get_allpar( const char *start_path, vector<pair<_Grid_Key,_Grid_Element> > &parlist );
		void get_childrenpath( const char *start_path, vector<pair<string,int> > &path );
		bool update( const char *full_path, const char *name, string &element, bool save );
		bool load( istream &file );
		void save( ostream &file );
virtual	size_t size( void ) const{ return Children.size(); };
virtual	bool get_unusednames( const char *full_path, vector<string> &names );
virtual	bool process( const char *path ){ return false; };
virtual	void copy_to( uni_template **unit, uni_template * parent ){};
virtual void copy_to( hhn_process **unit, hhn_process *parent ){};
virtual	t_data *find_data( const char *path );
	public: // to provide the backward compatibility with oldformat files
		bool load_data( string &str, istream &file ); 
		uni_template *get_child( const char *type, const char *name );
virtual	uni_template *get_template( const char *type, const char *name )
		{
			if( Parent ){
				return Parent->get_template( type, name );
			}
			return NULL;
		};
virtual	bool load_child( string &str, istream &file ){ return false; }
virtual	bool pre_add( void ){ return false; };
virtual	bool pre_del( void ){ return false; };
	protected:
virtual void collect_garbage( vector<string> &del_children ){};
virtual void reconfig( vector<string> &add_paths, vector<string> &del_paths ){};
virtual	bool add_child( const char *path ){	return false; };
virtual	int get_datattr( const char *path ){ return GRID_NONE; };
virtual	bool get_addparlist( vector<string> &parlist ){	return false; };
virtual	bool get_addpar( const char *path, string &name, string &element, int &type ){ return false; };
virtual	bool upd_addpar( const char *path, const char *name, string &element, bool save ){ return false; };
virtual	bool load_addpar( string str, istream &file ){	return false; };
virtual	void save_addpar( ostream &file ){};
virtual	void load_dummy( void ){};
		bool add_child( uni_template *unit );
		bool add_child( uni_template *unit, bool showtype );
		uni_template *get_child( const char *name );
		bool get_dlist( vector<string> &parlist );
		void clear( void );
		bool get_allunusednames( const char *full_path, vector<string> &names );
		void extract_unusednames( const char *full_path, vector<string> &names );
		int find_unit( const char *full_path, string &last_path );
		t_data *get_data( const char *path );
		t_data *get_data( const char *path, char delim );
		t_data *get_data( const char *path, const string &prefix );
	protected:
		map<string, t_data *> DList;
	public:
		bool IsShowType;
	protected:
		int IsActive;
		int UnitId;
		string Name;
		string TypeName;
		string TypeNameS;
		vector <string> TypeSynonym;
		uni_template *Parent;
		vector <uni_template *>Children;
		vector <uni_template *>DefaultChildren;
		_all_names PossibleNames;
};

#endif // _UNIT_TEMPLATE_H_
