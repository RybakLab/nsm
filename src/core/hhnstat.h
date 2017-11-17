/*****************************************************************
 ****              (C)  1997 - 2000                           ****
 **           Model of Hodgkin-Haxley type neuron               **
 **         Developed by Ilia Rybak and Sergey Markin           **
 ****                 Statistics (header)                     ****
 *****************************************************************/
#ifndef _HHN_STAT_H_
#define _HHN_STAT_H_

#include <iostream>

using std::istream;
using std::ostream;

#define IA 48271
#define IM 2147483647
#define AM ( 1./IM )
#define IQ 44488
#define IR 3399
#define NTAB 32
#define NDIV ( 1+( IM-1 )/NTAB )
#define EPS1 1.2e-7
#define RNMX ( 1.-EPS1 )

class CStat{
	public:
		CStat( void ) : Value( -1 ), Variance( -1 ){};
		CStat( double val, double var = 0 ): Value( val ), Variance( var ){};
		CStat( const CStat &x ): Value( x.Value ), Variance( x.Variance ){};
		~CStat( void ){};
	public:
		operator double ( void )
		{
			if( Variance >= EPS1 ){
				double var = Variance*GaussDistr();
				if( var < -2*Variance )
					var = -2*Variance;
				else if( var > 2*Variance )
					var = 2*Variance;
				return Value+var*Value;
			}
			return Value;
		};
		bool operator == ( const CStat &x ) const{ return bool(( Value == x.Value )&&( Variance == x.Variance )); };
		CStat &operator = ( double x ){ Value = x; Variance = 0; return *this; };
		CStat &operator = ( const CStat &x ){ Value = x.Value; Variance = x.Variance; return *this; };
	public:
static	void SetSeed( long seed );
static	double Random( void );
	private:
static	double GaussDistr( void );

	public:
		double Value;                     // Value
		double Variance;                  // 0 <= Variance <= 1
	private:
static	bool Flag;
static	long Seed;
static	long Iy;
static	long Iv[NTAB];
static	double Gset;
};

extern ostream &operator << ( ostream &s, const CStat &x );
extern istream &operator >> ( istream &s, CStat &x);

#endif
