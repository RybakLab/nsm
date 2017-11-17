/////////////////////////////////////////////////////////////////////////////
// class hhn.cpp
#include "precompile.h"

#include "hhnnetwork.h"
#include "hhntemplate.h"
#include "runman.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

/////////////////////////////////////////////////////////////////////////////
// class hhn_compart
hhn_compart::hhn_compart( hhn_neuron *neuron )
	: CompartId( -1 ), Neuron( neuron ), Populat( NULL ), NeuronT( NULL ),
	NaIons( NULL ), KIons( NULL ), CaIons( NULL ), ClIons( NULL ),
	Ipumps( 0.0 ), Iinj( 0.0 )
{
	Vm[0] = Vm[1] = -60.;
	Area = 1;
}

hhn_compart::hhn_compart( const hhn_compart &compartment )
	: hhn_process( compartment ), Name( compartment.Name ),
	CompartId( compartment.CompartId ), Neuron( compartment.Neuron ), Populat( NULL ), NeuronT( NULL ),
	NaIons( NULL ), KIons( NULL ), CaIons( NULL ), ClIons( NULL ),
	Ipumps( compartment.Ipumps ), Iinj( compartment.Iinj )
{
	Vm[0] = Vm[1] = compartment.Vm[0];
	Area = compartment.Area;
	create( compartment.NeuronT, compartment.Populat  );
}

hhn_compart::~hhn_compart( void )
{
	clear_all();
}

hhn_compart &hhn_compart::operator = ( const hhn_compart &compartment )
{
	hhn_process::operator =( compartment );
	CompartId = compartment.CompartId;
	Name = compartment.Name;
	Neuron = compartment.Neuron;
	Ipumps = compartment.Ipumps;
	Iinj = compartment.Iinj;
	create( compartment.NeuronT, compartment.Populat  );
	return *this;
}

hhn_channel *hhn_compart::operator []( size_t index ) const
{
	if( index < Chan.size() )
		return Chan[index];
	return NULL;
}

bool hhn_compart::create( t_compart *templ, hhn_populat *populat  )
{
	NeuronT = templ;
	Populat = populat;
	return ( NeuronT != NULL )&&( Populat != NULL );
}

bool hhn_compart::init( void )
{
	if( NeuronT && Populat ){
		clear_all();
		P = (( t_hhn *)( NeuronT->get_parent() ))->GetP();
		Cm = (( t_hhn *)( NeuronT->get_parent() ))->GetCm();
		Vm[0] = Vm[1] = NeuronT->Vm;
		Iinj = NeuronT->Iinj;
		Ipumps = 0.0; 
		Area = NeuronT->GetArea();
		create_ions( NeuronT );
		create_syn();
		create_chan( NeuronT );
		return true;
	 }
	return false;
}
            
void hhn_compart::reg_unit( runman *man )
{
	::reg_unit( this, hhn_compart::reset, _id_PASS_RESET, -1, man ); 
	::reg_unit( this, hhn_compart::calc_vm, _id_PASS_VM, -1, man ); 
	for( size_t i = 0; i < Chan.size(); ++i ){
		Chan[i]->reg_unit( man );
	}
	for( size_t i = 0; i < Syn.size(); ++i ){
		Syn[i]->reg_unit( man );
	}
	for( size_t i = 0; i < Ions.size(); ++i ){
		Ions[i]->reg_unit( man );
	}
	for( size_t i = 0; i < Pumps.size(); ++i ){
		Pumps[i]->reg_unit( man );
	}
}

void hhn_compart::create_syn( void )
{
	Syn.push_back( new hexa_synapse( this ));
#if defined( __RESPIRATION__ )
	Syn.push_back( new hexb_synapse( this ));
#endif // defined( __RESPIRATION__ )
	Syn.push_back( new hinha_synapse( this ));
	Syn.push_back( new hinhb_synapse( this ));
	for( size_t id = 0; id < Syn.size(); ++id ){
		DSyn.push_back( new base_dsynapse()); 
		SSyn.push_back( new base_ssynapse()); 
		Syn[id]->set_synapse( Populat->Synapses[id] );
		*DSyn[id] = Populat->SynapsesD[id];
		*SSyn[id] = Populat->SynapsesS[id];
	}
}

void hhn_compart::create_ions( t_compart *tcomp )
{
	tcomp->copy_ions( Ions, this );
	NaIons = Populat->Network->get_naions();
	KIons = Populat->Network->get_kions();
	CaIons = Populat->Network->get_caions();
	ClIons = Populat->Network->get_clions();
	for( size_t i = 0; i < Ions.size(); ++i ){
		switch( Ions[i]->getid()){
			case _id_Na_Ion:
				NaIons = ( hna_ions *)Ions[i];
			break;
			case _id_K_Ion:
				KIons = ( hk_ions *)Ions[i];
			break;
			case _id_Ca_Ion:
				CaIons = ( hca_ions *)Ions[i];
			break;
			case _id_Cl_Ion:
				ClIons = ( hcl_ions *)Ions[i];
			break;
		}
	}
//	pumps must be created AFTER ions
	tcomp->copy_pumps( Pumps, this );
}

void hhn_compart::create_chan( t_compart *tcomp )
{
	tcomp->copy_chan( Chan, this );
}

void hhn_compart::clear_all( void )
{
	for( size_t i = 0; i < Syn.size(); ++i ){
		delete Syn[i];
		delete DSyn[i];
		delete SSyn[i];
	}
	for( size_t i = 0; i < Chan.size(); ++i ){
		delete Chan[i];
	}
	for( size_t i = 0; i < Ions.size(); ++i ){
		delete Ions[i];
	}
	for( size_t i = 0; i < Pumps.size(); ++i ){
		delete Pumps[i];
	}
	Syn.clear();
	DSyn.clear();
	SSyn.clear();
	Ions.clear();
	Pumps.clear();
	Chan.clear();
}

void hhn_compart::reset( void )
{
	Vm[0] = Vm[1];
	G = GE = Ipumps = 0;
}

void hhn_compart::calc_vm( double step, double thresh )
{
	double v, vpre, g, v0, t;
	g = G;
	v = vpre = Vm[0];
	if( g > EPS ){
		v0 = ( GE+Ipumps+Iinj )/g; t = Cm/g;
		v = EXP_EULER( v, v0, step, t );
	}
	Vm[1] = v;
}

/////////////////////////////////////////////////////////////////////////////
// class hhn_soma
hhn_soma::hhn_soma( hhn_neuron *neuron )
	: hhn_compart( neuron )
{
	CompartId = _id_HHN_SOMA;
	Name = _CompartmentNames[CompartId];
}

hhn_soma::hhn_soma( const hhn_soma &soma )
	: hhn_compart( soma )
{
}
           
hhn_soma::~hhn_soma( void )
{
}

void hhn_soma::copy_to( hhn_compart **part )
{
	*part = new hhn_soma( *this );
}

void hhn_soma::create_chan( t_compart *tcomp )
{
	hhn_compart::create_chan( tcomp );
	if( Neuron->get_compart( _id_HHN_DENDRITE )){
		hdendr_channel *dendr = new hdendr_channel( this );
		dendr->init_channel( Neuron->get_compart( _id_HHN_DENDRITE ), P ); 
		Chan.push_back( dendr );
	}
}

void hhn_soma::reg_unit( runman *man )
{
	::reg_unit( this, hhn_compart::reset, _id_PASS_RESET, -1, man ); 
	::reg_unit( this, hhn_soma::calc_vm, _id_PASS_SOMA_VM, -1, man ); 
	for( size_t i = 0; i < Chan.size(); ++i ){
		Chan[i]->reg_unit( man );
	}
	for( size_t i = 0; i < Syn.size(); ++i ){
		Syn[i]->reg_unit( man );
	}
	for( size_t i = 0; i < Ions.size(); ++i ){
		Ions[i]->reg_unit( man );
	}
	for( size_t i = 0; i < Pumps.size(); ++i ){
		Pumps[i]->reg_unit( man );
	}
}

void hhn_soma::calc_vm( double step, double thresh )
{
	double v, vpre, g, v0, t;
	g = G;
	v = vpre = Vm[0];
	if( g > EPS ){
		v0 = ( GE+Ipumps+Iinj )/g; t = Cm/g;
		v = v = EXP_EULER( v, v0, step, t );
		( v > thresh && vpre <= thresh )? ( *( Neuron->Spike ) = 1 ): 0;
	}
	Vm[1] = v;
}

/////////////////////////////////////////////////////////////////////////////
// class hhn_dendr
hhn_dendr::hhn_dendr( hhn_neuron *neuron )
	: hhn_compart( neuron )
{
	CompartId = _id_HHN_DENDRITE;
	Name = _CompartmentNames[CompartId];
}

hhn_dendr::hhn_dendr( const hhn_dendr &dendr )
	: hhn_compart( dendr )
{
}
           
hhn_dendr::~hhn_dendr( void )
{
}

void hhn_dendr::copy_to( hhn_compart **part )
{
	*part = new hhn_dendr( *this );
}

void hhn_dendr::create_chan( t_compart *tcomp )
{
	hhn_compart::create_chan( tcomp );
	if( Neuron->get_compart( _id_HHN_SOMA )){
		hsoma_channel *soma = new hsoma_channel( this );
		soma->init_channel( Neuron->get_compart( _id_HHN_SOMA ), P ); 
		Chan.push_back( soma );
	}
}

/////////////////////////////////////////////////////////////////////////////
// class hhn_neuron
//--- constructor
hhn_neuron::hhn_neuron( hhn_populat *populat )
	: Populat( populat )

{
	for( size_t i = 0; i < _id_MAX_SYN; Y[i] = 0, H[i] = 1., IsY[i] = IsS[i] = IsH[i] = false, ++i );
}

hhn_neuron::hhn_neuron( const hhn_neuron &hhn )
	: hhn_process( hhn ), Populat( hhn.Populat )
{
	for( size_t i = 0; i < _id_MAX_SYN; Y[i] = hhn.Y[i], H[i] = 1., IsY[i] = hhn.IsY[i], IsS[i] = hhn.IsS[i], IsH[i] = hhn.IsH[i], ++i );
}

hhn_neuron::~hhn_neuron( void )
{
	for( size_t i = 0; i < Compart.size(); ++i ){
		delete Compart[i];
	}
}

hhn_neuron &hhn_neuron::operator = ( const hhn_neuron &hhn )
{
	hhn_process::operator =( hhn );
	Populat = hhn.Populat;
	for( size_t i = 0; i < _id_MAX_SYN; Y[i] = hhn.Y[i], H[i] = 1., IsY[i] = hhn.IsY[i], IsS[i] = hhn.IsS[i], IsH[i] = hhn.IsH[i], ++i );
	return *this;
}

//--- public functions
void hhn_neuron::init( void )
{
	create_comp();
	for( size_t i = 0; i < Compart.size(); Compart[i]->init(), ++i );
	t_hhn *neuron = Populat->get_neuronT();
	for( size_t i = 0; i < _id_MAX_SYN; ++i ){
		Y[i] = neuron->get_y( i );
		H[i] = 1.; IsY[i] = IsH[i] = false;
		Output[i] = Y[i]; OutputD[i] = Y[i]*H[i];
	}
}

void hhn_neuron::copy_to( hhn_neuron **neuron )
{
	*neuron = new hhn_neuron( *this );
}

void hhn_neuron::retrace( int yid, int sid, int hid )
{
	if( yid >= 0 && yid < _id_MAX_SYN )
		IsY[yid] = true;
	if( sid >= 0 && sid < _id_MAX_SYN ){
		IsS[sid] = true;
		IsY[sid] = false;
	}
	if( hid >= 0 && hid < _id_MAX_SYN )
		IsH[hid] = true;
}

void hhn_neuron::create_comp( void )
{
	t_hhn *neuron = Populat->get_neuronT();
	for( size_t i = 0; i < Compart.size(); ++i ){
		delete Compart[i];
	}
	Compart.clear();
	for( size_t i = 0; i < neuron->size(); ++i ){
		hhn_compart *compart = NULL;
		neuron->get_compart( i )->copy_to( &compart, this );
		if( compart ) Compart.push_back( compart );
	}
}

void hhn_neuron::reg_unit( int index, runman *man )
{
	Spike = &Populat->Spikes[index];
	for( size_t i = 0; i < _id_MAX_SYN; ++i ){
		if( IsY[i] ){
			::reg_unit( this, hhn_neuron::pre_y, _id_PASS_PREYS, -1, man );
			::reg_unit( this, hhn_neuron::post_y, _id_PASS_POSTY, -1, man );
			break;
		}
	}
	for( size_t i = 0; i < _id_MAX_SYN; ++i ){
		if( IsS[i] ){
			::reg_unit( this, hhn_neuron::pre_s, _id_PASS_PREYS, -1, man );
			break;
		}
	}
	for( size_t i = 0; i < _id_MAX_SYN; ++i ){
		if( IsH[i] ){
			::reg_unit( this, hhn_neuron::pre_h, _id_PASS_PREH, -1, man );
			break;
		}
	}
	for( size_t i = 0; i < Compart.size(); Compart[i]->reg_unit( man ), ++i );
}

void hhn_neuron::pre_y( double step, double vs[_id_MAX_SYN] )
{
	for( size_t i = 0; i < _id_MAX_SYN; ++i ){
		double y = Y[i];
		y += *Spike*vs[i];
		Output[i] = Y[i] = y;
	}
}

void hhn_neuron::pre_s( double step, double thresh,	double hv[_id_MAX_SYN],	double sl[_id_MAX_SYN] )
{
	double v = Compart[_id_HHN_SOMA]->Vm[1];
	for( size_t i = 0; i < _id_MAX_SYN; ++i ){
		if( IsS[i] ){
			double y = ( v < thresh )? 0.: GENERIC_G( v, hv[i], sl[i] );
			Y[i] = Output[i] = y;
		}
	}
}

void hhn_neuron::pre_h( double step, double kd[_id_MAX_SYN], double td[_id_MAX_SYN] )
{
	for( size_t i = 0; i < _id_MAX_SYN; ++i ){
		if( IsH[i] ){
			double y = Y[i];
			double h = H[i];
			double alpha = 1./( 1+kd[i]*y );
			h = EXP_EULER( h, alpha, step, td[i]*alpha );
			OutputD[i] = y*h;
			H[i] = h;
		}
	}
}

void hhn_neuron::post_y( double step, double expt[_id_MAX_SYN] )
{
	for( size_t i = 0; i < _id_MAX_SYN; ++i ){
		double y = Y[i];
		y *= expt[i];
		Output[i] = Y[i] = y;
	}
}
