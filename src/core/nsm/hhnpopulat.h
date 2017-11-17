/*****************************************************************
 ****              (C)  1997 - 2000                           ****
 **           Model of Hodgkin-Haxley type neuron               **
 **       Development by Ilia Rybak and Sergey Markin           **
 ****                  Populations                            ****
 *****************************************************************/
#ifndef _HHN_POPULAT_H_
#define _HHN_POPULAT_H_

#include "hhn.h"
#include "hhnconnect.h"
#include "hhntemplate.h"
#include "syntemplate.h"

class hhn_drive;
class CHhnNetwork;
class CHhnControlled;

//--- Model of population of Hodgkin-Haxley type neuron
class hhn_populat : public nn_unit, public hhn_process{
	public: //--- constructor
		hhn_populat( void );
		hhn_populat( const string &name );
		hhn_populat( const hhn_populat &populat );
		hhn_populat( const CHhnNetwork *network, const hhn_populat &populat );
		~hhn_populat( void );
	public: //--- operators
		hhn_populat &operator = ( const hhn_populat &populat );
		hhn_neuron &operator[]( size_t index ){ return Neurons[index]; };
	public:
		bool pre_del( void ); // new code 4 control
	public: //--- Get parameters of the population
		t_hhn *get_neuronT( void ){ return &NeuronT; };
		size_t size( void ) const{ return Neurons.size(); };

		void init( void );
		void reg_unit( runman *man = NULL );
		void *select( unit_code *view );
		void *select( CHhnControlled *ctrl ); 
		void create( const CHhnNetwork *network, const hhn_populat &populat );
		void create( const CHhnNetwork *network, const t_hhn &neuron, unsigned int size );
		bool load( CHhnNetwork *network, istream &file );
		void save( ostream &file );
	protected:
		hhn_synapse *get_syn( int type, int n, int sid );
		void add_inputs( const vector<wconnect> &inputs );
		void add_connection( const vector<nn_unit *> sources, const vector<CConnect> &weights );
	private:
		void set_kgmax( void );
		void setup( const CHhnNetwork *network );
	private:
		void reset( double step );
		void calc_out( double step );
static	void reset( size_t currstep, double step, hhn_process **start );
static	void calc_out( size_t currstep, double step, hhn_process **start );
	public:
		vector<int> Spikes;					//
		vector<hhn_neuron> Neurons;				// Array of neurons for the population
		vector<base_synapse *> Synapses;
		vector<base_dsynapse> SynapsesD;
		vector<base_ssynapse> SynapsesS;
		CHhnNetwork *Network;
	private:
		double KGmax[_id_MAX_CTRL];				// Contolled koeff. for channels Gmax
		double KIonE[_id_MAX_CTRL];				// Contolled koeff. for channels E
		t_hhn NeuronT;							// template for neuron of the population
		vector<base_synapse> _Synapses;			// 
};
#endif
