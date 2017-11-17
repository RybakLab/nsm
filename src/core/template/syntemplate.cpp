#include "precompile.h"

#include "syntemplate.h"
#include "hhnsynapse.h"
#include "iontemplate.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

///////////////////////////////////////////////////////////////////////////////
// class base_synapseTemplate
t_synapse::t_synapse( uni_template *parent, const string &name, int is_active, bool showtype )
	: uni_template( parent, is_active, NULL, 0, showtype ), 
	Gmax( 0.0 ), Vs( 0.5 ), Ts( 25.0 ), Eds( "0.0" )
{
	if( name == _SynapseNames[_id_ExSyn] ){
		Gmax = 1.0;
		Eds = "-10.0";
	}
#if defined( __RESPIRATION__ )
	else if( name == _SynapseNames[_id_ExBSyn] ){
		Gmax = 1.0;
		Eds = "-10.0";
	}
#endif // defined( __RESPIRATION__ )
	else if( name == _SynapseNames[_id_InhASyn] ){
		Gmax = 1.0;
		Eds = "K ions";
	}
	else if( name == _SynapseNames[_id_InhBSyn] ){
		Gmax = 1.0;
		Eds = "Cl ions";
	}
	Name = name;
	TypeNameS = TypeName = "Synapse";
	TypeSynonym.push_back( "Synaps" );
}

t_synapse::t_synapse( const t_synapse &synapse )
	: uni_template( synapse ), Eds( synapse.Eds ),
	Gmax( synapse.Gmax ), Ts( synapse.Ts ), Vs( synapse.Vs )
{
}

t_synapse &t_synapse::operator = ( const t_synapse &synapse )
{
	uni_template::operator =( synapse );
	Gmax = synapse.Gmax;
	Ts = synapse.Ts;
	Vs = synapse.Vs;
	Eds = synapse.Eds;
	return *this;
}

void t_synapse::copy_to( uni_template **unit, uni_template * parent )
{
	*unit = new t_synapse( *this );
	( *unit )->set_parent( parent );
}

void t_synapse::copy_to( base_synapse **synapse )
{
	*synapse = new base_synapse();
	( *synapse )->Gmax = Gmax;
	( *synapse )->Ts = Ts;
	( *synapse )->Vs = Vs;
	( *synapse )->Eds = Eds;
}

bool t_synapse::get_addparlist( vector<string> &parlist )
{
	parlist.push_back( "Amplitude" );
	parlist.push_back( "Maximal conductance" );
	parlist.push_back( "Time constant" );
	parlist.push_back( "Reversal potential (mV)" );
	return true;
}

bool t_synapse::get_addpar( const char *path, string &name, string &element, int &type )
{
	if( path ){
		string p_path = path;
		if( p_path == "Reversal potential (mV)" ){
			name = "Data";
			element = Eds;
			int sz = 1;
			if( Eds != "K ions" ){
				element += "\tK ions";
				sz++;
			}
			if( Eds != "Cl ions" ){
				element += "\tCl ions";
				sz++;
			}
			if( sz < 3 ){
				element += "\t0.0";
			}
			type = GRID_COMBO;
			return true;
		}
		else if( p_path == "Amplitude" ){
			name = "Data";
			element << Vs;
			type = GRID_DOUBLE;
			return true;
		}
		else if( p_path == "Maximal conductance" ){
			name = "Data";
			element << Gmax;
			type = GRID_DOUBLE;
			return true;
		}
		else if( p_path == "Time constant" ){
			name = "Data";
			element << Ts;
			type = GRID_DOUBLE;
			return true;
		}
	}
	return false;
}

bool t_synapse::upd_addpar( const char *path, const char *name, string &element, bool save )
{
	if( path && name ){
		string p_path = path;
		string p_name = name;
		if( p_path == "Reversal potential (mV)" && p_name == "Data" ){
			if( save ){
				Eds = element;
			}
			else{
				element = Eds;
			}
			return true;
		}
		else if( p_path == "Amplitude" && p_name == "Data" ){
			if( save ){
				element >> Vs;
			}
			else{
				element << Vs;
			}
			return true;
		}
		else if( p_path == "Maximal conductance" && p_name == "Data" ){
			if( save ){
				element >> Gmax;
			}
			else{
				element << Gmax;
			}
			return true;
		}
		else if( p_path == "Time constant" && p_name == "Data" ){
			if( save ){
				element >> Ts;
			}
			else{
				element << Ts;
			}
			return true;
		}
	}
	return false;
}

int t_synapse::get_datattr( const char *path )
{
	string p_path = path;
	if( p_path == "Amplitude" || p_path == "Maximal conductance" ){
		return GRID_POSITIVE;
	}
	else if( p_path == "Time constant" ){
		return GRID_POSITIVE|GRID_EXCL_ZERO;
	}
	else if( string( path ) == "Reversal potential (mV)" ){
		return GRID_COMBO|GRID_PROCESS;
	}
	return uni_template::get_datattr( path );
}

bool t_synapse::load_addpar( string str, istream &file )
{
	if( str == "K"){
		file >> str;
		file >> Gmax;
		return true;
	}
	if( str == "Gmax"){
		file >> str;
		file >> Gmax;
		return true;
	}
	else if( str == "Eds"){
		file >> str >> ws;
		getline( file, Eds );
		return true;
	}
	else if( str == "TimeCostant"){
		file >> str;
		file >> Ts;
		return true;
	}
	else if( str == "Amplitude"){
		file >> str;
		file >> Vs;
		return true;
	}
	return false;
}

void t_synapse::save_addpar( ostream &file )
{
	file << "Gmax = " << Gmax << endl;
	file << "Eds = " << Eds << endl;
	file << "TimeCostant = " << Ts << endl;
	file << "Amplitude = " << Vs << endl;
}

///////////////////////////////////////////////////////////////////////////////
// class t_dsynapse
t_dsynapse::t_dsynapse( uni_template *parent, const string &name, int is_active, bool showtype )
	: uni_template( parent, is_active, NULL, 0, showtype )
{
	Kdep = 1.;
	Tdep = 500.;
	Name = name;
	TypeNameS = TypeName = "Depression";
}

t_dsynapse::t_dsynapse( const t_dsynapse &synapse )
	: uni_template( synapse )
{
	Kdep = synapse.Kdep;
	Tdep = synapse.Tdep;
}

t_dsynapse &t_dsynapse::operator = ( const t_dsynapse &synapse )
{
	uni_template::operator =( synapse );
	Kdep = synapse.Kdep;
	Tdep = synapse.Tdep;
	return *this;
}

void t_dsynapse::copy_to( uni_template **unit, uni_template * parent )
{
	*unit = new t_dsynapse( *this );
	( *unit )->set_parent( parent );
}

void t_dsynapse::copy_to( base_dsynapse **synapse )
{
	*synapse = new base_dsynapse();
	( *synapse )->Kdep = Kdep;
	( *synapse )->Tdep = Tdep;
}

bool t_dsynapse::get_addparlist( vector<string> &parlist )
{
	parlist.push_back( "Amplitude" );
	parlist.push_back( "Time constant" );
	return true;
}

bool t_dsynapse::get_addpar( const char *path, string &name, string &element, int &type )
{
	if( path ){
		string p_path = path;
		if( p_path == "Amplitude" ){
			name = "Data";
			element << Kdep;
			type = GRID_DOUBLE;
			return true;
		}
		else if( p_path == "Time constant" ){
			name = "Data";
			element << Tdep;
			type = GRID_DOUBLE;
			return true;
		}
	}
	return false;
}

bool t_dsynapse::upd_addpar( const char *path, const char *name, string &element, bool save )
{
	if( path && name ){
		string p_path = path;
		string p_name = name;
		if( p_path == "Amplitude" && p_name == "Data" ){
			if( save ){
				element >> Kdep;
			}
			else{
				element << Kdep;
			}
			return true;
		}
		else if( p_path == "Time constant" && p_name == "Data" ){
			if( save ){
				element >> Tdep;
			}
			else{
				element << Tdep;
			}
			return true;
		}
	}
	return false;
}

int t_dsynapse::get_datattr( const char *path )
{
	string p_path = path;
	if( p_path == "Amplitude" ){
		return GRID_POSITIVE;
	}
	else if( p_path == "Time constant" ){
		return GRID_POSITIVE|GRID_EXCL_ZERO;
	}
	return uni_template::get_datattr( path );
}

bool t_dsynapse::load_addpar( string str, istream &file )
{
	if( str == "Amplitude"){
		file >> str;
		file >> Kdep;
		return true;
	}
	else if( str == "TimeCostant"){
		file >> str;
		file >> Tdep;
		return true;
	}
	return false;
}

void t_dsynapse::save_addpar( ostream &file )
{
	file << "Amplitude = " << Kdep << endl;
	file << "TimeCostant = " << Tdep << endl;
}

///////////////////////////////////////////////////////////////////////////////
// class t_ssynapse
t_ssynapse::t_ssynapse( uni_template *parent, const string &name, int is_active, bool showtype )
	: uni_template( parent, is_active, NULL, 0, showtype )
{
	Hv = 0.;
	Slp = 1.;
	Name = name;
	TypeNameS = TypeName = "Sigma";
}

t_ssynapse::t_ssynapse( const t_ssynapse &synapse )
	: uni_template( synapse )
{
	Hv = synapse.Hv;
	Slp = synapse.Slp;
}

t_ssynapse &t_ssynapse::operator = ( const t_ssynapse &synapse )
{
	uni_template::operator =( synapse );
	Hv = synapse.Hv;
	Slp = synapse.Slp;
	return *this;
}

void t_ssynapse::copy_to( uni_template **unit, uni_template * parent )
{
	*unit = new t_ssynapse( *this );
	( *unit )->set_parent( parent );
}

void t_ssynapse::copy_to( base_ssynapse **synapse )
{
	*synapse = new base_ssynapse();
	( *synapse )->Hv = Hv;
	( *synapse )->Slp = Slp;
}

bool t_ssynapse::get_addparlist( vector<string> &parlist )
{
	parlist.push_back( "Half-voltage" );
	parlist.push_back( "Slope" );
	return true;
}

bool t_ssynapse::get_addpar( const char *path, string &name, string &element, int &type )
{
	if( path ){
		string p_path = path;
		if( p_path == "Half-voltage" ){
			name = "Data";
			element << Hv;
			type = GRID_DOUBLE;
			return true;
		}
		else if( p_path == "Slope" ){
			name = "Data";
			element << Slp;
			type = GRID_DOUBLE;
			return true;
		}
	}
	return false;
}

bool t_ssynapse::upd_addpar( const char *path, const char *name, string &element, bool save )
{
	if( path && name ){
		string p_path = path;
		string p_name = name;
		if( p_path == "Half-voltage" && p_name == "Data" ){
			if( save ){
				element >> Hv;
			}
			else{
				element << Hv;
			}
			return true;
		}
		else if( p_path == "Slope" && p_name == "Data" ){
			if( save ){
				element >> Slp;
			}
			else{
				element << Slp;
			}
			return true;
		}
	}
	return false;
}

int t_ssynapse::get_datattr( const char *path )
{
	string p_path = path;
	if( p_path == "Half-voltage" ){
		return GRID_NONE;
	}
	else if( p_path == "Slope" ){
		return GRID_EXCL_ZERO;
	}
	return uni_template::get_datattr( path );
}

bool t_ssynapse::load_addpar( string str, istream &file )
{
	if( str == "Hv"){
		file >> str;
		file >> Hv;
		return true;
	}
	else if( str == "Slp"){
		file >> str;
		file >> Slp;
		return true;
	}
	return false;
}

void t_ssynapse::save_addpar( ostream &file )
{
	file << "Hv = " << Hv << endl;
	file << "Slp = " << Slp << endl;
}

///////////////////////////////////////////////////////////////////////////////
// class t_network
static t_ions DefaultIon( NULL, GRID_NONE, true );
static t_pumps DefaultPump( NULL, GRID_NONE, true );
static t_synapse DefaultSyn( NULL, "", GRID_NONE, true );
static t_dsynapse DefaultDSyn( NULL, "", GRID_NONE, true );
static t_ssynapse DefaultSSyn( NULL, "", GRID_NONE, true );

t_network::t_network( uni_template *parent, int is_active )
	: uni_template( parent, is_active ), Threshold( -10 )
{
	TypeNameS = TypeName = "Settings";
	for( size_t i = 0; i < _id_MAX_ION; i++ ){
		add_ion( i );
	}
	for( size_t i = 0; i < _id_MAX_PUMPS; i++ ){
		add_pump( i );
	}
	for( size_t i = 0; i < _id_MAX_SYN; i++ ){
		add_syn( i );
		add_dep( i );
		add_sig( i );
	}
	DefaultChildren.push_back( &DefaultIon );
	DefaultChildren.push_back( &DefaultPump );
	DefaultChildren.push_back( &DefaultSyn );
	DefaultChildren.push_back( &DefaultDSyn );
	DefaultChildren.push_back( &DefaultSSyn );
}

t_network &t_network::operator = ( const t_network &netpar )
{
	Threshold = netpar.Threshold;
	uni_template::operator = ( netpar );
	return *this;
}

bool t_network::get_addparlist( vector<string> &parlist )
{
	parlist.push_back( "Spike threshold" );
	return true;
}

bool t_network::get_addpar( const char *path, string &name, string &element, int &type )
{
	if( path ){
		string p_path = path;
		if( p_path == "Spike threshold" ){
			name = "Data";
			element << Threshold;
			type = GRID_DOUBLE;
			return true;
		}
	}
return false;
}

bool t_network::upd_addpar( const char *path, const char *name, string &element, bool save )
{
	if( path && name ){
		string p_path = path;
		string p_name = name;
		if( p_path == "Spike threshold" && p_name == "Data" ){
			if( save ){
				element >> Threshold;
			}
			else{
				element << Threshold;
			}
			return true;
		}
	}
	return false;
}

void t_network::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_network( *this );
	( *unit )->set_parent( parent );
}

bool t_network::add_child( const char *type, const char *name )
{
	string name_ = name;
	string type_ = type;
	uni_template *unit = get_template( type, name );
	if( unit ){
		unit->IsShowType = false;
		return uni_template::add_child( unit );
	}
	if( type_ == "Ions" ){
		for( size_t i = 0; i < _id_MAX_ION; i++ )
			if( name_ == _IonsNames[i] ){
				return add_ion( i );
			}
	}
	else if( type_ == "Pumps" ){
		for( size_t i = 0; i < _id_MAX_PUMPS; i++ )
			if( name_ == _PumpsNames[i] ){
				return add_pump( i );
			}
	}
	else if( type_ == "Synapse" ){
		for( size_t i = 0; i < _id_MAX_SYN; i++ )
			if( name_ == _SynapseNames[i] ){
				return add_syn( i );
			}
		}
	else if( type_ == "Depression" ){
		for( size_t i = 0; i < _id_MAX_SYN; i++ )
			if( name_ == _SynapseNames[i] ){
				return add_dep( i );
			}
		}
	else if( type_ == "Sigma" ){
		for( size_t i = 0; i < _id_MAX_SYN; i++ )
			if( name_ == _SynapseNames[i] ){
				return add_sig( i );
			}
		}
	return false;
}

bool t_network::load_addpar( string str, istream &file )
{
	if( str == "Threshold"){
		file >> str;
		file >> Threshold;
		return true;
	}
	else if( str == "<Ions" ){
		file >> ws;
		getline( file, str, '>');
		if( !get_child( "Ions", str.c_str()) && !add_child( "Ions", str.c_str() )){
			return false;
		}
		return get_child( "Ions", str.c_str() )->load( file );
	}
	else if( str == "<Pumps" ){
		file >> ws;
		getline( file, str, '>');
		if( !get_child( "Pumps", str.c_str() ) && !add_child( "Pumps", str.c_str() )){
			return false;
		}
		return get_child( "Pumps", str.c_str() )->load( file );
	}
	else if( str == "<Synapse" ){
		file >> ws;
		getline( file, str, '>');
		if( !get_child( "Synapse", str.c_str() ) && !add_child( "Synapse", str.c_str() )){
			return false;
		}
		return get_child( "Synapse", str.c_str() )->load( file );
	}
	else if( str == "<Depression" ){
		file >> ws;
		getline( file, str, '>');
		if( !get_child( "Depression", str.c_str() ) && !add_child( "Depression", str.c_str() )){
			return false;
		}
		return get_child( "Depression", str.c_str() )->load( file );
	}
	else if( str == "<Sigma" ){
		file >> ws;
		getline( file, str, '>');
		if( !get_child( "Sigma", str.c_str() ) && !add_child( "Sigma", str.c_str() )){
			return false;
		}
		return get_child( "Sigma", str.c_str() )->load( file );
	}
	else{
		return false;
	}
}

void t_network::save_addpar( ostream &file )
{
	file << "Threshold = " << Threshold << endl;
	for( size_t i = 0; i < Children.size(); i++ ){
		Children[i]->save( file );
	}
}

bool t_network::add_ion( int id )
{
	t_ions *unit = NULL;
	switch( id ){
		case _id_Na_Ion:
			unit = new tna_ions( this, GRID_COLLAPSE, true );
		break;
		case _id_K_Ion:
			unit = new tk_ions( this, GRID_COLLAPSE, true );
		break;
		case _id_Ca_Ion:
			unit = new tca_ions( this, GRID_COLLAPSE, true );
		break;
		case _id_Cl_Ion:
			unit = new tcl_ions( this, GRID_COLLAPSE, true );
		break;
		default:;
	}
	if( unit ){
		uni_template::add_child( unit );
		delete unit;
		return true;
	}
	return false;
}

bool t_network::add_pump( int id )
{
	t_pumps *unit = NULL;
	switch( id ){
		case _id_NaK_Pump:
			unit = new tnak_pump( this, GRID_COLLAPSE, true );
		break;
		case _id_Ca_Pump:
			unit = new tca_pump( this, GRID_COLLAPSE, true );
		default:;
	}
	if( unit ){
		uni_template::add_child( unit );
		delete unit;
		return true;
	}
	return false;
}

bool t_network::add_syn( int id )
{
	if( id < _id_MAX_SYN ){
		t_synapse *syn = new t_synapse( this, _SynapseNames[id], GRID_COLLAPSE, true );
		uni_template::add_child( syn );
		delete syn;
		return true;
	}
	return false;
}

bool t_network::add_dep( int id )
{
	if( id < _id_MAX_SYN ){
		t_dsynapse *syn = new t_dsynapse( this, _SynapseNames[id], GRID_COLLAPSE, true );
		uni_template::add_child( syn );
		delete syn;
		return true;
	}
	return false;
}

bool t_network::add_sig( int id )
{
	if( id < _id_MAX_SYN ){
		t_ssynapse *syn = new t_ssynapse( this, _SynapseNames[id], GRID_COLLAPSE, true );
		uni_template::add_child( syn );
		delete syn;
		return true;
	}
	return false;
}

uni_template *t_network::get_template( const char *type, const char *name )
{
	return get_child( type, name );
}