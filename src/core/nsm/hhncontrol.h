/*****************************************************************
 ****              (C)  1997 - 2000                           ****
 **           Model of Hodgkin-Haxley type neuron               **
 **         Developed by Ilia Rybak and Sergey Markin           **
 ****                   Control (header)                      ****
 *****************************************************************/
#ifndef _HHN_CONTROL_H
#define _HHN_CONTROL_H

#include "hhnstat.h"
#include "hhnunit.h"

class nn_unit;
class CSimulate;
class CHhnNetwork;

/////////////////////////////////////////////////////////////////////////////
// CHhnControlled class
class CHhnControlled{
	public:
		CHhnControlled( void ) : Code(), Init( 0. ), Origin( &Init ){};
		CHhnControlled( const unit_code &code ) : Code( code ), Init( 0. ), Origin( &Init ){};
		CHhnControlled( const CHhnControlled &ctrl ) : Code( ctrl.Code ), Init( ctrl.Init ), Origin( &Init ){};
		~CHhnControlled( void ){};
	public:
		CHhnControlled &operator = ( const CHhnControlled &ctrl )
		{
			*Origin = Init;
			Init = ctrl.Init;
			Code = ctrl.Code;
			Origin = &Init;
			return *this;
		};
		bool operator == ( const CHhnControlled &ctrl ) const{ return ( Code == ctrl.Code ); };
	public:
		unit_code get_par( void ) const{ return Code; };
	public:
		void select( nn_unit *object );
		void reset( void ){ *Origin = Init; }; // restore original value before applying control @current step
		void modify( double mod ){ *Origin *= mod; }; // apply control @current step
		bool load( istream &file, CSimulate *manager);
		void save( ostream &file, CSimulate *manager);
	private:
		double Init;
		double *Origin;
		unit_code Code;
};

/////////////////////////////////////////////////////////////////////////////
// hhn_control class
class hhn_control : public nn_unit, public hhn_process{
	public:
		hhn_control( CHhnNetwork *network = NULL );
		hhn_control( const string &name, CHhnNetwork *network = NULL );
		hhn_control( const hhn_control &ctrl );
		~hhn_control( void );
	public:
		hhn_control &operator = ( const hhn_control &control );
	public:
		size_t nctabs( void ) const{ return CtrTabFunc.size(); };
		size_t ncunits( void ) const{ return CtrUnits.size(); };
		hhn_pair<double> &tab_point( size_t index ){ return CtrTabFunc[index]; };
		CHhnControlled &ctrl_unit( size_t index ){ return CtrUnits[index]; };
		void add_tab( const hhn_pair<double> &func );
		void del_tab( size_t index );
		void sort_tabs( void );
		void clear_tabs( void );
		void add_unit( const CHhnControlled &ctrUnit );
		bool del_unit( const CHhnControlled &ctrl );

		bool load( istream &file, CSimulate *manager);
		void save( ostream &file, CSimulate *manager);
		bool load_tab( istream &file );
		void save_tab( ostream &file );

		void *select( unit_code *view ){ return &Output; };
		double &noise( void ){ return Variance; };
		bool &step( void ){ return StepFunc; };
	public: //--- overrided function
		void init( CHhnNetwork *network );
		void reg_unit( runman *man = NULL );
	private:
		void control( size_t currstep, double step );
static	void control( size_t currstep, double step, hhn_process **start );
	private:
		bool StepFunc;
		double LastOutput;
		double Variance;
		size_t CurrTabs;
		vector< hhn_pair<double> > CtrTabFunc;
		vector<CHhnControlled> CtrUnits;
};

#endif
