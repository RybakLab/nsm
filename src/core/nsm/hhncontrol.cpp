/*****************************************************************
 ****              (C)  1997 - 2000                           ****
 **           Model of Hodgkin-Huxley type neuron               **
 **         Developed by Ilia Rybak and Sergey Markin           **
 ****                 Control (module)                        ****
 *****************************************************************/
#include "precompile.h"

#include "hhncontrol.h"
#include "hhnnetwork.h"
#include "simulate.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

/////////////////////////////////////////////////////////////////////////////
// CHhnControlled class
void CHhnControlled::select( nn_unit *object )
{
	if( object ){
		Origin = ( double *)object->select( this );
		if( Origin ){ Init = *Origin; }
		else{ Origin = &Init; }
	}
}

bool CHhnControlled::load( istream &file, CSimulate *manager )
{
	string str;
	bool bOk = false;
	while( file >> str ){
		if( str == "<Code>" ){
			bOk = Code.load( file, manager );
			if( Code.Param >= _id_MAX_CTRL ){ bOk = false; }
		} else if( str == "</Target>" ){
			return bOk;
		} else{ unknown_string( file, str ); }
	}
	message( "Missing tag </Target>!", "Warning!" );
	return false;
}

void CHhnControlled::save( ostream &file, CSimulate *manager)
{
	file << endl << "<Target>" << endl;
	Code.save( file, manager );
	file << "</Target>" << endl;
}

/////////////////////////////////////////////////////////////////////////////
// hhn_control class
//////////////////////////////////////////////////////////////////////
// constructors/destructor
hhn_control::hhn_control( void )
	:  StepFunc( true ), LastOutput( 1. ), Variance( -1.0 ),CurrTabs( 0 )
{
	Name = "";
}

hhn_control::hhn_control( const string &name )
	:  StepFunc( true ), LastOutput( 1. ), Variance( -1.0 ), CurrTabs( 0 )
{
	Name = name;
}

hhn_control::hhn_control( const hhn_control &ctrl )
	: nn_unit( ctrl ), hhn_process( ctrl ), StepFunc( ctrl.StepFunc ),
	LastOutput( 1. ), Variance( ctrl.Variance ), CurrTabs( 0 ),
	CtrTabFunc( ctrl.CtrTabFunc ), CtrUnits( ctrl.CtrUnits )
{
}

hhn_control::~hhn_control( void )
{
}

hhn_control &hhn_control::operator = ( const hhn_control &control )
{
	hhn_process::operator =( control );
	nn_unit::operator = ( control );
	CtrTabFunc = control.CtrTabFunc;
	CtrUnits = control.CtrUnits;
	Variance = control.Variance;
	StepFunc = control.StepFunc;
	return *this;
}

//////////////////////////////////////////////////////////////////////
// methods
void hhn_control::reg_unit( runman *man )
{
	LastOutput = 1.;
	::reg_unit( this, hhn_control::control, _id_PASS_CONTROL, 1, man ); // should run in single-thread mode
}

void hhn_control::control( size_t currstep, double step )
{
	for( size_t i = 0; i < CtrUnits.size(); ++i ){
		CtrUnits[i].reset();
	}
	double time = currstep*step;
	size_t cur_tab = CurrTabs;
	if( cur_tab < CtrTabFunc.size() ){
		if( time >= CtrTabFunc[cur_tab].X ){
			for( ; cur_tab < CtrTabFunc.size(); cur_tab++ )
				if( time < CtrTabFunc[cur_tab].X ) 
					break;
		}
		if( cur_tab >= 1 ){
			double time1 = CtrTabFunc[cur_tab-1].X; 
			double output1 = CtrTabFunc[cur_tab-1].Y;
			double output = output1;
			if( !StepFunc && cur_tab < CtrTabFunc.size()){
				double time2 = CtrTabFunc[cur_tab].X;
				double output2 = CtrTabFunc[cur_tab].Y;
				output = output1+( time-time1 )*( output2-output1 )/( time2-time1 );
			}
			Output = ( Variance > 0. )? double( CStat( output, Variance )): output;
			LastOutput = double( Output );
			for( size_t i = 0; i < CtrUnits.size(); ++i ){
				CtrUnits[i].modify( Output );
			}
		}
		CurrTabs = cur_tab;
	} else{
		for( size_t i = 0; i < CtrUnits.size(); ++i ){
			CtrUnits[i].modify( LastOutput );
		}
	}
}

void hhn_control::add_unit( const CHhnControlled &ctrl )
{
	CtrUnits.push_back( ctrl );
}

bool hhn_control::del_unit( const CHhnControlled &ctrl )
{
	for( size_t i = 0; i < CtrUnits.size(); i++ ){
		if( CtrUnits[i] == ctrl ){
			CtrUnits.erase( CtrUnits.begin()+i );
			return true;
		}
	}
	return false;
}

void hhn_control::add_tab( const hhn_pair<double> &func )
{
	CtrTabFunc.push_back( func );
	sort( CtrTabFunc.begin(), CtrTabFunc.end());
}

void hhn_control::del_tab( size_t index )
{
	if( index < CtrTabFunc.size() ){
		CtrTabFunc.erase( CtrTabFunc.begin()+index );
	}
}

void hhn_control::sort_tabs( void )
{
	sort( CtrTabFunc.begin(), CtrTabFunc.end());
}

void hhn_control::init( CHhnNetwork *network )
{
	for( size_t i = 0; i < CtrUnits.size(); ++i ){
/*
		if CtrUnits[i].get_par() refers to the network properties (any synapse)
		then it should be added number of hidden CtrUnits which control 
		correspondent parameter of every population.
*/

		CtrUnits[i].select( network->get_nnunit( CtrUnits[i].get_par()));
	}
	Output = 0.0;
	CurrTabs = 0;
}

void hhn_control::clear_tabs( void )
{
	CtrTabFunc.clear();
}

bool hhn_control::load( istream &file, CSimulate * manager )
{
	string str;
	file >> ws;
	getline(file, str, '>');
	set_name(str);
	CtrUnits.clear();
	while( file >> str){
		if( str == "<Function>"){
			load_tab( file );
		} else if( str == "<Target>"){
			CHhnControlled ctrl;
			if( ctrl.load( file, manager )){ add_unit( ctrl ); }
		} else if( str == "Noise"){
			file >> str >> Variance;
		} else if( str == "StepFunc"){
			file >> str >> StepFunc;
		} else if( str == "</Control>"){
			return true;
		} else{ unknown_string( file, str); }
	}
	message( "Missing tag </Control>!", "Warning!" );
	return false;
}

void hhn_control::save( ostream &file, CSimulate * manager )
{
	file << endl << "<Control " << get_name() << ">" << endl;
	file << "Noise = " << Variance << endl;
	file << "StepFunc = " << StepFunc << endl;
	save_tab( file );
	for( size_t i = 0; i < CtrUnits.size(); i++ ){
		CtrUnits[i].save( file, manager );
	}
	file << endl << "</Control>" << endl;
}

bool hhn_control::load_tab( istream &file )
{
	hhn_pair<double> point;
	CtrTabFunc.clear();
	while(file >> point){
		add_tab(point);
	}
	file.clear();
	string str;
	while(file >> str){
		if( str == "</Function>" ){
			return true;
		} else{ unknown_string( file, str); }
	}
	message( "Missing tag </Function>!", "Warning!" );
	return false;
}

void hhn_control::save_tab( ostream &file )
{
	file << endl << "<Function>" << endl;
	for( size_t i = 0; i < CtrTabFunc.size(); i++ )
	file << CtrTabFunc[i] << endl;
	file << "</Function>" << endl;
}
