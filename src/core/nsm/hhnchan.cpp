/////////////////////////////////////////////////////////////////////////////
// class hhnchan.cpp
#include "precompile.h"
#include <cmath>

#include "hhnchan.h"
#include "hhn.h"
#include "hhnnetwork.h"
#include "runman.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG
#endif //__LINUX__

alignas( 16 ) static double _pKGmax = 1.;
alignas( 16 ) static double _pKIonsE = 1.;

/////////////////////////////////////////////////////////////////////////////
// class hhn_channel
//--- constructor
hhn_channel::hhn_channel( hhn_compart *neuron )
{
	Hhn = neuron;
	ChannelId = -1;
	Name = "";
	pKGmax = &_pKGmax;
	pKIonsE = &_pKIonsE;
	G = 0.;
	I = 0.;
	M = 0.;
	H = 0.;
	init_channel( 0, 1, 1 );
}

hhn_channel::hhn_channel( const hhn_channel &channel )
           : hhn_process( channel )
{
	Hhn = channel.Hhn;
	ChannelId = channel.ChannelId;
	Name = channel.Name;
	pKGmax = &_pKGmax;
	pKIonsE = &_pKIonsE;
	G = channel.G;
	I = channel.I;
	M = channel.M;
	H = channel.H;
	init_channel( channel.Gmax, channel.Tm, channel.Th );
}

hhn_channel &hhn_channel::operator = ( const hhn_channel &channel )
{
	hhn_process::operator =( channel );
	Hhn = channel.Hhn;
	ChannelId = channel.ChannelId;
	Name = channel.Name;
	pKGmax = &_pKGmax;
	pKIonsE = &_pKIonsE;
	G = channel.G;
	I = channel.I;
	M = channel.M;
	H = channel.H;
	init_channel( channel.Gmax, channel.Tm, channel.Th );
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// class gen_channel
gen_channel::gen_channel( hhn_compart *neuron, hhn_ions *ions, bool modi, size_t subid )
	: hhn_channel( neuron ), Kpump( 1.0 ), GChannelId( subid ),
	NScpE( 0. ), IsModI( modi ), IsNscpE( false )
{
	ChannelId = _id_generic_Chan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	IonsE = IsNscpE? &NScpE: &( ions->Eds );
	IonsI = IsModI? &( ions->I ): NULL;
	hhn_channel::init_channel( 0., 0., 0. );
	init_channel( 0., 1., 0., 1., 0., 0., 1, 
			  0, 1., 0., 0., 1., 0., 0., 1, 0 );
}

gen_channel::gen_channel( hhn_compart *neuron, double e, size_t subid )
	: hhn_channel( neuron ), Kpump( 1.0 ), GChannelId( subid ),
	NScpE( e ), IsModI( false ), IsNscpE( true )
{
	ChannelId = _id_generic_Chan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	IonsE = &NScpE;
	IonsI = NULL;
	hhn_channel::init_channel( 0., 0., 0. );
	init_channel( 0., 1., 0., 1., 0., 0., 1, 0,
			  0., 1., 0., 1., 0., 0., 1, 0 );
}

gen_channel::gen_channel( const gen_channel &channel )
	: hhn_channel( channel ), Kpump( channel.Kpump ), GChannelId( channel.GChannelId ),
	NScpE( channel.NScpE ), IsModI( channel.IsModI ), IsNscpE( channel.IsNscpE )
{
	IonsE = IsNscpE? &NScpE: channel.IonsE;
	IonsI = IsModI? channel.IonsI: NULL;
	init_channel( channel.M, channel.Mk, channel.Hvm, channel.Slpm, channel.Tm0, channel.Hvtm, channel.Slptm, channel.PowM, 
			  channel.H, channel.Hk, channel.Hvh, channel.Slph, channel.Th0, channel.Hvtm, channel.Slpth, channel.PowH );
}

gen_channel &gen_channel::operator = ( const gen_channel &channel )
{
	hhn_channel::operator = ( channel );
	GChannelId = channel.GChannelId;
	Kpump = channel.Kpump;
	init_channel( channel.M, channel.Mk, channel.Hvm, channel.Slpm, channel.Tm0, channel.Hvtm, channel.Slptm, channel.PowM, 
			  channel.H, channel.Hk, channel.Hvh, channel.Slph, channel.Th0, channel.Hvtm, channel.Slpth, channel.PowH );
	return *this;
}

void gen_channel::init_channel( double m, double mk, double hvm, double slpm, double tm0, double hvtm, double slptm, size_t powm, 
					  double h, double hk, double hvh, double slph, double th0, double hvth, double slpth, size_t powh )
{
	M = m;
	Mk = mk;
	Hvm = hvm;
	Slpm = slpm;
	Tm0 = tm0;
	Hvtm = hvtm;
	Slptm = slptm;
	PowM = powm;
	H = h;
	Hk = hk;
	Hvh = hvh;
	Slph = slph;
	Th0 = th0;
	Hvth = hvth;
	Slpth = slpth;
	PowH = powh;
}

void gen_channel::reg_unit( runman *man  )
{
	::reg_unit( this, gen_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void gen_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new gen_channel( *this );
}

void gen_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double g = *pKGmax*Gmax;
	if( PowM > 0 ){
		double m0 = Mk*( GENERIC_G( v, Hvm, Slpm ));
		double tm = Tm0+( GENERIC_T( Tm, v, Hvtm, Slptm ));
		double m = EXP_EULER( M, m0, step, tm );
		for( size_t i = 0; i < PowM; g *= m, ++i );
		M = m;
	}
	if( PowH > 0 ){
		double h0 = Hk*( GENERIC_G( v, Hvh, Slph ));
		double th = Th0+( GENERIC_T( Th, v, Hvth, Slpth ));
		double h = EXP_EULER( H, h0, step, th );
		for( size_t i = 0; i < PowH; g *= h, ++i );
		H = h;
	}

	double e = *IonsE*( *pKIonsE );
	G = g;
	I = g*( v-e );
	if( IonsI )
		*IonsI += I;

	Hhn->G += g;
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hgna_channel
//--- constructor
hgna_channel::hgna_channel( hhn_compart *neuron ) 
	: hhn_channel( neuron )
{
	ChannelId = _id_generic_NaChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	hhn_channel::init_channel( 4., 10., 1. );
	init_channel( 0.02, -40., -6., -40., -12., 1, 0.3, -48., 6, -48., 12., 1 );
	Th = 1.;
#if defined __MECHANICS_2D__ || __MECHANICS_3D__
	Th = 0.33;
#endif // __MECHANICS_2D__
}

hgna_channel::hgna_channel( const hgna_channel &channel )
           : hhn_channel( channel )
{
	init_channel( channel.M, channel.V12m, channel.Km, channel.V12tm, channel.Ktm, channel.PowM, channel.H, channel.V12h, channel.Kh, channel.V12th, channel.Kth, channel.PowH,
		channel.Shifttm, channel.Ktm_1, channel.Shiftth, channel.Kth_1 );
}

hgna_channel &hgna_channel::operator = ( const hgna_channel &channel )
{
	hhn_channel::operator = ( channel );
	init_channel( channel.M, channel.V12m, channel.Km, channel.V12tm, channel.Ktm, channel.PowM, channel.H, channel.V12h, channel.Kh, channel.V12th, channel.Kth, channel.PowH,
		channel.Shifttm, channel.Ktm_1, channel.Shiftth, channel.Kth_1 );
	return *this;
}

//--- public function
void hgna_channel::init_channel( double mna, double v12m, double km, double v12tm, double ktm, size_t m_power,
                                 double hna, double v12h, double kh, double v12th, double kth, size_t h_power, 
                                 double shifttm, double ktm_1, double shiftth, double kth_1 )
{
	M = mna;            
	V12m = v12m;
	Km = km;
	V12tm = v12tm;
	Ktm = ktm;
	PowM = m_power;
	H = hna;
	V12h = v12h;
	Kh = kh;
	V12th = v12th;
	Kth = kth;
	PowH = h_power;
	Shifttm = shifttm;
	Ktm_1 = ktm_1;
	Shiftth = shiftth;
	Kth_1 = kth_1;
}

void hgna_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hgna_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

//--- overrided function
void hgna_channel::copy_to( hhn_channel **chan ) const
{
 *chan = new hgna_channel( *this );
}

void hgna_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double g = *pKGmax*Gmax;
	if( PowM > 0 ){
		double m0 = GENERIC_G( v, V12m, Km );
		double tm = GENERIC_T_MOD( Shifttm, Tm, v, V12tm, Ktm, Ktm_1 );
		double m = EXP_EULER( M, m0, step, tm );
		for( size_t i = 0; i < PowM; g *= m, ++i );
		M = m;
	}
	if( PowH > 0 ){
		double h0 = GENERIC_G( v, V12h, Kh );
		double th = GENERIC_T_MOD( Shiftth, Th, v, V12th, Kth, Kth_1 );
		double h = EXP_EULER( H, h0, step, th );
		for( size_t i = 0; i < PowH; g *= h, ++i );
		H = h;
	}

	double e = *pKIonsE*Hhn->NaIons->Eds;
	G = g;
	I = g*( v-e );
	Hhn->G += g;
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hgnap_channel
//--- constructor
hgnap_channel::hgnap_channel( hhn_compart *neuron ) 
	: hgna_channel( neuron )
{
	ChannelId = _id_generic_NaPChan; 
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	Gmax = 2.8;
	Tm = 1.;
	Th = 10000.;
	init_channel( 0.02, -40., -6., -40., -12., 1, 0.3, -48., 6, -48., 12., 1 );
}

hgnap_channel::hgnap_channel( const hgnap_channel &channel )
	: hgna_channel( channel )
{
}

//--- overrided function
void hgnap_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hgnap_channel( *this );
}

void hgnap_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hgnap_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hgnap_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double m0 = GENERIC_G( v, V12m, Km );
	double tm = GENERIC_T( Tm, v, V12tm, Ktm );
	double m = EXP_EULER( M, m0, step, tm );
	double h0 = GENERIC_G( v, V12h, Kh );
	double th = GENERIC_T( Th, v, V12th, Kth );
	double h = EXP_EULER( H, h0, step, th );
	double g = *pKGmax*Gmax*pow( m, int( PowM ))*pow( h, int( PowH ));
	double e = *pKIonsE*Hhn->NaIons->Eds;
	M = m;
	H = h;
	G = g;
	I = g*( v-e );
	Hhn->G += g;
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hgkdr_channel
//--- constructor
hgkdr_channel::hgkdr_channel( hhn_compart *neuron ) 
	: hhn_channel( neuron )
{
	ChannelId = _id_generic_KdrChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	hhn_channel::init_channel( 100., 7., 1. );
	init_channel( 0.03, -29., -4., -29., -8., 4 );
}

hgkdr_channel::hgkdr_channel( const hgkdr_channel &channel )
	: hhn_channel( channel )
{
	init_channel( channel.M, channel.V12m, channel.Km, channel.V12tm, channel.Ktm, channel.PowM,
		channel.Shifttm, channel.Ktm_1 );
}

hgkdr_channel &hgkdr_channel::operator = ( const hgkdr_channel &channel )
{
	hhn_channel::operator = ( channel );
	init_channel( channel.M, channel.V12m, channel.Km, channel.V12tm, channel.Ktm, channel.PowM,
		channel.Shifttm, channel.Ktm_1 );
	return *this;
}

//--- public function
void hgkdr_channel::init_channel( double mk, double v12m, double km, double v12tm, double ktm, size_t m_power, double shifttm, double ktm_1 )
{
	M = mk;            
	V12m = v12m;
	Km = km;
	V12tm = v12tm;
	Ktm = ktm;
	PowM = m_power;
	Shifttm = shifttm;
	Ktm_1 = ktm_1;
}

//--- overrided function
void hgkdr_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hgkdr_channel( *this );
}

void hgkdr_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hgkdr_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hgkdr_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double m0 = GENERIC_G( v, V12m, Km );
	double tm = GENERIC_T_MOD( Shifttm, Tm, v, V12tm, Ktm, Ktm_1 );
	double m = EXP_EULER( M, m0, step, tm );
	double g = *pKGmax*Gmax*pow( m, int( PowM ));
	double e = *pKIonsE*Hhn->KIons->Eds;
	M = m;
	G = g;
	I = g*( v-e );
	Hhn->G += g;
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hgka_channel
//--- constructor
hgka_channel::hgka_channel( hhn_compart *neuron ) 
	: hhn_channel( neuron )
{
	ChannelId = _id_generic_KAChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	hhn_channel::init_channel( 4., 10., 1. );
	init_channel( 0.02, -40., -6., -40., -12., 1, 0.3, -48., 6, -48., 12., 1 );
}

hgka_channel::hgka_channel( const hgka_channel &channel )
	: hhn_channel( channel )
{
	init_channel( channel.M, channel.V12m, channel.Km, channel.V12tm, channel.Ktm, channel.PowM, channel.H, channel.V12h, channel.Kh, channel.V12th, channel.Kth, channel.PowH,
		channel.Shifttm, channel.Ktm_1, channel.Shiftth, channel.Kth_1 );
}

hgka_channel &hgka_channel::operator = ( const hgka_channel &channel )
{
	hhn_channel::operator = ( channel );
	init_channel( channel.M, channel.V12m, channel.Km, channel.V12tm, channel.Ktm, channel.PowM, channel.H, channel.V12h, channel.Kh, channel.V12th, channel.Kth, channel.PowH,
		channel.Shifttm, channel.Ktm_1, channel.Shiftth, channel.Kth_1 );
	return *this;
}

//--- public function
void hgka_channel::init_channel( double mka, double v12m, double km, double v12tm, double ktm, size_t m_power,
	double hka, double v12h, double kh, double v12th, double kth, size_t h_power, double shifttm, double ktm_1, double shiftth, double kth_1 )
{
	M = mka;            
	V12m = v12m;
	Km = km;
	V12tm = v12tm;
	Ktm = ktm;
	PowM = m_power;
	H = hka;
	V12h = v12h;
	Kh = kh;
	V12th = v12th;
	Kth = kth;
	PowM = h_power;
	Shifttm = shifttm;
	Ktm_1 = ktm_1;
	Shiftth = shiftth;
	Kth_1 = kth_1;
}

//--- overrided function
void hgka_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hgka_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hgka_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hgka_channel( *this );
}

void hgka_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double g = *pKGmax*Gmax;
	if( PowM > 0 ){
		double m0 = GENERIC_G( v, V12m, Km );
		double tm = GENERIC_T_MOD( Shifttm, Tm, v, V12tm, Ktm, Ktm_1 );
		double m = EXP_EULER( M, m0, step, tm );
		for( size_t i = 0; i < PowM; g *= m, ++i );
		M = m;
	}
	if( PowH > 0 ){
		double h0 = GENERIC_G( v, V12h, Kh );
		double th = GENERIC_T_MOD( Shiftth, Th, v, V12th, Kth, Kth_1 );
		double h = EXP_EULER( H, h0, step, th );
		for( size_t i = 0; i < PowH; g *= h, ++i );
		H = h;
	}

	double e = *pKIonsE*Hhn->KIons->Eds;
	G = g;
	I = g*( v-e );
	Hhn->G += g;
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hgkca_channel
//--- constructor
hgkca_channel::hgkca_channel( hhn_compart *neuron ) 
	: hhn_channel( neuron )
{
	ChannelId = _id_generic_KCa;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	Gmax = 7; 
	H = 1;
	Tm = 2.0;
	init_channel( 0.22, 0.5, 2.0, 2 );
}

hgkca_channel::hgkca_channel( const hgkca_channel &channel )
	: hhn_channel( channel )
{
	init_channel( channel.M, channel.Alpha, channel.Lymbda, channel.PowM );
}

hgkca_channel &hgkca_channel::operator = ( const hgkca_channel &channel )
{
	hhn_channel::operator = ( channel );
	init_channel( channel.M, channel.Alpha, channel.Lymbda, channel.PowM );
	return *this;
}

//--- public function
void hgkca_channel::init_channel( double minit, double alpha, double lymdba, size_t power )
{
	M = minit;
	Alpha = alpha;
	Lymbda = lymdba;
	PowM = power;
}

//--- overrided function
void hgkca_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hgkca_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hgkca_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hgkca_channel( *this );
}

void hgkca_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double g = *pKGmax*Gmax;
	if( PowM > 0 ){
		double cacnc = Hhn->CaIons->In;
		double k1 = Alpha*pow( 10000*cacnc, Lymbda );
		double m0 = k1/( k1+1. );
		double tm = Tm*400./(k1+1.); 
		double m = EXP_EULER( M, m0, step, tm );
		for( size_t i = 0; i < PowM; g *= m, ++i );
		M = m;
	}

	double e = *pKIonsE*Hhn->KIons->Eds;
	G = g;
	I = g*( v-e );
	Hhn->G += g;
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hgcan_channel
//--- constructor
hgcan_channel::hgcan_channel( hhn_compart *neuron ) 
	: hgkca_channel( neuron )
{
	ChannelId = _id_generic_Can;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	Egcan = -30.0;
}

hgcan_channel::hgcan_channel( const hgcan_channel &channel )
	: hgkca_channel( channel )
{
	Egcan = channel.Egcan;
}

hgcan_channel &hgcan_channel::operator = ( const hgcan_channel &channel )
{
	hgkca_channel::operator = ( channel );
	Egcan = channel.Egcan;
	return *this;
}

//--- overrided function
void hgcan_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hgcan_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hgcan_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hgcan_channel( *this );
}

void hgcan_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double g = *pKGmax*Gmax;
	if( PowM > 0 ){
		double cacnc = Hhn->CaIons->In;
		double k1 = Alpha*pow( 10000*cacnc, Lymbda );
		double m0 = k1/( k1+1. );
		double tm = Tm*400./( k1+1. ); 
		double m = EXP_EULER( M, m0, step, tm );
		for( size_t i = 0; i < PowM; g *= m, ++i );
		M = m;
	}

	double e = *pKIonsE*Egcan;
	G = g;
	I = g*( v-e );
	Hhn->G += g;
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hgh_channel
//--- constructor
hgh_channel::hgh_channel( hhn_compart *neuron ) 
	: hhn_channel( neuron )
{
	ChannelId = _id_generic_HChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	hhn_channel::init_channel( 1, 1, 3900. );
	init_channel( -43., 0, -75., 5.5, -80., 13. );
	M = 1.;
	Tm = 1.;
}

hgh_channel::hgh_channel( const hgh_channel &channel )
	: hhn_channel( channel )
{
	init_channel( channel.Eh, channel.H, channel.V12h, channel.Kh, channel.V12th, channel.Kth );
}

hgh_channel &hgh_channel::operator = ( const hgh_channel &channel )
{
	hhn_channel::operator = ( channel );
	init_channel( channel.Eh, channel.H, channel.V12h, channel.Kh, channel.V12th, channel.Kth );
	return *this;
}

void hgh_channel::init_channel( double eds, double hh, double v12h, double kh, double v12th, double kth )
{
	H = hh;          
	V12h = v12h;
	Kh = kh;
	V12th = v12th;
	Kth = kth;
	Eh = eds;
}

void hgh_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hgh_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hgh_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hgh_channel( *this );
}

void hgh_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double h0 = GENERIC_G( v, V12h, Kh );
	double th = GENERIC_T( Th, v, V12th, Kth );
	double h = EXP_EULER( H, h0, step, th );
	double g = *pKGmax*Gmax*h;

	double e = *pKIonsE*Eh;
	H = h;
	G = g;
	I = g*( v-e );

	Hhn->G += g;
	Hhn->GE += g*e;
}

#if defined( __LOCOMOTION__ ) || defined( __RESPIRATION__ )
/////////////////////////////////////////////////////////////////////////////
// class hna_channel
//--- constructor
hna_channel::hna_channel( hhn_compart *neuron ) 
	: hhn_channel( neuron )
{
#ifdef __LOCOMOTION__
	ChannelId = _id_NaChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	hhn_channel::init_channel( 120., 1, 1 );
	init_channel( 0.02, 0.3 );
#endif // __LOCOMOTION__
	Th = 1.;
#if defined __MECHANICS_2D__ || __MECHANICS_3D__
	Th = 0.33;
#endif // __MECHANICS_2D__
}

hna_channel::hna_channel( const hna_channel &channel )
	: hhn_channel( channel )
{
}

//--- public function
void hna_channel::init_channel( double mna, double hna )
{
	M = mna;
	H = hna;
}

//--- overrided function
void hna_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hna_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hna_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hna_channel( *this );
}

void hna_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double h0 = 1./( 1+EXP(( v+55. )/7.0 ));
	double th =  Th*30./( EXP(( v+50. )/15. )+EXP( -( v+50. )/16. ));
	double m = 1./( 1+EXP(( v+35. )/-7.8 ));
	double h = EXP_EULER( H, h0, step, th );
	double g = *pKGmax*Gmax*POW_3( m )*h;
	double e = *pKIonsE*Hhn->NaIons->Eds;

	M = m;
	H = h;
	G = g;
	I = g*( v-e ); // pass g

	Hhn->G += g;	// pass prevm
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hk_channel
//--- constructor
hk_channel::hk_channel( hhn_compart *neuron )
	: hhn_channel( neuron )
{
	ChannelId = _id_KChan; 
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	hhn_channel::init_channel( 100., 1, 1 );
	init_channel( 0.03 );
	H = 1;
	Tm = 1.;
#if defined __MECHANICS_2D__ || __MECHANICS_3D__
	Tm = 0.33;
#endif // __MECHANICS_2D__
}

hk_channel::hk_channel( const hk_channel &channel )
	: hhn_channel( channel )
{
}

//--- public function
void hk_channel::init_channel( double mk )
{
	M = mk;
}

//--- overrided function
void hk_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hk_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hk_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hk_channel( *this );
}

void hk_channel::calc_g( double step )
{
	double v = Hhn->get_vm();

#if defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	//McCormick
	double shift = -1.;
	double k0 = v+45+shift; 
	double k1 = 0.01*k0/( 1-EXP( -k0/5. ));
	double k2 = 0.17*EXP( -( v+50+shift )/40. );
	double k3 = 1./( k1+k2 );
	double tm = Tm*k3;
	double m0 = k1*k3;
#else
	double tm = Tm*7./( EXP(( v+40. )/40.)+EXP(-( v+40. )/50. ));
	double m0 = 1./( 1+EXP(( v+28. )/-15. ));
#endif
	double m = EXP_EULER( M, m0, step, tm );
	double g = *pKGmax*Gmax*POW_4( m );
	double e = *pKIonsE*Hhn->KIons->Eds;
	M = m;
	G = g;
	I = g*( v-e );  // pass g

	Hhn->G += g; 	// pass prevm
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hnaf_channel
//--- constructor
hnaf_channel::hnaf_channel( hhn_compart *neuron ) 
	: hna_channel( neuron )
{
	ChannelId = _id_NaFChan; 
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	Gmax = 150; 
	Tm = 0.28;
	Th = 0.28;
	init_channel( 0.02, 0.3 );
}

hnaf_channel::hnaf_channel( const hnaf_channel &channel )
	: hna_channel( channel )
{
}

void hnaf_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hnaf_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hnaf_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hnaf_channel( *this );
}

void hnaf_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double m0 = 1./( 1.+EXP( -( v+34. )/5. ));
	double h0 = 1.-1/( 1.+EXP( -( v+29. )/4. ));
	double th = 10./COSH(( v+29. )/8. );
	double m = m0;
	double h = EXP_EULER( H, h0, step, th );
	double g = *pKGmax*Gmax*POW_3( m )*h;
	double e = *pKIonsE*Hhn->NaIons->Eds;

	M = m;
	H = h;
	G = g;
	I = g*( v-e );

	Hhn->G += g; 	// pass prevm
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hnap_channel
//--- constructor
hnap_channel::hnap_channel( hhn_compart *neuron ) 
	: hna_channel( neuron )
{
	ChannelId = _id_NaPChan; 
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	Gmax = 8;
	Tm = 0.28;
	Th = 3000.;
	init_channel( 0.02, 0.3 );
}

hnap_channel::hnap_channel( const hnap_channel &channel )
	: hna_channel( channel )
{
}

void hnap_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hnap_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hnap_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hnap_channel( *this );
}

void hnap_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double m0 = 1/( 1.+EXP( -( v+40. )/6 ));
	double h0 = 1./( 1.+EXP(( v+48. )/6. ));
	double th = Th/COSH(( v+48. )/12. );
	double m = m0;
	double h = EXP_EULER( H, h0, step, th );
	double g = *pKGmax*Gmax*m*h;
	double e = *pKIonsE*Hhn->NaIons->Eds;

	M = m;
	H = h;
	G = g;
	I = g*( v-e );
	Hhn->G += g;
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hkdr_channel
//--- constructor
hkdr_channel::hkdr_channel( hhn_compart *neuron )
	: hk_channel( neuron )
{
	ChannelId = _id_KdrChan; 
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	Gmax = 90;
	Tm = 4.0;
	H = 1;
	init_channel( 0.03 );
}

hkdr_channel::hkdr_channel( const hkdr_channel &channel )
	: hk_channel( channel )
{
}

void hkdr_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hkdr_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hkdr_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hkdr_channel( *this );
}

void hkdr_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double m0 = 1/( 1.+EXP( -( v+29. )/4. ));
	double tm = 10./COSH(( v+29. )/8. );
	double m = EXP_EULER( M, m0, step, tm );
	double g = *pKGmax*Gmax*POW_4( m );
	double e = *pKIonsE*Hhn->KIons->Eds;

	M = m;
	G = g;
	I = g*( v-e );

	Hhn->G += g; 	// pass prevm
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hka_channel
//--- constructor
hka_channel::hka_channel( hhn_compart *neuron ) 
	: hhn_channel( neuron )
{
#ifdef __LOCOMOTION__
	ChannelId = _id_KAChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	hhn_channel::init_channel( 0.07, 1, 500 );
	init_channel( 0.47, 0.05, 0.25, 0.05 );
	Th = 1.;
#endif // __LOCOMOTION__
}

hka_channel::hka_channel( const hka_channel &channel )
	: hhn_channel( channel )
{
	init_channel( channel.Mka1, channel.Hka1, channel.M, channel.H );
}

hka_channel &hka_channel::operator = ( const hka_channel &channel )
{
	hhn_channel::operator = ( channel );
	init_channel( channel.Mka1, channel.Hka1, channel.M, channel.H );
	return *this;
}

void hka_channel::init_channel( double mka1, double hka1, double mka2, double hka2 )
{
	Mka1 = mka1;
	Hka1 = hka1;
	M = mka2;
	H = hka2;
}

void hka_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hka_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hka_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hka_channel( *this );
}

void hka_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double m0ka1 = 1./( 1+EXP( -( v+60 )/8.5 ));
	double tm1 = 1./( EXP(( v+35.82 )/19.69 )+EXP( -( v+79.69 )/12.7 )+0.37 );
	double h0ka1 = 1./( 1+EXP(( v+78 )/6. ));
	double th1 = ( v < -63 )? 1./( 1+EXP(( v+46.05 )/5. )+EXP( -( v+238.4 )/37.45 )): 19.0;
	double m0 = 1./( 1+EXP( -( v+36 )/20. ));
	double tm2 = Tm/( EXP(( v+35.82 )/19.69 )+EXP( -( v+79.69 )/12.7 )+0.37 );
	double h0 = 1./( 1+EXP(( v+78 )/6. ));
	double th2 = ( v < -73 )? Th/( 1+EXP(( v+46.05 )/5 )+EXP( -( v+238.4 )/37.45 )): Th*500.;
	double mka1 = EXP_EULER( Mka1, m0ka1, step, tm1 );
	double hka1 = EXP_EULER( Hka1, h0ka1, step, th1 );
	double m = EXP_EULER( M, m0, step, tm2 );
	double h = EXP_EULER( H, h0, step, th2 );
	double g = *pKGmax*Gmax*( 0.6*POW_4( mka1 )*hka1+0.4*POW_4( m )*h );
	double e = *pKIonsE*Hhn->KIons->Eds;

	M = m;
	H = h;
	Mka1 = mka1;
	Hka1 = hka1;
	G = g;
	I = g*( v-e );

	Hhn->G += g; 	// pass prevm
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hcal_channel
//--- constructor
hcal_channel::hcal_channel( hhn_compart *neuron ) 
	: hhn_channel( neuron )
{
#ifdef __LOCOMOTION__
	ChannelId = _id_CaLChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	hhn_channel::init_channel( 0.33, 1, 1 );
	init_channel( 0.01 );
	H = 1;
	Tm = 40.;
#endif // __LOCOMOTION__
}

hcal_channel::hcal_channel( const hcal_channel &channel )
	: hhn_channel( channel )
{
}

void hcal_channel::init_channel( double mcal, double hcal )
{
	M = mcal;
	H = hcal;
}

//--- overrided function
void hcal_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hcal_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hcal_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hcal_channel( *this );
}

void hcal_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double tm = Tm;
	double m0 = 1./( 1+EXP(( v+40. )/-7 ));
	double m = EXP_EULER( M, m0, step, tm );
	double g = *pKGmax*Gmax*m;
	double e = *pKIonsE*Hhn->CaIons->Eds;

	M = m;
	G = g;
	I = g*( v-e );
	Hhn->CaIons->I += I;

	Hhn->G += g;			// pass prevm
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hm_channel
//--- constructor
hm_channel::hm_channel( hhn_compart *neuron ) 
	: hhn_channel( neuron )
{
	ChannelId = _id_MChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	hhn_channel::init_channel( 0, 1, 1 );
	init_channel( 0 );
	Tm = 1.;
}

hm_channel::hm_channel( const hm_channel &channel )
	: hhn_channel( channel )
{
}

void hm_channel::init_channel( double mm )
{
	M = mm;
}

void hm_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hm_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hm_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hm_channel( *this );
}

void hm_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double m0 = 1./( 1+EXP( -( v+35 )/10. ));
	double tm = Tm*1000./( 3.3*EXP(( v+35 )/40. )+EXP( -( v+35 )/20. ));
	double m = EXP_EULER( M, m0, step, tm );
	double g = *pKGmax*Gmax*m;
	double e = *pKIonsE*Hhn->KIons->Eds;

	M = m;
	G = g;
	I = g*( v-e );

	Hhn->G += g;		// pass prevm
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hcat_channel
//--- constructor
hcat_channel::hcat_channel( hhn_compart *neuron ) 
	: hhn_channel( neuron )
{
#ifdef __LOCOMOTION__
	ChannelId = _id_CaTChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	hhn_channel::init_channel( 0.003, 1, 1 );
	init_channel( 0.5, 0 );
#endif // __LOCOMOTION__
}

hcat_channel::hcat_channel( const hcat_channel &channel )
	: hhn_channel( channel )
{
}

void hcat_channel::init_channel( double mcat, double hcat )
{
	M = mcat;
	H = hcat;
}

void hcat_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hcat_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hcat_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hcat_channel( *this );
}

void hcat_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double m0 = 1./( 1.+EXP( -( v+60.5 )/6.2 ));
	double tm = ( 0.612+( 1./( 1.+EXP( -( v+131.6 )/16.7 )+EXP(( v+16.8 )/18.2 ))))*Tm;
	double h0 = 1./( 1.+EXP(( v+84.5 )/4.03 ));
	double th = ( v < -80 )? Th*( EXP(( v+467 )/66.6 )): Th*( 28.+EXP( -( v+21.88 )/10.52 ));
	double m = EXP_EULER( M, m0, step, tm );
	double h = EXP_EULER( H, h0, step, th );
	double g = *pKGmax*Gmax*POW_2( m )*h;;
	double e = *pKIonsE*Hhn->CaIons->Eds;

	M = m;
	H = h;
	G = g;
	I = g*( v-e );
	Hhn->CaIons->I += I;

	Hhn->G += g; // pass prevm
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hcan_channel
//--- constructor
hcan_channel::hcan_channel( hhn_compart *neuron ) 
	: hhn_channel( neuron )
{
#ifdef __LOCOMOTION__
	ChannelId = _id_CaNChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	hhn_channel::init_channel( 14, 1, 1 );
	init_channel( 0, 0 );
	Tm = 4.;
	Th = 40.;
#endif // __LOCOMOTION__
}

hcan_channel::hcan_channel( const hcan_channel &channel )
	: hhn_channel( channel )
{
}

void hcan_channel::init_channel( double mcan, double hcan )
{
	M = mcan;
	H = hcan;
}

void hcan_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hcan_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hcan_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hcan_channel( *this );
}

void hcan_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double m0 = 1./( 1+EXP(( v+30. )/-5. ));
	double h0 = 1./( 1+EXP(( v+45.)/5. ));
	double tm = Tm;
	double th = Th;
	double m = EXP_EULER( M, m0, step, tm );
	double h = EXP_EULER( H, h0, step, th );
	double g = *pKGmax*Gmax*POW_2( m )*h;;
	double e = *pKIonsE*Hhn->CaIons->Eds;

	M = m;
	H = h;
	G = g;
	I = g*( v-e );
	Hhn->CaIons->I += I;

	Hhn->G += g;	// pass prevm
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hkc_channel
//--- constructor
hkc_channel::hkc_channel( hhn_compart *neuron ) 
	: hhn_channel( neuron )
{
	ChannelId = _id_KCChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	hhn_channel::init_channel( 0, 1, 1 );
	init_channel( 0 );
	H = 1.;
	Tm = 1.;
}

hkc_channel::hkc_channel( const hkc_channel &channel )
	: hhn_channel( channel )
{
}

void hkc_channel::init_channel( double mkc )
{
	M = mkc;
}

void hkc_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hkc_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hkc_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hkc_channel( *this );
}

void hkc_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double cacnc = Hhn->CaIons->In;
	double tm = Tm/( 250.*cacnc*EXP( v/24. )+0.1*EXP( -( v/24. )));
	double m0 = 250.*cacnc*EXP( v/24. )*tm;
	double m = EXP_EULER( M, m0, step, tm );
	double g = *pKGmax*Gmax*m;
	double e = *pKIonsE*Hhn->KIons->Eds;

	M = m;
	G = g;
	I = g*( v-e );

	Hhn->G += g;	// pass prevm
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hkca_channel
//--- constructor
hkca_channel::hkca_channel( hhn_compart *neuron ) 
	: hhn_channel( neuron )
{
#ifdef __LOCOMOTION__
	ChannelId = _id_KCaChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	hhn_channel::init_channel( 5, 0.2, 1 );
	init_channel( 0 );
	H = 1;
	Tm = 0.2;
#endif // __LOCOMOTION__
}

hkca_channel::hkca_channel( const hkca_channel &channel )
	: hhn_channel( channel )
{
}

void hkca_channel::init_channel( double mkca )
{
	M = mkca;
}

void hkca_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hkca_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hkca_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hkca_channel( *this );
}

void hkca_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double cacnc = Hhn->CaIons->In;
	double m0 = cacnc/( cacnc+0.2 );
	double tm = Tm*1000./( cacnc+0.2 );
	double m = EXP_EULER( M, m0, step, tm );
	double g = *pKGmax*Gmax*m;
	double e = *pKIonsE*Hhn->KIons->Eds;

	M = m;
	G = g;
	I = g*( v-e );

	Hhn->G += g;	// pass prevm
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hh_channel
//--- constructor
hh_channel::hh_channel( hhn_compart *neuron ) 
	: hhn_channel( neuron )
{
	ChannelId = _id_HChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	hhn_channel::init_channel( 0, 1, 1 );
	init_channel( 0, -43 );
	H = 1;
	Tm = 1.;
}

hh_channel::hh_channel( const hh_channel &channel )
	: hhn_channel( channel )
{
	init_channel( channel.M, channel.Eh );
}

hh_channel &hh_channel::operator = ( const hh_channel &channel )
{
	hhn_channel::operator = ( channel );
	init_channel( channel.M, channel.Eh );
	return *this;
}

void hh_channel::init_channel( double mh, double eds )
{
	M = mh;
	Eh = eds;
}

void hh_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hh_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hh_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hh_channel( *this );
}

void hh_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double m0 = 1./( 1.+EXP(( v+75. )/5.5 ));
	double tm = Tm*3900./( EXP( -7.68-0.086*v )+EXP( 5.04+0.0701*v ));
	double m = EXP_EULER( M, m0, step, tm );
	double g = *pKGmax*Gmax*m;
	double e = *pKIonsE*Eh;

	M = m;
	G = g;
	I = g*( v-e );

	Hhn->G += g;	// pass prevm
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hleak_channel
//--- constructor
hleak_channel::hleak_channel( hhn_compart *neuron ) 
	: hhn_channel( neuron )
{
#ifdef __LOCOMOTION__
	ChannelId = _id_LeakChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	hhn_channel::init_channel( 1, 1, 1 );
	init_channel( -60 );
	M = 1.;
	H = 1.;
#endif // __LOCOMOTION__
}

hleak_channel::hleak_channel( const hleak_channel &channel )
	: hhn_channel( channel )
{
	init_channel( channel.Eleak );
}

hleak_channel &hleak_channel::operator = ( const hleak_channel &channel )
{
	hhn_channel::operator = ( channel );
	init_channel( channel.Eleak );
	return *this;
}

void hleak_channel::init_channel( double eds )
{
	Eleak = eds;
}

void hleak_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hleak_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hleak_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hleak_channel( *this );
}

void hleak_channel::calc_g( double step )
{
	double g = *pKGmax*Gmax;
	double e = *pKIonsE*Eleak;

	G = g;
	I = g*( Hhn->get_vm()-e );

	Hhn->G  += g;	// pass prevm
	Hhn->GE += g*e;
}

#endif // defined( __LOCOMOTION__ ) || defined( __RESPIRATION__ )

/////////////////////////////////////////////////////////////////////////////
// class hsoma_channel
hsoma_channel::hsoma_channel( hhn_compart *neuron )
	: hhn_channel( neuron )
{
	ChannelId = _id_SomaChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	hhn_channel::init_channel( 0.1, 1, 1 );
	I = 0;
	init_channel( NULL, 0.1 );
}

hsoma_channel::hsoma_channel( const hsoma_channel &channel )
	: hhn_channel( channel )
{
	init_channel( channel.Soma, channel.P );
}

hsoma_channel &hsoma_channel::operator = ( const hsoma_channel &channel )
{
	hhn_channel::operator = ( channel );
	init_channel( channel.Soma, channel.P );
	return *this;
}

void hsoma_channel::init_channel( const hhn_compart *soma, double p )
{
	Soma = ( hhn_compart * )soma;
	P = p;
	G = Gmax/(1-P);
}

void hsoma_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hsoma_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hsoma_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hsoma_channel( *this );
}

void hsoma_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double g = *pKGmax*G;
	double e = *pKIonsE*Soma->get_vm();

	I = g*( v-e );

	Hhn->G += g;	// pass prevm
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hdendr_channel
hdendr_channel::hdendr_channel( hhn_compart *neuron )
	: hhn_channel( neuron )
{
	ChannelId = _id_DendrChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	hhn_channel::init_channel( 0.1, 1, 1 );
	I = 0;
	init_channel( NULL, 0.1 );
}

hdendr_channel::hdendr_channel( const hdendr_channel &channel )
	: hhn_channel( channel )
{
	init_channel( channel.Dendr, channel.P );
}

hdendr_channel &hdendr_channel::operator = ( const hdendr_channel &channel )
{
	hhn_channel::operator = ( channel );
	init_channel( channel.Dendr, channel.P );
	return *this;
}

void hdendr_channel::init_channel( const hhn_compart *dendr, double p )
{
	Dendr = ( hhn_compart * )dendr;
	P = p;
	G = Gmax/P;
}

void hdendr_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hdendr_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hdendr_channel::copy_to( hhn_channel **chan ) const
{
	*chan = new hdendr_channel( *this );
}

void hdendr_channel::calc_g( double step )
{
	double v = Hhn->get_vm();
	double g = *pKGmax*G;
	double e = *pKIonsE*Dendr->get_vm();
	I = g*( v-e );
	Hhn->G += g;	// pass prevm
	Hhn->GE += g*e;
}
