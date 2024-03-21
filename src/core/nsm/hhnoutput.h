//////////////////////////////////////////////////////////////////////
// hhnoutput.h
#include <cmath>

#include "hhnunit.h"
#include "hhnsynapse.h"

#ifndef __HHN_OUTPUT_H
#define __HHN_OUTPUT_H

#if defined (__MECHANICS_2D__)
class zmuscle;
#elif defined (__MECHANICS_3D__)
// TODO implementation 3d model
#endif // __MECHANICS_2D__
//////////////////////////////////////////////////////////////////////
// class hhn_output y = X+K1*t+const; X = input > threshold? LPF(input-threshold): 0
class alignas( 16 ) hhn_output : public nn_unit, public hhn_process{
	public:
		hhn_output( void );
		hhn_output( const string &name );
		hhn_output( const hhn_output &output );
		~hhn_output( void );
	public:
		hhn_output &operator = ( const hhn_output &output );
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
	public:
		double &tup( void ){ return Tup; };
		double &tdown( void ){ return Tdown; };
		double &threshold( void ){ return Threshold; };
		double &bias( void ){ return Bias; };
		double &slp_a( void ){ return SlopeA; };
		double &pow_a( void ){ return PowA; };
		double &slp_t( void ){ return SlopeT; };
		double &pow_t( void ){ return PowT; };
		double &cap( void ){ return Cap; };
#if defined (__MECHANICS_2D__)
		void attach( zmuscle *muscle );
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
		void prerun( double step ) final;
		bool init( void ) final;
		//--- overrided function
		void reg_unit( runman *man = NULL );
		void *select( CHhnControlled *ctrl ){ return &CtrlOut; };
		void *select( unit_code *view ){ return &Output; };
		bool load( istream &file );
		void save( ostream &file );
	protected:
		void add_inputs( const vector<wconnect> &inputs );
		void add_connection( const vector<nn_unit *> sources, const vector<CConnect> &weights );
	private:
		void calc_out( double step, size_t currstep );
static		void calc_out( size_t currstep, double step, hhn_process **start );
	public:
//		size_t T0;
		double SlopeA;
		double SlopeT;
		double Bias;
		double Tup;
		double Tdown;
		double Threshold;
		double PowT;
		double PowA;
	public:
		double G;
		double Ginh;
	private:
		double ExpTup;
		double ExpTdw;
		double Input;
		double TimeOut;
		double CtrlOut;
		double SlpT; // increment for each integration step
		double Cap;
};

#endif // __HHN_OUTPUT_H
