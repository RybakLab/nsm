//////////////////////////////////////////////////////////////////////
// hhnsynapse.h
#ifndef __HHN_SYNAPSE_H
#define __HHN_SYNAPSE_H

#include "hhnunit.h"

class hhn_compart;

//////////////////////////////////////////////////////////////////////
// class base_synapse
class alignas( 16 ) base_synapse{
	public: //--- constructors/destructor
		base_synapse( void ) : Gmax( 0.0 ), Ts( 0.0 ), Vs( 0.0 ), Eds( "0.0" ){};
		base_synapse( const base_synapse &syn ): Gmax( syn.Gmax ), Ts( syn.Ts ), Vs( syn.Vs ), Eds( syn.Eds ){};
		~base_synapse( void ){};
	public: // operators
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
		base_synapse &operator = ( const base_synapse &syn )
		{
			Gmax = syn.Gmax;
			Ts = syn.Ts;
			Vs = syn.Vs;
			Eds = syn.Eds;
			return *this;
		};
	public: // data
		double Gmax;		// Maximal conductance
		double Ts;		// Time constant
		double Vs;		// Amplitude of sinaptic stimulation
		string Eds;
};
//////////////////////////////////////////////////////////////////////
// class base_dsynapse
class base_dsynapse{
	public: //--- constructors/destructor
		base_dsynapse( void ) : Kdep( 5.0 ), Tdep( 500.0 ){};
		base_dsynapse( const base_dsynapse &syn ) : Kdep( syn.Kdep ), Tdep( syn.Tdep ){};
		~base_dsynapse( void ){};
	public: // operators
		base_dsynapse &operator = ( const base_dsynapse &syn )
		{
			Kdep = syn.Kdep;
			Tdep = syn.Tdep;      
			return *this;
		};
	public: // data
		double Kdep;
		double Tdep;
};

//////////////////////////////////////////////////////////////////////
// class base_ssynapse
class base_ssynapse{
	public: //--- constructors/destructor
		base_ssynapse( void ) : Hv( 0.0 ), Slp( 1.0 ), Thr( 0. ){};
		base_ssynapse( const base_ssynapse &syn ) : Hv( syn.Hv ), Slp( syn.Slp ), Thr( syn.Thr ){};
virtual		~base_ssynapse( void ){};
	public: // operators
		base_ssynapse &operator = ( const base_ssynapse &syn )
		{
			Hv = syn.Hv;
			Slp = syn.Slp;
			Thr = syn.Thr;
			return *this;
		};
	public: // data
		double Hv;
		double Slp;
		double Thr;
};
//////////////////////////////////////////////////////////////////////
// class base_modssynapse
class base_modssynapse: public base_ssynapse{
	public: //--- constructors/destructor
		base_modssynapse( void ) : base_ssynapse(){};
		base_modssynapse( const base_modssynapse &syn ) : base_ssynapse( syn ){};
		~base_modssynapse( void ){};
	public: // operators
		base_modssynapse &operator = ( const base_modssynapse &syn )
		{
			base_ssynapse::operator = ( syn );
			return *this;
		};
};
//////////////////////////////////////////////////////////////////////
// class base_lsynapse
class base_lsynapse{
	public: //--- constructors/destructor
		base_lsynapse( void ) : Lvm( -70. ), Hvm(-20. ){};
		base_lsynapse( const base_lsynapse &syn ) : Lvm( syn.Lvm ), Hvm(syn.Hvm ){};
		~base_lsynapse( void ){};
	public: // operators
		base_lsynapse &operator = ( const base_lsynapse &syn )
		{
			Lvm = syn.Lvm;
			Hvm = syn.Hvm;
			return *this;
		};
	public: // data
		double Lvm;
		double Hvm;
};
//////////////////////////////////////////////////////////////////////
// class hhn_synapse
class alignas( 16 ) hhn_synapse : public hhn_process{ 
	public: //--- constructors/destructor
		hhn_synapse( hhn_compart *neuron );
		hhn_synapse( const hhn_synapse &synapse );
		~hhn_synapse( void );
	public: // operators
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
		hhn_synapse &operator = ( const hhn_synapse &synapse );
	public:
		void set_synapse( base_synapse *type );
		void reg_unit( runman *man = NULL );
	private:
		void calc_prevm( double step );
static		void calc_prevm( size_t currstep, double step, hhn_process **start );
	public: // data
		double G;
		double Ginh;
	private:
		double *Gmax;
		double *IonsE;
		hhn_compart *Hhn;	// Point to Hodgkin-Huxley type neuron (in order to receive neuron parameters)
		double NScpE;
};

#endif // __HHN_SYNAPSE_H
