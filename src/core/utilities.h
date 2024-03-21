#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include "../config.h"

#pragma warning( disable : 4996 )

const double Gravity = 0.00981; //mm/msec
const double PI = 3.14159265358979323846;

#define EPS 1.2e-7

#define POW_2( x ) \
        ( x )*( x )
#define POW_3( x ) \
        POW_2( x )*( x )
#define POW_4( x ) \
        POW_3( x )*( x )
#define EXP( x ) exp( x )
#define COSH( x ) cosh( x )

#define SIGMOID( vm, hvm, slp ) \
        1./( 1.+exp((( vm )-( hvm ))/( slp )))
#define GENERIC_G( vm, hvm, slp ) \
        1./( 1.+exp((( vm )-( hvm ))/( slp )))
#define GENERIC_T( t, vm, hvm, slp ) \
        (( t ) <= EPS ) ? ( t ) : ( t )/( cosh((( vm )-( hvm ))/( slp )))
#define GENERIC_T_MOD( shift, t, vm, hvm, slp_1, slp_2 ) \
        (( t ) <= EPS ) ? ( shift ) : ( shift )+(2.*( t ))/( exp((( vm )-( hvm ))/( slp_1 ))+exp((( vm )-( hvm ))/( slp_2 )))
#define EXP_EULER( y, y0, step, t ) \
        (( t ) <= EPS ) ? ( y0 ) : ( exp(-(( step )/(t))))*(( y )-( y0 ))+( y0 )
#define EXP_EULER_EXPT( y, y0, expt ) \
        (( expt ) <= EPS ) ? ( y0 ) : ( expt )*(( y )-( y0 ))+( y0 ) /*expt = exp(-(( step )/(t)))*/
/*
#define EPS 1.0e-6

#define GENERIC_G( vm, hvm, slp ) \
        1./( 1.+EXP((( vm )-( hvm ))/( slp )))
#define GENERIC_T( t, vm, hvm, slp ) \
        (( t ) <= EPS ) ? ( 0. ) : ( t )/( COSH((( vm )-( hvm ))/( slp )))
#define GENERIC_T_MOD( shift, t, vm, hvm, slp_1, slp_2 ) \
        (( t ) <= EPS ) ? ( shift ) : ( shift )+(2.*( t ))/( EXP((( vm )-( hvm ))/( slp_1 ))+EXP((( vm )-( hvm ))/( slp_2 )))
#define EXP_EULER( y, y0, step, t ) \
        (( t ) < step ) ? ( y0 ) : ( EXP(-(( step )/(t))))*(( y )-( y0 ))+( y0 )
*/
#include <iostream>
#include <string>
#include <sstream>

using std::istream;
using std::string;
using std::stringstream;

inline int sign( double d )
{
	return d > 0? 1:( d < 0? -1:0 );
}

inline double deg2rad( double angle )
{
	return ( angle*PI )/180.;
}

template <class T >
bool operator << ( string &data, T &x )
{
	stringstream line;
	line << x;
	line >> data;
	return true;
}

template <class T >
bool operator >> ( string &data, T &x )
{
	x = 0;
	stringstream line;
	line << data;
	line >> x;
	return true;
}


extern void message( const char *mess, const char *name );
extern bool remove_char( const char *filename, char ch );
extern void unknown_string( istream &file, string str );

#endif //_UTILITIES_H_
