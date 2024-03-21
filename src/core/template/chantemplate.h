/******************************************************************************
 * File: chantemplate.h                                                        *
 *----------------------------------------------------------------------------*
 * Sergey Markin, 2005 University of Manitoba                                 *
 ******************************************************************************/
#ifndef _CHANNEL_TEMPLATE_H_
#define _CHANNEL_TEMPLATE_H_

#include "hhnchan.h"
#include "unitemplate.h"

extern string get_gchanname( int id ); // new code 4 control
extern int get_gchanid( const string &name ); // new code 4 control
extern bool add_gchan( const string &name ); // new code 4 control
extern bool del_gchan( const string &name ); // new code 4 control
extern bool rename_gchan( const string &oldname, const string &newname ); // new code 4 control

class hhn_compart;

///////////////////////////////////////////////////////////////////////////////
// class t_channel
class t_channel : public uni_template{   
	public: //--- constructor
		t_channel( uni_template *parent, int is_active = GRID_ADD_ROW );
		t_channel( const t_channel &channel );
virtual		~t_channel( void ){};
	public:
		t_channel &operator = ( const t_channel &channel );
	public:
virtual		void copy_to( hhn_process **unit, hhn_process *parent ){};
virtual		size_t get_iontype( void ){ return 0xFF; };
virtual		size_t get_pumptype( void ){ return 0xFF; };
      protected:
virtual		int get_datattr( const char *path ){ return GRID_DOUBLE; };
virtual		bool load_addpar( string str, istream &file );
virtual		void save_addpar( ostream &file );
	protected:
		t_data Gmax;
		t_data M;
		t_data H;
		t_data Tm;
		t_data Th;
};

///////////////////////////////////////////////////////////////////////////////
// class tgen_channel
class tgen_channel : public t_channel{   
	public: //--- constructor
		tgen_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		tgen_channel( const tgen_channel &channel );
virtual		~tgen_channel( void ){}; 
	public:
		tgen_channel &operator = ( const tgen_channel &channel );
	public: //--- overrided function
		size_t get_iontype( void );
		size_t get_pumptype( void );
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
		bool pre_add( void ); // new code 4 control
		bool pre_del( void ); // new code 4 control
		bool process( const char *path );
	protected:
virtual		int get_datattr( const char *path );
virtual		bool get_addparlist( vector<string> &parlist );
virtual		bool get_addpar( const char *path, string &name, string &element, int &type );
virtual		bool upd_addpar( const char *path, const char *name, string &element, bool save );
virtual		bool load_addpar( string str, istream &file );
virtual		void save_addpar( ostream &file );
	protected:
		bool ModI;
		string Eds;
		t_data Km;
		t_data Hvm;
		t_data Slpm;
		t_data Tm0;
		t_data Hvtm;
		t_data Slptm;
		t_data PowM;
		t_data Kh;
		t_data Hvh;
		t_data Slph;
		t_data Th0;
		t_data Hvth;
		t_data Slpth;
		t_data PowH;
		t_data Kpump;
};

///////////////////////////////////////////////////////////////////////////////
// class tgna_channel
class tgna_channel : public t_channel{   
	public: //--- constructor
		tgna_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		tgna_channel( const tgna_channel &channel );
virtual		~tgna_channel( void ){}; 
	public:
		tgna_channel &operator = ( const tgna_channel &channel );
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
	protected:
		t_data Hvm;
		t_data Slpm;
		t_data Tm0;
		t_data Hvtm;
		t_data Slptm;
		t_data Slptm_1;
		t_data PowM;

		t_data Hvh;
		t_data Slph;
		t_data Th0;
		t_data Hvth;
		t_data Slpth;
		t_data Slpth_1;
		t_data PowH;     
};

///////////////////////////////////////////////////////////////////////////////
// class tgnap_channel
class tgnap_channel : public tgna_channel{   
	public: //--- constructor
		tgnap_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		tgnap_channel( const tgnap_channel &channel );
virtual		~tgnap_channel( void ){}; 
	public:
		tgnap_channel &operator = ( const tgnap_channel &channel );
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class tgkdr_channel
class tgkdr_channel : public t_channel{   
	public: //--- constructor
		tgkdr_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		tgkdr_channel( const tgkdr_channel &channel );
virtual		~tgkdr_channel( void ){}; 
	public:
		tgkdr_channel &operator = ( const tgkdr_channel &channel );
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
	protected:
		t_data Hvm;
		t_data Slpm;
		t_data PowM;
		t_data Tm0;
		t_data Hvtm;
		t_data Slptm;
		t_data Slptm_1;
};

///////////////////////////////////////////////////////////////////////////////
// class tgka_channel
class tgka_channel : public t_channel{   
	public: //--- constructor
		tgka_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		tgka_channel( const tgka_channel &channel );
virtual		~tgka_channel( void ){}; 
	public:
		tgka_channel &operator = ( const tgka_channel &channel );
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
	protected:
		t_data Hvm;
		t_data Slpm;
		t_data Tm0;
		t_data Hvtm;
		t_data Slptm;
		t_data Slptm_1;
		t_data PowM;

		t_data Hvh;
		t_data Slph;
		t_data Th0;
		t_data Hvth;
		t_data Slpth;
		t_data Slpth_1;
		t_data PowH;     
};

///////////////////////////////////////////////////////////////////////////////
// class tgkca_channel
class tgkca_channel : public t_channel{  
	public: //--- constructor
		tgkca_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		tgkca_channel( const tgkca_channel &channel );
virtual		~tgkca_channel( void ){};
	public:
		tgkca_channel &operator = ( const tgkca_channel &channel );
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
	protected:
		t_data Alpha;      // Initial conditionns
		t_data Lymbda;        // Initial conditionns
		t_data Pow;
};

///////////////////////////////////////////////////////////////////////////////
// class tgcan_channel
class tgcan_channel : public tgkca_channel{  
	public: //--- constructor
		tgcan_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		tgcan_channel( const tgcan_channel &channel );
virtual		~tgcan_channel( void ){};
	public:
		tgcan_channel &operator = ( const tgcan_channel &channel );
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
	protected:
		t_data Ecan;       // Initial conditionns
};

///////////////////////////////////////////////////////////////////////////////
// class tgh_channel
class tgh_channel : public t_channel{   
	public: //--- constructor
		tgh_channel( uni_template *parent, int is_active = GRID_DEL_ROW  );
		tgh_channel( const tgh_channel &channel );
virtual		~tgh_channel( void ){};
	public:
		tgh_channel &operator = ( const tgh_channel &channel );
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
	protected:
virtual		int get_datattr( const char *path )
		{
			if( string( path ) == "Reversal potential (mV)" )
				return GRID_NONE;
			return t_channel::get_datattr( path );
		};
	protected:
		t_data Eh;
		t_data Hvh;
		t_data Slph;
		t_data Hvth;
		t_data Slpth;
};

#if defined( __LOCOMOTION__ ) || defined( __RESPIRATION__ )
///////////////////////////////////////////////////////////////////////////////
// class tna_channel
class tna_channel : public t_channel{   
	public: //--- constructor
		tna_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		tna_channel( const tna_channel &channel );
virtual		~tna_channel( void ){}; 
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
	};

///////////////////////////////////////////////////////////////////////////////
// class tk_channel
class tk_channel : public t_channel{   
	public: //--- constructor
		tk_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		tk_channel( const tk_channel &channel );
virtual		~tk_channel( void ){};
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class tnaf_channel
class tnaf_channel : public tna_channel{   
	public: //--- constructor
		tnaf_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		tnaf_channel( const tnaf_channel &channel );
virtual		~tnaf_channel( void ){}; 
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class tnap_channel
class tnap_channel : public tna_channel{   
	public: //--- constructor
		tnap_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		tnap_channel( const tnap_channel &channel );
virtual		~tnap_channel( void ){}; 
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class tkdr_channel
class tkdr_channel : public tk_channel{   
	public: //--- constructor
		tkdr_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		tkdr_channel( const tkdr_channel &channel );
virtual		~tkdr_channel( void ){}; 
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
	};

///////////////////////////////////////////////////////////////////////////////
// class tka_channel
class tka_channel : public t_channel{   
	public: //--- constructor
		tka_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		tka_channel( const tka_channel &channel );  
virtual		~tka_channel( void ){};
	public:
		tka_channel &operator = ( const tka_channel &channel );
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
	protected:
		t_data Mka1;
		t_data Hka1;
};

///////////////////////////////////////////////////////////////////////////////
// class tcal_channel
class tcal_channel : public t_channel{   
	public: //--- constructor
		tcal_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		tcal_channel( const tcal_channel &channel );
virtual		~tcal_channel( void ){};
	public: //--- overrided function
		size_t get_iontype( void ){ return _id_Ca_Ion; };
		size_t get_pumptype( void ){ return _id_Ca_Pump; };
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class tm_channel
class tm_channel : public t_channel{   
	public: //--- constructor
		tm_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		tm_channel( const tm_channel &channel );
virtual		~tm_channel( void ){};
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class tcat_channel
class tcat_channel : public t_channel{   
	public: //--- constructor
		tcat_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		tcat_channel( const tcat_channel &channel );
virtual		~tcat_channel( void ){};
	public: //--- overrided function
		size_t get_iontype( void ){ return _id_Ca_Ion; };
		size_t get_pumptype( void ){ return _id_Ca_Pump; };
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class tcan_channel
class tcan_channel : public t_channel{   
	public: //--- constructor
		tcan_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		tcan_channel( const tcan_channel &channel );
virtual		~tcan_channel( void ){};
	public: //--- overrided function
		size_t get_iontype( void ){ return _id_Ca_Ion; };
		size_t get_pumptype( void ){ return _id_Ca_Pump; };
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class tkc_channel
class tkc_channel : public t_channel{   
	public: //--- constructor
		tkc_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		tkc_channel( const tkc_channel &channel );
virtual		~tkc_channel( void ){};
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class tkca_channel
class tkca_channel : public t_channel{  
	public: //--- constructor
		tkca_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		tkca_channel( const tkca_channel &channel );
virtual		~tkca_channel( void ){};
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class th_channel
class th_channel : public t_channel{   
	public: //--- constructor
		th_channel( uni_template *parent, int is_active = GRID_DEL_ROW  );
		th_channel( const th_channel &channel );
virtual		~th_channel( void ){};
	public:
		th_channel &operator = ( const th_channel &channel );
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
	protected:
virtual		int get_datattr( const char *path )
		{
			if( string( path ) == "Reversal potential (mV)" )
				return GRID_NONE;
			return t_channel::get_datattr( path );
		};
	protected:
		t_data Eh;
};

///////////////////////////////////////////////////////////////////////////////
// class tleak_channel
class tleak_channel : public t_channel{   
	public: //--- constructor
		tleak_channel( uni_template *parent, int is_active = GRID_DEL_ROW  );
		tleak_channel( const tleak_channel &channel );
virtual		~tleak_channel( void ){}; 
	public:
		tleak_channel &operator = ( const tleak_channel &channel );
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
	protected:
virtual		int get_datattr( const char *path )
		{
			if( string( path ) == "Reversal potential (mV)" )
				return GRID_NONE;
			return t_channel::get_datattr( path );
		};
	protected:
		t_data Eleak;
};
#endif // __LOCOMOTION__

#include "respchantemplate.h"
#endif //_CHANNEL_TEMPLATE_H_
