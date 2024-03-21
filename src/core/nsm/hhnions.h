//////////////////////////////////////////////////////////////////////
// hhnions.h TODO make sure the methods are correct
#ifndef __HHN_IONS_H
#define __HHN_IONS_H

#include "hhnunit.h"

class CHhnControlled;
class hhn_compart;

//////////////////////////////////////////////////////////////////////
// class hhn_ions
class alignas( 16 )hhn_ions : public hhn_process{
friend		class CHhnControlled; 
	public:
		hhn_ions( hhn_process *parent ) : hhn_process(), ID( -1 ), Hhn(( hhn_compart * )parent ), IsDyn( false ), IsEds( true ),
			Z( 1 ), RTF( 26.54 ), In( 1.0 ), Out( 1.0 ), Eds( 0.0 ), 
			Alpha( 1.0 ), Beta( 1.0 ), T( 1.0 ), Ipump( 0.0 ), I( 0.0 ){};
 		hhn_ions( const hhn_ions &ions ) : hhn_process( ions ), ID( ions.ID ), Hhn( ions.Hhn ), IsDyn( ions.IsDyn ), IsEds( ions.IsEds ),
			Z( ions.Z ), RTF( ions.RTF ), In( ions.In ), Out( ions.Out ), Eds( ions.Eds ), 
			Alpha( ions.Alpha ), Beta( ions.Beta ), T( ions.T ), Ipump( ions.Ipump ), I( ions.I ){};
virtual		~hhn_ions( void ){};
	public:
		hhn_ions &operator = (  const hhn_ions &ions );
	public:
		int getid( void ) const{ return ID; };
	public:
virtual		void copy_to( hhn_ions **ions ){};
		bool init( void ) override;
virtual		void init( double in, double out );
		void reg_unit( runman *man ) final;
	protected:
virtual		void modifyAB( double &a, double &b ){};
	private:
		void calc_eds( void );
static		void reset( size_t currstep, double step, hhn_process **start );
static		void calc_ieds( size_t currstep, double step, hhn_process **start );
#ifndef __RK__
		void calc_idyn( double step );
static		void calc_idyn( size_t currstep, double step, hhn_process **start );
#else
		void calc_idyn1( double step );
		void calc_idyn2( double step );
static		void calc_idyn1( size_t currstep, double step, hhn_process **start );
static		void calc_idyn2( size_t currstep, double step, hhn_process **start );
#endif /*__RK__*/

	protected:
		int ID;
	public:
		hhn_compart *Hhn;
		bool IsDyn;
		bool IsEds;

		int Z;
		double RTF;
		double In;     // Ions concentration inside cell
		double Out;    // Ions concentration outside cell
		double Eds;    // Nerst potential

		double Alpha;	// Rate of ions current
		double Beta;	// Rate of pump current
		double T;		// Time constant
		double Ipump;	// Pump current
		double I;		// Ions current

#ifdef __RK__
		double InK1;
#endif
};

//////////////////////////////////////////////////////////////////////
// class hna_ions
class alignas( 16 ) hna_ions : public hhn_ions{ 
	public:
		hna_ions( hhn_process *parent ) : hhn_ions( parent ){ In = 15.0; Out = 120.0; Alpha = 0.0003; Beta = 3; ID = _id_Na_Ion; };
		hna_ions( const hna_ions &ions ) : hhn_ions( ions ){};
		~hna_ions( void ){};
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
	public: //--- overrided function
		void copy_to( hhn_ions **ions );
};

//////////////////////////////////////////////////////////////////////
// class hk_ions
class alignas( 16 ) hk_ions : public hhn_ions{ 
	public:
		hk_ions( hhn_process *parent ) : hhn_ions( parent ){ In = 140.0; Out = 4.0; ID = _id_K_Ion; };
		hk_ions( const hk_ions &ions ) : hhn_ions( ions ){};
		~hk_ions( void ){};
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
	public: //--- overrided function
		void copy_to( hhn_ions **ions );
};

//////////////////////////////////////////////////////////////////////
// class hcl_ions
class alignas( 16 ) hcl_ions : public hhn_ions{ 
	public:
		hcl_ions( hhn_process *parent ) : hhn_ions( parent ){ In = 4.0; Out = 110.0; Z = -1; ID = _id_Cl_Ion; };
		hcl_ions( const hcl_ions &ions ) : hhn_ions( ions ){};
		~hcl_ions( void ){};
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
	public: //--- overrided function
		void copy_to( hhn_ions **ions );
};

//////////////////////////////////////////////////////////////////////
// class hca_ions
class alignas( 16 ) hca_ions : public hhn_ions{ 
	public:
		hca_ions( hhn_process *parent );
		hca_ions( const hca_ions &ions );
		~hca_ions( void ){};
	public:
		hca_ions &operator = ( const hca_ions &ions );
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
	public: 
		void copy_to( hhn_ions **ions );
		bool init( void ) final;
		void init( double in, double out );
	protected:
#ifndef __LOCOMOTION__
		void modifyAB( double &a, double &b );
	public:
		double B;
		double K;
		double Kca;
#endif // __LOCOMOTION__
};


//////////////////////////////////////////////////////////////////////
// class hhn_pump
class alignas( 16 ) hhn_pump : public hhn_process{
	protected: // constructors
		hhn_pump( hhn_process *hnn ) : hhn_process(),
				Hhn(( hhn_compart *)hnn ), Ions( NULL ), PumpRV( 0. ){};
		hhn_pump( const hhn_pump &pump ) :  hhn_process( pump ),
				Hhn( pump.Hhn ), Ions( pump.Ions ), PumpRV( pump.PumpRV ){};
	public: // destructor
virtual		~hhn_pump( void ){};
	public:
		hhn_pump &operator = ( const hhn_pump &pump )
		{
			hhn_process::operator = ( pump );
			Hhn = pump.Hhn; Ions = pump.Ions; PumpRV = pump.PumpRV;
			return *this;
		};
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
	public:
virtual		void reg_unit( runman *man = NULL ) = 0;
	public:
		hhn_compart *Hhn;
		hhn_ions *Ions;
		double PumpRV;
};

//////////////////////////////////////////////////////////////////////
// class hnak_pump
class alignas( 16 ) hnak_pump : public hhn_pump{
	public: // constructors/destructor
		hnak_pump( hhn_process *hnn );
		hnak_pump( const hnak_pump &pump ) :  hhn_pump( pump ),
			Na0( pump.Na0 ), Kp( pump.Kp ), Rp( pump.Rp ){};
		~hnak_pump( void ){};
	public:
		hnak_pump &operator = ( const hnak_pump &pump )
		{
			hhn_pump::operator = ( pump );
			Na0 = pump.Na0; Kp = pump.Kp; Rp = pump.Rp;
			return *this;
		};
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
	public:
		void copy_to( hhn_pump **pump );
		void reg_unit( runman *man = NULL );
	private:
		void calc_ipump( double step );
static		void calc_ipump( size_t currstep, double step, hhn_process **start );
#ifdef __RK__
		void calc_ipump2( double step );
static	void calc_ipump2( size_t currstep, double step, hhn_process **start );
#endif /*RK*/
	public:
		double Na0;
		double Kp;
		double Rp;
};

//////////////////////////////////////////////////////////////////////
// class hca_pump
class alignas( 16 ) hca_pump : public hhn_pump{
	public: // constructors/destructor
		hca_pump( hhn_process *hnn );
		hca_pump( const hca_pump &pump ) : hhn_pump( pump ), Ca0( pump.Ca0 ){};
		~hca_pump( void ){};
	public:
		hca_pump &operator = ( const hca_pump &pump )
		{
			hhn_pump::operator = ( pump ); Ca0 = pump.Ca0;
			return *this;
		};
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
	public:
		void copy_to( hhn_pump **pump );
		void reg_unit( runman *man = NULL );
	private:
		void calc_ipump( double step );
static		void calc_ipump( size_t currstep, double step, hhn_process **start );
#ifdef __RK__
		void calc_ipump2( double step );
static		void calc_ipump2( size_t currstep, double step, hhn_process **start );
#endif /*RK*/
	public:
		double Ca0;
};

#endif // __HHN_IONS_H
