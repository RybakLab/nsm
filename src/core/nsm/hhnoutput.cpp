//////////////////////////////////////////////////////////////////////
// hhnoutput.cpp
#include "precompile.h"

#include "hhnoutput.h"
#include "hhn.h"
#include "runman.h"
#if defined (__MECHANICS_2D__)
#include "walker.h"
#elif defined (__MECHANICS_3D__)
// TODO implementation 3d model
#endif // __MECHANICS_2D__

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

//////////////////////////////////////////////////////////////////////
// class hhn_output
hhn_output::hhn_output( void )
	: SlopeA( 1. ), SlopeT( 0. ), Bias( 0. ), Tup( 0.0 ), Tdown( 0.0 ), Threshold( 0.0 ), PowT( 1. ), PowA( 1. ), 
	G( 0.0 ), Ginh( 0.0 ), ExpTup( 0. ), ExpTdw( 0. ), Input( 0. ), TimeOut( 0. ), CtrlOut( 1. ), SlpT( 0. ), Cap( 0. )

{ 
	UnitId = _id_NNOutput;
 	ConnectType.add_connect( CConnect( CHhnConnect(), _id_NN_OUTPUT ));    // only target (type _id_NN_OUTPUT)
}

hhn_output::hhn_output( const string &name )
	: SlopeA( 1. ), SlopeT( 0. ), Bias( 0. ), Tup( 0.0 ), Tdown( 0.0 ), Threshold( 0.0 ), PowT( 1. ), PowA( 1. ), 
	G( 0.0 ), Ginh( 0.0 ), ExpTup( 0. ), ExpTdw( 0. ), Input( 0. ), TimeOut( 0. ), CtrlOut( 1. ), SlpT( 0. ), Cap( 0. )
{
	Name = name;
	UnitId = _id_NNOutput;
 	ConnectType.add_connect( CConnect( CHhnConnect(), _id_NN_OUTPUT ));    // only target (type _id_NN_OUTPUT)
}

hhn_output::hhn_output( const hhn_output &output )
	: nn_unit( output ), hhn_process( output ),
	SlopeA( output.SlopeA ), SlopeT( output.SlopeT ), Bias( output.Bias ),
	Tup( output.Tup ), Tdown( output.Tdown ), Threshold( output.Threshold ),
	PowT( output.PowT ), PowA( output.PowA ), 
	G( output.G ), Ginh( output.Ginh ), ExpTup( output.ExpTup ), ExpTdw( output.ExpTdw ), 
	Input( output.Input ), TimeOut( output.TimeOut ), CtrlOut( output.CtrlOut ), SlpT( output.SlpT ), Cap( output.Cap )
{
}

hhn_output::~hhn_output( void )
{ 
}

hhn_output &hhn_output::operator = ( const hhn_output &output )
{
	hhn_process::operator =( output );
	nn_unit::operator = ( output );
	G = output.G;
	Ginh = output.Ginh;
	Tup = output.Tup;
	Tdown = output.Tdown;
	ExpTup = output.ExpTup;
	ExpTdw = output.ExpTdw;
	Threshold = output.Threshold;
	SlopeT = output.SlopeT;
	SlopeA = output.SlopeA;
	Bias = output.Bias;
	Input = output.Input;
	TimeOut = output.TimeOut;
	CtrlOut = output.CtrlOut;
	SlpT = output.SlpT;
	Cap = output.Cap;
	PowT = output.PowT;
	PowA = output.PowA;
	return *this;
}

#if defined (__MECHANICS_2D__)
void hhn_output::attach( zmuscle *muscle )
{
	muscle->MN = &Output;
}
#elif defined (__MECHANICS_3D__)
// TODO implementation 3d model
#endif // __MECHANICS_2D__

bool hhn_output::init( void )
{
	Units.clear();
	Input = TimeOut = Output = 0.0; CtrlOut = 1.0; SlpT = 0.;
	G = Ginh = 0.0; TimeOut = 0.;
	Units.push_back( this );
	return true;
}

void hhn_output::prerun( double step )
{
	ExpTup = Tup > 0.? exp(-step/Tup) :0.;
	ExpTdw = Tdown > 0.? exp(-step/Tdown) :0.;
	SlpT = SlopeT*step/1000.;
}

void hhn_output::reg_unit( runman *man )
{
	::reg_unit( this, hhn_output::calc_out, _id_PASS_NNUNIT, -1, man );
	for( inputs_::iterator it = Inputs.begin(); it != Inputs.end(); ++it ){
		it->second.reg_unit( man );
	} 
}

void hhn_output::calc_out( double step, size_t currstep )
{
	double sum = G/( 1.0+Ginh );
	double signal_up = EXP_EULER_EXPT( Input, sum, ExpTup );
	Input = ( signal_up >= Input )? signal_up: ( EXP_EULER_EXPT( Input, sum, ExpTdw ));
	if( Input > Threshold ){
		TimeOut += SlpT; // change the time component of the output signal
		Output = CtrlOut*( SlopeA*( Input-Threshold )+Bias+TimeOut );
		Output = Output > 0.? Output: 0.;
		Output = ( Cap <= 0. )? Output: ( Output < Cap )? Output: Cap;
	} else{ TimeOut = Output = 0.; }
	G = Ginh = 0.0;
}

void hhn_output::add_inputs( const vector<wconnect> &inputs )
{
	Inputs.insert( make_pair( size_t( &G ), hhn_input( &G, inputs.size()/2 )));
	Inputs.insert( make_pair( size_t( &Ginh ), hhn_input( &Ginh, inputs.size()/2 )));
	for( size_t i = 0; i < inputs.size(); ++i ){
		hhn_process *inp = inputs[i].Source;
		int sid = inp->retrace( inputs[i].SynapseID, inputs[i].Sigma, inputs[i].ModSigma, inputs[i].Linear );
		double *src = inp->get_y( sid );
		double *trg = ( inputs[i].Modulate == 0 )? &G: &Ginh;
		Inputs[size_t( trg )].add_src( src, inputs[i].Weight );
	}
}

void hhn_output::add_connection( const vector<nn_unit *> sources, const vector<CConnect> &weights )
{
	size_t src_size = 0;
	for( size_t i = 0; i < sources.size(); src_size += sources[i]->size(), ++i );
	if( src_size > 0 ){
		vector<wconnect> connect;
		connect.reserve( 2*src_size );
		for( size_t i = 0; i < sources.size(); ++i ){
			wconnect wc;
			CHhnConnect weight = weights[i].Connect;
			wc.Modulate = weight.Modulate;
			for( size_t id = 0; id < _id_MAX_SYN; ++id ){
				CStat WS = weight.Weights[id];
				WS.Value /= sources[i]->size();
				if( WS.Value != 0.0 ){
					wc.SynapseID = id;
					wc.Sigma = weight.Sigma[id];
					wc.ModSigma = weight.ModSigma[id];
					wc.Linear = weight.Linear[id];
					for( size_t j = 0; j < sources[i]->size(); ++j ){
						wc.Weight = double( WS );
						wc.Source = sources[i]->Units[j];
						if( CStat::Random() <= weight.Probability && wc.Source != NULL && wc.Weight != 0.0  ){
							connect.push_back( wc );
						}
					}
				}
			}
		}
		add_inputs( connect );
	}
}

bool hhn_output::load( istream &file )
{
	nn_unit::load( file );
	string str;
	while( file >> str){
		if( str == "Size"){
			int size = 0;
			file >> str >> size;
		} else if( str == "Tup"){
			file >> str >> Tup;
		} else if( str == "Tdown"){
			file >> str >> Tdown;
		} else if( str == "Threshold"){
			file >> str >> Threshold;
		} else if( str == "PowT"){
			file >> str >> PowT;
		} else if( str == "SlpT"){
			file >> str >> SlopeT;
		} else if( str == "PowA"){
			file >> str >> PowA;
		} else if( str == "SlpA"){
			file >> str >> SlopeA;
		} else if( str == "Bias"){
			file >> str >> Bias;
		} else if( str == "Cap"){
			file >> str >> Cap;
		} else if( str == "</Output>" || str == "</Object>" ){
			return true;
		} else{ unknown_string( file, str); }
	}
	message( "Missing tag </Output>!", "Warning!" );
	return false;
}

void hhn_output::save( ostream &file )
{
	file << "\n<Output ";
	nn_unit::save( file );
	file << "Tup = " << Tup << "\n";
	file << "Tdown = " << Tdown << "\n";
	file << "Threshold = " << Threshold << "\n";
	file << "PowT = " << PowT << "\n";
	file << "SlpT = " << SlopeT << "\n";
	file << "PowA = " << PowA << "\n";
	file << "SlpA = " << SlopeA << "\n";
	file << "Bias = " << Bias << "\n";
	file << "Cap = " << Cap << "\n";
	file << "</Output>\n";
}
