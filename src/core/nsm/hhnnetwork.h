//////////////////////////////////////////////////////////////////////
// hhnnetwork.h
#ifndef __HHN_NETWORK_H
#define __HHN_NETWORK_H

#include "hhnunit.h"
#include "hhnconnect.h"
#include "hhndrive.h"
#include "hhnfeedback.h"
#include "hhnpopulat.h"
#include "hhnoutput.h"
#include "hhncontrol.h"
#include "walker.h"
#include "syntemplate.h"
#include "frameview.h"
#include "runman.h"

class netuni_templ{
		class __id{
			public:
				__id( void ) : Id(-1), Links( 0 ){};
				__id( const __id &id ) : Id( id.Id ), Links( id.Links ){};
				~__id( void ){};
			public:
				__id &operator = ( const __id &id ){ Id = id.Id; Links = id.Links; return *this; };
				__id &operator = ( int id ){ Id = id; return *this; };
				__id &operator ++ ( void ){ Links++; return *this; };
				__id &operator ++ ( int ){ Links++; return *this; };
				__id &operator -- ( void ){ Links--; return *this; };
				__id &operator -- ( int ){ Links--; return *this; };
			public:
				bool empty( void ){ return bool( Links == 0 ); };
			public:
				int Id;
				int Links;
		};
	public:
		netuni_templ( void );
		netuni_templ( const netuni_templ &t );
		~netuni_templ( void );
	public:
		netuni_templ &operator = ( const netuni_templ &t );
	public:
		string getname( int id )  const; // new code 4 control
		int getid( const string &name ) const; // new code 4 control
		bool add( const string &name ); // new code 4 control
		bool del( const string &name ); // new code 4 control
		bool rename( const string &oldname, const string &newname ); // new code 4 control
	public:
		bool load( istream &file );
		void save( ostream &file );
	private:
		int make_newid( void );
		bool remove( map<string,__id>::iterator pos );
		bool load_chan( istream &file );
	private:
		map<string,__id> ChanList;		// new code 4 control
};

//--- Model of neural network based on Hodgkin-Haxley type neuron
class CHhnNetwork : public nn_unit{
	public:
		CHhnNetwork( void );
		CHhnNetwork( CHhnNetwork &network );
		~CHhnNetwork( void );
	public:
		CHhnNetwork &operator = ( const CHhnNetwork &network );
	public:
		base_synapse &synapse( size_t id ){ return *( Synapses[id] ); }; 
		size_t size_pop( void ) const{ return Populat.size(); };
		size_t size_drv( void ) const{ return Drive.size(); };
		size_t size_out( void ) const{ return Output.size(); };
		size_t size_ctr( void ) const{ return Control.size(); };
		hhn_populat &get_pop( size_t inx ){ return Populat[inx]; };
		hhn_drive &get_drv( size_t inx ){ return Drive[inx];};
		hhn_output &get_out( size_t inx ){ return Output[inx]; };
		hhn_control &get_ctr( size_t inx ){ return Control[inx]; };
		hna_ions *get_naions( void ){ return ( hna_ions *)Ions[_id_Na_Ion]; };
		hk_ions *get_kions( void ){ return ( hk_ions *)Ions[_id_K_Ion]; };
		hca_ions *get_caions( void ){ return ( hca_ions *)Ions[_id_Ca_Ion]; };
		hcl_ions *get_clions( void ){ return ( hcl_ions *)Ions[_id_Cl_Ion]; };

		bool add_pop( const hhn_populat &populat );
		bool set_pop( const hhn_populat &populat, size_t inx );
		bool del_pop( size_t index );
		bool add_drv( const hhn_drive &drive );
		bool set_drv( const hhn_drive &drive, size_t inx );
		bool del_drv( size_t inx );
		bool add_out( const hhn_output &output );
		bool set_out( const hhn_output &output, size_t inx );
		bool del_out( size_t inx );
		bool add_ctr( const hhn_control &control );
		bool set_ctr( const hhn_control &control, unsigned int index );
		bool del_ctr( size_t index );
		bool load( istream &file, CSimulate * manager );
		void save( ostream &file, CSimulate * manager );
		void diff( const string &p1, const string &p2, const string &expr, ostream &log );

		hhn_pair<int> get_uid( const string &name )const;
		nn_unit *get_nnunit( const unit_code &code );
		nn_unit *get_nnunit( const string &name );
		hhn_control *get_crtunit( const unit_code &code );
		hhn_control *get_crtunit( const string &name );
		CConnectMatrix &nnconnect( void ){ return NNConnect; };
		void *select( CHhnControlled *ctrl );
		void init( long seed, runman *man = NULL, bool rand = false );
	private:
		void init_syn( void );
		void init_ions( void );
		void clear( void );
		void compare( const multimap<string,string> &unitsmap, ostream &log );
		void compare( const multimap<string, string>::const_iterator &n1, const multimap<string, string>::const_iterator &n2, ostream &log );
		bool compare_units( const nn_unit *u1, const nn_unit *u2, ostream &log );
		bool compare_connections( const nn_unit *u1, const nn_unit *u2, const string &key, ostream &log );
		bool LoadNNUnits( istream &file );
		void SaveNNUnits( ostream &file );
		bool LoadConnections( istream &file );
		void SaveConnections( ostream &file );
		bool LoadSource( const string &src, istream &file );
	private:
		CConnectMatrix NNConnect;		// Matrix of links between NN elements:
		vector<hhn_drive> Drive;		// Array of drive elements (ONLY SOURCES)
		vector<hhn_output> Output;		// Array of output elements (SOURCES & TARGETS)
		vector<hhn_populat> Populat;	// Array of populations for the network (SOURCES & TARGETS)
		vector<hhn_control> Control;
		vector<base_synapse *> Synapses;// Parameters of excitatory synapse
		vector<hhn_ions *>Ions;
		runman *RunMan;
	public:
		double Threshold;
	public:
#ifdef __MECHANICS__
		size_t size_fbk( void ) const{ return Feedback.size(); };
		hhn_feedback &get_fbk( size_t inx ){ return Feedback[inx]; };
		bool add_fbk( const hhn_feedback &feedback );
		bool set_fbk( const hhn_feedback &feedback, size_t inx );
		bool del_fbk( size_t inx );
	private:
		vector<hhn_feedback> Feedback; // Array of feedbacks for the network (ONLY SOURCES)
#endif // __MECHANICS__
// temporary solution ------- begin
     public:
		netuni_templ TUniList;		// new code 4 control
		t_network NetParam;
//--------------------------- end
};

#endif // __HHN_NETWORK_H
