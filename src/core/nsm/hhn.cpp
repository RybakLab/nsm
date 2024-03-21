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
//#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

/////////////////////////////////////////////////////////////////////////////
// class hhn_compart
hhn_compart::hhn_compart( hhn_neuron *neuron )
	: CompartId( -1 ), Ipumps( 0.0 ), Iinj( 0.0 ), Vm( -60. ), 
	NaIons( NULL ), KIons( NULL ), CaIons( NULL ), ClIons( NULL ),
	Populat( NULL ), Neuron( neuron ), NeuronT( NULL )
{
#if defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	Area = 0.0025;
#endif // defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
}

hhn_compart::hhn_compart( const hhn_compart &compartment )
	: hhn_process( compartment ), CompartId( compartment.CompartId ), Ipumps( compartment.Ipumps ), 
	Iinj( compartment.Iinj ), Vm( compartment.Vm ), NaIons( NULL ), KIons( NULL ), CaIons( NULL ), ClIons( NULL ),
	Populat( NULL ), Name( compartment.Name ), Neuron( compartment.Neuron ), NeuronT( NULL )
{
#if defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	Area = compartment.Area;
#endif // defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
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
	Vm = compartment.Vm;
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
		Vm = NeuronT->Vm;
		Iinj = NeuronT->Iinj;
		Ipumps = 0.0;
		G = GE = Ipumps = 0;
#if defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
		Area = NeuronT->GetArea();
#endif // defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
		create_ions( NeuronT );
		create_syn();
		create_chan( NeuronT );
		return true;
	 }
	return false;
}

void hhn_compart::reg_unit( runman *man )
{
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
	for( int i = 0; i < _id_MAX_SYN; ++i ){
		Syn.push_back( new hhn_synapse( this ));
	}
	for( size_t id = 0; id < Syn.size(); ++id ){
		Syn[id]->set_synapse( Populat->Synapses[id] );
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
	Ions.clear();
	Pumps.clear();
	Chan.clear();
}

void hhn_compart::calc_vm( double step, double thresh )
{
	double v = Vm, g = G;
	if( g > EPS ){
		double v0 = ( GE+Ipumps+Iinj )/g, t = Cm/g;
		Vm = EXP_EULER( v, v0, step, t );
	}
	G = GE = Ipumps = 0;
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
	double v = Vm, g = G;
	if( g > EPS ){
		double v0 = ( GE+Ipumps+Iinj )/g, t = Cm/g;
		Vm = EXP_EULER( v, v0, step, t );
		*( Neuron->Spike ) = int( Vm > thresh && v <= thresh ); // detect the spike
	}
	G = GE = Ipumps = 0;
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
alignas( 16 ) double hhn_neuron::EXPT[_id_MAX_SYN] = {0.};
alignas( 16 ) double hhn_neuron::VS[_id_MAX_SYN] = {0.};

alignas( 16 ) double hhn_neuron::HV[_id_MAX_SYN] = {0.};
alignas( 16 ) double hhn_neuron::SLP[_id_MAX_SYN] = {0.};
alignas( 16 ) double hhn_neuron::THR[_id_MAX_SYN] = {0.};

alignas( 16 ) double hhn_neuron::MODHV[_id_MAX_SYN] = {0.};
alignas( 16 ) double hhn_neuron::MODSLP[_id_MAX_SYN] = {0.};
alignas( 16 ) double hhn_neuron::MODTHR[_id_MAX_SYN] = {0.};
alignas( 16 ) double hhn_neuron::MODSCALE[_id_MAX_SYN] = {0.};

alignas( 16 ) double hhn_neuron::Lvm[_id_MAX_SYN] = {0.};
alignas( 16 ) double hhn_neuron::Hvm[_id_MAX_SYN] = {0.};
alignas( 16 ) double hhn_neuron::LINSLOPE[_id_MAX_SYN] = {0.};

//--- constructor
hhn_neuron::hhn_neuron( hhn_populat *populat )
	: Populat( populat ), IsH( false ), Kd( 0. ), Td( 0. )
{
}

hhn_neuron::hhn_neuron( const hhn_neuron &hhn )	: hhn_process( hhn ), 
	Populat( hhn.Populat ), IsH( hhn.IsH ), Kd( hhn.Kd ), Td( hhn.Td )
{
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
	Kd = hhn.Kd; Td = hhn.Td; IsH = hhn.IsH;
	return *this;
}

//--- public functions
void hhn_neuron::copy_to( hhn_neuron **neuron )
{
	*neuron = new hhn_neuron( *this );
}

bool hhn_neuron::init( void )
{
	create_comp();
	for( size_t i = 0; i < Compart.size(); Compart[i]->init(), ++i );
	t_hhn *tneuron = Populat->get_neuronT();
	for( size_t i = 0; i < _id_MAX_SYN*presyn_max; ++i ){
		Y[i] = tneuron->get_y( i%_id_MAX_SYN );
		H[i] = 1.;
		Output[i] = Y[i]*H[i];
		SynProc[i] = presyn_no;
	}
	IsH = ( Kd == 0. || Td == 0.f )? false: true;
	return true;
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
	::reg_unit( this, hhn_neuron::pre_y, _id_PASS_PREY, -1, man );
	for( size_t i = 0; i < Compart.size(); Compart[i]->reg_unit( man ), ++i );
}

void hhn_neuron::prerun( double step )
{
	for( size_t i = 0; i < _id_MAX_SYN; ++i ){
		hhn_neuron::EXPT[i] = EXP( -step/( Populat->Synapses[i]->Ts ) );
		hhn_neuron::VS[i] = Populat->Synapses[i]->Vs;

		hhn_neuron::HV[i] = Populat->SynapsesS[i].Hv;
		hhn_neuron::SLP[i] = Populat->SynapsesS[i].Slp;
		hhn_neuron::THR[i] = Populat->SynapsesS[i].Thr;

		hhn_neuron::MODHV[i] = Populat->SynapsesMS[i].Hv;
		hhn_neuron::MODSLP[i] = Populat->SynapsesMS[i].Slp;
		hhn_neuron::MODTHR[i] = Populat->SynapsesMS[i].Thr;
		hhn_neuron::MODSCALE[i] = SIGMOID( hhn_neuron::MODTHR[i], hhn_neuron::MODHV[i], hhn_neuron::MODSLP[i] );

		hhn_neuron::Lvm[i] = Populat->SynapsesL[i].Lvm;
		hhn_neuron::Hvm[i] = Populat->SynapsesL[i].Hvm;
		hhn_neuron::LINSLOPE[i] = hhn_neuron::Hvm[i]-hhn_neuron::Lvm[i];
	}
}

int hhn_neuron::retrace( int synid, bool sigma, bool modsigma, bool linear )
{
	if( synid >= 0 && synid < _id_MAX_SYN ){
		int procid = sigma? presyn_sigma: modsigma? presyn_modsigma: linear? presyn_lin: presyn_spike;
		SynProc[synid+procid*_id_MAX_SYN] = procid;
		return synid+procid*_id_MAX_SYN;
	}
	return -1;
}

void hhn_neuron::pre_y( double step )
{
	int spike = *Spike;
	double v = Compart[_id_HHN_SOMA]->get_vm();
	for( size_t i = 0; i < _id_MAX_SYN*presyn_max; ++i ){
		int synid = i%_id_MAX_SYN;
		switch( SynProc[i] ){
			case presyn_spike:
				Y[i] = Y[i]*hhn_neuron::EXPT[synid]+spike*hhn_neuron::VS[synid];
			break;
			case presyn_sigma:
				Y[i] = ( v < hhn_neuron::THR[synid] )? 0.: SIGMOID(v, hhn_neuron::HV[synid], hhn_neuron::SLP[synid]);
			break;
			case presyn_modsigma:
				Y[i] = (( v >= hhn_neuron::THR[synid] )? ( SIGMOID(v, hhn_neuron::HV[synid], hhn_neuron::SLP[synid])-hhn_neuron::MODSCALE[synid] )/( 1.-hhn_neuron::MODSCALE[synid] ): 0. ); // transformed output
			break;
			case presyn_lin:
				Y[i] = ( v < hhn_neuron::Lvm[synid] )? 0: ( v < hhn_neuron::Hvm[synid] )? (v-hhn_neuron::Lvm[synid])/(hhn_neuron::LINSLOPE[synid]): 1;
			break;
		}
		if( IsH ){
			double alpha = 1./( 1+Kd*Y[i] );
			H[i] = EXP_EULER( H[i], alpha, step, Td*alpha );
		}
		Output[i] = Y[i]*H[i];
	}
}
