//////////////////////////////////////////////////////////////////////
// hhnnetwork.h
#include "precompile.h"

#include "hhnnetwork.h"
#include "iontemplate.h"
#include "simulate.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

netuni_templ::netuni_templ( void )
{
}

netuni_templ::netuni_templ( const netuni_templ &t )
	: ChanList( t.ChanList )
{
}

netuni_templ::~netuni_templ( void )
{
}

netuni_templ &netuni_templ::operator = ( const netuni_templ &t )
{
	ChanList = t.ChanList;
	return *this;
}

string netuni_templ::getname( int id ) const
{
	typedef map<string,__id>::const_iterator it;
	for( it pos = ChanList.begin(); pos != ChanList.end(); ++pos ){
		if( id == pos->second.Id )
			return pos->first;
	}
	return string("");
}

int netuni_templ::getid( const string &name ) const
{
	map<string,__id>::const_iterator pos = ChanList.find( name );
	if( pos != ChanList.end() ){
		return pos->second.Id;
	}
	return -1;
}

bool netuni_templ::add( const string &name )
{
	map<string,__id>::iterator pos = ChanList.find( name );
	if( pos != ChanList.end() ){
		++( pos->second );
	}
	else{
		int nid = make_newid();
		if( nid >= 0 ){
			__id newid;
			newid.Id = nid;
			newid.Links = 1;
			ChanList.insert( make_pair( name, newid ));
			return true;
		}
	}
	return false;
}

bool netuni_templ::del( const string &name )
{
	map<string,__id>::iterator pos = ChanList.find( name );
	if( pos != ChanList.end() ){
		--( pos->second );
		if( pos->second.Links <= 0 ){
			remove( pos );
		}
		return true;
	}
	return false;
}

bool netuni_templ::rename( const string &oldname, const string &newname )
{
	if( oldname == newname ){
		return true;
	}
	map<string,__id>::iterator pos = ChanList.find( oldname );
	if( pos != ChanList.end() ){
		__id id = pos->second;
		--( pos->second );
		if( pos->second.Links <= 0 ){
			remove( pos );
		}
		pos = ChanList.find( newname );
		if( pos == ChanList.end()){
			ChanList.insert( make_pair( newname, id ));
		}
		else{
			++( pos->second );
		}
		return true;
	}
	return false;
}

bool netuni_templ::load( istream &file )
{
	bool success = true;
	string str;
	while( file >> str ){
		if( str == "</Template>"){
			return success;
		}
		else if( str == "<Channels>"){
			load_chan( file ); // new code 4 control
		}
	}
	message( "Missing tag </Template>!", "Warning!" );
	return false;
}

void netuni_templ::save( ostream &file )
{
	file << endl << "<Template>" << endl;
	file << "<Channels>" << endl;
	typedef map<string,__id>::const_iterator it;
	for( it pos = ChanList.begin(); pos != ChanList.end(); ++pos ){
		file << "Name = " << pos->first << "(" << pos->second.Id << ")"<< endl;
	}
	file << "</Channels>" << endl;
	file << "</Template>" << endl;
}

bool netuni_templ::load_chan( istream &file )
{
	bool success = true;
	string str;
	while( file >> str ){
		if( str == "</Channels>"){
			return success;
		}
		else if( str == "Name" ){ //		new code 4 control
			file >> str >> ws;
			getline( file, str, '(' );
			string name = str;			
// <todo>	remove white spaces at the end of the name
			if( getid( name ) == -1 ){	// the channel 'name' is not exist
				getline( file, str, ')' );
				__id newid;
				newid.Id = atoi( str.c_str());
				newid.Links = 0;
				ChanList.insert( make_pair( name, newid ));
			}
			else{	// exclude dublicated channel
				getline( file, str );
			}
		}
		else{
			unknown_string( file, str);
		}
	}
	message( "Missing tag </Channels>!", "Warning!" );
	return false;
}

int netuni_templ::make_newid( void )
{
	typedef map<string,__id>::const_iterator it;
	vector<int> ids;
	ids.reserve( ChanList.size());
	for( it pos = ChanList.begin(); pos != ChanList.end(); ++pos ){
		ids.push_back( pos->second.Id );
	}
	for( int nid = 0; nid < _id_MAX_CTRL-_id_NUM_CONTROLLED; ++nid ){
		if( find( ids.begin(), ids.end(), nid ) == ids.end()){
			return nid;
		}
	}
	return -1;
}

bool netuni_templ::remove( map<string,__id>::iterator pos )
{
	ChanList.erase( pos );
	return true;
}
/*****************************************************************
 *  Model of neural network based on Hodgkin-Huxley type neuron  *
 *****************************************************************/
//--- constructor
CHhnNetwork::CHhnNetwork( void )
	: RunMan( NULL ), Threshold( -10.0 ), 
	NetParam( NULL ), TUniList()
{
	NNIndex = 0;
	UnitId = _id_Network;
	set_name("Network");
	for( size_t id = 0; id < _id_MAX_SYN; ++id ){
		Synapses.push_back( new base_synapse );
	}
	ConnectType.del_connect();    // only source
}

CHhnNetwork::CHhnNetwork( CHhnNetwork &network )
	: nn_unit( network ), NNConnect( network.NNConnect ), Drive( network.Drive ), Output( network.Output ),
	Control( network.Control ), RunMan( network.RunMan ), Threshold( network.Threshold ), 
#if defined (__MECHANICS_2D__)
	Feedback( network.Feedback ),
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	NetParam( network.NetParam ), TUniList( network.TUniList )
{
	for( size_t id = 0; id < network.Synapses.size(); ++id ){
		Synapses.push_back( new base_synapse( *network.Synapses[id] ) );
	}
	for( size_t i = 0; i < network.size_pop(); i++ ){
		hhn_populat populat( this, network.Populat[i] );
		Populat.push_back( populat );
	}
}

CHhnNetwork::~CHhnNetwork( void )
{
	for( size_t id = 0; id < Synapses.size(); ++id ){
		delete Synapses[id];
	}
	clear();
}

CHhnNetwork &CHhnNetwork::operator = ( const CHhnNetwork &network )
{
	clear();
	nn_unit::operator = ( network );
	RunMan = network.RunMan;
	NetParam = network.NetParam;
	Threshold = network.Threshold;
	TUniList = network.TUniList;
	for( size_t id = 0; id < _id_MAX_SYN; ++id ){
		*Synapses[id] = *network.Synapses[id];;
	} 
	Drive = network.Drive;
	Output = network.Output;
#if defined (__MECHANICS_2D__)
	Feedback = network.Feedback;
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	Control = network.Control;
	NNConnect = network.NNConnect;
	for( size_t i = 0; i < network.size_pop(); i++ ){
		hhn_populat populat( this, network.Populat[i] );
		Populat.push_back( populat );
	}
	return *this;
} 

bool CHhnNetwork::add_pop( const hhn_populat &populat )
{
	if( !NNConnect.occupied( populat.get_name())){
		hhn_populat newpopulat;
		newpopulat.create( this, populat );
		Populat.push_back( newpopulat );
		for( size_t i = 0; i < Populat.size(); i++ )
			Populat[i].nn_inx() = i;
		NNConnect.add_trg( newpopulat.get_name() );
		NNConnect.add_src( newpopulat.get_name() );
		return true;
	}
	return false;
}

bool CHhnNetwork::set_pop( const hhn_populat &populat, size_t inx )
{
	if( inx < Populat.size() && !populat.get_name().empty() ){
		string oldname = Populat[inx].get_name();
		string newname = populat.get_name();
		if( !NNConnect.occupied( newname )){
			NNConnect.rename_src( oldname, newname );
			NNConnect.rename_trg( oldname, newname );
		}
		Populat[inx].create( this, populat );
		for( size_t i = 0; i < Populat.size(); i++ )
			Populat[i].nn_inx() = i;
		return true;
	}
	return false;
}

bool CHhnNetwork::del_pop( size_t inx )
{
	if( inx < Populat.size()){
		NNConnect.del_trg( Populat[inx].get_name() );
		NNConnect.del_src( Populat[inx].get_name() );
		Populat[inx].pre_del();
		Populat.erase( Populat.begin()+inx );
		for( size_t i = 0; i < size_pop(); i++ )
			Populat[i].nn_inx() = i;
		return true;
	}
	return false;
}

bool CHhnNetwork::add_drv( const hhn_drive &drive )
{
	if( !NNConnect.occupied( drive.get_name() )){
		Drive.push_back( drive );
		for( size_t i = 0; i < Drive.size(); i++ )
			Drive[i].nn_inx() = i;
		NNConnect.add_src( drive.get_name() );
		return true;
	}
	return false;
}

bool CHhnNetwork::set_drv( const hhn_drive &drive, size_t inx )
{
	if( inx < Drive.size() && !drive.get_name().empty() ){
		string oldname = Drive[inx].get_name();
		string newname = drive.get_name();
		if( !NNConnect.occupied( newname )){
			NNConnect.rename_src( oldname, newname );
			NNConnect.rename_trg( oldname, newname );
		}
		Drive[inx] = drive;
		return true;
	}
	return false;
}

bool CHhnNetwork::del_drv( size_t inx )
{
	if( inx < Drive.size() ){
		NNConnect.del_src( Drive[inx].get_name() );
		Drive.erase( Drive.begin()+inx );
		for( size_t i = 0; i < Drive.size(); i++ )
			Drive[i].nn_inx() = i;
		return true;
	}
	return false;
}

bool CHhnNetwork::add_out( const hhn_output &output )
{
	if( !NNConnect.occupied( output.get_name()) ){
		Output.push_back( output );
		for( size_t i = 0; i < Output.size(); i++ )
			Output[i].nn_inx() = i;
		NNConnect.add_trg( output.get_name() );
		NNConnect.add_src( output.get_name() );
		return true;
	}
	return false;
}

bool CHhnNetwork::set_out( const hhn_output &output, size_t inx )
{
	if( inx < Output.size() && !output.get_name().empty() ){
		string oldname = Output[inx].get_name();
		string newname = output.get_name();
		if( !NNConnect.occupied( newname )){
			NNConnect.rename_src( oldname, newname );
			NNConnect.rename_trg( oldname, newname );
		}
		Output[inx] = output;
		for( size_t i = 0; i < Output.size(); i++ )
			Output[i].nn_inx() = i;
		return true;
	}
	return false;
}

bool CHhnNetwork::del_out( size_t inx )
{
	if( inx < Output.size() ){
		NNConnect.del_trg( Output[inx].get_name() );
		NNConnect.del_src( Output[inx].get_name() );
		Output.erase( Output.begin()+inx );
		for( size_t i = 0; i < Output.size(); i++ )
			Output[i].nn_inx() = i;
		return true;
	}
	return false;
}

#if defined (__MECHANICS_2D__)
bool CHhnNetwork::add_fbk( const hhn_feedback &feedback )
{
	if( !NNConnect.occupied( feedback.get_name()) ){
		hhn_feedback newfeedback( feedback );
		Feedback.push_back( newfeedback );
		for( unsigned int i = 0; i < size_fbk(); i++ )
			Feedback[i].nn_inx() = i;

		NNConnect.add_src( newfeedback.get_name() );
		return true;
	}
	return false;
}

bool CHhnNetwork::set_fbk( const hhn_feedback &feedback, size_t inx )
{
	if( inx < Feedback.size() && !feedback.get_name().empty() ){
		string oldname = Feedback[inx].get_name();
		string newname = feedback.get_name();
		if( !NNConnect.occupied( newname )){
			NNConnect.rename_src( oldname, newname );
		}
		Feedback[inx] = feedback;
		for( unsigned int i = 0; i < size_fbk(); i++ )
			Feedback[i].nn_inx() = i;
		return true;
	}
	return false;
}

bool CHhnNetwork::del_fbk( size_t inx )
{
	if( inx < Feedback.size() ){
		NNConnect.del_src( Feedback[inx].get_name() );
		Feedback.erase( Feedback.begin()+inx );
		for( unsigned int i = 0; i < size_fbk(); i++ )
			Feedback[i].nn_inx() = i;
		return true;
	}
	return false;
}
#elif defined (__MECHANICS_3D__)
// TODO implementation 3d model
#endif // __MECHANICS_2D__

bool CHhnNetwork::add_ctr( const hhn_control &control )
{
	Control.push_back( control );
	return true;
}

bool CHhnNetwork::del_ctr( size_t inx )
{
	if( inx < Control.size()){
		Control.erase( Control.begin()+inx );
		return true;
	}
	return false;
}

bool CHhnNetwork::set_ctr( const hhn_control &control, size_t inx )
{
	if( inx < Control.size()){
		Control[inx] = control;
		return true;
	}
	return false;
}

void CHhnNetwork::init_syn( void )
{
	for( size_t id = 0; id < _id_MAX_SYN; ++id ){
		t_ssynapse *ssyn = ( t_ssynapse *)NetParam.get_child( "Sigma", _SynapseNames[id] );
		if( ssyn ){
			base_ssynapse *syn = NULL;
			ssyn->copy_to( &syn );
			if( syn ){
				for( size_t i = 0; i < Populat.size(); ++i ){
					Populat[i].SynapsesS[id] = *syn;
				}
				delete syn;
			}
		}
		t_modssynapse *modssyn = ( t_modssynapse *)NetParam.get_child( "ModSigma", _SynapseNames[id] );
		if( modssyn ){
			base_modssynapse *syn = NULL;
			modssyn->copy_to( &syn );
			if( syn ){
				for( size_t i = 0; i < Populat.size(); ++i ){
					Populat[i].SynapsesMS[id] = *syn;
				}
				delete syn;
			}
		}
		t_lsynapse *lsyn = ( t_lsynapse *)NetParam.get_child( "Linear", _SynapseNames[id] );
		if( lsyn ){
			base_lsynapse *syn = NULL;
			lsyn->copy_to( &syn );
			if( syn ){
				for( size_t i = 0; i < Populat.size(); ++i ){
					Populat[i].SynapsesL[id] = *syn;
				}
				delete syn;
			}
		}
		t_synapse *tsyn = ( t_synapse *)NetParam.get_child( "Synapse", _SynapseNames[id] );
		if( tsyn ){
			base_synapse *syn = NULL;
			tsyn->copy_to( &syn );
			if( syn ){
				*Synapses[id] = *syn;
				delete syn;
			}
		}
	}
}

void CHhnNetwork::init_ions( void )
{
	for( size_t id = 0; id < Ions.size(); ++id ){
		delete Ions[id];
	}
	Ions.clear();
	for( size_t id = 0; id < _id_MAX_ION; ++id ){
		t_ions *tion = ( t_ions *)NetParam.get_child( "Ions", _IonsNames[id] );
		if( tion ){
			hhn_process *ion = NULL;
			tion->copy_to( &ion, NULL );
			Ions.push_back(( hhn_ions *)ion );
		}
	}
	for( size_t id = 0; id < Ions.size(); ++id ){
		Ions[id]->init();
	}
}

void CHhnNetwork::init( long seed, runman *man, bool rand )
{
#ifdef __OMP__
	#pragma omp master	// control/reaction on external events
	{
#endif
	::unreg_units( RunMan );
	if( man != RunMan ){
		::unreg_units( man );
		RunMan = man;
	}
	CStat::SetSeed( seed );
	Threshold = NetParam.Threshold;
	init_ions();
	init_syn();
	for( size_t i = 0; i < Drive.size(); Drive[i].init(), ++i );
#if defined (__MECHANICS_2D__)
	for( size_t i = 0; i < Feedback.size(); Feedback[i].init(), ++i );
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	for( size_t i = 0; i < Output.size(); Output[i].init(), ++i );
	for( size_t i = 0; i < Populat.size(); Populat[i].init(), ++i );
	for( size_t i = 0; i < Control.size(); Control[i].init( this ), ++i );

	vector<string> src_name = NNConnect.name_src();
	vector<string> trg_name = NNConnect.name_trg();
	vector<nn_unit *> src;
	for( size_t i = 0; i < src_name.size(); src.push_back( get_nnunit( src_name[i] )), ++i );
	vector<nn_unit *> trg;
	for( size_t i = 0; i < trg_name.size(); trg.push_back( get_nnunit( trg_name[i] )), ++i );
	for( size_t i = 0; i < trg_name.size(); i++ ){
		vector<CNNConnect> weights = NNConnect.sources( trg_name[i] );
		trg[i]->connect( src, weights );
	}
#if defined (__MECHANICS_2D__)
	for( size_t i = 0; i < Feedback.size(); Feedback[i].reg_unit( RunMan ), ++i );
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	for( size_t i = 0; i < Output.size(); Output[i].reg_unit( RunMan ), ++i );
	for( size_t i = 0; i < Populat.size(); Populat[i].reg_unit( RunMan ), ++i );
	for( size_t i = 0; i < Control.size(); Control[i].reg_unit( RunMan ), ++i );
	for( size_t i = 0; i < Ions.size(); Ions[i]->reg_unit( RunMan ), ++i );
#ifdef __OMP__
	}
#endif
}

void CHhnNetwork::prerun( double step )
{
#ifdef __OMP__
	#pragma omp master	// control/reaction on external events
	{
#endif
	if( !Populat.empty() ){ Populat[0].prerun( step ); }
	for( size_t i = 0; i < Output.size(); Output[i].prerun( step ), ++i );
#ifdef __OMP__
	}
#endif
}

void *CHhnNetwork::select( CHhnControlled *ctrl )
{
	int ctrl_par = ctrl->get_par().Param;
	if( ctrl_par >= _id_MAX_CHAN && ctrl_par < _id_MAX_CHAN+_id_MAX_SYN ){
		int id_syn = ctrl_par-_id_MAX_CHAN;
		return &( Synapses[id_syn]->Gmax );
	}
	if( ctrl_par == _id_KIons ){
		return &get_kions()->Out;
	}
	return nn_unit::select( ctrl );
}

//--- Load/save parameters of the network (without initial conditions)
bool CHhnNetwork::load( istream &file, CSimulate *manager )
{
	clear();
	file >> ws;
	string str;
	getline( file, str, '>' );
	set_name( str );
	while( file >> str){
		if( str == "</Network>"){
			return true;
		}
		else if( str == "<Settings>" ){
			NetParam.load( file );
		}
		else if( str == "<Template>"){
			TUniList.load( file ); // new code 4 control
		}
		else if( str == "<Units>"){
			LoadNNUnits( file );
		}
		else if( str == "<Connections>"){
			LoadConnections( file );
		}
		else if( str == "<Control"){
			hhn_control control;
			if( control.load( file, manager ))
				add_ctr( control );
		}
		else{
			unknown_string( file, str);
		}
	}
	message( "Missing tag </Network>!", "Warning!" );
	return false;
}

void CHhnNetwork::save( ostream &file, CSimulate *manager )
{
	file << "\n<Network " << nn_unit::get_name() << ">\n";
	NetParam.save( file );
	TUniList.save( file ); // new code 4 control
	SaveNNUnits( file );
	SaveConnections( file );
	for( size_t i = 0; i < Control.size(); Control[i].save( file, manager ), ++i );
	file << "\n</Network>\n";
}

void CHhnNetwork::diff( const string &p1, const string &p2, const string &expr, ostream &log )
{
	vector<string> src = NNConnect.name_src();
	vector<string> trg = NNConnect.name_trg();
	vector<string> allunits = src;
	for( size_t i = 0; i < trg.size(); ++i ){
		if( find( allunits.begin(), allunits.end(), trg[i] ) == allunits.end() ){
			allunits.push_back( trg[i] );
		}
	}
	vector<string> p1units, p2units;
	log << "start searching" << endl;
	log << "==============================" << endl;
	std::regex re1( p1+expr ), re2( p2+expr );
	for( size_t i = 0; i < allunits.size(); ++i ){
		bool found = false;
		if( std::regex_match( allunits[i], re1 )){
			p1units.push_back( allunits[i] );
			found = true;
		}
		if( std::regex_match( allunits[i], re2 )){
			p2units.push_back( allunits[i] );
			found = true;
		}
		if( !found ){
			log << "unit '" << allunits[i] << "' does not fit to search criteria" << endl;
		}
	}
	log << "==============================" << endl;
	log << "compare selected units...." << endl;
	log << "==============================" << endl;
	multimap<string,string> unitsmap;
	std::regex nre1( p1 );
	for( size_t i = 0; i < p1units.size(); ++i ){
		std::smatch matches;
		if( std::regex_search( p1units[i], matches, nre1 )){
			string key = matches.suffix();
			if( !key.empty() ){
				unitsmap.insert( make_pair( key, p1units[i] ));
			}
		}
	}
	std::regex nre2( p2 );
	for( size_t i = 0; i < p2units.size(); ++i ){
		std::smatch matches;
		if( std::regex_search( p2units[i], matches, nre2 )){
			string key = matches.suffix();
			if( !key.empty() ){
				unitsmap.insert( make_pair( key, p2units[i] ));
			}
		}
	}
	compare( unitsmap, log );
	log << "==============================" << endl;
	log << "operation is completed" << endl;
}

void CHhnNetwork::compare( const multimap<string, string> &unitsmap, ostream &log )
{
	typedef multimap<string, string> nmap;
	vector<string> keys;
	for( nmap::const_iterator pos = unitsmap.begin(); pos != unitsmap.end(); ++pos ){
		if( keys.empty() || keys.back() != pos->first ){
			keys.push_back( pos->first );
		}
	}
	for( vector<string>::const_iterator pos = keys.begin(); pos != keys.end(); ++pos ){
		pair<nmap::const_iterator, nmap::const_iterator> names = unitsmap.equal_range( *pos );
		compare( names.first, names.second, log );
	}
}

void CHhnNetwork::compare( const multimap<string, string>::const_iterator &n1, const multimap<string, string>::const_iterator &n2, ostream &log )
{
	typedef multimap<string, string> nmap;
	string key = n1->first;
	vector<nn_unit *> units;
	for( nmap::const_iterator pos = n1; pos != n2; ++pos ){
		nn_unit *u = get_nnunit( pos->second );
		if( u != NULL ){
			units.push_back( u );
		}
	}
	if( units.size() > 1 ){
		for( size_t i = 0; i < units.size()-1; ++i ){
//			compare_units( units[i], units[i+1], log );		
			compare_connections( units[i], units[i+1], key, log );
		}
	}
	else{
		log << "error: only one unit '" << n1->second << "' corresponds to '" << key << "'" << endl;
	}
}

bool CHhnNetwork::compare_units( const nn_unit *u1, const nn_unit *u2, ostream &log )
{
	log << " not implemented, yet" << endl;
	return false;
}

bool CHhnNetwork::compare_connections( const nn_unit *u1, const nn_unit *u2, const string &key, ostream &log )
{
	vector<CNNConnect> src1, trg1, src2, trg2;
	src1 = NNConnect.sources( u1->get_name() );
	src2 = NNConnect.sources( u2->get_name() );
	trg1 = NNConnect.targets( u1->get_name() );
	trg2 = NNConnect.targets( u2->get_name() );
	if( src1.size() != src2.size()){
		log << "connectivity matrix error: source sizes are different" << endl;
		return false;
	}
	if( trg1.size() != trg2.size()){
		log << "connectivity matrix error: target sizes are different" << endl;
		return false;
	}
	vector<string> all_src = NNConnect.name_src();
	vector<string> all_trg = NNConnect.name_trg();
	string diff1 = u1->get_name().substr( 0, u1->get_name().find( key ));
	string diff2 = u2->get_name().substr( 0, u2->get_name().find( key ));

	vector<string> new_all_src = all_src;
	vector<string>::iterator new_end;
	for( size_t i = 0; i < src1.size(); ++i ){
		if( !src1[i].empty() ){
			if( src1[i] == src2[i] ){ // u1 and u2 have the same source
				log << "connections: " << u1->get_name() << "<-" << all_src[i];
				log << " = " << u2->get_name() << "<-" << all_src[i] << endl;
				new_end = remove( new_all_src.begin(), new_all_src.end(), all_src[i] );
				new_all_src.erase( new_end, new_all_src.end());
				continue;
			}
			else if( !diff1.empty() && !diff2.empty() ){ // try to guess
				std::smatch matches;
				string guess1, guess2;
				std::regex d1( string( "^" )+diff1 );
				if( std::regex_search( all_src[i], matches, d1 )){
					string key = matches.suffix();
					if( !key.empty() ){
						guess1 = diff1+key;
						guess2 = diff2+key;
					}
				}
				else{
					std::regex d2( string( "^" )+diff2 );
					std::smatch matches;
					if( std::regex_search( all_src[i], matches, d2 )){
						string key = matches.suffix();
						if( !key.empty() ){
							guess1 = diff1+key;
							guess2 = diff2+key;
						}
					}
				}
				if( !guess1.empty() || !guess2.empty() ){
					if( NNConnect( u2->get_name(), guess1 ) == src1[i] ){
						log << "connections: " << u1->get_name() << "<-" << all_src[i];
						log << " = " << u2->get_name() << "<-" << guess1 << endl;
						new_end = remove( new_all_src.begin(), new_all_src.end(), guess1 );
						new_all_src.erase( new_end, new_all_src.end());
						if( src2[i].empty() ){
							new_end = remove( new_all_src.begin(), new_all_src.end(), all_src[i] );
							new_all_src.erase( new_end, new_all_src.end());
						}
						continue;
					}
					if( NNConnect( u2->get_name(), guess2 ) == src1[i] ){
						log << "connections: " << u1->get_name() << "<-" << all_src[i];
						log << " = " << u2->get_name() << "<-" << guess2 << endl;
						new_end = remove( new_all_src.begin(), new_all_src.end(), guess2 );
						new_all_src.erase( new_end, new_all_src.end());
						if( src2[i].empty() ){
							new_end = remove( new_all_src.begin(), new_all_src.end(), all_src[i] );
							new_all_src.erase( new_end, new_all_src.end());
						}
						continue;
					}
				}
			}
			log << endl << "connections: " << u1->get_name() << "<-" << all_src[i];
			log << " >>>>>>>>>>>>>>>>>>>>>>>>>>> NO PAIR FOUND <<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl << endl;
		}
		else{
			if( src2[i].empty() ){
				new_end = remove( new_all_src.begin(), new_all_src.end(), all_src[i] );
				new_all_src.erase( new_end, new_all_src.end());
			}
		}
	}

	return true;
}

bool CHhnNetwork::LoadNNUnits( istream &file )
{
	string str;
	bool success = true;
	while( file >> str){
		if( str == "</Units>"){
			return success;
		} else if( str == "<Population"){
			hhn_populat populat;
			success &= populat.load( this, file );
			success &= add_pop( populat );
		} else if( str == "<Drive"){
			hhn_drive drive;
			success &= drive.load( file );
			success &= add_drv( drive );
		} else if( str == "<Output"){
			hhn_output output;
			success &= output.load( file );
			success &= add_out( output );
		} else if( str == "<Object"){
			hhn_output output;
			success &= output.load( file );
			success &= add_out( output );
		} 
#if defined (__MECHANICS_2D__)
		else if( str == "<Feedback"){
			hhn_feedback feedback;
			success &= feedback.load( file );
			success &= add_fbk( feedback );
		}
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
		else{
			unknown_string( file, str);
		}
	}
	message( "Missing tag </Units>!", "Warning!" );
	return false;
}

void CHhnNetwork::SaveNNUnits( ostream &file )
{
	file << "\n<Units>\n";
	for( size_t i = 0; i < Populat.size(); Populat[i].save( file ), ++i );
	for( size_t i = 0; i < Drive.size(); Drive[i].save( file ), ++i );
	for( size_t i = 0; i < Output.size(); Output[i].save( file ), ++i );
#if defined (__MECHANICS_2D__)
	for( size_t i = 0; i < Feedback.size(); Feedback[i].save( file ), ++i );
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	file << "\n</Units>\n";
}

bool CHhnNetwork::LoadConnections( istream &file )
{
	string str;
	while( file >> str ){
		if( str == "<Source"){
			file >> ws;
			getline( file, str, '>' );
			LoadSource( str, file );
		}
		else if( str == "</Connections>" ){
			return true;
		}
		else{
			unknown_string( file, str );
		}
	}
	message( "Missing tag </Connections>!", "Warning!" );
	return false;
}

void CHhnNetwork::SaveConnections( ostream &file )
{
	
	vector<string> src_name = NNConnect.name_src();
	vector<string> trg_name = NNConnect.name_trg();
	sort( src_name.begin(), src_name.end());
	sort( trg_name.begin(), trg_name.end());
	file << endl << "<Connections>" << endl;
	for( size_t i = 0; i < src_name.size(); ++i ){          // loop for sources
		file << endl << "<Source " << src_name[i] << ">" << endl;
		bool new_line = false;
		for( size_t j = 0; j < trg_name.size(); ++j ){     // loop for targets
			if( !NNConnect( trg_name[j], src_name[i] ).empty() ){
				file << endl << "<Target " << trg_name[j] << ">" << endl;
				NNConnect( trg_name[j], src_name[i] ).save( file );
				new_line = true;
			}
		}
		if( new_line ){ file << endl; }
		file << "</Source>" << endl;
	}
	file << endl << "</Connections>" << endl;
}

bool CHhnNetwork::LoadSource( const string &src, istream &file )
{
	string str;
	while( file >> str ){
		if( str == "<Target" ){
			file >> ws;
			string trg;
			getline( file, trg, '>' );
			CNNConnect weight;
			if( weight.load( file ) && !weight.empty()){
				if( !NNConnect.connect( weight, trg, src ) ){
					str = "Unknown names: " + src + ", " + trg + "!";
					message( str.c_str(), "Warning!" );
				}
			}
		}
		else if( str == "</Source>")
			return true;
		else
			unknown_string( file, str );
	}
	message( "Missing tag </Source>!", "Warning!" );
	return false;
}

hhn_pair<int> CHhnNetwork::get_uid( const string &name )const
{
	hhn_pair<int> unit(-1,-1);
	if( name == get_name() ){
		unit = hhn_pair<int>(_id_Network, 0);
		return unit;
	}
    size_t i = 0;
	for( i = 0; i < Populat.size(); i++ )
		if( name == Populat[i].get_name()){
			unit = hhn_pair<int>(_id_NNPopulat, i);
			return unit;
		}
#if defined (__MECHANICS_2D__)
	for( i = 0; i < Feedback.size(); i++ )
		if( name == Feedback[i].get_name()){
			unit = hhn_pair<int>(_id_NNFeedback, i);
			return unit;
		}
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	for( i = 0; i < Drive.size(); i++ )
		if( name == Drive[i].get_name()){
			unit = hhn_pair<int>(_id_NNDrive, i);
			return unit;
		}
	for( i = 0; i < Output.size(); i++ )
		if( name == Output[i].get_name()){
			unit = hhn_pair<int>(_id_NNOutput, i);
			return unit;
		}

	for( i = 0; i < Control.size(); i++ )
		if( name == Control[i].get_name()){
			unit = hhn_pair<int>(_id_NNControl, i);
			return unit;
		}
	return unit;
}

nn_unit *CHhnNetwork::get_nnunit( const unit_code &code )
{
	switch( code.UnitId ){
		case _id_Network:
			return ( CHhnNetwork *)this;
		case _id_NNPopulat:
			if( code.NNIndex < Populat.size() )
				return &Populat[code.NNIndex];
#if defined (__MECHANICS_2D__)
		case _id_NNFeedback:
			if( code.NNIndex < Feedback.size() )
				return &Feedback[code.NNIndex];
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
		case _id_NNDrive:
			if( code.NNIndex < Drive.size() )
				return &Drive[code.NNIndex];
		case _id_NNOutput:
			if( code.NNIndex < Output.size() )
				return &Output[code.NNIndex];
		default:;
	}
	return NULL;
}

nn_unit *CHhnNetwork::get_nnunit( const string &name )
{
	if( name == get_name()) return this;
	for( size_t i = 0; i < Populat.size(); i++ )
		if( name == Populat[i].get_name())
			return &Populat[i];
#if defined (__MECHANICS_2D__)
	for( size_t i = 0; i < Feedback.size(); i++ )
		if( name == Feedback[i].get_name())
			return &Feedback[i];
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	for( size_t i = 0; i < Drive.size(); i++ )
		if( name == Drive[i].get_name())
			return &Drive[i];
	for( size_t i = 0; i < Output.size(); i++ )
		if( name == Output[i].get_name())
			return &Output[i];
	return NULL;
}

hhn_control *CHhnNetwork::get_crtunit( const unit_code &code )
{
	switch( code.UnitId ){
		case _id_NNControl:
			if( code.NNIndex < Control.size())
				return &Control[code.NNIndex];
		default:;
	}
	return NULL;
}

hhn_control *CHhnNetwork::get_crtunit( const string &name )
{
 for( size_t i = 0; i < Control.size(); i++ )
      if( name == Control[i].get_name())
          return &Control[i];
 return NULL;
}

//--- private functions
void CHhnNetwork::clear( void )
{
	Populat.clear();
	Drive.clear();
	Output.clear();
#if defined (__MECHANICS_2D__)
	Feedback.clear();
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	Control.clear();
	NNConnect.clear();
	for( size_t id = 0; id < Ions.size(); ++id ){
		delete Ions[id];
	}
	Ions.clear();
}
