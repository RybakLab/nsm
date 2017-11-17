//////////////////////////////////////////////////////////////////////
// hhnoutput.h
#include <cmath>

#include "hhnunit.h"
#include "hhnsynapse.h"

#ifndef __HHN_OUTPUT_H
#define __HHN_OUTPUT_H

#ifdef __MECHANICS__
class zmuscle;
#endif // __MECHANICS__
//////////////////////////////////////////////////////////////////////
// class hhn_output
class hhn_output : public nn_unit, public hhn_process{
	public:
		hhn_output( void );
		hhn_output( const string &name );
		hhn_output( const hhn_output &output );
		~hhn_output( void );
	public:
		hhn_output &operator = ( const hhn_output &output );
	public:
		double &tup( void ){ return Tup; };
		double &tdown( void ){ return Tdown; };
		double &threshold( void ){ return Threshold; };
#ifdef __MECHANICS__
		void attach( zmuscle *muscle );
#endif // __MECHANICS__
		void init( void );
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
		void reset( double step );
		void calc_out( double step );
static	void reset( size_t currstep, double step, hhn_process **start );
static	void calc_out( size_t currstep, double step, hhn_process **start );
	public:
		double Tup;
		double Tdown;
		double Threshold;
	public:
		double G;
		double Ginh;
	private:
		double State;
		double CtrlOut;
};

#endif // __HHN_OUTPUT_H
