/*****************************************************************
 ****              (C)  1997 - 2000                           ****
 **           Model of Hodgkin-Haxley type neuron               **
 **       Development by Ilia Rybak and Sergey Markin           **
 ****                  Neuron template                        ****
 *****************************************************************/
#ifndef _RESP_CHAN_TEMPLATE_H_
#define _RESP_CHAN_TEMPLATE_H_

#include "chantemplate.h"
#include "hhnrespchan.h"

#ifdef __RESPIRATION__
///////////////////////////////////////////////////////////////////////////////
// class trnaf_channel
class trnaf_channel : public tna_channel{   
	public: //--- constructor
		trnaf_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		trnaf_channel( const trnaf_channel &channel ) : tna_channel( channel ){};
virtual		~trnaf_channel( void ){}; 
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class trnap_channel
class trnap_channel : public tna_channel{   
	public: //--- constructor
		trnap_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		trnap_channel( const trnap_channel &channel );
virtual		~trnap_channel( void ){}; 
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class trkdr_channel
class trkdr_channel : public tk_channel{   
	public:
	//--- constructor
		trkdr_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		trkdr_channel( const trkdr_channel &channel ) : tk_channel( channel ){};
virtual		~trkdr_channel( void ){}; 
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class trka_channel
class trka_channel : public tka_channel{   
	public: //--- constructor
		trka_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		trka_channel( const trka_channel &channel ) : tka_channel( channel ){};
virtual		~trka_channel( void ){};
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class trcal_channel
class trcal_channel : public tcal_channel{   
	public: //--- constructor
		trcal_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		trcal_channel( const trcal_channel &channel ) : tcal_channel( channel ){};
virtual		~trcal_channel( void ){};
	public: //--- overrided function
		size_t get_iontype( void ){ return _id_Ca_Ion; };
		size_t get_pumptype( void ){ return _id_Ca_Pump; };
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class trcat_channel
class trcat_channel : public tcat_channel{   
	public: //--- constructor
		trcat_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		trcat_channel( const trcat_channel &channel ) : tcat_channel( channel ){};
virtual		~trcat_channel( void ){};
	public: //--- overrided function
		size_t get_iontype( void ){ return _id_Ca_Ion; };
		size_t get_pumptype( void ){ return _id_Ca_Pump; };
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class trkca_channel
class trkca_channel : public tkca_channel{  
	public: //--- constructor
		trkca_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		trkca_channel( const trkca_channel &channel ) : tkca_channel( channel ){};
virtual		~trkca_channel( void ){};
	public: //--- overrided function
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
};

///////////////////////////////////////////////////////////////////////////////
// class trleak_channel
class trleak_channel : public tleak_channel{   
	public: //--- constructor
		trleak_channel( uni_template *parent, int is_active = GRID_DEL_ROW );
		trleak_channel( const trleak_channel &channel );  
virtual		~trleak_channel( void ){};
	public:
		trleak_channel &operator = ( const trleak_channel &channel );
	public: //--- overrided function
virtual		bool process( const char *path );
virtual		void copy_to( uni_template **unit, uni_template * parent );
virtual		void copy_to( hhn_process **unit, hhn_process *parent );
	protected:
virtual		int  get_datattr( const char *path );
virtual		bool get_addparlist( vector<string> &parlist );
virtual		bool get_addpar( const char *path, string &name, string &element, int &type );
virtual		bool upd_addpar( const char *path, const char *name, string &element, bool save );

virtual		bool load_addpar( string str, istream &file );
virtual		void save_addpar( ostream &file );
	protected:
		void calc_eds( void );
	private:
		t_data Pna;               // Initial conditionns
		t_data Pcl;               // Initial conditionns
		bool Adjustable;
};

#endif // __RESPIRATION__
#endif //_RESP_CHAN_TEMPLATE_H_
