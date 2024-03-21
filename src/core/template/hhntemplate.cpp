/*****************************************************************
 ****              (C)  1997 - 2000                           ****
 **           Model of Hodgkin-Haxley type neuron               **
 **       Development by Ilia Rybak and Sergey Markin           **
 ****                   Neuron template                       ****
 *****************************************************************/
#include "precompile.h"

#include "hhntemplate.h"
#include "syntemplate.h"
#include "hhn.h"
#include "hhnpopulat.h"
#include "iontemplate.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG
#endif //__LINUX__

static t_ions DefaultIon( NULL, GRID_NONE );
static t_pumps DefaultPump( NULL, GRID_NONE );
static t_channel DefaultChannel( NULL, GRID_ADD_ROW );
static t_compart DefaultCompartment( NULL, GRID_ADD_ROW );
static t_hhn DefaultNeuron( NULL, GRID_NONE );

///////////////////////////////////////////////////////////////////////////////
// class t_compart
t_compart::t_compart( uni_template *parent, int is_active )
	: uni_template( parent, is_active, ( char ** )_CompartmentNames, _id_MAX_COMP ),
	Vm( -60. ), Iinj( 0. ), Area( 1.0 )
{
#if defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	Area = 0.0025;
#endif // defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	DList.insert( make_pair( "Membrane potential (mV)\nV", &Vm ));
	DList.insert( make_pair( "Injected current (mA)\nIinj", &Iinj ));
	DList.insert( make_pair( "Membrane area (mm\xB2)\nS", &Area ));
	TypeNameS = TypeName = "Compartment";
	DefaultChildren.push_back( &DefaultIon );
	DefaultChildren.push_back( &DefaultPump );
	DefaultChildren.push_back( &DefaultChannel );
}

t_compart::t_compart( const t_compart &compartment )
	: uni_template( compartment ),
	Vm( compartment.Vm ), Iinj( compartment.Iinj ), Area( compartment.Area )
{
	DList.insert( make_pair( "Membrane potential (mV)\nV", &Vm ));
	DList.insert( make_pair( "Injected current (mA)\nIinj", &Iinj ));
	DList.insert( make_pair( "Membrane area (mm\xB2)\nS", &Area ));
}

t_compart::~t_compart( void )
{
}

t_compart &t_compart::operator = ( const t_compart &compartment )
{
	uni_template::operator = ( compartment );
	Vm = compartment.Vm;
	Iinj = compartment.Iinj;
	Area = compartment.Area;
	return *this;
}

//--- public functions
void t_compart::copy_ions( nsm_vector(hhn_ions *) &ions, hhn_compart *cmp )
{
	for( size_t i = 0; i < Children.size(); ++i ){
		string type_ = Children[i]->get_type();
		if( type_ == "Ions" ){
			hhn_process *ion_ = NULL;
			(( t_ions *)Children[i])->copy_to( &ion_, cmp );
			ions.push_back(( hhn_ions * )ion_ );
		}
	}
}

void t_compart::copy_pumps( nsm_vector(hhn_pump *) &pumps, hhn_compart *cmp )
{
	for( size_t i = 0; i < Children.size(); ++i ){
		string type_ = Children[i]->get_type();
		if( type_ == "Pumps" ){
			hhn_process *pump_ = NULL;
			(( t_pumps *)Children[i])->copy_to( &pump_, cmp );
			pumps.push_back(( hhn_pump * )pump_ );
		}
	}
}

void t_compart::copy_chan( nsm_vector(hhn_channel *) &channels, hhn_compart *cmp )
{
	for( size_t i = 0; i < Children.size(); ++i ){
		string type_ = Children[i]->get_type();
		if( type_ == "Channel" ){
			hhn_process *chan = NULL;
			(( t_channel *)Children[i] )->copy_to( &chan, cmp );
			if( chan ){
				channels.push_back(( hhn_channel * )chan );
			}
		}
	}
}

t_channel *t_compart::get_channel( size_t channelId, size_t &num )
{
	string type_;
	for( size_t i = num; i < Children.size(); i++ ){
		type_ = Children[i]->get_type();
		if( type_ == "Channel" && Children[i]->uid() == channelId ){
			num = i;
			return ( t_channel * )Children[i];
		}
	}
	num = Children.size();
	return NULL;
}

void t_compart::reconfig( vector<string> &add_paths, vector<string> &del_paths )
{
	size_t ion_types[_id_MAX_ION] = {0};
	size_t pump_types[_id_MAX_PUMPS] = {0};
	for( size_t i = 0; i < Children.size(); i++ ){
		string type_ = Children[i]->get_type();
		if( type_ == "Channel" ){
			if((( t_channel *)Children[i] )->get_iontype() < _id_MAX_ION ){
				ion_types[(( t_channel *)Children[i] )->get_iontype()] |= 0x001;
			}
			if((( t_channel *)Children[i] )->get_pumptype() < _id_MAX_PUMPS ){
				pump_types[(( t_channel *)Children[i] )->get_pumptype()] |= 0x001;
			}
		}
		else if( type_ == "Ions" ){
			if( Children[i]->uid() < _id_MAX_ION ){
				ion_types[Children[i]->uid()] |= 0x002;
			}
		}
		else if( type_ == "Pumps" ){
			if( Children[i]->uid() < _id_MAX_PUMPS ){
				pump_types[Children[i]->uid()] |= 0x002;
			}
		}
	}
	string sub_path;
	for( size_t i = 0; i < _id_MAX_PUMPS; ++i ){
		switch( pump_types[i] ){
			case 0x001: // add pump
				if( add_child( "Pumps", _PumpsNames[i] )){
					sub_path = "Pumps\t";
					sub_path += _PumpsNames[i];
					add_paths.push_back( sub_path );
				}
			break;
			case 0x002: // delete pump
				if( del_child( _PumpsNames[i], sub_path )){
					del_paths.push_back( sub_path );
				}
			break;
			case 0x003: // modify pump
			break;
		}
	}
	for( size_t i = 0; i < _id_MAX_ION; ++i ){
		switch( ion_types[i] ){
			case 0x001: // add ion
				if( add_child( "Ions", _IonsNames[i] )){
					sub_path = "Ions\t";
					sub_path += _IonsNames[i];
					add_paths.push_back( sub_path );
				}
			break;
			case 0x002: // delete ion
				if( del_child( _IonsNames[i], sub_path )){
					del_paths.push_back( sub_path );
				}
			break;
			case 0x003: // modify ion
			break;
		}
	}
}

bool t_compart::pre_del( void )
{
	bool success = true;
	for( size_t i = 0; i < Children.size(); ++i ){
		success &= Children[i]->pre_del();
	}
	return success;
}

bool t_compart::add_child( const char *name )
{
	string name_ = name;
	for( size_t i = 0; i < _id_MAX_CHAN; i++ ){
		if( _ChannelNames[i] && name_ == _ChannelNames[i] ){
			return add_child( "Channel", name );
		}
	}
	for( size_t i = 0; i < _id_MAX_ION; i++ ){
		if( _IonsNames[i] && name_ == _IonsNames[i] ){
			return add_child( "Ions", name );
		}
	}
	for( size_t i = 0; i < _id_MAX_PUMPS; i++ ){
		if( _PumpsNames[i] && name_ == _PumpsNames[i] ){
			return add_child( "Pumps", name );
		}
	}
	return false;
}

bool t_compart::add_child( const char *type, const char *name )
{
	string name_ = name;
	string type_ = type;
	uni_template *unit = get_template( type, name );
	if( unit ){
		unit->IsShowType = false;
		return uni_template::add_child( unit );
	}
	if( type_ == "Channel" ){
		for( size_t i = 0; i < _id_MAX_CHAN; i++ )
			if( _ChannelNames[i] && name_ == _ChannelNames[i] ){
				return add_chan( i, _ChannelNames[i] );
			}
	} else if( type_ == "gChannel" ){
		return add_chan( _id_generic_Chan, name );
	} else if( type_ == "Ions" ){
		for( size_t i = 0; i < _id_MAX_ION; i++ )
			if( _IonsNames[i] && name_ == _IonsNames[i] ){
				return add_ion( i );
			}
	} else if( type_ == "Pumps" ){
		for( size_t i = 0; i < _id_MAX_PUMPS; i++ )
			if( _PumpsNames[i] && name_ == _PumpsNames[i] ){
				return add_pump( i );
			}
	}
	return false;
}

bool t_compart::load_child( string &str, istream &file ) // backward compatibility
{
	if( str == "<Channel" ){
		file >> ws;
		getline( file, str, '>');
		int cid = getChannelID( str.c_str() );
		if( cid >= 0 ){
			string name = _ChannelNames[cid];
			if( !get_child( "Channel", name.c_str() ) && !add_child( "Channel", name.c_str() ))
				return false;
			return get_child( "Channel", name.c_str() )->load( file );
		}
	} else if( str == "<gChannel" ){
		file >> ws;
		getline( file, str, '>');
		if( !get_child( "Channel", str.c_str() ) && !add_child( "gChannel", str.c_str() ))
			return false;
		return get_child( "Channel", str.c_str() )->load( file );
	} else if( str == "<Ions" ){
		file >> ws;
		getline( file, str, '>');
		if( !get_child( "Ions", str.c_str() ) && !add_child( "Ions", str.c_str() ))
			return false;
		return get_child( "Ions", str.c_str() )->load( file );
	} else if( str == "<Pumps" ){
		file >> ws;
		getline( file, str, '>');
		if( !get_child( "Pumps", str.c_str() ) && !add_child( "Pumps", str.c_str() ))
			return false;
		return get_child( "Pumps", str.c_str() )->load( file );
	}
	return false;
}

bool t_compart::load_addpar( string str, istream &file )
{
	if( str == "<Channel" ){
		file >> ws;
		getline( file, str, '>');
		int cid = getChannelID( str.c_str() );
		if( cid >= 0 ){
			string name = _ChannelNames[cid];
			if( !get_child( "Channel", name.c_str() ) && !add_child( "Channel", name.c_str() ))
				return false;
			return get_child( "Channel", name.c_str() )->load( file );
		}
	} else if( str == "<gChannel" ){
		file >> ws;
		getline( file, str, '>');
		if( !get_child( "Channel", str.c_str() ) && !add_child( "gChannel", str.c_str() ))
			return false;
		return get_child( "Channel", str.c_str() )->load( file );
	} else if( str == "<Ions" ){
		file >> ws;
		getline( file, str, '>');
		if( !get_child( "Ions", str.c_str() ) && !add_child( "Ions", str.c_str() ))
			return false;
		return get_child( "Ions", str.c_str() )->load( file );
	} else if( str == "<Pumps" ){
		file >> ws;
		getline( file, str, '>');
		if( !get_child( "Pumps", str.c_str() ) && !add_child( "Pumps", str.c_str() ))
			return false;
		return get_child( "Pumps", str.c_str() )->load( file );
	}
	return false;
}

void t_compart::save_addpar( ostream &file )
{
	for( size_t i = 0; i < Children.size(); i++ ){
		Children[i]->save( file );
	}
}

//--- private functions
bool t_compart::add_pump( int pumpId )
{
	t_pumps *pump = NULL;
	switch( pumpId ){
		case _id_NaK_Pump:
			pump = new tnak_pump( this );
		break;
		case _id_Ca_Pump:
			pump = new tca_pump( this );
		break;
		default:;
	}
	if( pump ){
		uni_template::add_child( pump );
		delete pump;
		return true;
	}
	return false;
}

bool t_compart::add_ion( int ionId )
{
	t_ions *ion = NULL;
	switch( ionId ){
		case _id_Na_Ion:
			ion = new tna_ions( this );
		break;
		case _id_K_Ion:
			ion = new tk_ions( this );
		break;
		case _id_Ca_Ion:
			ion = new tca_ions( this );
		break;
		case _id_Cl_Ion:
			ion = new tcl_ions( this );
		break;
		default:;
	}
	if( ion ){
		uni_template::add_child( ion );
		delete ion;
		return true;
	}
	return false;
}

bool t_compart::add_chan( int channelId, const char *name )
{
	t_channel *unit = NULL;
	switch( channelId ){
		case _id_generic_Chan:
			unit = new tgen_channel( this );
		break;
		case _id_generic_NaPChan:
			unit = new tgnap_channel( this );
		break;
		case _id_generic_NaChan:
			unit = new tgna_channel( this );
		break;
		case _id_generic_KdrChan:
			unit = new tgkdr_channel( this );
		break;
		case _id_generic_KAChan:
			unit = new tgka_channel( this );
		break;
		case _id_generic_KCa:
			unit = new tgkca_channel( this );
		break;
		case _id_generic_Can:
			unit = new tgcan_channel( this );
		break;
		case _id_generic_HChan:
			unit = new tgh_channel( this );
		break;
#if defined( __LOCOMOTION__ ) && defined( __RESPIRATION__ )
		case _id_NaChan:
			unit = new tna_channel( this );
		break;
		case _id_KChan:
			unit = new tk_channel( this );
		break;
		case _id_NaFChan:
			unit = new tnaf_channel( this );
		break;
		case _id_NaPChan:
			unit = new tnap_channel( this );
		break;
		case _id_KdrChan:
			unit = new tkdr_channel( this );
		break;
		case _id_KAChan:
			unit = new tka_channel( this );
		break;
		case _id_CaLChan:
			unit = new tcal_channel( this );
		break;
		case _id_MChan:
			unit = new tm_channel( this );
		break;
		case _id_CaTChan:
			unit = new tcat_channel( this );
		break;
		case _id_CaNChan:
			unit = new tcan_channel( this );
		break;
		case _id_KCChan:
			unit = new tkc_channel( this );
		break;
		case _id_KCaChan:
			unit = new tkca_channel( this );
		break;
		case _id_HChan:
			unit = new th_channel( this );
		break;
		case _id_LeakChan:
			unit = new tleak_channel( this );
		break;
		case _id_resp_NaFChan:
			unit = new trnaf_channel( this );
		break;
		case _id_resp_NaPChan:
			unit = new trnap_channel( this );
		break;
		case _id_resp_KdrChan:
			unit = new trkdr_channel( this );
		break;
		case _id_resp_KAChan:
			unit = new trka_channel( this );
		break;
		case _id_resp_CaLChan:
			unit = new trcal_channel( this );
		break;
		case _id_resp_CaTChan:
			unit = new trcat_channel( this );
		break;
		case _id_resp_KCaChan:
			unit = new trkca_channel( this );
		break;
		case _id_resp_LeakChan:
			unit = new trleak_channel( this );
		break;
#endif // defined( __LOCOMOTION__ ) && defined( __RESPIRATION__ )
#if defined( __LOCOMOTION__ )  && !defined( __RESPIRATION__ )
		case _id_NaChan:
			unit = new tna_channel( this );
		break;
		case _id_KChan:
			unit = new tk_channel( this );
		break;
		case _id_NaFChan:
			unit = new tnaf_channel( this );
		break;
		case _id_NaPChan:
			unit = new tnap_channel( this );
		break;
		case _id_KdrChan:
			unit = new tkdr_channel( this );
		break;
		case _id_KAChan:
			unit = new tka_channel( this );
		break;
		case _id_CaLChan:
			unit = new tcal_channel( this );
		break;
		case _id_MChan:
			unit = new tm_channel( this );
		break;
		case _id_CaTChan:
			unit = new tcat_channel( this );
		break;
		case _id_CaNChan:
			unit = new tcan_channel( this );
		break;
		case _id_KCChan:
			unit = new tkc_channel( this );
		break;
		case _id_KCaChan:
			unit = new tkca_channel( this );
		break;
		case _id_HChan:
			unit = new th_channel( this );
		break;
		case _id_LeakChan:
			unit = new tleak_channel( this );
		break;
#endif // defined( __LOCOMOTION__ )  && !defined( __RESPIRATION__ )
#if defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
		case _id_resp_NaFChan:
			unit = new trnaf_channel( this );
		break;
		case _id_resp_NaPChan:
			unit = new trnap_channel( this );
		break;
		case _id_KChan:
			unit = new tk_channel( this );
		break;
		case _id_resp_KdrChan:
			unit = new trkdr_channel( this );
		break;
		case _id_NaFChan:
			unit = new tnaf_channel( this );
		break;
		case _id_NaPChan:
			unit = new tnap_channel( this );
		break;
		case _id_KdrChan:
			unit = new tkdr_channel( this );
		break;
		case _id_resp_KAChan:
			unit = new trka_channel( this );
		break;
		case _id_resp_CaLChan:
			unit = new trcal_channel( this );
		break;
		case _id_MChan:
			unit = new tm_channel( this );
		break;
		case _id_resp_CaTChan:
			unit = new trcat_channel( this );
		break;
		case _id_KCChan:
			unit = new tkc_channel( this );
		break;
		case _id_resp_KCaChan:
			unit = new trkca_channel( this );
		break;
		case _id_HChan:
			unit = new th_channel( this );
		break;
		case _id_resp_LeakChan:
			unit = new trleak_channel( this );
		break;
#endif // defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
		default:;
	}
	if( unit ){
		vector<string> add_paths;
		vector<string> del_paths;
		unit->set_name( name );
		uni_template::add_child( unit );
		delete unit;
		reconfig( add_paths, del_paths );
		return true;
	}
	return false;
}

bool t_compart::del_child( const char *name, string &sub_path )
{
	string name_ = name;
	for( size_t i = 0; i < Children.size(); i++ ){
		if( name_ == Children[i]->get_name()){
			Children[i]->pre_del();
			sub_path = Children[i]->get_type();
			sub_path += '\t';
			sub_path += Children[i]->get_name();
			if( Children[i] )
				delete Children[i];
			Children[i] = NULL;
			Children.erase( Children.begin()+i );
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
// class t_soma
t_soma::t_soma( uni_template *parent, int is_active )
	: t_compart( parent, is_active )
{
	UnitId = _id_HHN_SOMA;
	Name = _CompartmentNames[_id_HHN_SOMA];
 // to setup the default configuration of a compartment
#ifdef __LOCOMOTION__
	add_child( "Channel", _ChannelNames[_id_NaChan] );
	add_child( "Channel", _ChannelNames[_id_KChan] );
	add_child( "Channel", _ChannelNames[_id_LeakChan] );
#elif defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	add_child( "Channel", _ChannelNames[_id_resp_NaFChan] );
	add_child( "Channel", _ChannelNames[_id_KChan] );
	add_child( "Channel", _ChannelNames[_id_resp_LeakChan] );
#endif // __RESPIRATION__
}

t_soma::t_soma( const t_soma &soma )
	: t_compart( soma )
{
}

t_soma::~t_soma( void )
{
}

void t_soma::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_soma( *this );
	( *unit )->set_parent( parent );
}

void t_soma::copy_to( hhn_compart **compart, hhn_neuron *neuron )
{
	*compart = new hhn_soma( neuron );
	( *compart )->create( this, neuron->Populat );
}

///////////////////////////////////////////////////////////////////////////////
// class t_dendr
t_dendr::t_dendr( uni_template *parent, int is_active )
	: t_compart( parent, is_active )
{
	UnitId = _id_HHN_DENDRITE;
	Name = _CompartmentNames[_id_HHN_DENDRITE];
}

t_dendr::t_dendr( const t_dendr &dendrite )
	: t_compart( dendrite )
{
}

t_dendr::~t_dendr( void )
{
}

void t_dendr::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_dendr( *this );
	( *unit )->set_parent( parent );
}

void t_dendr::copy_to( hhn_compart **compart, hhn_neuron *neuron )
{
	*compart = new hhn_dendr( neuron );
	( *compart )->create( this, neuron->Populat );
}

///////////////////////////////////////////////////////////////////////////////
// class t_hhn
t_hhn::t_hhn( uni_template *parent, int is_active )
	: uni_template( parent, is_active )
{
	P = 0.1;
	Kd = 0.;
	Td = 0.;
#if defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	DList.insert( make_pair( "Membrane capacitance (\xB5 F)\nC", &Cm ));
	Cm = 25.;
#else
	DList.insert( make_pair( "Specific membrane capacitance (\xB5 F/sm\xB2)\nC", &Cm ));
	Cm = 1.;
#endif // defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	TypeNameS = TypeName = "Neuron";
	for( size_t i = 0; i < _id_MAX_SYN; Y[i++] = 0. );
	add_child( _CompartmentNames[_id_HHN_SOMA] );

	DList.insert( make_pair( "P\nP", &P ));
	DList.insert( make_pair( "Synaptic initial conditions\tExcitatory\nEx", &Y[_id_ExSyn] ));
	DList.insert( make_pair( "Synaptic initial conditions\tExcitatory II\nEx2", &Y[_id_ExBSyn] ));
	DList.insert( make_pair( "Synaptic initial conditions\tInhibitory I\nInhI", &Y[_id_InhASyn] ));
	DList.insert( make_pair( "Synaptic initial conditions\tInhibitory II\nInhII", &Y[_id_InhBSyn] ));
	DList.insert( make_pair( "Synaptic initial conditions\tAdditional synapse 1\nSyn1", &Y[_id_Syn1] ));
	DList.insert( make_pair( "Synaptic initial conditions\tAdditional synapse 2\nSyn2", &Y[_id_Syn2] ));
	DList.insert( make_pair( "Synaptic initial conditions\tAdditional synapse 3\nSyn3", &Y[_id_Syn3] ));
	DList.insert( make_pair( "Synaptic initial conditions\tAdditional synapse 4\nSyn4", &Y[_id_Syn4] ));

	DList.insert( make_pair("Depression\tMagnitude\nKd", &Kd ));
	DList.insert( make_pair("Depression\tTime constant\nTd", &Td ));
	DefaultChildren.push_back( &DefaultCompartment );
}

t_hhn::t_hhn( const t_hhn &neuron )
	: uni_template( neuron )
{
	P = neuron.P;
	Cm = neuron.Cm;
	Kd = neuron.Kd;
	Td = neuron.Td;
	for( size_t i = 0; i < _id_MAX_SYN; Y[i] = neuron.Y[i], i++ );

#if defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	DList.insert( make_pair( "Membrane capacitance (\xB5 F)\nC", &Cm ));
#else
	DList.insert( make_pair( "Specific membrane capacitance (\xB5 F/sm\xB2)\nC", &Cm ));
#endif // defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	DList.insert( make_pair( "P\nP", &P ));
	DList.insert( make_pair( "Synaptic initial conditions\tExcitatory\nEx", &Y[_id_ExSyn] ));
	DList.insert( make_pair( "Synaptic initial conditions\tExcitatory II\nEx2", &Y[_id_ExBSyn] ));
	DList.insert( make_pair( "Synaptic initial conditions\tInhibitory I\nInhI", &Y[_id_InhASyn] ));
	DList.insert( make_pair( "Synaptic initial conditions\tInhibitory II\nInhII", &Y[_id_InhBSyn] ));
	DList.insert( make_pair( "Synaptic initial conditions\tAdditional synapse 1\nSyn1", &Y[_id_Syn1] ));
	DList.insert( make_pair( "Synaptic initial conditions\tAdditional synapse 2\nSyn2", &Y[_id_Syn2] ));
	DList.insert( make_pair( "Synaptic initial conditions\tAdditional synapse 3\nSyn3", &Y[_id_Syn3] ));
	DList.insert( make_pair( "Synaptic initial conditions\tAdditional synapse 4\nSyn4", &Y[_id_Syn4] ));

	DList.insert( make_pair("Depression\tMagnitude\nKd", &Kd ));
	DList.insert( make_pair("Depression\tMagnitude\nKd", &Kd ));
	DList.insert( make_pair("Depression\tTime constant\nTd", &Td ));
}

t_hhn::~t_hhn( void )
{
}

t_hhn &t_hhn::operator = ( const t_hhn &neuron )
{
	uni_template::operator = ( neuron );
	P = neuron.P;
	Cm = neuron.Cm;
	Kd = neuron.Kd;
	Td = neuron.Td;
	for( size_t i = 0; i < _id_MAX_SYN; Y[i] = neuron.Y[i], i++ );
	return *this;
}

bool t_hhn::pre_del( void )
{
	bool success = true;
	for( size_t i = 0; i < Children.size(); ++i ){
		success &= Children[i]->pre_del();
	}
	return success;
}

void t_hhn::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_hhn( *this );
	( *unit )->set_parent( parent );
}

t_compart *t_hhn::get_compart( size_t compart )
{
	if( compart < Children.size() )
		return ( t_compart *)Children[compart];
	return NULL;
}

const t_compart *t_hhn::get_compart( size_t compart ) const
{
	if( compart < Children.size() )
		return ( t_compart *)Children[compart];
	return NULL;
}

bool t_hhn::add_child( const char *name )
{
	string comp_name = name;
	if( !get_child( name )){
		if( comp_name == _CompartmentNames[_id_HHN_SOMA] ){
		uni_template *unit = new t_soma( this );
		uni_template::add_child( unit );
		delete unit;
		return true;
		}
		else if( comp_name == _CompartmentNames[_id_HHN_DENDRITE] ){
			uni_template *unit = new t_dendr( this );
			uni_template::add_child( unit );
			delete unit;
			return true;
		}
	}
	else{
		return true;
	}
	return false;
}

//--- Load/save parameters of the neuron template
bool t_hhn::load_addpar( string str, istream &file )
{
	if( _FileVersion >= 2 ){
		if( str == "<Compartment" ){
			file >> ws;
			getline( file, str, '>');
			if( add_child( str.c_str() )){
				uni_template *compartment = get_child( str.c_str() );
			if( compartment )
				return compartment->load( file );
			}
		}
	}
	else{
		add_child( "Soma" );
		if( Children[_id_HHN_SOMA]->load_data( str, file ) ){
			return true;
		}
		else if( str == "<Channel" ){
			return Children[_id_HHN_SOMA]->load_child( str, file );
		}
		else if( str == "<gChannel" ){
			return Children[_id_HHN_SOMA]->load_child( str, file );
		}
	}
	return false;
}

void t_hhn::save_addpar( ostream &file )
{
	for( size_t i = 0; i < Children.size(); i++ ){
		Children[i]->save( file );
	}
}

///////////////////////////////////////////////////////////////////////////////
// class t_populat
t_populat::t_populat( uni_template *parent, int is_active )
	: uni_template( parent, is_active )
{
	Size = 1;
	TypeNameS = TypeName = "Population";
	DefaultChildren.push_back( &DefaultNeuron );
	add_child( "Neuron" );
}

t_populat::t_populat( const t_populat &populat )
	: uni_template( populat )
{
	Size = populat.Size;
}

t_populat::~t_populat( void )
{
}

t_populat &t_populat::operator = ( const t_populat &populat )
{
	uni_template::operator = ( populat );
	Size = populat.Size;
	return *this;
}

void t_populat::copy_to( uni_template **unit, uni_template *parent )
{
	*unit = new t_populat( *this );
	( *unit )->set_parent( parent );
}

bool t_populat::add_child( const char *name )
{
	uni_template *unit = NULL;
	string child_name = name;
	if( child_name == "Neuron" ){
		unit = new t_hhn( this );
		uni_template::add_child( unit );
		delete unit;
		return true;
	}
	return false;
}

bool t_populat::get_addparlist( vector<string> &parlist )
{
	parlist.push_back( "Name" );
	parlist.push_back( "Size" );
	return true;
}

bool t_populat::get_addpar( const char *path, string &name, string &element, int &type )
{
	if( path ){
		string p_path = path;
		if( p_path == "Size" ){
			name = "Data";
			element << Size;
			type = GRID_INT|GRID_POSITIVE|GRID_EXCL_ZERO;
			return true;
		}
		if( p_path == "Name" ){
			name = "Data";
			element = Name;
			type = GRID_STRING;
			return true;
		}
	}
	return false;
}

bool t_populat::upd_addpar( const char *path, const char *name, string &element, bool save )
{
	if( path && name ){
		string p_path = path;
		string p_name = name;
// <<<--- dirty hack on
		typedef string::size_type str_size;
		str_size str_pos = p_path.find( '\t', 0 );	
		if( str_pos != string::npos )
			p_path = p_path.substr( str_pos+1 );
// <<<--- dirty hack off
		if( p_path == "Size" && p_name == "Data" ){
			if( save )
				element >> Size;
			else
				element << Size;
			return true;
		}
		if( p_path == "Name" && p_name == "Data" ){
			if( save )
				Name = element;
			else
				element = Name;
			return true;
		}
	}
	return false;
}

bool t_populat::load_addpar( string str, istream &file )
{
	if( str == "Size"){
		file >> str >> Size;
		return true;
	}
	else if( str == "<Neuron>" ){
		if( add_child( "Neuron" )){
			uni_template *neuron = get_child( "Neuron" );
			if( neuron )
				return neuron->load( file );
		}
	}
	else if( str == "<Depression"){
		file >> ws;
		getline( file, str, '>');
		if( add_child( str.c_str() )){
			uni_template *synapse = get_child( str.c_str() );
			if( synapse )
				return synapse->load( file );
		}
	}
	return false;
}

void t_populat::save_addpar( ostream &file )
{
	file << "Size = " << Size << endl;
	for( size_t i = 0; i < Children.size(); i++ ){
		Children[i]->save( file );
	}
}

// temporary solution ------- begin
void t_populat::CopyFrom( hhn_populat *populate )
{
	Name = populate->get_name();
	Size = populate->size();
	clear();
	uni_template::add_child( populate->get_neuronT());
}

//--------------------------- end
