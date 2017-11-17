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
#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

//////////////////////////////////////////////////////////////////////
// class hhn_synapse
hhn_synapse::hhn_synapse( hhn_compart *neuron )
	: IsNscpE( true ), NScpE( 0. ), SynapseId( -1 ), Hhn( neuron ), Type( NULL ),
	G( 0.0 ), Ginh( 0.0 ), Active( false )
{
	IonsE = &NScpE;
}


hhn_synapse::hhn_synapse( const hhn_synapse &synapse )
	: hhn_process( synapse ), IsNscpE( synapse.IsNscpE ),
	NScpE( synapse.NScpE ), SynapseId( synapse.SynapseId ), Hhn( synapse.Hhn ), Type( synapse.Type ),
	G( synapse.G ),	Ginh( synapse.Ginh ), Active( synapse.Active )
{
	IonsE = IsNscpE? &NScpE: synapse.IonsE;
}

hhn_synapse::~hhn_synapse( void )
{
}

hhn_synapse &hhn_synapse::operator = ( const hhn_synapse &synapse )
{
	hhn_process::operator =( synapse );
	G = synapse.G;
	Ginh = synapse.Ginh;
	Active = synapse.Active;
	IsNscpE = synapse.IsNscpE;
	NScpE = synapse.NScpE;
	SynapseId = synapse.SynapseId;
	Type = synapse.Type;
	IonsE = IsNscpE? &NScpE: synapse.IonsE;
	return *this;
}

//--- Setup functions
void hhn_synapse::set_synapse( const base_synapse *type )
{ 
	IsNscpE = false;
	Type = ( base_synapse *)type;
	if( Type->Eds == "Na ions" ){
		IonsE = &( Hhn->NaIons->Eds);
	}
	else if( Type->Eds == "K ions" ){
		IonsE = &( Hhn->KIons->Eds);
	}
	else if( Type->Eds == "Ca ions" ){
		IonsE = &( Hhn->CaIons->Eds);
	}
	else if( Type->Eds == "Cl ions" ){
		IonsE = &( Hhn->ClIons->Eds);
	}
	else{
		NScpE = atof( Type->Eds.c_str());
		IonsE = &NScpE;
		IsNscpE = true;
	}
}

void hhn_synapse::reg_unit( runman *man )
{
	::reg_unit( this, hhn_synapse::reset, _id_PASS_RESET, -1, man ); 
	::reg_unit( this, hhn_synapse::calc_prevm, _id_PASS_PREVM, size_t( Hhn ), man ); 
}

void hhn_synapse::reset( double step )
{
	G = Ginh = 0.;
}

void hhn_synapse::calc_prevm( double step )
{
	double g = Type->Gmax*G/( 1.+Ginh );
	Hhn->G += g;
	Hhn->GE += *IonsE*g;
}
