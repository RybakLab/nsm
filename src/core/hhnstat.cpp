/*****************************************************************
 ****              (C)  1997 - 2000                           ****
 **           Model of Hodgkin-Haxley type neuron               **
 **         Developed by Ilia Rybak and Sergey Markin           **
 ****                      Statistics                         ****
 *****************************************************************/
#include "precompile.h"

#include <cmath>

#include <time.h>
#include "hhnstat.h"

/////////////////////////////////////////////////////////////////////////////
// local methods
static long newseed( long seed )
{
	long k = seed/IQ;
	seed = IA*(seed-k*IQ)-IR*k;
	if( seed < 0 ) 
		seed += IM;
	return seed;
}

/////////////////////////////////////////////////////////////////////////////
// class CStat
bool CStat::Flag = true;
long CStat::Seed = long( -time( NULL ));
long CStat::Iy = 0;
long CStat::Iv[NTAB] = {0};
double CStat::Gset = 0.;

// --- public functions
void CStat::SetSeed( long  seed )
{
	Seed = seed;
	Iy = 0;
	for( int i = 0; i < NTAB; Iv[i] = 0, i++ );
	Flag = true;
	Gset = 0.;
}

double CStat::Random( void )
{
	if( Seed <= 0 || !Iy ){
		Seed = ( -Seed < 1 )? 1: -Seed;
		for( int i = NTAB + 7; i >= 0; i--){
			Seed = newseed( Seed );
			if( i < NTAB ) 
				Iv[i] = Seed;
		}
		Iy = Iv[0];
	}
	Seed = newseed( Seed );
	int index = Iy/NDIV;
	Iy = Iv[index];
	Iv[index] = Seed;
	double temp = AM*Iy;
	if( temp > RNMX) 
		return RNMX;
	return temp;
}

// --- private functions
double CStat::GaussDistr( void )
{
	if( Flag ){
		Flag = false;
		double rsq = 0., v1 = 0., v2 = 0.;
		for( rsq = 0., v1 = 0., v2 = 0.; rsq >= 1. || rsq == 0.; rsq = v1*v1+v2*v2 ){
			v1 = 2.0*Random()-1.0;
			v2 = 2.0*Random()-1.0;
		}
		double fac = sqrt( -2.*log( rsq )/rsq );
		Gset = v1*fac;
		return v2*fac;
	} 
	else{
		Flag = true;
		return Gset;
	}
}

ostream &operator << ( ostream &s, const CStat &x )
{
	return s << x.Value << "(" << x.Variance << ")";
}

istream &operator >> ( istream &s, CStat &x )
{
	char ch =0;
	double val = 0, var = 0;
	bool OK = true;

	s >> val;
	s.get(ch);
	if( ch == '(' ){
		s >> var;
		s.get(ch);
		OK = (ch == ')');
	}
	else{
		s.putback(ch);
	}
	
	if(s && OK){
		x.Value=val;
		x.Variance=var;
	}
	return s;
}
