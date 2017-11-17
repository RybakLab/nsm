/*****************************************************************
 ****              (C)  1997 - 2000                           ****
 **           Model of Hodgkin-Haxley type neuron               **
 **       Development by Ilia Rybak and Sergey Markin           **
 ****                  Populations                            ****
 *****************************************************************/
#include "precompile.h"

#include "hhnpopulat.h"
#include "hhnnetwork.h"
#include "runman.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

/*****************************************************************
 *       Model of population of Hodgkin-Haxley type neuron       *
 *****************************************************************/
//--- constructor
hhn_populat::hhn_populat( void )
	: NeuronT( NULL )
{
	UnitId = _id_NNPopulat;
	for( int id = 0; id < _id_MAX_SYN; ++id ){
		Synapses.push_back( NULL );
		_Synapses.push_back( base_synapse() );
		SynapsesD.push_back( base_dsynapse());
		SynapsesS.push_back( base_ssynapse());
	}
	Network = NULL;
	ConnectType.add_connect( CConnect( CHhnConnect(), _id_NN_SOMA ));    // default target (type 0)
}

hhn_populat::hhn_populat( const string &name )
	: NeuronT( NULL )
{
	Name = name;
	UnitId = _id_NNPopulat;
	for( size_t id = 0; id < _id_MAX_SYN; ++id ){
		Synapses.push_back( NULL );
		_Synapses.push_back( base_synapse() );
		SynapsesD.push_back( base_dsynapse());
		SynapsesS.push_back( base_ssynapse());
	}
	Network = NULL;
	ConnectType.add_connect( CConnect( CHhnConnect(), _id_NN_SOMA ));    // default target (type 0)
	hhn_neuron hhn( this );
	Neurons.push_back( hhn );
	Spikes.resize( Neurons.size(), 0 );
}

hhn_populat::hhn_populat( const hhn_populat &populat )
	: nn_unit( populat ), NeuronT( populat.NeuronT ), _Synapses( populat._Synapses ), SynapsesD( populat.SynapsesD ), SynapsesS( populat.SynapsesS )
{
	for( size_t id = 0; id < _id_MAX_SYN; ++id ){
		Synapses.push_back( NULL );
	}
	create( populat.Network, populat.NeuronT, populat.size() );
}

hhn_populat::hhn_populat( const CHhnNetwork *network, const hhn_populat &populat )
	: nn_unit( populat ), NeuronT( populat.NeuronT ), _Synapses( populat._Synapses ),	SynapsesD( populat.SynapsesD ),	SynapsesS( populat.SynapsesS )
{
	for( size_t id = 0; id < _id_MAX_SYN; ++id ){
		Synapses.push_back( NULL );
	}
	create( network, populat.NeuronT, populat.size() );
}

hhn_populat::~hhn_populat( void )
{
}

hhn_populat &hhn_populat::operator = ( const hhn_populat &populat )
{
	nn_unit::operator = ( populat );
	for( size_t id = 0; id < _id_MAX_SYN; ++id ){
		Synapses[id] = NULL;
	}
	_Synapses = populat._Synapses;
	SynapsesD = populat.SynapsesD;
	SynapsesS = populat.SynapsesS;
	create( populat.Network, populat.NeuronT, populat.size() );
	return *this;
}

void hhn_populat::create( const CHhnNetwork *network, const hhn_populat &populat )
{
	nn_unit::operator = ( populat );
	_Synapses = populat._Synapses;
	SynapsesD = populat.SynapsesD;
	SynapsesS = populat.SynapsesS;
	create( network, populat.NeuronT, populat.size() );
}

void hhn_populat::create( const CHhnNetwork *network, const t_hhn &neuron, unsigned int size )
{
	setup( network ); 
	NeuronT = neuron;
	Neurons.clear();
	hhn_neuron hhn( this );
	for( size_t i = 0; i < size; ++i ){
		Neurons.push_back( hhn );
	}
	Spikes.resize( Neurons.size(), 0 );
	ConnectType.del_connect();
	for( size_t i = 0; i < NeuronT.size(); i++ ){
		switch( NeuronT.get_compart( i )->uid()){
			case _id_HHN_SOMA:
				ConnectType.add_connect( CConnect( CHhnConnect(), _id_NN_SOMA ));
				break;
			case _id_HHN_DENDRITE:
				ConnectType.add_connect( CConnect( CHhnConnect(), _id_NN_DENDRITE ));
				break;
			default:;
		}
	}
}

bool hhn_populat::pre_del( void )
{
	NeuronT.pre_del();
	return false;
}

void hhn_populat::init( void )
{
	Units.clear();
	for( size_t i = 0; i < Neurons.size(); Neurons[i].init(), Units.push_back( &Neurons[i] ), ++i );
	set_kgmax();
}

void hhn_populat::reg_unit( runman *man )
{
	if( !Neurons.empty()){
		::reg_unit( this, hhn_populat::reset, _id_PASS_PREVM, -1, man ); 
		::reg_unit( this, hhn_populat::calc_out, _id_PASS_NNUNIT, -1, man ); 
		for( size_t i = 0; i < Neurons.size(); ++i ){
			Neurons[i].reg_unit( i, man );
		}
		for( inputs_::iterator it = Inputs.begin(); it != Inputs.end(); ++it ){
			it->second.reg_unit( man );
		} 
	}
}

void hhn_populat::reset( double step )
{
	fill( Spikes.begin(), Spikes.end(), 0 );	// reset spikes moments for next step;
}

void hhn_populat::calc_out( double step )
{
	size_t nsize = Neurons.size();
	size_t nspikes = accumulate( Spikes.begin(), Spikes.end(), 0 );
	Output = double( nspikes )/double( nsize );
}

void *hhn_populat::select( unit_code *view )
{
	if( view->is_stat() && Spikes.size())
		return &Spikes;
	else if(( unsigned int )view->HhnIndex < Neurons.size())
		switch( view->Param ){
			case CViewParam::Potent:
				if( view->HhnIndex != -1 && view->NNIndex != -1 && view->PartIndex != -1 ){
					hhn_compart *compart = ( hhn_compart *)( Neurons[view->HhnIndex].get_compart( view->PartIndex ));
					if( compart )
						return &compart->Vm[1];
				}
				break;
			case CViewParam::CaConc:
				if( view->HhnIndex != -1 && view->NNIndex != -1 && view->PartIndex != -1 ){
					hhn_compart *compart = ( hhn_compart *)( Neurons[view->HhnIndex].get_compart( view->PartIndex ));
					if( compart )
						return &compart->CaIons->In;
				}
				break;
			case CViewParam::NaConc:
				if( view->HhnIndex != -1 && view->NNIndex != -1 && view->PartIndex != -1 ){
					hhn_compart *compart = ( hhn_compart *)( Neurons[view->HhnIndex].get_compart( view->PartIndex ));
					if( compart )
						return &compart->NaIons->In;
				}
				break;
			case CViewParam::Condc:
				if( view->HhnIndex != -1 && view->NNIndex != -1 && view->PartIndex != -1 && view->ChanIndex != -1){
					hhn_compart *compart = ( hhn_compart *)( Neurons[view->HhnIndex].get_compart( view->PartIndex ));
					if( compart ){
						hhn_channel *chan = ( hhn_channel * )(( *compart )[view->ChanIndex]);
						if( chan )
							return &chan->G;
					}
				}
				else if( view->HhnIndex != -1 && view->NNIndex != -1 && view->PartIndex != -1 ){
					hhn_compart *compart = ( hhn_compart *)( Neurons[view->HhnIndex].get_compart( view->PartIndex ));
					if( compart )
						return &compart->G;
				}
				break;
			case CViewParam::Curr:
				if( view->HhnIndex != -1 && view->NNIndex != -1 && view->PartIndex != -1 && view->ChanIndex != -1){
					hhn_compart *compart = ( hhn_compart *)( Neurons[view->HhnIndex].get_compart( view->PartIndex ));
					if( compart ){
						hhn_channel *chan = ( hhn_channel * )(( *compart )[view->ChanIndex]);
						if( chan )
							return &chan->I;
					}
				}
				break;
			case CViewParam::Activation:
				if( view->HhnIndex != -1 && view->NNIndex != -1 && view->PartIndex != -1 && view->ChanIndex != -1){
					hhn_compart *compart = ( hhn_compart *)( Neurons[view->HhnIndex].get_compart( view->PartIndex ));
					if( compart ){
						hhn_channel *chan = ( hhn_channel * )(( *compart )[view->ChanIndex]);
						if( chan )
							return &chan->M;
					}
				}
				break;
			case CViewParam::Inactivation:
				if( view->HhnIndex != -1 && view->NNIndex != -1 && view->PartIndex != -1 && view->ChanIndex != -1){
					hhn_compart *compart = ( hhn_compart *)( Neurons[view->HhnIndex].get_compart( view->PartIndex ));
					if( compart ){
						hhn_channel *chan = ( hhn_channel * )(( *compart )[view->ChanIndex]);
						if( chan )
							return &chan->H;
					}
				}
				break;
			default:;
		}
	return nn_unit::select( view );
}

void *hhn_populat::select( CHhnControlled *ctrl )
{
	int ctrl_par = ctrl->get_par().Param;
	int part_index = ctrl->get_par().PartIndex;
	if( ctrl_par < _id_MAX_CHAN ){
		int id_chan = ctrl_par;
		// pickup correspondent KGmax
		if( part_index != 0 )
			return &KGmax[id_chan];
		else
			return &KIonE[id_chan];
	}
	else if( ctrl_par >= _id_MAX_CHAN && ctrl_par < _id_MAX_CHAN+_id_MAX_SYN ){
		int id_syn = ctrl_par-_id_MAX_CHAN;
		_Synapses[id_syn] = Network->synapse( id_syn );
		Synapses[id_syn] = &_Synapses[id_syn];
		return &( Synapses[id_syn]->Gmax );
	}
	else if( ctrl_par >= _id_NUM_CONTROLLED && ctrl_par < _id_MAX_CTRL ){	// hack
		if( part_index != 0 )
			return &KGmax[ctrl_par];
		else
			return &KIonE[ctrl_par];
	}
	return nn_unit::select( ctrl );
}

bool hhn_populat::load( CHhnNetwork *network, istream &file )
{
    setup( network );
	nn_unit::load( file );
	t_hhn neuron( NULL );
	string str;
	int size = 0;
	while( file >> str){
		if( str == "Size"){
			file >> str >> size;
		}
		else if( str == "<Neuron>"){
			neuron.load( file );
		}
		else if( str == "</Population>"){
			create( network, neuron, size );
			return true;
		}
		else{
			unknown_string( file, str);
		}
	}
	message( "Missing tag </Population>!", "Warning!" );
	return false;
}

void hhn_populat::save( ostream &file )
{
	file << endl << "<Population ";
	nn_unit::save( file );
	NeuronT.save( file );
	file << endl << "</Population>\n";
}

//--- protected functions
void hhn_populat::set_kgmax( void )
{
	for( size_t i = 0; i < _id_MAX_CTRL; KIonE[i] = KGmax[i] = 1., ++i ); // new class
	for( size_t i = 0; i < Neurons.size(); i++ ){
		for( size_t ii = 0; ii < Neurons[i].size(); ii++ ){
			const hhn_compart *cmp = Neurons[i].get_compart( ii );
			for( size_t iii = 0; iii < cmp->size(); iii++ ){
// run throught all channel and pickup correspondent KGmax
				int channelId = ( *cmp )[iii]->get_chanid(); // put new identification
				if( channelId == _id_generic_Chan ){
					hhn_channel *chan = ( *cmp )[iii];
					channelId = _id_generic_Chan+_id_NUM_CONTROLLED+( *cmp )[iii]->get_gchanid();
				}
				( *cmp )[iii]->set_kgmax( &KGmax[channelId] );
				( *cmp )[iii]->set_kione( &KIonE[channelId] );
			}
		}
	}
}

hhn_synapse *hhn_populat::get_syn( int type, int n, int sid )
{
	hhn_synapse *syn = Neurons[n].Compart[_id_HHN_SOMA]->Syn[sid];
	if ( type == _id_NN_DENDRITE ){
		syn = Neurons[n].Compart[_id_HHN_DENDRITE]->Syn[sid];
	}
	return syn;
}

void hhn_populat::add_inputs( const vector<wconnect> &inputs )
{
	for( size_t i = 0; i < inputs.size(); ++i ){
		hhn_process *inp = inputs[i].Source;
		int sid = inputs[i].SynapseID;
		bool sigma = inputs[i].Sigma;
		bool depr = inputs[i].Depression;
		bool pre_inh = inputs[i].PreInh;
		hhn_synapse *syn = ( hhn_synapse *)inputs[i].Target;
		inp->retrace( sid, ( sigma )? sid:-1, ( depr )? sid:-1 );

		double weight = inputs[i].Weight;
		double *src = ( depr )? inp->get_hy( sid ): inp->get_y( sid );
		double *trg = ( pre_inh )? &syn->Ginh: &syn->G;
		inputs_::iterator pos = Inputs.find( size_t( trg ));
		if( pos == Inputs.end()){
			Inputs.insert( make_pair( size_t( trg ), hhn_input( trg, inputs.size()/2 )));
			pos = Inputs.find( size_t( trg ));
		}
		pos->second.add_src( src, weight );
	}
}

void hhn_populat::add_connection( const vector<nn_unit *> sources, const vector<CConnect> &weights )
{
	size_t src_size = 0;
	for( size_t i = 0; i < sources.size(); src_size += sources[i]->size(), ++i );
	if( src_size > 0 ){
		vector<wconnect> connect;
		connect.reserve( _id_MAX_SYN*Neurons.size()*src_size );
		for( size_t k = 0; k < Neurons.size(); ++k ){
			for( size_t i = 0; i < sources.size(); ++i ){
				for( size_t id = _id_ExSyn; id < _id_MAX_SYN; ++id ){
					CHhnConnect weight = weights[i].Connect;
					CStat WS = weight.Weights[id];
					WS.Value /= sources[i]->size();
					if( WS.Value > 0.0 ){
						wconnect wc;
						wc.SynapseID = id;
						wc.Sigma = weight.Sigma[id];
						wc.PreInh = weight.PreInh[id];
						wc.Depression = weight.Depression[id];
						wc.Target = get_syn( weights[i].Type, k, id );
						for( size_t j = 0; j < sources[i]->size(); ++j ){
							wc.Weight = double( WS );
							wc.Source = sources[i]->Units[j];
							if( CStat::Random() <= weight.Probability && wc.Source != NULL && wc.Weight > 0.0  ){
								(( hhn_synapse *)wc.Target )->Active = true;
								connect.push_back( wc );
							}
						}
					}
				}
			}
		}
		add_inputs( connect );
	}
}

void hhn_populat::setup( const CHhnNetwork *network )
{
	Network = ( CHhnNetwork *)network;
	ConnectType.del_connect();
	for( size_t i = 0; i < NeuronT.size(); i++ ){
		switch( NeuronT.get_compart( i )->uid()){
			case _id_HHN_SOMA:
				ConnectType.add_connect( CConnect( CHhnConnect(), _id_NN_SOMA ));
				break;
			case _id_HHN_DENDRITE:
				ConnectType.add_connect( CConnect( CHhnConnect(), _id_NN_DENDRITE ));
				break;
			default:;
		}
	}
	for( int id = 0; id < _id_MAX_SYN; ++id ){
		Synapses[id] = &Network->synapse( id );
	}
}
