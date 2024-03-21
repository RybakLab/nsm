#include "precompile.h"

#include "unitemplate.h"
#include "utilities.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

///////////////////////////////////////////////////////////////////////////////
// class uni_template
uni_template::uni_template( uni_template *parent, int is_active, char **names, int size, bool showtype )
	: IsShowType( showtype ), IsActive( is_active ), UnitId( -1 ), Parent( parent ), PossibleNames( names, size )
	
{
//	set_default();
}

uni_template::uni_template( const uni_template &unit )
	: Parent( unit.Parent ),
	Name( unit.Name ), 
	TypeName( unit.TypeName ),
	TypeNameS( unit.TypeNameS ),
	TypeSynonym( unit.TypeSynonym ),
	DefaultChildren( unit.DefaultChildren ),
	PossibleNames( unit.PossibleNames )
{
	UnitId = unit.UnitId;
	IsActive = unit.IsActive;
	IsShowType = unit.IsShowType;
	for( unsigned i = 0; i < unit.Children.size(); i++ ){
		uni_template *child = NULL;
		unit.Children[i]->copy_to( &child, this );
		Children.push_back( child );
	}
}

uni_template::~uni_template( void )
{
	clear();
}

uni_template &uni_template::operator = ( const uni_template &unit )
{
	UnitId = unit.UnitId;
	IsActive = unit.IsActive;
	IsShowType = unit.IsShowType;
	Name = unit.Name;
	Parent = unit.Parent;
	TypeName = unit.TypeName;
	TypeNameS = unit.TypeNameS;
	TypeSynonym = unit.TypeSynonym;
	PossibleNames = unit.PossibleNames;
	clear();
	for( unsigned i = 0; i < unit.Children.size(); i++ ){
		uni_template *child = NULL;
		unit.Children[i]->copy_to( &child, this );
		Children.push_back( child );
	}
	return *this;
}

bool uni_template::add_unit( const char *full_path )
{
	string last_path;
	int uid = find_unit( full_path, last_path );
	if( uid >= 0 ){
		if( !last_path.empty()){
			return Children[uid]->add_unit( last_path.c_str() );
		}
	}
	else if( !last_path.empty() && !DefaultChildren.empty() ){
		for( size_t k = 0; k < DefaultChildren.size(); ++k ){
			uni_template *child = DefaultChildren[k];
			for( size_t i = 0; i < child->PossibleNames.Size; ++i ){
				if( child->PossibleNames.Names[i] && last_path == child->PossibleNames.Names[i] ){
					return add_child( last_path.c_str());
				}
			}
		}
	}
	return false;
}

bool uni_template::del_unit( const char *path, vector<string> &del_paths, string &parent_path )
{
	string last_path;
	int uid = find_unit( path, last_path );
	if( uid >= 0 ){
		if( !last_path.empty()){
			if( !Name.empty() ){
				parent_path += Name;
				parent_path += '\t';
			}
			parent_path += Children[uid]->get_type();
			parent_path += '\t';
			return Children[uid]->del_unit( last_path.c_str(), del_paths, parent_path );
		}
		else{
			if( !Name.empty() ){
				parent_path += Name;
				parent_path += '\t';
			}
			Children[uid]->pre_del();
			if( Children[uid] )
				delete Children[uid];
			Children[uid] = NULL;
			Children.erase( Children.begin()+uid );
			vector<string> add_paths;
			reconfig( add_paths, del_paths );
			return true;
		}
	}
	return false;
}

bool uni_template::reconfig( const char *path, vector<string> &add_paths, vector<string> &del_paths, string &parent_path )
{
	string last_path;
	int uid = find_unit( path, last_path );
	if( uid >= 0 ){
		if( !last_path.empty()){
			if( !Name.empty() ){
				parent_path += Name;
				parent_path += '\t';
			}
			parent_path += Children[uid]->get_type();
			parent_path += '\t';
			return Children[uid]->reconfig( last_path.c_str(), add_paths, del_paths, parent_path );
		}
		else{
			if( !Name.empty() ){
				parent_path += Name;
				parent_path += '\t';
			}
			reconfig( add_paths, del_paths );
			return true;
		}
	}
	return false;
}

t_data *uni_template::find_data( const char *path )
{
	t_data *par = get_data( path );
	if( !par ){
		par = get_data( path, Name );
		if( !par ){
			par = get_data( path, TypeName );
			if( !par && Parent){
				par = Parent->find_data( path );
			}
		}
	}
	return par;
}

uni_template *uni_template::get_unit( const char *full_path )
{
	string last_path;
	int uid = find_unit( full_path, last_path );
	if( uid >= 0 ){	// try to find unit among children 
		if( !last_path.empty()){
			return Children[uid]->get_unit( last_path.c_str() );
		}
		else{
			return Children[uid];
		}
	}
	else{
	}
	return NULL;
}

t_data *uni_template::get_data( const char *path )
{
	typedef map<string,t_data *>::iterator it;
	for( it pos = DList.begin(); pos != DList.end(); ++pos ){
		if( pos->first.find( string( path )) != string::npos )
			return pos->second;
	} 
	return NULL;
}

t_data *uni_template::get_data( const char *path, char delim )
{
	typedef map<string,t_data *>::iterator it;
	typedef string::size_type str_size;
	vector<str_size> list_pos;
	for( it pos = DList.begin(); pos != DList.end(); ++pos ){
		list_pos.clear();
		str_size str_pos = 0;
		do{
			str_pos = pos->first.find( delim, str_pos+1 );
			list_pos.push_back( str_pos );
		} while( str_pos != string::npos );
		for( size_t i = 0; i < list_pos.size(); ++i ){
			if( list_pos[i] != string::npos ){
				size_t count = ( list_pos[i+1] != string::npos )? list_pos[i+1]-list_pos[i]-1: string::npos;
				string name = pos->first.substr( list_pos[i]+1, count );
				if( pos->first.substr( list_pos[i]+1, count ) == path ){
					return pos->second;
				}
			}
		}
	} 
	return NULL;
}

t_data *uni_template::get_data( const char *path, const string &prefix )
{
	if( prefix.empty()){
		return get_data( path );
	}
	else{
		string p_path = path;
		string::size_type pos = p_path.find( prefix );
		if( pos != string::npos ){
			p_path = p_path.substr( pos+prefix.size());
			return get_data( p_path.c_str() );
		}
	}
	return NULL;
}

void uni_template::get_childrenpath( const char *start_path, vector<pair<string,int> > &path )
{
	string child_path = start_path;
	if( !DefaultChildren.empty() ){
		for( unsigned int k = 0; k < DefaultChildren.size(); ++k ){
			uni_template *child = DefaultChildren[k];
			string child_path = start_path;
			if( !child->TypeName.empty() ){
				child_path += "\t";
				child_path += child->TypeName;
				path.push_back( make_pair( child_path, child->IsActive ) );
			}
			for( unsigned int pos = 0; pos < Children.size(); ++pos ){
				string new_path = child_path;
				if( !Children[pos]->Name.empty()){
					if( child->TypeName == Children[pos]->TypeName){
						new_path += "\t";
						new_path += Children[pos]->Name;
						path.push_back( make_pair( new_path, Children[pos]->IsActive ) );
					}
				}
			}
		}
	}
	else{
		for( unsigned int pos = 0; pos < Children.size(); ++pos ){
			string new_path = child_path;
			if( !Children[pos]->Name.empty()){
				new_path = start_path;
				new_path += "\t";
				if( Children[pos]->IsShowType ){
					new_path += Children[pos]->TypeName;
					new_path += "\t";
				} 
				new_path += Children[pos]->Name;
				path.push_back( make_pair( new_path, Children[pos]->IsActive ) );
			}
		}
	}
}

void uni_template::get_allpar( const char *start_path, vector<pair<_Grid_Key,_Grid_Element> > &parlist )
{
	vector<string> par_paths;
	get_dlist( par_paths );
	for( unsigned int pos = 0; pos < par_paths.size(); ++pos ){
		t_data *par = get_data( par_paths[pos].c_str());
		pair<_Grid_Key, _Grid_Element> elem;
		elem.first.Path = "";
		if( start_path ){
			elem.first.Path = start_path;
			elem.first.Path += "\t";
			elem.first.Path += par_paths[pos];
		}
		if( par ){
			elem.second.Type = GRID_DOUBLE|get_datattr( par_paths[pos].c_str() );
			elem.first.Name = "Data";
			elem.second.Data << par->Value;
			parlist.push_back( elem );
			elem.first.Name = "Var";
			elem.second.Type = GRID_DOUBLE|GRID_POSITIVE;
			elem.second.Data << par->Variance;
			parlist.push_back( elem );
		}
		else{
			int par_type;
			string par_name;
			string element;
			if( get_addpar( par_paths[pos].c_str(), par_name, element, par_type )){
				elem.second.Type = par_type|get_datattr( par_paths[pos].c_str() );
				elem.first.Name = par_name;
				elem.second.InitStr = element;
				if( elem.second.Type & GRID_COMBO ){
					elem.second.Data = element.substr( 0, element.find( "\t", 0 ));
				}
				else{
					elem.second.Data = element;
				}
				parlist.push_back( elem );
			}
		}
	}
}

bool uni_template::get_dlist( vector<string> &parlist )
{
	typedef map<string,t_data *>::iterator it;
	parlist.clear();
	for( it pos = DList.begin(); pos != DList.end(); ++pos ){
		parlist.push_back( pos->first.substr(0, pos->first.find( '\n' )));
	}
	get_addparlist( parlist );
	return true;
}

bool uni_template::add_child( uni_template *unit )
{
	for( unsigned int i = 0; i < Children.size(); i++ )
		if( Children[i]->Name == unit->Name && Children[i]->TypeName == unit->TypeName )
			return false;
	uni_template *new_child = NULL;
	unit->copy_to( &new_child, this );
	new_child->pre_add();
	Children.push_back( new_child );
	return true;
}

bool uni_template::add_child( uni_template *unit, bool showtype )
{
	for( unsigned int i = 0; i < Children.size(); i++ )
		if( Children[i]->Name == unit->Name && Children[i]->TypeName == unit->TypeName )
			return false;
	uni_template *new_child = NULL;
	unit->copy_to( &new_child, this );
	new_child->pre_add();
	new_child->IsShowType = showtype;
	Children.push_back( new_child );
	return true;
}

uni_template *uni_template::get_child( const char *name )
{
	for( unsigned int i = 0; i < Children.size(); ++i )
		if( Children[i]->Name == name )
			return Children[i];
	return NULL;
}

uni_template *uni_template::get_child( const char *type, const char *name )
{
	string type_ = type;
	string name_ = name;
	for( size_t pos = 0; pos < Children.size(); ++pos ){
		if( type_ == Children[pos]->get_type() && name_ == Children[pos]->get_name())
			return Children[pos];
	}
	return NULL;
}

void uni_template::clear( void )
{
	for( unsigned int i = 0; i < Children.size(); i++ ){
		if( Children[i] )
			delete Children[i];
	}
	Children.clear();
}

bool uni_template::get_unusednames( const char *full_path, vector<string> &names )
{
	uni_template *unit = get_unit( full_path );
	if( get_allunusednames( full_path, names )){
		extract_unusednames( full_path, names );
		return true;
	}
	return false;
}

void uni_template::extract_unusednames( const char *full_path, vector<string> &names )
{
	string last_path;
	for( size_t i = 0; i < Children.size(); ++i ){
		for( size_t ii = 0; ii < names.size(); ++ii ){
			if( names[ii] == Children[i]->Name ){
				names.erase( names.begin()+ii );
				ii = 0;
			}
		}
	}
	int uid = find_unit( full_path, last_path );
	if( uid >= 0  && !last_path.empty())
		Children[uid]->extract_unusednames( last_path.c_str(), names );
}

bool uni_template::get_allunusednames( const char *full_path, vector<string> &names )
{
	char Delim = '\t';
	string path = full_path;
	typedef string::size_type size_type;
	for( unsigned int k = 0; k < DefaultChildren.size(); ++k ){
		string sub_name = Name;
		uni_template *child = DefaultChildren[k];
		if( sub_name.empty() )
			sub_name = child->TypeName;
		for( size_type pos = 0, next_pos = 0; next_pos < string::npos; pos = next_pos+1 ){
			next_pos = path.find( Delim, pos );
			string sub_path = path.substr( pos, next_pos-pos );
			if( sub_path == TypeName && next_pos == string::npos ){
				for( size_t i = 0; i < PossibleNames.Size; ++i ){
					if( PossibleNames.Names[i] ){
						size_t ii = 0;
						for( ii = 0; ii < names.size(); ++ii ){
							if( names[ii] == PossibleNames.Names[i] )
								break; 
						}
						if( ii >= names.size() )
							names.push_back( PossibleNames.Names[i] );
					}
				}
				return true;
			}
			else if( sub_path == sub_name ){
				if( next_pos != string::npos )
					sub_path = path.substr( next_pos );
				return child->get_allunusednames( sub_path.c_str(), names );;
			}
		}
	}
	for( size_type pos = 0, next_pos = 0; next_pos < string::npos; pos = next_pos+1 ){
		next_pos = path.find( Delim, pos );
		string sub_path = path.substr( pos, next_pos-pos );
		if( sub_path == TypeName && next_pos == string::npos ){
			for( size_t i = 0; i < PossibleNames.Size; ++i ){
				if( PossibleNames.Names[i] ){
					size_t ii = 0;
					for( ii = 0; ii < names.size(); ++ii ){
						if( names[ii] == PossibleNames.Names[i] )
							break; 
					}
					if( ii >= names.size() )
						names.push_back( PossibleNames.Names[i] );
				}
			}
			return true;
		}
	}
	return false;
}

bool uni_template::update( const char *full_path, const char *name, string &element, bool save )
{
	string last_path;
	int uid = find_unit( full_path, last_path );
	if( uid >= 0 ){
		if( !last_path.empty()){
			return Children[uid]->update( last_path.c_str(), name, element, save );
		}
	}
	else{
		last_path = full_path;
		string::size_type pos = last_path.find( TypeName );
		if( pos != string::npos ){
			last_path = last_path.substr( pos );
			pos = last_path.find( '\t' );
			if( pos != string::npos ){
				last_path = last_path.substr( pos+1 );
			}
		}
		t_data *par = get_data( last_path.c_str() );
		if( par ){
			string p_name = name;
			if( p_name == "Data" ){
				save? element >> par->Value:element << par->Value;
				return true;
			}
			else if( p_name == "Var" ){
				save? element >> par->Variance:element << par->Variance;
				return true;
			}
			else{
				if( !save )
					element = "";
				return false;
			}
		}
		else{
			return upd_addpar( last_path.c_str(), name, element, save );
		}
	}
	return false;
}

int uni_template::find_unit( const char *full_path, string &last_path )
{
	char Delim = '\t';
	string path = full_path;
	typedef string::size_type size_type;
	for( size_type pos = 0, next_pos = 0; next_pos < string::npos; pos = next_pos+1 ){
		next_pos = path.find( Delim, pos );
		string sub_path = path.substr( pos, next_pos-pos );
		if( next_pos != string::npos ){
			pos = next_pos+1;
			next_pos = path.find( Delim, pos );
			string sub_path = path.substr( pos, next_pos-pos );
			last_path = sub_path;
			for( unsigned int i = 0; i < Children.size(); ++i ){
				if( Children[i]->Name.empty()){
					if( sub_path == Children[i]->TypeName ){
						last_path = ( next_pos != string::npos )? path.substr( next_pos+1 ):""; 
						return i; 
					}
				}
				else{
					if( Children[i]->IsShowType ){
						if( sub_path == Children[i]->TypeName && next_pos != string::npos ){
							last_path = path.substr( next_pos+1 );
							for( size_t j = 0; j < Children.size(); ++j ){
								if( !Children[j]->IsShowType || sub_path != Children[j]->TypeName )
									continue;
								size_type pos = last_path.find( Delim );
								if( pos != string::npos ){
									string sub_path = last_path.substr( 0, pos );
									if( sub_path == Children[j]->Name ){
										string last_path1 = last_path.substr( pos+1 );
										swap( last_path, last_path1 );
										return j;
									}
								}
								else if( last_path == Children[j]->Name ){
									last_path = "";
									return j;
								}
							}
							return -1;
						}
					}
					else if( sub_path == Children[i]->Name ){
						last_path = ( next_pos != string::npos )? path.substr( next_pos+1 ):""; 
						return i; 
					}
				}
			}
		}
	}
	return -1;
}

bool uni_template::load( istream &file )
{
	string str;
	vector<string> endtag;
	str = "</";
	str += TypeNameS;
	str += ">";
	endtag.push_back( str );
	for( vector<string>::size_type pos = 0; pos < TypeSynonym.size(); ++pos ){
		str = "";
		str = "</";
		str += TypeSynonym[pos];
		str += ">";
		endtag.push_back( str );
	}
	clear();
	set_default();
	t_data *tmpVar;
	while( file >> str){
		if(( tmpVar = get_data( str.c_str(), '\n' )) != NULL ){
			file >> str >> ( *tmpVar );
		}
		else if( load_addpar( str, file )){
		}
		else{
			for( vector<string>::size_type pos = 0; pos < endtag.size(); ++pos )
				if( str == endtag[pos] ){
					load_dummy();
					return true;
				}
			unknown_string( file, str);
		}
	}
	string mess = "Missing tag ";
	mess += endtag[0];
	message( mess.c_str(), "Warning!" );
	return false;
}

bool uni_template::load_data( string &str, istream &file )
{
	t_data *tmpVar;
	if(( tmpVar = get_data( str.c_str(), '\n' )) != NULL ){
		file >> str >> ( *tmpVar );
		return true;
	}
	return false;   
}

void uni_template::save( ostream &file )
{
	typedef map<string,t_data *>::iterator it;
	string namePar;
	if( Name.empty() )
		file << endl << "<" << TypeNameS << ">" << endl;
	else
		file << endl << "<" << TypeNameS << " " << Name << ">" << endl;
	for( it pos = DList.begin(); pos != DList.end(); ++pos ){
		namePar = pos->first.substr( pos->first.find( '\n' )+1 );
		string::size_type str_pos = namePar.find( '\n' );
		if( str_pos != string::npos ){
			namePar = namePar.substr( 0, str_pos );
		}
		file << namePar << " = " << *get_data( namePar.c_str(), '\n' ) << endl;
	}
	save_addpar( file );
	file << "</" << TypeNameS << ">" << endl;
}

