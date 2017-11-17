/*****************************************************************
 ****              (C)  1997 - 2000                           ****
 **           Model of Hodgkin-Haxley type neuron               **
 **       Development by Ilia Rybak and Sergey Markin           **
 ****                  Neuron template                        ****
 *****************************************************************/
#ifndef _HHN_TEMPLATE_H_
#define _HHN_TEMPLATE_H_

#include "sptypes.h"
#include "chantemplate.h"
#include "syntemplate.h"

class hhn_neuron;
class hhn_compart;
class hhn_populat;
class hhn_ions;
class hhn_pump;

///////////////////////////////////////////////////////////////////////////////
// class t_compart
class t_compart : public uni_template{
friend	class hhn_compart;
	public:	//--- constructor
		t_compart( uni_template *parent, int is_active = GRID_ADD_ROW );
		t_compart( const t_compart &compartment );
virtual	~t_compart( void );
	public:
		t_compart &operator = ( const t_compart &compartment );
	public:
virtual	void copy_to( hhn_compart **compart, hhn_neuron *neuron ){};
		void copy_chan( vector<hhn_channel *> &channels, hhn_compart *cmp );
		void copy_ions( vector<hhn_ions *> &ions, hhn_compart *cmp );
		void copy_pumps( vector<hhn_pump *> &pumps, hhn_compart *cmp );
		bool pre_del( void ); // new code 4 control
	protected:
		int get_datattr( const char *path )
		{
			if( string( path ) == "Membrane area (mm\xB2)" )
				return GRID_POSITIVE|GRID_EXCL_ZERO;
			return uni_template::get_datattr( path );
		}
	public:
		t_data &GetArea( void )
		{
			return Area;
		};
	public:
		bool load_child( string &str, istream &file );
      protected:
		void reconfig( vector<string> &add_paths, vector<string> &del_paths );
		bool add_child( const char *name );
		bool add_child( const char *type, const char *name );
	private:
		bool add_ion( int ionId );
		bool add_pump( int pumpId );
		bool add_chan( int channelId, const char *name );
		bool del_child( const char *name, string &sub_path );
	private:
		t_channel *get_channel( size_t channelId, size_t &num );
	//--- Load/save parameters of the compartment template
	protected:
		bool load_addpar( string str, istream &file );
		void save_addpar( ostream &file );
	public:
	//--- Parameters of Hodgkin-Haxley neuron
		t_data Vm;       // membrane potential (initial)
		t_data Iinj;     // injected current
		t_data Area;       // membrane area
};

///////////////////////////////////////////////////////////////////////////////
// class t_soma
class t_soma : public t_compart{
	public:
		t_soma( uni_template *parent, int is_active = GRID_NONE );
		t_soma( const t_soma &soma );
virtual	~t_soma( void );
	public:
		void copy_to( uni_template **unit, uni_template * parent );
		void copy_to( hhn_compart **compart, hhn_neuron *neuron );
};

///////////////////////////////////////////////////////////////////////////////
// class t_dendr
class t_dendr : public t_compart{
	public:
		t_dendr( uni_template *parent, int is_active = GRID_DEL_ROW );
		t_dendr( const t_dendr &dendrite );
virtual	~t_dendr( void );
	public:
		void copy_to( uni_template **unit, uni_template * parent );
		void copy_to( hhn_compart **compart, hhn_neuron *neuron );
};

///////////////////////////////////////////////////////////////////////////////
// class t_hhn
class t_hhn : public uni_template{
	public:
		t_hhn( uni_template *parent, int is_active = GRID_NONE );
		t_hhn( const t_hhn &neuron );
virtual	~t_hhn( void );
	public:
		t_hhn &operator = ( const t_hhn &neuron );
	public:
		void copy_to( uni_template **unit, uni_template * parent );
		t_data &GetP( void )
		{
			return P;
		};
		t_data &GetCm( void )
		{
			return Cm;
		};
		t_data &get_y( int synapseId )
		{
			return Y[synapseId];
		};
		t_compart *get_compart( size_t compart );
		const t_compart *get_compart( size_t compart ) const;
		bool pre_del( void ); // new code 4 control
      //--- Load/save parameters of the neuron template
	protected:
		bool add_child( const char *name );
		int get_datattr( const char *path )
		{
		#if defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
			if( string( path ) == "Membrane capacitance (\xB5 F)" || string( path ) == "P" )
				return GRID_POSITIVE|GRID_EXCL_ZERO;
		#else
			if( string( path ) == "Specific membrane capacitance (\xB5 F/sm\xB2)" || string( path ) == "P" )
				return GRID_POSITIVE|GRID_EXCL_ZERO;
		#endif // defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )

			return uni_template::get_datattr( path );
		};
		bool load_addpar( string str, istream &file );
		void save_addpar( ostream &file );
	protected:
	//--- Parameters of Hodgkin-Haxley neuron
		t_data P;
		t_data Y[_id_MAX_SYN]; // outputs for other neurons (0 - ex, 1 - inhA, 2- inhB)
		t_data Cm;             // membrane capacitance
};

///////////////////////////////////////////////////////////////////////////////
// class t_populat
class t_populat : public uni_template{
	public:
		t_populat( uni_template *parent, int is_active = GRID_DEL_ROW );
		t_populat( const t_populat &populat );
virtual	~t_populat( void );
	public:
		t_populat &operator = ( const t_populat &populat );
	public:
		void copy_to( uni_template **unit, uni_template * parent );
	protected:
		bool add_child( const char *name );
		bool get_addparlist( vector<string> &parlist );
		bool get_addpar( const char *path, string &name, string &element, int &type );
		bool upd_addpar( const char *path, const char *name, string &element, bool save );
		bool load_addpar( string str, istream &file );
		void save_addpar( ostream &file );
// temporary solution ------- begin
	protected:
		size_t Size;
	public:
		size_t GetSize( void ){ return Size; };
		t_hhn *get_neuronT( void )
		{ 
			return ( t_hhn *)Children[0]; 
		};
		void CopyFrom( hhn_populat *populate );
	public:
//--------------------------- end
};

#endif
