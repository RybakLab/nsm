/////////////////////////////////////////////////////////////////////////////
// hhn.h
#ifndef __HHN_H
#define __HHN_H

#include <cassert>

#include "hhnions.h"
#include "hhnsynapse.h"
#include "hhnunit.h"

class t_hhn;
class t_compart;
class hhn_populat;
class hhn_neuron;
class hhn_channel;
class hsoma_channel;
class hdendr_channel;

/////////////////////////////////////////////////////////////////////////////
// class hhn_compart
class alignas( 16 ) hhn_compart : public hhn_process{
	public:
		hhn_compart( hhn_neuron *neuron );
		hhn_compart( const hhn_compart &compartment );
virtual		~hhn_compart( void );
	public:
		hhn_compart &operator = ( const hhn_compart &compartment );
		hhn_channel *operator []( size_t index ) const;
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
	public:
		const char *get_name( void ) const{ return Name.c_str(); };
		double &get_vm( void ){ return Vm; };
		size_t size( void ) const{ return Chan.size();};
		bool create( t_compart *templ, hhn_populat *populat  );
		//--- overrided function
		void reg_unit( runman *man ) override;
		bool init( void ) final;
virtual		void copy_to( hhn_compart **part ){};
	protected:
virtual		void create_chan( t_compart *tcomp );
		void create_syn( void );
		void create_ions( t_compart *tcomp );
		void clear_all( void );
	protected:
static		void calc_vm( size_t currstep, double step, hhn_process **start );
		void calc_vm( double step, double thresh );
	public:
		int CompartId;
		double Ipumps;
		double Iinj;
		double G;
		double GE;
		double Cm;			// membrane capacitance
		double Vm;			// membrane potential (previouse - 0, current - 1)

		hna_ions *NaIons;		// Concentration of the Na ions
		hk_ions  *KIons;		// Concentration of the K ions
		hca_ions *CaIons;		// Concentration of the Ca ions
		hcl_ions *ClIons;		// Concentration of the Cl ions

		nsm_vector(hhn_ions *) Ions;	//
		nsm_vector(hhn_pump *) Pumps;	//

		hhn_populat *Populat;
		nsm_vector(hhn_synapse *) Syn;	// synapses of the neuron (0 - ex, 1 - inhA, 2 - inhB)
#if defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
		double Area;
#endif // defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	protected:
		double P;			// the ratio of somatic surface to total cell surface area
		string Name;
		hhn_neuron *Neuron;
		t_compart *NeuronT;
		nsm_vector(hhn_channel *) Chan;	// Ion cahnnels
};

/////////////////////////////////////////////////////////////////////////////
// class hhn_soma
class alignas( 16 ) hhn_soma : public hhn_compart{
	public:
		hhn_soma( hhn_neuron *neuron );
		hhn_soma( const hhn_soma &soma );
		~hhn_soma( void );
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
	public:
		void copy_to( hhn_compart **part ) final;
		void reg_unit( runman *man ) final;
	protected:
		void create_chan( t_compart *tcomp ) final;
	private:
static		void calc_vm( size_t currstep, double step, hhn_process **start );
		void calc_vm( double step, double thresh );

};

/////////////////////////////////////////////////////////////////////////////
// class hhn_dendr
class alignas( 16 ) hhn_dendr : public hhn_compart{
	public:
		hhn_dendr( hhn_neuron *neuron );
		hhn_dendr( const hhn_dendr &dendr );
		~hhn_dendr( void );
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
	public:
		void copy_to( hhn_compart **part ) final;
	protected:
		void create_chan( t_compart *tcomp ) final;
};

/////////////////////////////////////////////////////////////////////////////
// class hhn_neuron
class alignas( 16 ) hhn_neuron : public hhn_process{
	public:
		enum presyn_proc{
			presyn_spike = 0,
			presyn_sigma,
			presyn_modsigma,
			presyn_lin,
			presyn_max,
			presyn_no,
		};
	public:
		hhn_neuron( hhn_populat *populat = NULL );
		hhn_neuron( const hhn_neuron &hhn );
		~hhn_neuron( void );
	public:
		hhn_neuron &operator = ( const hhn_neuron &hhn );
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
	public:
		size_t size( void ) const{ return Compart.size(); };
		const hhn_compart *get_compart( size_t id ) const{ hhn_compart *cmp = ( id < Compart.size())? Compart[id]: NULL; return cmp; };
		double *get_y( int id ) final{ assert( id != -1 ); return &Output[id]; };
		void copy_to( hhn_neuron **neuron );
		void reg_unit( int index, runman *man );

		bool init( void ) final;
		void prerun( double step ) final;
		int retrace( int synid, bool sigma, bool modsigma, bool linear ) final;
	private:
		using hhn_process::reg_unit;
	protected:
		void create_comp( void );
	private:
		void pre_y( double step );
static		void pre_y( size_t currstep, double step, hhn_process **start );
	public:
		alignas( 16 ) int *Spike;				// then spike is generated turn on 1, otherwise to 0
		alignas( 16 ) hhn_populat *Populat;
		alignas( 16 ) nsm_vector(hhn_compart *) Compart;
	private: // outputs
		alignas( 16 ) int SynProc[_id_MAX_SYN*presyn_max];	// encode the type of presynaptic processing
		alignas( 16 ) double H[_id_MAX_SYN*presyn_max];		// depression
		alignas( 16 ) double Y[_id_MAX_SYN*presyn_max];		// output
		alignas( 16 ) double Output[_id_MAX_SYN*presyn_max];	// output for other neurons (y*h)
	private: // parameters
		alignas( 16 ) bool IsH;					// true if synaptic depression
		alignas( 16 ) double Kd;				// magnitude of depression
		alignas( 16 ) double Td;				// time constant of  depression
	private: // precomputed variables
alignas( 16 )   static double EXPT[_id_MAX_SYN];
alignas( 16 )   static double VS[_id_MAX_SYN];

alignas( 16 )   static double HV[_id_MAX_SYN];
alignas( 16 )   static double SLP[_id_MAX_SYN];
alignas( 16 )   static double THR[_id_MAX_SYN];

alignas( 16 )   static double MODHV[_id_MAX_SYN];
alignas( 16 )   static double MODSLP[_id_MAX_SYN];
alignas( 16 )   static double MODTHR[_id_MAX_SYN];
alignas( 16 )   static double MODSCALE[_id_MAX_SYN];

alignas( 16 )   static double Lvm[_id_MAX_SYN];
alignas( 16 )   static double Hvm[_id_MAX_SYN];
alignas( 16 )   static double LINSLOPE[_id_MAX_SYN];
};

#endif // __HHN_H
