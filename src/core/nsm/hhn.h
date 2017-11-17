/////////////////////////////////////////////////////////////////////////////
// hhn.h
#ifndef __HHN_H
#define __HHN_H

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
class hhn_compart : public hhn_process{
	public:
		hhn_compart( hhn_neuron *neuron );
		hhn_compart( const hhn_compart &compartment );
virtual	~hhn_compart( void );
	public:
		hhn_compart &operator = ( const hhn_compart &compartment );
		hhn_channel *operator []( size_t index ) const;
	public:
		const char *get_name( void ) const{ return Name.c_str(); };
		size_t size( void ) const{ return Chan.size();};
		bool init( void );
		bool create( t_compart *templ, hhn_populat *populat  );
      //--- overrided function
		void reg_unit( runman *man = NULL );
virtual	void copy_to( hhn_compart **part ){};
	protected:
virtual	void create_chan( t_compart *tcomp );
		void create_syn( void );
		void create_ions( t_compart *tcomp );
		void clear_all( void );
	protected:
static	void reset( size_t currstep, double step, hhn_process **start );
static	void calc_vm( size_t currstep, double step, hhn_process **start );
		void reset( void );
		void calc_vm( double step, double thresh );
	public:
		int CompartId;
		double Ipumps;
		double Iinj;
		double G;
		double GE;
		double Cm;                      // membrane capacitance
		double Vm[2];                   // membrane potential (previouse - 0, current - 1)

		hna_ions *NaIons;                // Concentration of the Na ions
		hk_ions  *KIons;                 // Concentration of the K ions
		hcl_ions *ClIons;                // Concentration of the Cl ions
		hca_ions *CaIons;                // Concentration of the Ca ions

		vector<hhn_ions *> Ions;		 //
		vector<hhn_pump *> Pumps;		 //

		hhn_populat *Populat;
		vector<hhn_synapse *>   Syn;    // synapses of the neuron (0 - ex, 1 - inhA, 2 - inhB)
		vector<base_dsynapse *> DSyn;   // synapses of the neuron (0 - ex, 1 - inhA, 2 - inhB)
		vector<base_ssynapse *> SSyn;   // synapses of the neuron (0 - ex, 1 - inhA, 2 - inhB)
		double Area;
	protected:
		double P;                            // the ratio of somatic surface to total cell surface area
		string Name;
		hhn_neuron *Neuron;
		t_compart *NeuronT;
		vector<hhn_channel *> Chan;   // Ion cahnnels
};

/////////////////////////////////////////////////////////////////////////////
// class hhn_soma
class hhn_soma : public hhn_compart{
	public:
		hhn_soma( hhn_neuron *neuron );
		hhn_soma( const hhn_soma &soma );
		~hhn_soma( void );
	public:
		void copy_to( hhn_compart **part );
		void reg_unit( runman *man = NULL );
	protected:
		void create_chan( t_compart *tcomp );
	private:
static	void calc_vm( size_t currstep, double step, hhn_process **start );
		void calc_vm( double step, double thresh );

};

/////////////////////////////////////////////////////////////////////////////
// class hhn_dendr
class hhn_dendr : public hhn_compart{
	public:
		hhn_dendr( hhn_neuron *neuron );
		hhn_dendr( const hhn_dendr &dendr );
		~hhn_dendr( void );
	public:
		void copy_to( hhn_compart **part );
	protected:
		void create_chan( t_compart *tcomp );
};

/////////////////////////////////////////////////////////////////////////////
// class hhn_neuron
class hhn_neuron : public hhn_process{
	public:
		hhn_neuron( hhn_populat *populat = NULL );
		hhn_neuron( const hhn_neuron &hhn );
virtual	~hhn_neuron( void );
	public:
		hhn_neuron &operator = ( const hhn_neuron &hhn );
	public:
		size_t size( void ) const{ return Compart.size(); };
		const hhn_compart *get_compart( size_t id ) const{ hhn_compart *cmp = ( id < Compart.size())? Compart[id]: NULL; return cmp; };
		double *get_y( int id ){ return &Output[id]; };
		double *get_hy( int id ){ return &OutputD[id]; };
		void retrace( int yid = -1, int sid = -1, int hid = -1 );
		void init( void );
		void reg_unit( int index, runman *man = NULL );
		void copy_to( hhn_neuron **neuron );
	protected:
		void create_comp( void );
	private:
		void pre_y( double step, double vs[_id_MAX_SYN] );
		void pre_s( double step, double thresh,	double hv[_id_MAX_SYN],	double sl[_id_MAX_SYN] );
		void pre_h( double step, double kd[_id_MAX_SYN], double td[_id_MAX_SYN] );
		void post_y( double step, double expt[_id_MAX_SYN] );

static	void pre_y( size_t currstep, double step, hhn_process **start );
static	void pre_s( size_t currstep, double step, hhn_process **start );
static	void pre_h( size_t currstep, double step, hhn_process **start );
static	void post_y( size_t currstep, double step, hhn_process **start );
	public:
		int *Spike;					// then spike is generated turn on 1
		bool IsY[_id_MAX_SYN];
		bool IsS[_id_MAX_SYN];
		bool IsH[_id_MAX_SYN];			// 
		double Y[_id_MAX_SYN];			// output for other neurons
		double H[_id_MAX_SYN];			// output for other neurons
		hhn_populat *Populat;
		vector<hhn_compart *> Compart;
	private:
		double Output[_id_MAX_SYN];
		double OutputD[_id_MAX_SYN];
};

#endif // __HHN_H
