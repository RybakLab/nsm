/******************************************************************************
 * File: iontemplate.h                                                        *
 *----------------------------------------------------------------------------*
 * Sergey Markin, 2006 KRINC                                                  *
 ******************************************************************************/
#ifndef _ION_TEMPLATE_H_
#define _ION_TEMPLATE_H_

#include "config.h"
#include "hhnions.h"
#include "unitemplate.h"

///////////////////////////////////////////////////////////////////////////////
// class t_ions
class t_ions : public uni_template{
	public:	//--- constructor
		t_ions( uni_template *parent, int is_active = GRID_COLLAPSE, bool showtype = false );
		t_ions( const t_ions &ion );
virtual ~t_ions( void );
	public:
		t_ions &operator = ( const t_ions &ions );
	public:	//--- overrided function
		bool process( const char *path );
virtual void copy_to( uni_template **unit, uni_template * parent ){};
virtual void copy_to( hhn_process **unit, hhn_process *parent ){};
	protected:	//--- overrided function
		int get_datattr( const char *path );
		bool get_addparlist( vector<string> &parlist );
		bool get_addpar( const char *path, string &name, string &element, int &type );
		bool upd_addpar( const char *path, const char *name, string &element, bool save );
      //--- Load/save parameters of the synaps
virtual	bool load_addpar( string str, istream &file );
virtual	void save_addpar( ostream &file );
	protected:
		void copy_to( hhn_ions *ion );
		void calc_eds( void );
	public: // hidden parameters
		t_data Z;	// The valence of ion
		t_data RTF;	// RT/F where 
					// R = The ideal gas constant (joules per kelvin per mole)
					// T = The temperature in Kelvin
					// F = Faraday's constant (coulombs per mole)
	public: // accessible parameters
		bool Adjustable;
		t_data In;	// Ions concentration inside cell
		t_data Out;	// Ions concentration outside cell
		t_data Eds;	// Nernst potential

		bool Dynamics;
		t_data PumpR;	// Pump rate
		t_data PumpT;	// Pump time constant
		t_data IonsR;	// Ions current rate
};

///////////////////////////////////////////////////////////////////////////////
// class tna_ions
class tna_ions : public t_ions{ 
	public: //--- constructor
		tna_ions( uni_template *parent, int is_active = GRID_COLLAPSE, bool showtype = false );
	public: //--- overrided function
		void copy_to( uni_template **unit, uni_template * parent );
		void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class tk_ions
class tk_ions : public t_ions{ 
	public: //--- constructor
		tk_ions( uni_template *parent, int is_active = GRID_COLLAPSE, bool showtype = false );
	public:	//--- overrided function
		void copy_to( uni_template **unit, uni_template * parent );
		void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class tca_ions
class tca_ions : public t_ions{ 
	public: //--- constructor
		tca_ions( uni_template *parent, int is_active = GRID_COLLAPSE, bool showtype = false );
		tca_ions( const tca_ions &ions );
	public:
		tca_ions &operator = ( const tca_ions &ions );
	public: //--- overrided function
		void copy_to( uni_template **unit, uni_template * parent );
		void copy_to( hhn_process **unit, hhn_process *parent );
#ifndef __LOCOMOTION__
	public: // accessible parameters
		t_data B;
		t_data K;
#endif // __LOCOMOTION__
};

///////////////////////////////////////////////////////////////////////////////
// class tcl_ions
class tcl_ions : public t_ions{ 
	public: //--- constructor
		tcl_ions( uni_template *parent, int is_active = GRID_COLLAPSE, bool showtype = false );
	public: //--- overrided function
virtual void copy_to( uni_template **unit, uni_template *parent );
virtual void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class t_pumps
class t_pumps : public uni_template{
	public: //--- constructor
		t_pumps( uni_template *parent, int is_active = GRID_COLLAPSE, bool showtype = false );
		t_pumps( const t_pumps &pump );
virtual ~t_pumps( void );
	public:
		t_pumps &operator = ( const t_pumps &pump );
	public:
		t_data PumpRV;	// Pump current rate (involved into dynamics of membrane potential)
};

///////////////////////////////////////////////////////////////////////////////
// class tnak_pump
class tnak_pump : public t_pumps{
	public: //--- constructor
		tnak_pump( uni_template *parent, int is_active = GRID_COLLAPSE, bool showtype = false );
		tnak_pump( const tnak_pump &pump );
virtual ~tnak_pump( void );
	public:
		tnak_pump &operator = ( const tnak_pump &pump );
	public: //--- overrided function
//		int get_datattr( const char *path );
virtual void copy_to( uni_template **unit, uni_template *parent );
virtual void copy_to( hhn_process **unit, hhn_process *parent );
	public:
		t_data Na0;
		t_data Rp;
		t_data Kp;
};

///////////////////////////////////////////////////////////////////////////////
// class tca_pump
class tca_pump : public t_pumps{
	public: //--- constructor
		tca_pump( uni_template *parent, int is_active = GRID_COLLAPSE, bool showtype = false );
		tca_pump( const tca_pump &pump );
virtual ~tca_pump( void );
	public:
		tca_pump &operator = ( const tca_pump &pump );
	public: //--- overrided function
virtual void copy_to( uni_template **unit, uni_template *parent );
virtual void copy_to( hhn_process **unit, hhn_process *parent );
	public:
		t_data Ca0;
};

#endif // _ION_TEMPLATE_H_
