//////////////////////////////////////////////////////////////////////
// hhnfeedback.h
#ifndef __HHN_FEEDBACK_H
#define __HHN_FEEDBACK_H

#include "hhnunit.h"

#ifdef __MECHANICS__

class zmuscle;
struct touch;
//////////////////////////////////////////////////////////////////////
// hhn_feedback
class hhn_feedback : public nn_unit, public hhn_process{
	public:
		hhn_feedback( void );
		hhn_feedback( const string &name );
		hhn_feedback( const hhn_feedback &feedback );
		~hhn_feedback( void ){};
	public:
		hhn_feedback &operator = ( const hhn_feedback &feedback );
	public:
		bool load( istream &file );
		void save( ostream &file );
		void init( void );
		void attach( zmuscle *muscle, touch *grf );
	public: //--- overrided function
		void reg_unit( runman *man = NULL );
		void *select( CHhnControlled *ctrl ){ return &CtrlOut; };
		void *select( unit_code *view ){ return &Output; };
	private:
		void feedback( double step );
static	void feedback( size_t currstep, double step, hhn_process **start );
	private:
		double Ltr;
	public:
		string MechName;
		double Kg;
		double Pv;
		double Kv;
		double Kl;
		double Kemg;
		double Kc;
		double Kf;
		double Ftr;			// F threshold (if muscle force is below Ftr then Ib signal is 0)
	private:
		double CtrlOut;
		double *FbBuff[_id_NUM_FBS];
static	double _FbNull;
};

#endif // __MECHANICS__
#endif // __HHN_FEEDBACK_H
