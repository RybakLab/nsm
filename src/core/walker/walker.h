//////////////////////////////////////////////////////////////////////
// walker.h
#ifndef __WALKER_H
#define __WALKER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (__MECHANICS_2D__)

#include "hhnunit.h"
#include "muscle.h"

using namespace std;

class CHhnNetwork;
//////////////////////////////////////////////////////////////////////
// Independance variable
enum _Independance {
	A_TL = 0,		//Angle of Left Thigh
	A_SL,
	A_FL,
	A_TR,			//Angle of Right Thigh
	A_SR,
	A_FR,
	X_H,
	Y_H,
	A_P,
	A_R,			//Angle of tRunk
	NUM_INDEPEND,
};
//////////////////////////////////////////////////////////////////////
// Relative angles (Joints)
enum _Joints {
	A_L = 0,		// Loins
	A_HL,			// Left Hip
	A_HR,			// Right Hip
	A_KL,			// Left Knee
	A_KR,			// Right Knee
	A_AL,			// Left Ankle
	A_AR,			// Right Ankle
	NUM_JOINT,
};
//////////////////////////////////////////////////////////////////////
// Vertices
enum _Vertices {
	TRUNK_H	= 0,	// High point of Trunk
	LOINS,			// Loins
	HIP,			// Hip
	KNEE_L,			// Left Knee
	KNEE_R,			// Right Knee
	ANKLE_L,		// Left Ankle
	ANKLE_R,		// Right Ankle
	TOE__L,			// Low Point of Left Foot
	TOE__R,			// Low Point of Right Foot
	NUM_VERTEX,
};
//////////////////////////////////////////////////////////////////////
// Types of Links
enum _TypeLink {
	TRUNK = 0,		// Trunk
	PELVIS,			// Pelvis
	THIGH,			// Thigh
	SHANK,			// Shank
	FOOT,			// Foot
	HEAD,			// Head
	NUM_TYPELINK,
};
//////////////////////////////////////////////////////////////////////
// Links
enum _Links {
	TRUNK_ = 0,		// Trunk
	PELVIS_,		// Pelvis
	THIGH_L,		// Thigh
	THIGH_R,		// Thigh
	SHANK_L,		// Shank
	SHANK_R,		// Shank
	FOOT_L,			// Foot
	FOOT_R,			// Foot
	NUM_LINK,
};
//////////////////////////////////////////////////////////////////////
// Points of Touch ground
enum _TouchPoints {
	HEEL_L = 0,		// Left Heel
	HEEL_R,			// Right Heel
	TOE_L,			// Left Toe
	TOE_R,			// Right Toe
	NUM_TOUCH,
};
//////////////////////////////////////////////////////////////////////
// Joint properties
struct alignas( 16 ) RG_Joint{
	void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
	void operator delete( void * p ){ nsm_free( p ); };
	double Min;		// Minimum of Angle
	double Max;		// Maximum of Angle
	int UppSegment;	// Upper link
	int LowSegment;	// Lower link
};

//////////////////////////////////////////////////////////////////////
// Touch ground properties
struct alignas( 16 ) touch{
	touch() : Underground( false ), X0( 0. ), Y0( 0. ), Fx( 0. ), Fy( 0. ){};
	void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
	void operator delete( void * p ){ nsm_free( p ); };
	bool Underground;
	double X0;		// Coordinates of touch point
	double Y0;
	double Fx;			// Force of ground reaction
	double Fy;
};
//////////////////////////////////////////////////////////////////////
// vertex properties
struct alignas( 16 ) vertex{
	vertex( void ) : X (0.), Y( 0. ), dX( 0. ), dY( 0. ){};
	void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
	void operator delete( void * p ){ nsm_free( p ); };
	double X;
	double Y;
	double dX;
	double dY;
};
//////////////////////////////////////////////////////////////////////
// spring properties
class alignas( 16 ) spring{
	public:
		spring( void ) : K( 0. ), B( 0. ){};
		spring( double k, double b ) : K( k ), B( b ){};
		spring( const spring &s ) : K( s.K ), B( s.K ){};
		~spring( void ){};
	public:
		spring &operator = ( const spring &s ){ K = s.K; B = s.K; return *this; };
		spring &operator() ( double k, double b ){ K = k; B = b; return *this; };
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); };
	public:
		double force( double x, double v ){ return K*x-B*v; };
	public:
		double K;
		double B;
};

//////////////////////////////////////////////////////////////////////
// walker
class alignas( 16 ) walker : public hhn_process{ // new line
	public:
		walker( void );
		~walker( void );
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); };
	public: // general constants
		double GL;			// ground level
		double GV_L;			// treadmil velocity (left)
		double GV_R;			// treadmil velocity (right)
		double Gravity;			// gravity
		double BM;			// muscle viscosity (for all muscles)
		double BT;			// tendon viscosity (for all muscles)
		double BJ;			// joint viscosity (for all joints)
		double L[NUM_TYPELINK];	// link lenghts
		spring Ground;			// ground reaction
		spring Forepart;		// forepart reaction
		spring TrunkLim;		// trunk restriction
		spring LoinsLim;		// loins restriction
		spring PelvisLim;		// pelvis restriction
		spring JointsLim;		// joints restriction
	public: // restrictions
		bool IsPlvFixed;
		bool IsTrXFixed;		// specify if trunk is fixed (horizontal component)
		bool IsTrYFixed;		// specify if trunk is fixed (vertical component)
		bool IsTrQFixed;		// specify if trunk is fixed (angle)
		double X_Trunk;			// horizontal coordinate of trunk
		double Y_Trunk;			// vertical coordinate of trunk
		double Q_Trunk;			// angle of trunk
		double Q_Plvs;			// angle of trunk
	public: // independence variables
		double Q[NUM_INDEPEND];
		double P[NUM_INDEPEND];
	public: // dependence variables
		wlink	Link[NUM_LINK];		// absolute angles
		RG_Joint RGJ[NUM_INDEPEND];	// ???
		wjoint Joint[NUM_JOINT];	// Relative angles
		wjoint JointDeg[NUM_JOINT];	// Relative angles
	private:
		touch TG[NUM_TOUCH];
		vertex Ver[NUM_VERTEX];		// Massive of endpoints of links
		double TanSl;			// tg(slope)
		double SinSl;			// sin(slope)
		double CosSl;			// cos(slope)
		bool IsTouched;			// first touch
		double X0;			// position where a toe touch the ground first time
		double A0[3][3];		// Kinetic energy
		double AB[6]; 
	public: // the modified code
		const void *select( void ) const{ return Ver; };
		const void *select( unit_code *view )const;
		void *select( CHhnControlled *ctrl );
		double ground( double x );
		void set_slope( double slope = 0. );
		const string &get_name( const unit_code &code ) const;
		void init( double *m, double *cm, double *i, double x, double y );
		void attach( CHhnNetwork *net );
//		void next_step( double step );	// old code
	public:
		void reg_unit( runman *man = NULL ); // new code
	private:
		void calc_biomech( double step ); // new code
static		void calc_biomech( size_t currstep, double step, hhn_process **start ); // new code

	private: // calculation
		void get_f( touch *tg, double *q, double *p, double *lt, double *vt, double *fq, double *flt );
		void add_fm( double *b );
		void jfill( double *q, double *p );
		void vfill( double *q, double *cosq, double *sinq, double *p );
		void bcvfill( double *q, double *cosq, double *sinq, double *p, double *b );
		void gfill( double *q, double *cosq, double *sinq, touch *tg, double *b );
		void mfill( double *q, double *p, double *fq, touch *tg );
		void restrictions( double *cosq, double *sinq, double *b );
		void solve( double *q, double *cosq, double *sinq, double *b, double *result );
		double x2xt( double x, double y )
		{
			return x*CosSl-y*SinSl;
		};
		double y2yp( double x, double y )
		{
			return x*SinSl+y*CosSl;
		};
		double xt2x( double xt, double yp )
		{
			return xt*CosSl+yp*SinSl;
		};
		double yp2y( double xt, double yp )
		{
			return -xt*SinSl+yp*CosSl;
		};
	private: // utilities
		zmuscle *get_muscle( const char *name );
		touch *get_ground( const char *name );
		void init_qp( void );
		void sincos( double *q, double *cosq, double *sinq );
	public:
		vector<zmuscle *> Muscles;	// all muscles
};

extern const char *get_lname( size_t link );
extern const char *get_jname( size_t joint );
extern const char *get_tname( size_t tg );
extern const char *get_vname( size_t ver );

#elif defined (__MECHANICS_3D__)
// TODO implementation 3d model
#endif // __MECHANICS_2D__
#endif // __WALKER_H
