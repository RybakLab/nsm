/******************************************************************************
 *     file syntemplate.h                                                     *
 *----------------------------------------------------------------------------*
 * Sergey Markin, 2005 University of Manitoba                                 *
 ******************************************************************************/

#ifndef _SYNAPSE_TEMPLATE_H_
#define _SYNAPSE_TEMPLATE_H_

#include "unitemplate.h"

class base_synapse;
class base_dsynapse;
class base_ssynapse;
class base_modssynapse;
class base_lsynapse;
///////////////////////////////////////////////////////////////////////////////
// class base_synapseTemplate
class t_synapse : public uni_template{
	public: //--- constructor
		t_synapse( uni_template *parent, const string &name, int is_active = GRID_COLLAPSE, bool showtype = false );
		t_synapse( const t_synapse &synapse );
virtual		~t_synapse( void ){};
	public:
		t_synapse &operator = ( const t_synapse &synapse );
	public:
		void copy_to( uni_template **unit, uni_template * parent ) final;
		void copy_to( base_synapse **synapse );
	private:
		using uni_template::copy_to;
	protected:
		int get_datattr( const char *path ) final;
		bool get_addparlist( vector<string> &parlist ) final;
		bool get_addpar( const char *path, string &name, string &element, int &type ) final;
		bool upd_addpar( const char *path, const char *name, string &element, bool save ) final;
		//--- Load/save parameters of the synaps
		bool load_addpar( string str, istream &file ) final;
		void save_addpar( ostream &file ) final;
	public:
		string Eds;
		double Gmax;          // Maximal conductance
		double Ts;            // Time constant
		double Vs;            // Amplitude of sinaptic stimulation
};
///////////////////////////////////////////////////////////////////////////////
// class t_ssynapse
class t_ssynapse : public uni_template{
	public:	//--- constructor
		t_ssynapse( uni_template *parent, const string &name, int is_active = GRID_COLLAPSE, bool showtype = false );
		t_ssynapse( const t_ssynapse &synapse );
virtual		~t_ssynapse( void ){};
	public:
		t_ssynapse &operator = ( const t_ssynapse &synapse );
	public:
		void copy_to( uni_template **unit, uni_template *parent ) override;
		void copy_to( base_ssynapse **synapse );
	private:
		using uni_template::copy_to;
	protected:
		int get_datattr( const char *path ) final;
		bool get_addparlist( vector<string> &parlist ) final;
		bool get_addpar( const char *path, string &name, string &element, int &type ) final;
		bool upd_addpar( const char *path, const char *name, string &element, bool save ) final;
		//--- Load/save parameters of the synapse
		bool load_addpar( string str, istream &file ) final;
		void save_addpar( ostream &file ) final;
	public:
		double Hv;
		double Slp;
		double Thr;
};
///////////////////////////////////////////////////////////////////////////////
// class t_modssynapse
class t_modssynapse : public t_ssynapse{
	public:
		t_modssynapse( uni_template *parent, const string &name, int is_active = GRID_COLLAPSE, bool showtype = false ) : t_ssynapse( parent, name, is_active, showtype )
		{
			TypeNameS = TypeName = "ModSigma";
		};
		t_modssynapse( const t_ssynapse &synapse ) : t_ssynapse( synapse ){};
virtual		~t_modssynapse( void ){};
	public:
		t_modssynapse &operator = ( const t_modssynapse &syn ){ t_ssynapse::operator = ( syn ); return *this; }
	public:
		void copy_to( uni_template **unit, uni_template * parent ) final;
		void copy_to( base_modssynapse **synapse );
};
///////////////////////////////////////////////////////////////////////////////
// class t_lsynapse
class t_lsynapse : public uni_template{
	public:	//--- constructor
		t_lsynapse( uni_template *parent, const string &name, int is_active = GRID_COLLAPSE, bool showtype = false );
		t_lsynapse( const t_lsynapse &synapse );
virtual		~t_lsynapse( void ){};
	public:
		t_lsynapse &operator = ( const t_lsynapse &synapse );
	public:
		void copy_to( uni_template **unit, uni_template * parent ) final;
		void copy_to( base_lsynapse **synapse );
	private:
		using uni_template::copy_to;
	protected:
		int get_datattr( const char *path ) final;
		bool get_addparlist( vector<string> &parlist ) final;
		bool get_addpar( const char *path, string &name, string &element, int &type ) final;
		bool upd_addpar( const char *path, const char *name, string &element, bool save ) final;
		//--- Load/save parameters of the synapse
		bool load_addpar( string str, istream &file ) final;
		void save_addpar( ostream &file ) final;
	public:
		double Lvm;
		double Hvm;
};

// temporary solution ------- begin
class t_network : public uni_template{
	public: //--- constructor
		t_network( uni_template *parent, int is_active = GRID_NONE );
		t_network( const t_network &netpar ) : uni_template( netpar ), Threshold( netpar.Threshold ){};
virtual		~t_network( void ){};
	public:
		t_network &operator = ( const t_network &netpar );
	public:
		uni_template *get_template( const char *type, const char *name ) final;
		bool get_addparlist( vector<string> &parlist ) final;
		bool get_addpar( const char *path, string &name, string &element, int &type ) final;
		bool upd_addpar( const char *path, const char *name, string &element, bool save )final;
		void copy_to( uni_template **unit, uni_template * parent ) final;
	public:
		bool add_child( const char *type, const char *name );
	private:
		using uni_template::add_child;
	protected: //--- Load/save parameters of the synaps
		void set_default( void ) final;
		bool load_addpar( string str, istream &file ) final;
		void save_addpar( ostream &file ) final;
	private:
		bool add_ion( int id );
		bool add_pump( int id );
		bool add_syn( int id );
		bool add_sig( int id );
		bool add_modsig( int id );
		bool add_lin( int id );
	public:
		double Threshold;
};
//--------------------------- end

#endif // _SYNAPSE_TEMPLATE_H_
