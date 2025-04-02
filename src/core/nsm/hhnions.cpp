//////////////////////////////////////////////////////////////////////
// hhnions.cpp
#include "precompile.h"

#include "hhnions.h"
#include "hhn.h"
#include "runman.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

//////////////////////////////////////////////////////////////////////
// class hhn_ions
hhn_ions &hhn_ions::operator = (  const hhn_ions &ions )
{
	hhn_process::operator =( ions );

	ID = ions.ID;
	IsDyn = ions.IsDyn;
	IsEds = ions.IsEds;

	Z = ions.Z;
	RTF = ions.RTF;
	In = ions.In;
	Out = ions.Out;
	Eds = ions.Eds;

	Alpha = ions.Alpha;
	Beta = ions.Beta;
	T = ions.T;

	I = ions.I;
	Ipump = ions.Ipump;
	return *this;
}

//--- virtual functions
void hhn_ions::reg_unit( runman *man )
{
	if( IsEds ){
		::reg_unit( this, hhn_ions::calc_ieds, _id_PASS_IEDS, -1, man ); 
	}
	if( IsDyn && Hhn ){
		::reg_unit( this, hhn_ions::reset, _id_PASS_RESET, -1, man ); 
#ifdef __RK__
		::reg_unit( this, hhn_ions::calc_idyn1, _id_PASS_IDYN_K1, -1, man ); 
#endif /*__RK__*/
		::reg_unit( this, hhn_ions::calc_idyn, _id_PASS_IDYN, -1, man ); 
	}
}

void hhn_ions::init( double in, double out )
{
	In = in;
	Out = out;
	init();
}

bool hhn_ions::init( void )
{
	I = Ipump = 0.0;
#ifdef __RK__
	InK1 = In;
#endif 
	return true;
}

void hhn_ions::calc_eds( void )
{
	Eds = ( RTF/Z )*log( Out/In );
}

#ifdef __RK__
void hhn_ions::calc_idyn1( double step )
{
	double alpha = Alpha;
	double beta = Beta;
	modifyAB( alpha, beta );
	InK1 = In+step*( -alpha*I+beta*Ipump/T )/2.;
}
#endif /*__RK__*/

void hhn_ions::calc_idyn( double step )
{
	double alpha = Alpha;
	double beta = Beta;
	modifyAB( alpha, beta );
	In += step*( -alpha*I+beta*Ipump/T );
#ifdef __RK__
	InK1 = In;
#endif /*__RK__*/
}

//////////////////////////////////////////////////////////////////////
// class hna_ions
void hna_ions::copy_to( hhn_ions **ions ) const
{
	*ions = new hna_ions( *this );
}

//////////////////////////////////////////////////////////////////////
// class hk_ions
void hk_ions::copy_to( hhn_ions **ions ) const
{
	*ions = new hk_ions( *this );
}

//////////////////////////////////////////////////////////////////////
// class hcl_ions
void hcl_ions::copy_to( hhn_ions **ions ) const
{
	*ions = new hcl_ions( *this );
}

//////////////////////////////////////////////////////////////////////
// class hca_ions
hca_ions::hca_ions( hhn_process *parent )
	: hhn_ions( parent )
{ 
	ID = _id_Ca_Ion;
	In = 0.00005;
	Out =4.;
	Z = 2; 
	IsDyn = true;
#ifdef __COMPATIBLE__
	B = 0.03;
	K = 0.001;
	T = 500.;
	Alpha = 1.0;
#else
	IsEds = false;
	Eds = 80.;
	T = 50;
	Alpha = 0.0045/T;
#endif /*__COMPATIBLE__*/ 
}

hca_ions::hca_ions( const hca_ions &ions )
	: hhn_ions( ions )
#ifdef __COMPATIBLE__
	, B( ions.B ), K( ions.K )
#endif /*__COMPATIBLE__*/ 
{

}

hca_ions &hca_ions::operator = (  const hca_ions &ions )
{
	hhn_ions::operator = ( ions );
#ifdef __COMPATIBLE__
	B = ions.B;
	K = ions.K;
#endif /*__COMPATIBLE__*/
	return *this;
}

void hca_ions::copy_to( hhn_ions **ions ) const
{
	*ions = new hca_ions( *this );
}

bool hca_ions::init( void )
{
#if defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	Alpha = T;
#endif /*defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )*/ 
	return hhn_ions::init();
}

void hca_ions::init( double in, double out )
{
	In = in;
	Out = out;
	init();
}

#ifdef __COMPATIBLE__
void hca_ions::modifyAB( double &a, double &/*b*/ )
{
	double kca = ( 1.E-6/( 2.0*9.648 ))/Hhn->Area;
#ifdef __RK__
	double in = InK1;
#else
	double in = In;
#endif /*__RK__*/
	a *= kca*( in+K )/( in+K+B );
}
#endif /*__COMPATIBLE__*/ 

//////////////////////////////////////////////////////////////////////
// class hnak_pump
//--- constructor
hnak_pump::hnak_pump( hhn_process *hnn ) 
	: hhn_pump( hnn ), Na0( 8.0 ),Kp( 15.0 ), Rp( 0.0006 )
{
	if( Hhn ){
		Ions = Hhn->NaIons;
	}
}

void hnak_pump::copy_to( hhn_pump **pump ) const
{
	*pump = new hnak_pump( *this );
}

void hnak_pump::reg_unit( runman *man )
{
	if( Hhn && Ions ){
#ifdef __RK__
		::reg_unit( this, hnak_pump::calc_ipump1, _id_PASS_IPUMP_K1, size_t( Hhn ), man ); 
#endif /*__RK__*/
		::reg_unit( this, hnak_pump::calc_ipump, _id_PASS_IPUMP, size_t( Hhn ), man );
	}
}

#define PHI( x, kp ) (( x )*( x )*( x ))/(( x )*( x )*( x )+( kp )*( kp )*( kp ))
bool hnak_pump::init( void )
{
	if( Hhn && Ions ){
		double kp = Kp; 
		Ions->Ipump = Rp*( PHI( Na0, kp )-PHI( Ions->In, kp ));
		return true;
	}
	return false;
}

#ifdef __RK__
void hnak_pump::calc_ipump1( double step )
{
	double kp = Kp;
	Ions->Ipump = Rp*( PHI( Na0, kp )-PHI( Ions->InK1, kp ));
}
#endif /*__RK__*/
void hnak_pump::calc_ipump( double step )
{
	double kp = Kp; 
	double ipump = Rp*( PHI( Na0, kp )-PHI( Ions->In, kp ));
	Ions->Ipump = ipump;
	Hhn->Ipumps -= PumpRV*ipump;
}
#undef PHI

//////////////////////////////////////////////////////////////////////
// class hca_pump
//--- constructor
hca_pump::hca_pump( hhn_process *hnn )
	: hhn_pump( hnn ), Ca0( 0. )
{
	if( Hhn ){
		Ions = Hhn->CaIons;
	}
#ifndef __LOCOMOTION__
	Ca0 = 0.00005;
#endif /*__LOCOMOTION__*/ 
}

void hca_pump::copy_to( hhn_pump **pump ) const
{
	*pump = new hca_pump( *this );
}

bool hca_pump::init( void )
{
	if( Hhn && Ions ){
		Ions->Ipump = Ca0-Ions->In;
		return true;
	}
	return false;
}

void hca_pump::reg_unit( runman *man )
{
	if( Hhn && Ions ){
#ifdef __RK__
		::reg_unit( this, hca_pump::calc_ipump1, _id_PASS_IPUMP_K1, size_t( Hhn ), man ); 
#endif /*__RK__*/
		::reg_unit( this, hca_pump::calc_ipump, _id_PASS_IPUMP, size_t( Hhn ), man ); 
	}
}

#ifdef __RK__
void hca_pump::calc_ipump1( double step )
{
	Ions->Ipump = Ca0-Ions->InK1;
}
#endif /*__RK__*/

void hca_pump::calc_ipump( double step )
{
	double ipump = Ca0-Ions->In;
	Ions->Ipump = ipump;
	Hhn->Ipumps -= PumpRV*ipump;
}
//--- end of file hhnions.cpp
