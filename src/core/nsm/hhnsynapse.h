//////////////////////////////////////////////////////////////////////
// hhnsynapse.h
#ifndef __HHN_SYNAPSE_H
#define __HHN_SYNAPSE_H

#include "hhnunit.h"

class hhn_compart;

//////////////////////////////////////////////////////////////////////
// class base_synapse
class base_synapse{
	public: //--- constructor
		base_synapse( void ) : Gmax( 0.0 ), Ts( 0.0 ), Vs( 0.0 ), Eds( "0.0" ){};
		base_synapse( const base_synapse &syn ): Gmax( syn.Gmax ), Ts( syn.Ts ), Vs( syn.Vs ), Eds( syn.Eds ){};
virtual	~base_synapse( void ){};
	public:
		base_synapse &operator = ( const base_synapse &syn )
		{
			Gmax = syn.Gmax;
			Ts = syn.Ts;
			Vs = syn.Vs;
			Eds = syn.Eds;
			return *this;
		};
	public:
		string Eds;
		double Gmax;          // Maximal conductance
		double Ts;            // Time constant
		double Vs;            // Amplitude of sinaptic stimulation
};

//////////////////////////////////////////////////////////////////////
// class base_dsynapse
class base_dsynapse{
	public:
		base_dsynapse( void ) : Kdep( 5.0 ), Tdep( 500.0 ){};
		base_dsynapse( const base_dsynapse &syn ) : Kdep( syn.Kdep ), Tdep( syn.Tdep ){};
virtual	~base_dsynapse( void ){};
	public:
		base_dsynapse &operator = ( const base_dsynapse &syn )
		{
			Kdep = syn.Kdep;
			Tdep = syn.Tdep;      
			return *this;
		};
	public:
		double Kdep;
		double Tdep;      
};

//////////////////////////////////////////////////////////////////////
// class base_ssynapse
class base_ssynapse{
	public:
		base_ssynapse( void ) : Hv( 0.0 ), Slp( 1.0 ){};
		base_ssynapse( const base_ssynapse &syn ) : Hv( syn.Hv ), Slp( syn.Slp ){};
virtual	~base_ssynapse( void ){};
	public:
		base_ssynapse &operator = ( const base_ssynapse &syn )
		{
			Hv = syn.Hv;
			Slp = syn.Slp;      
			return *this;
		};
	public:
		double Hv;
		double Slp;
};

//////////////////////////////////////////////////////////////////////
// class hhn_synapse
class hhn_synapse : public hhn_process{ 
	public: //--- constructor
		hhn_synapse( hhn_compart *neuron );
		hhn_synapse( const hhn_synapse &synaps );
virtual	~hhn_synapse( void );   // destructor
	public:
		hhn_synapse &operator = ( const hhn_synapse &synaps );
	public:
		int get_synid( void ) const{ return SynapseId; };
		const char *get_name( void ) const{ return _SynapseNames[SynapseId]; };
		void set_synapse( const base_synapse *type );
	public:
		void reg_unit( runman *man = NULL );
	private:
		void reset( double step );
		void calc_prevm( double step );
static	void reset( size_t currstep, double step, hhn_process **start );
static	void calc_prevm( size_t currstep, double step, hhn_process **start );
	public:
		bool Active;
		base_synapse *Type;	// Point to Synapse property structure
		hhn_compart *Hhn;	// Point to Hodgkin-Haxley type neuron
							// (in order to receive neuron parameters)
		double *IonsE;
		double G;
		double Ginh;
	protected:
		int SynapseId;	// Synapse Id
		bool IsNscpE;
		double NScpE;
};

//////////////////////////////////////////////////////////////////////
// class hexa_synapse
class hexa_synapse : public hhn_synapse{   
	public:
		hexa_synapse( hhn_compart *neuron ) : hhn_synapse( neuron ){ SynapseId = _id_ExSyn; };
		hexa_synapse( const hexa_synapse &syn ) : hhn_synapse( syn ){};
		~hexa_synapse( void ){};
};

#if defined( __RESPIRATION__ )
//////////////////////////////////////////////////////////////////////
// class hexb_synapse
class hexb_synapse : public hhn_synapse{   
	public:
		hexb_synapse( hhn_compart *neuron ) : hhn_synapse( neuron ){ SynapseId = _id_ExBSyn; };
		hexb_synapse( const hexa_synapse &syn ) : hhn_synapse( syn ){};
		~hexb_synapse( void ){};
};
#endif // defined( __RESPIRATION__ )

//////////////////////////////////////////////////////////////////////
// class hinha_synapse
class hinha_synapse : public hhn_synapse{   
	public:
		hinha_synapse( hhn_compart *neuron ) : hhn_synapse( neuron ){ SynapseId = _id_InhASyn; };
		hinha_synapse( const hinha_synapse &syn ) : hhn_synapse( syn ){};
		~hinha_synapse( void ){};
};

//////////////////////////////////////////////////////////////////////
// class hinhb_synapse
class hinhb_synapse : public hhn_synapse{   
	public:
		hinhb_synapse( hhn_compart *neuron ) : hhn_synapse( neuron ){ SynapseId = _id_InhBSyn; };
		hinhb_synapse( const hinhb_synapse &syn ) : hhn_synapse( syn ){};
		~hinhb_synapse( void ){};
};

#endif // __HHN_SYNAPSE_H
