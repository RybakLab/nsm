//////////////////////////////////////////////////////////////////////
// hhnsynapse.cpp
#include "precompile.h"

#include "hhnsynapse.h"
#include "hhn.h"
#include "runman.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

//////////////////////////////////////////////////////////////////////
// class hhn_synapse
hhn_synapse::hhn_synapse( hhn_compart *neuron )
	: G( 0.0 ), Ginh( 0.0 ), Gmax( NULL ), IonsE( NULL ), Hhn( neuron ), NScpE( 0. )
{
}


hhn_synapse::hhn_synapse( const hhn_synapse &synapse )
	: hhn_process( synapse ), G( synapse.G ), Ginh( synapse.Ginh ), Gmax( NULL ), IonsE( NULL ), Hhn( synapse.Hhn ), NScpE( synapse.NScpE )
{
}

hhn_synapse::~hhn_synapse( void )
{
}

hhn_synapse &hhn_synapse::operator = ( const hhn_synapse &synapse )
{
	hhn_process::operator =( synapse );
	G = synapse.G;
	Ginh = synapse.Ginh;
	NScpE = synapse.NScpE;
	return *this;
}

//--- Setup functions
void hhn_synapse::set_synapse( base_synapse *type )
{ 
	G = Ginh = 0.;
	Gmax = &( type->Gmax );
	if( type->Eds == "Na ions" ){
		IonsE = &( Hhn->NaIons->Eds);
	} else if( type->Eds == "K ions" ){
		IonsE = &( Hhn->KIons->Eds);
	} else if( type->Eds == "Ca ions" ){
		IonsE = &( Hhn->CaIons->Eds);
	} else if( type->Eds == "Cl ions" ){
		IonsE = &( Hhn->ClIons->Eds);
	} else{
		NScpE = atof( type->Eds.c_str());
		IonsE = &NScpE;
	}
}

void hhn_synapse::reg_unit( runman *man )
{
	::reg_unit( this, hhn_synapse::calc_prevm, _id_PASS_PREVM, size_t( Hhn ), man ); 
}

void hhn_synapse::calc_prevm( double step )
{
	// TODO syn change the model of presynaptic inhibition
	double g = *Gmax*G/( 1.+Ginh );
	Hhn->G += g;
	Hhn->GE += *IonsE*g;
	G = Ginh = 0.;
}
