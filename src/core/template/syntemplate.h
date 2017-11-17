/******************************************************************************
 *     file unitemplate.h                                                     *
 *----------------------------------------------------------------------------*
 * Sergey Markin, 2005 University of Manitoba                                 *
 ******************************************************************************/

#ifndef _SYNAPSE_TEMPLATE_H_
#define _SYNAPSE_TEMPLATE_H_

#include "unitemplate.h"

class base_synapse;
class base_dsynapse;
class base_ssynapse;
//class t_ions;
///////////////////////////////////////////////////////////////////////////////
// class base_synapseTemplate
class t_synapse : public uni_template{
      public:
      //--- constructor
            t_synapse( uni_template *parent, const string &name, int is_active = GRID_COLLAPSE, bool showtype = false );
            t_synapse( const t_synapse &synapse );
virtual     ~t_synapse( void ){};
      public:
            t_synapse &operator = ( const t_synapse &synapse );
      public:
            void copy_to( uni_template **unit, uni_template * parent );
            void copy_to( base_synapse **synapse );

      protected:
            int get_datattr( const char *path );
            bool get_addparlist( vector<string> &parlist );
            bool get_addpar( const char *path, string &name, string &element, int &type );
            bool upd_addpar( const char *path, const char *name, string &element, bool save );
      //--- Load/save parameters of the synaps
            bool load_addpar( string str, istream &file );
            void save_addpar( ostream &file );
      public:
            string Eds;
            double Gmax;          // Maximal conductance
            double Ts;            // Time constant
            double Vs;            // Amplitude of sinaptic stimulation
      };

///////////////////////////////////////////////////////////////////////////////
// class t_dsynapse
class t_dsynapse : public uni_template{
      public:
      //--- constructor
            t_dsynapse( uni_template *parent, const string &name, int is_active = GRID_COLLAPSE, bool showtype = false );
            t_dsynapse( const t_dsynapse &synapse );
virtual     ~t_dsynapse( void ){};
      public:
            t_dsynapse &operator = ( const t_dsynapse &synapse );
      public:
            void copy_to( uni_template **unit, uni_template * parent );
            void copy_to( base_dsynapse **synapse );

      protected:
            int get_datattr( const char *path );
            bool get_addparlist( vector<string> &parlist );
            bool get_addpar( const char *path, string &name, string &element, int &type );
            bool upd_addpar( const char *path, const char *name, string &element, bool save );
      //--- Load/save parameters of the synaps
            bool load_addpar( string str, istream &file );
            void save_addpar( ostream &file );
      public:
            double Kdep;
            double Tdep;      
      };

///////////////////////////////////////////////////////////////////////////////
// class t_ssynapse
class t_ssynapse : public uni_template{
	public:
	//--- constructor
		t_ssynapse( uni_template *parent, const string &name, int is_active = GRID_COLLAPSE, bool showtype = false );
		t_ssynapse( const t_ssynapse &synapse );
virtual	~t_ssynapse( void ){};
	public:
		t_ssynapse &operator = ( const t_ssynapse &synapse );
	public:
		void copy_to( uni_template **unit, uni_template * parent );
		void copy_to( base_ssynapse **synapse );
	protected:
		int get_datattr( const char *path );
		bool get_addparlist( vector<string> &parlist );
		bool get_addpar( const char *path, string &name, string &element, int &type );
		bool upd_addpar( const char *path, const char *name, string &element, bool save );
	//--- Load/save parameters of the synaps
		bool load_addpar( string str, istream &file );
		void save_addpar( ostream &file );
	public:
		double Hv;
		double Slp;
};

// temporary solution ------- begin
class t_network : public uni_template{
	public:   //--- constructor
		t_network( uni_template *parent, int is_active = GRID_NONE );
		t_network( const t_network &netpar ) : uni_template( netpar ), Threshold( netpar.Threshold ){};
virtual	~t_network( void ){};
	public:
		t_network &operator = ( const t_network &netpar );
	public:
		uni_template *get_template( const char *type, const char *name );
		bool get_addparlist( vector<string> &parlist );
		bool get_addpar( const char *path, string &name, string &element, int &type );
		bool upd_addpar( const char *path, const char *name, string &element, bool save );
		void copy_to( uni_template **unit, uni_template * parent );
	public:
		bool add_child( const char *type, const char *name );
	protected:
	//--- Load/save parameters of the synaps
		bool load_addpar( string str, istream &file );
		void save_addpar( ostream &file );
	private:
		bool add_ion( int id );
		bool add_pump( int id );
		bool add_syn( int id );
		bool add_dep( int id );
		bool add_sig( int id );
	public:
		double Threshold;
};
//--------------------------- end

#endif // _SYNAPSE_TEMPLATE_H_
