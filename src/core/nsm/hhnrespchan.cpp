/////////////////////////////////////////////////////////////////////////////
// class hhnrespchan.h
#include "precompile.h"

#ifdef __RESPIRATION__

#include <cmath>
#include "hhnrespchan.h"
#include "hhn.h"
#include "hhnnetwork.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG
#endif //__LINUX__

/*****************************************************************
 *       Model of Na channel for Hodgkin-Haxley type neuron      *
 *****************************************************************/
//--- constructor
hrnaf_channel::hrnaf_channel( hhn_compart *neuron ) 
             : hna_channel( neuron )
{
 ChannelId = _id_resp_NaFChan; 
 if( _ChannelNames[ChannelId] )
     Name = _ChannelNames[ChannelId];
 Gmax = 150; 
 Tm = 0.28;
 Th = 0.28;
 init_channel( 0.02, 0.3 );
}

hrnaf_channel::hrnaf_channel( const hrnaf_channel &channel )
             : hna_channel( channel )
{
}

//--- overrided function
void hrnaf_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hrnaf_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hrnaf_channel::copy_to( hhn_channel **chan )
{
 *chan = new hrnaf_channel( *this );
}

void hrnaf_channel::calc_g( double step )
{
	double v = Hhn->Vm[1];
	double m0 = 1./( 1.+EXP( -( v+43.8 )/6. ));
	double tm = Tm*0.9/COSH(( v+43.8 )/14. );
	double h0 = 1./( 1.+EXP(( v+67.5 )/10.8 ));
	double th = Th*30.2/COSH(( v+67.5 )/12.8 );
	double m = EXP_EULER( M, m0, step, tm );
	double h = EXP_EULER( H, h0, step, th );

	double g = *pKGmax*Gmax*POW_3( m )*h;
	double e = *pKIonsE*Hhn->NaIons->Eds;


	M = m;
	H = h;
	G = g;
	I = g*( v-e );
	Hhn->G += g;
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hrnap_channel
//--- constructor
hrnap_channel::hrnap_channel( hhn_compart *neuron ) 
	: hna_channel( neuron )
{
	ChannelId = _id_resp_NaPChan; 
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	Gmax = 8;
	Tm = 0.28;
	Th = 3000.;
	init_channel( 0.02, 0.3 );
}

hrnap_channel::hrnap_channel( const hrnap_channel &channel )
	: hna_channel( channel )
{
}

void hrnap_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hrnap_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hrnap_channel::copy_to( hhn_channel **chan )
{
	*chan = new hrnap_channel( *this );
}

void hrnap_channel::calc_g( double step )
{
	double v = Hhn->Vm[1];
	double m0 = 1/( 1.+EXP( -( v+47.1 )/3.1 ));
	double tm = Tm*0.9/COSH( -( v+47.1 )/6.2 );
	double h0 = 1./( 1.+EXP(( v+57. )/4. ));
	double th = Th/COSH(( v+57 )/8 );
	double m = EXP_EULER( M, m0, step, tm );
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
// class hrkdr_channel
//--- constructor
hrkdr_channel::hrkdr_channel( hhn_compart *neuron )
	: hk_channel( neuron )
{
	ChannelId = _id_resp_KdrChan; 
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	Gmax = 90;
	Tm = 1.0;
	H = 1;
	init_channel( 0.03 );
}

hrkdr_channel::hrkdr_channel( const hrkdr_channel &channel )
	: hk_channel( channel )
{
}

void hrkdr_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hrkdr_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hrkdr_channel::copy_to( hhn_channel **chan )
{
	*chan = new hrkdr_channel( *this );
}

void hrkdr_channel::calc_g( double step )
{
	double v = Hhn->Vm[1];
	double shift = -1.;
	double k0 = v+45+shift; 
	double k1 = 0.01*k0/( 1-EXP( -k0/5. ));
	double k2 = 0.17*EXP( -( v+50+shift )/40. );
	double k3 = 1./( k1+k2 );
	double tm = Tm*k3;
	double m0 = k1*k3;
	double m = EXP_EULER( M, m0, step, tm );
	double g = *pKGmax*Gmax*POW_4( m );
	double e = *pKIonsE*Hhn->KIons->Eds;

	M = m;
	G = g;
	I = g*( v-e );
	Hhn->G += g;
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hrka_channel
//--- constructor
hrka_channel::hrka_channel( hhn_compart *neuron ) 
	: hka_channel( neuron )
{
	ChannelId = _id_resp_KAChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	hhn_channel::init_channel( 70, 1, 300 );
	Th = 300.;
	init_channel( 0.47, 0.05, 0.25, 0.05 );
}

hrka_channel::hrka_channel( const hrka_channel &channel )
	: hka_channel( channel )
{
}

void hrka_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hrka_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hrka_channel::copy_to( hhn_channel **chan )
{
	*chan = new hrka_channel( *this );
}

void hrka_channel::calc_g( double step )
{
	double v = Hhn->Vm[1];
	double m0ka1 = 1./( 1+EXP( -( v+56.83 )/13.8 ));
	double tm1 = 5.5*EXP( -( v+65. )*( v+65. )*0.000484 )+1.;
	double h0ka1 = 1./( 1+EXP(( v+78. )/4.7 ));
	double th1 = 70.0;
	double m0 = 1./( 1+EXP( -( v+56.83 )/13.8 ));
	double tm2 = Tm*5.5*EXP(-( v+65. )*( v+65. )*0.000484 )+1.;
	double h0 = h0ka1; 
	double th2 = Th;
	double mka1 = EXP_EULER( Mka1, m0ka1, step, tm1 );
	double hka1 = EXP_EULER( Hka1, h0ka1, step, th1 );
	double m = EXP_EULER( M, m0, step, tm2 );
	double h = EXP_EULER( H, h0, step, th2 );

	double g = *pKGmax*Gmax*( 0.5*POW_3( mka1 )*hka1+0.5*POW_3( m )*h );;
	double e = *pKIonsE*Hhn->KIons->Eds;

	M = m;
	H = h;
	Mka1 = mka1;
	Hka1 = hka1;
	G = g;
	I = g*( v-e );
	Hhn->G += g;
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hrcal_channel
//--- constructor
hrcal_channel::hrcal_channel( hhn_compart *neuron ) 
	: hcal_channel( neuron )
{
	ChannelId = _id_resp_CaLChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	Gmax = 3;
	Tm = 0.5;
	Th = 18.;
	init_channel( 0.01, 0.01 );
}

hrcal_channel::hrcal_channel( const hrcal_channel &channel )
	: hcal_channel( channel )
{
}

void hrcal_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hrcal_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hrcal_channel::copy_to( hhn_channel **chan )
{
	*chan = new hrcal_channel( *this );
}

void hrcal_channel::calc_g( double step )
{
	double v = Hhn->Vm[1];
	double m0 = 1./( 1+EXP( -( v+27.4 )/5.7 ));
	double h0 = 1./( 1+EXP(( v+52.4 )/5.2 ));
	double tm = Tm;
	double th = Th;
	double m = EXP_EULER( M, m0, step, tm );
	double h = EXP_EULER( H, h0, step, th );
	double g = *pKGmax*Gmax*m*h;
	double e = *pKIonsE*Hhn->CaIons->Eds;

	M = m;
	H = h;
	G = g;
	I = g*( v-e );
	Hhn->G += g;
	Hhn->GE += g*e;
	Hhn->CaIons->I += I;
}

/////////////////////////////////////////////////////////////////////////////
// class hrcat_channel
//--- constructor
hrcat_channel::hrcat_channel( hhn_compart *neuron ) 
	: hcat_channel( neuron )
{
	ChannelId = _id_resp_CaTChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	Gmax = 3;
	init_channel( 0.5, 0 );
}

hrcat_channel::hrcat_channel( const hrcat_channel &channel )
	: hcat_channel( channel )
{
}

void hrcat_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hrcat_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hrcat_channel::copy_to( hhn_channel **chan )
{
	*chan = new hrcat_channel( *this );
}

void hrcat_channel::calc_g( double step )
{
	double v = Hhn->Vm[1];
	double m0 = 1./( 1.+EXP( -( v+58.91 )/2.38 ));
	double tm = Tm;
	double h0 = 1./( 1.+EXP(( v+82. )/5.34 ));
	double th = ( v < -80 )? Th*EXP(( v+467. )/66.6 ): Th*( 28.+EXP( -( v+21.88 )/10.52 ));
	double m = EXP_EULER( M, m0, step, tm );
	double h = EXP_EULER( H, h0, step, th );
	double g = *pKGmax*Gmax*m*h;
	double e = *pKIonsE*Hhn->CaIons->Eds;

	M = m;
	H = h;
	G = g;
	I = g*( v-e );
	Hhn->G += g;
	Hhn->GE += g*e;
	Hhn->CaIons->I += I;
}

/////////////////////////////////////////////////////////////////////////////
// class hrcat_channel
//--- constructor
hrkca_channel::hrkca_channel( hhn_compart *neuron ) 
	: hkca_channel( neuron )
{
	ChannelId = _id_resp_KCaChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	Gmax = 7; 
	H = 1;
	Tm = 2.0;
	init_channel( 0.22 );
}

hrkca_channel::hrkca_channel( const hrkca_channel &channel )
	: hkca_channel( channel )
{
}

void hrkca_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hrkca_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hrkca_channel::copy_to( hhn_channel **chan )
{
	*chan = new hrkca_channel( *this );
}

void hrkca_channel::calc_g( double step )
{
	double v = Hhn->Vm[1];
	double cacnc = Hhn->CaIons->In;
	double k1 = 125000000.*cacnc*cacnc;
	double m0 = k1/(k1+2.5);
	double tm = Tm*1000./(k1+2.5); 
	double m = EXP_EULER( M, m0, step, tm );
	double g = *pKGmax*Gmax*POW_2( m );
	double e = *pKIonsE*Hhn->KIons->Eds;

	M = m;
	G = g;
	I = g*( v-e );
	Hhn->G += g;
	Hhn->GE += g*e;
}

/////////////////////////////////////////////////////////////////////////////
// class hrleak_channel
//--- constructor
hrleak_channel::hrleak_channel( hhn_compart *neuron ) 
	: hleak_channel( neuron )
{
	ChannelId = _id_resp_LeakChan;
	if( _ChannelNames[ChannelId] )
		Name = _ChannelNames[ChannelId];
	Gmax = 10;
	M = 1;
	H = 1;
	init_channel( false, 0.05, 0.45, -65 );
}

hrleak_channel::hrleak_channel( const hrleak_channel &channel )
	: hleak_channel( channel )
{
	init_channel( channel.Adjustable, channel.Pna, channel.Pcl, channel.Eleak );
}

hrleak_channel &hrleak_channel::operator = ( const hrleak_channel &channel )
{
	hleak_channel::operator = ( channel );
	init_channel( channel.Adjustable, channel.Pna, channel.Pcl, channel.Eleak );
	return *this;
}

void hrleak_channel::init_channel( bool adjustable, double pna, double pcl, double eds )
{
	Adjustable = adjustable;
	Pna = pna;
	Pcl = pcl;
	if( Adjustable )
		calc_eds();
	else 
		Eleak = eds;
}

void hrleak_channel::reg_unit( runman *man  )
{
	::reg_unit( this, hrleak_channel::calc_g, _id_PASS_G, size_t( Hhn ), man ); 
}

void hrleak_channel::calc_eds(void) 
{
	double InK = Hhn->KIons->In;
	double OutK = Hhn->KIons->Out;
	double InNa = Hhn->NaIons->In;
	double OutNa = Hhn->NaIons->Out;
	double InCl = Hhn->ClIons->In;
	double OutCl = Hhn->ClIons->Out;
	double RTF = Hhn->NaIons->RTF;

	Eleak = RTF*log((OutK + Pna*OutNa + Pcl*InCl)/(InK + Pna*InNa + Pcl*OutCl));
}

void hrleak_channel::copy_to( hhn_channel **chan )
{
	*chan = new hrleak_channel( *this );
}

void hrleak_channel::calc_g( double step )
{
	if( Adjustable )
		calc_eds();
	double v = Hhn->Vm[1];
	double g = *pKGmax*Gmax;
	double e = *pKIonsE*Eleak;
	G = g;
	I = g*( v-e );
	Hhn->G += g;
	Hhn->GE += g*e;
}

#endif // __RESPIRATION__
