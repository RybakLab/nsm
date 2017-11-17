/////////////////////////////////////////////////////////////////////////////
// class hhnchan.h
#ifndef __HHN_CHAN_H
#define __HHN_CHAN_H

#include "hhnunit.h"

class hhn_ions;
class hhn_compart;
class CHhnControlled;

/////////////////////////////////////////////////////////////////////////////
// class hhn_channel
class hhn_channel : public hhn_process{
friend	class CHhnControlled; 
	public: //--- constructor
		hhn_channel( hhn_compart *neuron );
		hhn_channel( const hhn_channel &channel );
virtual	~hhn_channel( void ){};
	public:
		hhn_channel &operator = ( const hhn_channel &channel );
	public: //--- Functions to geting channel parameters
		size_t get_chanid( void ) const{ return ChannelId; };
virtual	size_t get_gchanid( void ) const{ return -1; };
		const char *get_name( void ) const{ return Name.c_str(); };
	public: //--- Setup functions
		void set_kgmax( double *p ){ pKGmax = p; };
		void set_kione( double *e ){ pKIonsE = e; };
		void init_channel( double g, double tm, double th ){ I = G = M = H = 0.; Gmax = g; Tm = tm; Th = th; };
		void reg_unit( runman *man = NULL ) = 0;
virtual	void copy_to( hhn_channel **chan ) = 0;
	public:
		double I;			// Channel's current
		double G;			// Current channel's conductance  
		double M;			// Channel activation
		double H;			// Current channel inactivation 
		double Gmax;		// Maximal channel's conductance 
		double Tm;			// activation
		double Th;			// inactivation
		double *pKIonsE;	// Controlled coefficient for IonsE
		double *pKGmax;		// Controlled coefficient for Gmax
		hhn_compart *Hhn;	// Point to Hodgkin-Huxley type neuron
							// (in order to receive neuron parameters)
		string Name;
	protected:
		int ChannelId;       // Channel Id
};

/////////////////////////////////////////////////////////////////////////////
// class gen_channel
class gen_channel : public hhn_channel{   
	public:                       
		gen_channel( hhn_compart *neuron, hhn_ions *ions, bool modi = false, size_t subid = 0 );
		gen_channel( hhn_compart *neuron, double e, size_t subid = 0 );
		gen_channel( const gen_channel &channel );
virtual	~gen_channel( void ){};
	public:
		gen_channel &operator = ( const gen_channel &channel );
	public:
		size_t get_gchanid( void ) const{ return GChannelId; }; // new code 4 control
	public:
		void init_channel( double m, double mk, double hvm, double slpm, double tm0, double hvtm, double slptm, size_t powm, 
                               double h, double hk, double hvh, double slph, double th0, double hvth, double slpth, size_t powh );
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
      private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
	public:
		double Kpump;
		double Mk;
		double Hvm;
		double Slpm;
		double Tm0;
		double Hvtm;
		double Slptm;
		double Hk;
		double Hvh;
		double Slph;
		double Th0;
		double Hvth;
		double Slpth;
		double *IonsE;
		double *IonsI;
		size_t PowM;
		size_t PowH;
	protected:
		int GChannelId;       // generic channel Id, new code 4 control
	private:
		double NScpE;
		bool IsModI;
		bool IsNscpE;
};

/////////////////////////////////////////////////////////////////////////////
// class hgna_channel
class hgna_channel : public hhn_channel{   
	public:                       
		hgna_channel( hhn_compart *neuron );
		hgna_channel( const hgna_channel &channel );
virtual	~hgna_channel( void ){}; 
	public:
		hgna_channel &operator = ( const hgna_channel &channel );
	public:
		void init_channel( double mna, double v12m, double km, double v12tm, double ktm, size_t m_power, 
						   double hna, double v12h, double kh, double v12th, double kth, size_t h_power, 
						   double shifttm = 0, double ktm_1 = 0, double shiftth = 0, double kth_1 = 0 );
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
      private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
	public:
		double V12m;
		double Km;
		double V12tm;
		double Ktm;
		double V12h;
		double Kh;
		double V12th;
		double Kth;
		double Shifttm;
		double Ktm_1;
		double Shiftth;
		double Kth_1;
		size_t PowM;
		size_t PowH;
};

/////////////////////////////////////////////////////////////////////////////
// class hgnap_channel
class hgnap_channel : public hgna_channel{   
	public:                       
		hgnap_channel( hhn_compart *neuron );   
		hgnap_channel( const hgnap_channel &channel );
virtual	~hgnap_channel( void ){}; 
	public: //--- overrided function
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};

/////////////////////////////////////////////////////////////////////////////
// class hgkdr_channel
class hgkdr_channel : public hhn_channel{   
	public:                       
		hgkdr_channel( hhn_compart *neuron );
		hgkdr_channel( const hgkdr_channel &channel );
virtual	~hgkdr_channel( void ){}; 
	public:
		hgkdr_channel &operator = ( const hgkdr_channel &channel );
	public:
		void init_channel( double mk, double v12m, double km, double v12tm, double ktm, size_t m_power,	double shifttm = 0, double ktm_1 = 0 );
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
	public:
		double V12m;
		double Km;
		double V12tm;
		double Ktm;
		double Shifttm;
		double Ktm_1;
		size_t PowM;
};

/////////////////////////////////////////////////////////////////////////////
// class hgka_channel
class hgka_channel : public hhn_channel{   
	public:                       
		hgka_channel( hhn_compart *neuron );
		hgka_channel( const hgka_channel &channel );
virtual	~hgka_channel( void ){}; 
	public:
		hgka_channel &operator = ( const hgka_channel &channel );
	public:
		void init_channel( double mka, double v12m, double km, double v12tm, double ktm, size_t m_power, 
			double hka, double v12h, double kh, double v12th, double kth, size_t h_power, double shifttm = 0, double ktm_1 = 0, double shiftth = 0, double kth_1 = 0 );
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
	public:
		double V12m;
		double Km;
		double V12tm;
		double Ktm;
		double V12h;
		double Kh;
		double V12th;
		double Kth;
		double Shifttm;
		double Ktm_1;
		double Shiftth;
		double Kth_1;
		size_t PowM;
		size_t PowH;
};

/////////////////////////////////////////////////////////////////////////////
// class hgkca_channel
class hgkca_channel : public hhn_channel{   
	public:
		hgkca_channel( hhn_compart *neuron );
		hgkca_channel( const hgkca_channel &channel );
virtual	~hgkca_channel( void ){};
	public:
		hgkca_channel &operator = ( const hgkca_channel &channel );
	public:
		void init_channel( double minit, double alpha, double lymdba, size_t power );
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
	public:
		double Alpha;
		double Lymbda;
		size_t PowM;
};

/////////////////////////////////////////////////////////////////////////////
// class hgcan_channel
class hgcan_channel : public hgkca_channel{   
	public:
		hgcan_channel( hhn_compart *neuron );
		hgcan_channel( const hgcan_channel &channel );
virtual	~hgcan_channel( void ){};
	public:
		hgcan_channel &operator = ( const hgcan_channel &channel );
	public:
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
      private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
	public:
		double Egcan;
};

/////////////////////////////////////////////////////////////////////////////
// class hgh_channel
class hgh_channel : public hhn_channel{   
	public:
		hgh_channel( hhn_compart *neuron ); 
		hgh_channel( const hgh_channel &channel );
virtual	~hgh_channel( void ){};
	public:
		hgh_channel &operator = ( const hgh_channel &channel );
	public:
		void init_channel( double eds, double hh, double v12h, double kh, double v12th, double kth );
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
	public:
		double Eh;
		double V12h;
		double Kh;
		double V12th;
		double Kth;
};

#if defined( __LOCOMOTION__ ) || defined( __RESPIRATION__ )
/////////////////////////////////////////////////////////////////////////////
// class hna_channel
class hna_channel : public hhn_channel{   
	public:                       
		hna_channel( hhn_compart *neuron );
		hna_channel( const hna_channel &channel );
virtual	~hna_channel( void ){}; 
	public:
		void init_channel( double mna, double hna );
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};

/////////////////////////////////////////////////////////////////////////////
// class hk_channel
class hk_channel : public hhn_channel{   
	public:
		hk_channel( hhn_compart *neuron );
		hk_channel( const hk_channel &channel );
virtual	~hk_channel( void ){};
	public:
		void init_channel( double mk );
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};

/////////////////////////////////////////////////////////////////////////////
// class hnaf_channel
class hnaf_channel : public hna_channel{   
	public:                       
		hnaf_channel( hhn_compart *neuron );   
		hnaf_channel( const hnaf_channel &channel );
virtual	~hnaf_channel( void ){}; 
	public:
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};

/////////////////////////////////////////////////////////////////////////////
// class hnap_channel
class hnap_channel : public hna_channel{   
	public:                       
		hnap_channel( hhn_compart *neuron );   
		hnap_channel( const hnap_channel &channel );
virtual	~hnap_channel( void ){}; 
	public:
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};


/////////////////////////////////////////////////////////////////////////////
// class hkdr_channel
class hkdr_channel : public hk_channel{   
	public:
		hkdr_channel( hhn_compart *neuron );
		hkdr_channel( const hkdr_channel &channel );
virtual	~hkdr_channel( void ){};
	public:
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};

/////////////////////////////////////////////////////////////////////////////
// class hka_channel
class hka_channel : public hhn_channel{   
	public:
		hka_channel( hhn_compart *neuron );
		hka_channel( const hka_channel &channel );
virtual	~hka_channel( void ){};
	public:
		hka_channel &operator = ( const hka_channel &channel );
	public:
		void init_channel( double mka1, double hka1, double mka2, double hka2 );
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
	public:
		double Mka1;            
		double Hka1;
};

/////////////////////////////////////////////////////////////////////////////
// class hcal_channel
class hcal_channel : public hhn_channel{   
	public:
		hcal_channel( hhn_compart *neuron );
		hcal_channel( const hcal_channel &channel );
virtual	~hcal_channel( void ){};
	public:
		void init_channel( double mcal, double hcal = 0 );
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};

/////////////////////////////////////////////////////////////////////////////
// class hm_channel
class hm_channel : public hhn_channel{   
	public:
		hm_channel( hhn_compart *neuron );
		hm_channel( const hm_channel &channel );
virtual	~hm_channel( void ){};
	public:
		void init_channel( double mm );
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};

/////////////////////////////////////////////////////////////////////////////
// class hcat_channel
class hcat_channel : public hhn_channel{   
	public:
		hcat_channel( hhn_compart *neuron );
		hcat_channel( const hcat_channel &channel );
virtual	~hcat_channel( void ){};
	public:
		void init_channel( double mcat, double hcat );
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};

/////////////////////////////////////////////////////////////////////////////
// class hcan_channel
class hcan_channel : public hhn_channel{   
	public:
		hcan_channel( hhn_compart *neuron );
		hcan_channel( const hcan_channel &channel );
virtual	~hcan_channel( void ){};
	public:
		void init_channel( double mcan, double hcan );
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};

/////////////////////////////////////////////////////////////////////////////
// class hkc_channel
class hkc_channel : public hhn_channel{   
	public:
		hkc_channel( hhn_compart *neuron );
		hkc_channel( const hkc_channel &channel );
virtual	~hkc_channel( void ){};
	public:
		void init_channel( double mkc );
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};

/////////////////////////////////////////////////////////////////////////////
// class hkca_channel
class hkca_channel : public hhn_channel{   
	public:
		hkca_channel( hhn_compart *neuron );
		hkca_channel( const hkca_channel &channel );
virtual	~hkca_channel( void ){};
	public:
		void init_channel( double mkca );
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};

/////////////////////////////////////////////////////////////////////////////
// class hh_channel
class hh_channel : public hhn_channel{   
	public:
		hh_channel( hhn_compart *neuron ); 
		hh_channel( const hh_channel &channel );
virtual	~hh_channel( void ){};
	public:
		hh_channel &operator = ( const hh_channel &channel );
	public:
		void init_channel( double mh, double eds = -43 );
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	public:
		double Eh;
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
};

/////////////////////////////////////////////////////////////////////////////
// class hleak_channel
class hleak_channel : public hhn_channel{   
	public:
		hleak_channel( hhn_compart *neuron ); 
		hleak_channel( const hleak_channel &channel );
virtual	~hleak_channel( void ){}; 
	public:
		hleak_channel &operator = ( const hleak_channel &channel );
	public:
		void init_channel( double eds = -57 );
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
	public:
		double Eleak;
};
#endif // defined( __LOCOMOTION__ ) || defined( __RESPIRATION__ )

/////////////////////////////////////////////////////////////////////////////
// class hsoma_channel
class hsoma_channel : public hhn_channel{   
	public:
		hsoma_channel( hhn_compart *neuron ); 
		hsoma_channel( const hsoma_channel &channel );
virtual	~hsoma_channel( void ){}; 
	public:
		hsoma_channel &operator = ( const hsoma_channel &channel );
	public:
		void init_channel( const hhn_compart *soma, double p = 0.1 );
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
	public:
		hhn_compart *Soma;
	protected:
		double P;               // Initial conditionns
};

/////////////////////////////////////////////////////////////////////////////
// class hdendr_channel
class hdendr_channel : public hhn_channel{   
	public:
		hdendr_channel( hhn_compart *neuron ); 
		hdendr_channel( const hdendr_channel &channel );
virtual	~hdendr_channel( void ){}; 
	public:
		hdendr_channel &operator = ( const hdendr_channel &channel );
	public:
		void init_channel( const hhn_compart *dendr, double p = 0.1 );
		void reg_unit( runman *man = NULL );
		void copy_to( hhn_channel **chan );
	private:
		void calc_g( double step );
static	void calc_g( size_t currstep, double step, hhn_process **start );
	public:
		hhn_compart *Dendr;
	protected:
		double P;               // Initial conditionns
};

#endif // __HHN_CHAN_H
