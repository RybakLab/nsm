////////////////////////////////////////////////////////////////////////
// muscle.h
//---------------------------------------------------------------------
#ifndef __MUSCLE_H
#define __MUSCLE_H

#ifdef __MECHANICS__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "sptypes.h"

struct wlink{
	int Type;
	int In;	
};

struct wjoint{
	double A;
	double dA;
};

class hhn_output;
class unit_code;
/////////////////////////////////////////////////////////////////////////////
// zelastic class
class zelastic{
	public: // constructors/destructor
		zelastic( double kl, double ke );
		zelastic( const zelastic &ee );
		~zelastic( void );
	public: // operators
		zelastic &operator = ( const zelastic &ee );
	public: // methods
		void preproc( void );
		double force( double l )
		{
			return KLbyke*( exp( KE*l )-1.0 )/( 1.0+exp( -8000000.0*KE*l ));
		};
	public:	// constants
		double KL;
		double KE;
	private: // variables
		double KLbyke;
#ifdef __NEW_MUSCLE__
	public: // methods
		double force2( double l )
		{
			if( l < 0.0 ){
				return 0.0;
			}
			if( l < dL1 ){
				return F1*( exp( Ksh*l/dL1 )-1.0 )/( exp( Ksh )-1.0 );
			}
			else{
				return F1+Klo*( l-dL1 );
			}
		};

	public:
		double dL1; // new
		double F1;	// new
		double Klo; // new
		double Ksh;	// new
#endif // __NEW_MUSCLE__
};

/////////////////////////////////////////////////////////////////////////////
// zcontrac class
class zcontrac{
	public: // constructors/destructor
		zcontrac( void );
		zcontrac( const zcontrac &ce );
		~zcontrac( void );
	public: // operators
		zcontrac &operator = ( const zcontrac &ce );
	public: // methods
		void preproc( void );
		double force( double l, double v )
		{
			double fm = ( Vmax+v )/( Vmax-AV*v );
			double fp = ( B0+B1*v )/( B0+v );
			return ( fm+( fp-fm )/( 1.0+exp( -4000.0*v )))*exp( -pow( fabs(( pow( l, Beta )-1.0 )/Omega ), RHO ));
		};
	public:	// constants
		double AV;
		double Beta;
		double Fmax;
		double Omega;
		double RHO;
		double Vmax;
	private: // variables
		double B0;
		double B1;
#ifdef __NEW_MUSCLE__
	public: // methods
		double force2( double l, double v )
		{
			return fl( l )*fv( v );
		};
	private: // methods
		double fv( double v )
		{
			if( v < -Vtr ){
				return ( Vmax+v )/( Vmax-AV*v );
			}
			else if( v < 0 ){
				double k0 = 100.0; 
				double h = ( F0-Ftr )*Vtr/( Vtr*k0-F0+Ftr );
				double k = F0-k0*h;
				double m = -k0*h*h;
				return k+m/( v-h );
			}
			else{
				double k0 = 50.0;
				double k = 1.85;
				double h = ( F0-k )/k0;
				double m = - k0*h*h;
				return k+m/( v-h );
			}
			return 0.0;
		};
		double fl( double l )
		{
 			double l0 = 0.38961039;
			double l1 = 0.636363636;
			double l2 = 1.0;
			double l3 = 1.077922078;
			double l4 = 1.909090909;

 			double a0 = -1.326315789;
			double a1 = 0.56;
			double a2 = 1;
			double a3 = 2.296875;

			double b0 = 3.404210526;
			double b1 = 0.44;
			double b2 = 0;
			double b3 = -1.203125;

			if( l < l0 )
				return 0.0;
			else if( l < l1 )
				return a0+b0*l;
			else if( l < l2 )
				return a1+b1*l;
			else if( l < l3 )
				return 1.;
			else if( l < l4 )
				return a3+b3*l;
			return 0.0;
		};

	public:	// constants
		double F0;		// new
		double Ftr;		// new
		double Vtr;		// new
#endif // __NEW_MUSCLE__
};

/////////////////////////////////////////////////////////////////////////////
// zmuscle class
class zmuscle{
	public: // constructors/destructor
		zmuscle( void );
		zmuscle( const zmuscle &muscle );
virtual	~zmuscle( void ){};
	public: // operators
		zmuscle &operator = ( const zmuscle &muscle );
	public: // methods
		void init( void );
		double get_at( double l, double v )
		{
			double lt = ( L-l )/( 1.0+exp( -400.0*( L-l )));
			double lm = sqrt( lt*lt+W2 );
			double cos_alpha = lt/lm;
			double vm = ( V-v )*cos_alpha;		// Fiber velocity
			double mn = *MN*Amax;				// Activation
			lm /= L0;							// Normalization
			F = PT0*TE.force(( l-LT0 )/LT0 );	// total muscle force
			double ft = F+PT0*BT*v;
			double ff = P0*( PE.force( lm+Shift-1 )+CE.force( lm, vm )*mn+BM*vm )*cos_alpha;
			if( ff < 0.0 ) ff = 0.0;
			if( ft < 0.0 ) ft = 0.0;
			return -( ft-ff )/M;
		};
		void calc_lvm( void )
		{
			double lt = ( L-LT );
			LM = sqrt( lt*lt+W2 );		// muscle length
			double cos_alpha = lt/LM;	
			VM = ( V-VT )*cos_alpha;	// muscle velocity
		};
	public: // methods
		double get_lrest( void ){ return Ltr*L0*cos( Alpha0 ); };
		double get_ltr( void ){ return Ltr*L0*cos( Alpha0 ); };
		const void *select( unit_code *view )const;
	public:
virtual	const char *get_name( void ) const{ return NULL; };
virtual	void preproc( double *llink );
virtual	void proc( wjoint *joints ){};
virtual	void add_f( double *force, wlink *link ){};
	protected:	// attachment properties
		double H1;		// Upper Arm
		double X1;
		double X2;
		double A1A1;
		double A2A2;
		double A1A2;
		double R1R1;
		double Beta1;
		double Beta2;
	private: // muscle properties
		double L0;		// optimal length of fiber (at this length fiber produce maximum force)
		double W;
		double W2;
	public: // muscle properties
		double Amax;
		double *MN;		// motoneuron activity
		double P0;		// maximum fiber force
		double F;		// total force produced by the muscle
		double L;		// total muscle length
		double V;		// total muscle velocity
		double LM;		// muscle length
		double VM;		// muscle velocity
		double LT;		// tendon length
		double VT;		// tendon velocity
		double BM;		// muscle viscosity
		double BT;		// tendon viscosity
		double Ltr;		//
	public:
		int F1;			// F1 == 1 -> Counterclockwise Muscle; F1 == -1 -> Clockwise Muscle
		double Phi1;	// Angle between Upper Link and Direction to Upper Attachment point
		double Phi2;	// Angle between Lower Link and Direction to Lower Attachment point
		double A1;		// Distance between Joint and Upper Attachment point
		double A2;		// Distance between Joint and Lower Attachment point
		double R1;		// Radius of joint block
	public:
		double LT0;		// slack (rest) length of tendon
		double PT0;		// maximum tendon force
		double Alpha0;	// angle
		double LT0perL0;//
		double M;
		double Shift;
		double Ladd;
	public:
		string MnName;
		zelastic TE;
		zelastic PE;
		zcontrac CE;
	private:
static	double _MN;
#ifdef __NEW_MUSCLE__
	public: // methods
		double get_at2( double l, double v )
		{
			double lt = ( L-l > 0)? L-l: 0; // Tendom displacement
			double lm = sqrt( lt*lt+W2 );	// Fiber length
			double cos_alpha = lt/lm;		// cos of pinnation angle
			double vm = ( V-v )*cos_alpha;	// Fiber velocity
			double mn = *MN*Amax;			// Activation
			lm /= L0;						// Normalization of the fiber length
			F = TE.force2( l/LT0-1.0 );		// Normalized muscle force = Normalized tendon force
			F += Wnet;						// Add history dependent force depression/enhancement
			if( F < 0.0 ) F = 0.0;
			double ff = P0*( PE.force( lm+Shift-1 )+CE.force2( lm, vm )*mn+BM*vm )*cos_alpha;
			double ft = PT0*( F+BT*v );
			ff = ( ff > 0.0 )? ff: 0.0;
			ft = ( ft > 0.0 )? ft: 0.0;
			return -( ft-ff )/M;
		};
		void afterstep( double step )
		{
			VMp = VM;
		};
		void prestep( double step )
		{
			if( *MN > 1.0e-02 ){
				double wi = 0.00943*step*F*V; //*N_sc; //F*v*dt ~[N]*[m/sec]*[sec]
				if( VM > 0 )
					wi /= 2.; //Force enhancement less then force depession about twice
				if( Wnet == 0 || wi == 0 || sign( VM ) == sign( VMp ))
					Wnet += wi;
				else if( VM > 0 && VMp < 0 )
					Wnet = Wnet/2+wi;
				else
					Wnet = 0;
			} 
			else 
			{
				Wnet = 0;
			}
		};
	private: // muscle properties
		double VMp;		// previous fiber velocity
		double Wnet;	// accumulated muscle work
#endif // __NEW_MUSCLE__
};

/////////////////////////////////////////////////////////////////////////////
// j1scheme class
class j1scheme{
	public:
		j1scheme( void ) : Upper( -1 ), Lower( -1 ), Joint( -1 ){};
		j1scheme( const j1scheme &x ) : Upper( x.Upper ), Lower( x.Lower ), Joint( x.Joint ){};
virtual	~j1scheme( void ){};
	public:
		j1scheme &operator = ( const j1scheme &x )
		{
			Upper = x.Upper; 
			Lower = x.Lower; 
			Joint = x.Joint;
			return *this;
		};
	public:	// i/o operations
		int Upper;
		int Lower;
		int Joint;
};

/////////////////////////////////////////////////////////////////////////////
// j2scheme class
class j2scheme : public j1scheme{
	public:
		j2scheme( void ) : j1scheme(), Middle( -1 ), LowerJoint( -1 ){};
		j2scheme( const j2scheme &x ) : j1scheme( x ), Middle( x.Middle ), LowerJoint( x.LowerJoint ){};
		~j2scheme( void ){};
	public:
		j2scheme &operator = ( const j2scheme &x )
		{
			j1scheme::operator = ( x );
			Middle = x.Middle;
			LowerJoint = x.LowerJoint;
			return *this;
		};
	public:	// i/o operations
		int Middle;
		int LowerJoint;
};


/////////////////////////////////////////////////////////////////////////////
// j1muscle class
class j1muscle : public zmuscle{
	public: // constructors/destructor
		j1muscle( const j1scheme &link );
		j1muscle( const j1muscle &muscle );
		~j1muscle( void ){};
	public: // operators
		j1muscle &operator = ( const j1muscle &m );
	public: // methods
		const char *get_name( void ) const;
		void preproc( double *llink );
		void proc( wjoint *joints );
		void add_f( double *force, wlink *link )
		{
			double q = F*H1;
			force[link[Link.Upper].In] -= q;
			force[link[Link.Lower].In] += q;
		};
	public:	// variables
		j1scheme Link;
};

/////////////////////////////////////////////////////////////////////////////
// j2muscle class
class j2muscle : public zmuscle{
	public: // constructors/destructor
		j2muscle( const j2scheme &link );
		j2muscle( const j2muscle &muscle );
		~j2muscle( void ){};
	public: // operators
		j2muscle &operator = ( const j2muscle &m );
	public: // methods
		const char *get_name( void ) const;
		void preproc( double *llink );
		void proc( wjoint *joints );
		void add_f( double *force, wlink *link )
		{
			double q1 = F*H1;
			double q2 = F*H2;
			force[link[Link.Upper].In] -= q1;
			force[link[Link.Lower].In] += q2;
			force[link[Link.Middle].In] += (q1-q2);
		};
	private: // Constants
		double R2R2;
		double LmLm;
		double LmA1;
		double LmA2;
		double R1A2;
		double R2A1;
		double LmR1;
		double ZP;
		double ZM;
		double H2;	// lower arm
	public:	// i/o operations
		j2scheme Link;
	public:	// i/o operations
		int F2;	// F2 == 1 -> Counterclockwise Muscle; F2 == -1 -> Clockwise Muscle
		double R2;	// radius of lower joint block
		double Lm;	// length of middle link
};

inline zmuscle *zcreate( const j1scheme &scheme )
{
	return ( zmuscle * )new j1muscle( scheme );
}

inline zmuscle *zcreate( const j2scheme &scheme )
{
	return ( zmuscle * )new j2muscle( scheme );
}

#endif // __MECHANICS__
#endif // __MUSCLE_H
