/////////////////////////////////////////////////////////////////////////////
// class hhnrespchan.h
#ifndef __HHN_RESP_CHAN_H
#define __HHN_RESP_CHAN_H

#include "hhnchan.h"

#ifdef __RESPIRATION__
/////////////////////////////////////////////////////////////////////////////
// class hna_channel
class hrnaf_channel : public hna_channel{   
	public:                       
		hrnaf_channel( hhn_compart *neuron );   
		hrnaf_channel( const hrnaf_channel &channel );
virtual ~hrnaf_channel( void ){}; 
	public:
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};

/////////////////////////////////////////////////////////////////////////////
// class hrnap_channel
class hrnap_channel : public hna_channel{   
      public:                       
		hrnap_channel( hhn_compart *neuron );   
		hrnap_channel( const hrnap_channel &channel );
virtual ~hrnap_channel( void ){}; 
	public:
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};


/////////////////////////////////////////////////////////////////////////////
// class hrkdr_channel
class hrkdr_channel : public hk_channel{   
	public:
		hrkdr_channel( hhn_compart *neuron );
		hrkdr_channel( const hrkdr_channel &channel );
virtual ~hrkdr_channel( void ){};
	public:
		hrkdr_channel &operator = ( const hrkdr_channel &channel );
	public:
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};


/////////////////////////////////////////////////////////////////////////////
// class hrka_channel
class hrka_channel : public hka_channel{   
	public:
		hrka_channel( hhn_compart *neuron );  
		hrka_channel( const hrka_channel &channel );
virtual ~hrka_channel( void ){};
	public:
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};

/////////////////////////////////////////////////////////////////////////////
// class hrcal_channel
class hrcal_channel : public hcal_channel{   
	public:
		hrcal_channel( hhn_compart *neuron );
		hrcal_channel( const hrcal_channel &channel );
virtual ~hrcal_channel( void ){};
	public:
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};

/////////////////////////////////////////////////////////////////////////////
// class hrcat_channel
class hrcat_channel : public hcat_channel{   
	public:
		hrcat_channel( hhn_compart *neuron );
		hrcat_channel( const hrcat_channel &channel );
virtual ~hrcat_channel( void ){};
	public:
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};

/////////////////////////////////////////////////////////////////////////////
// class hrcat_channel
class hrkca_channel : public hkca_channel{   
	public:
		hrkca_channel( hhn_compart *neuron ); 
		hrkca_channel( const hrkca_channel &channel );
virtual ~hrkca_channel( void ){};
	public:
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};

/////////////////////////////////////////////////////////////////////////////
// class hrleak_channel
class hrleak_channel : public hleak_channel{   
	public:
		hrleak_channel( hhn_compart *neuron ); 
		hrleak_channel( const hrleak_channel &channel );
virtual ~hrleak_channel( void ){}; 
	public:
		hrleak_channel &operator = ( const hrleak_channel &channel );
	public:
		void init_channel( bool adjustable, double pna, double pcl, double eds = -65 );
		void calc_eds( void ); 
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
	private:
		bool Adjustable;
		double Pna;   // Initial conditionns
		double Pcl;   // Initial conditionns
};

#endif // __RESPIRATION__
#endif //__HHN_RESP_CHAN_H
