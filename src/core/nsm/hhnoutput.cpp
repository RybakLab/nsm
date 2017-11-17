//////////////////////////////////////////////////////////////////////
// hhnoutput.cpp
#include "precompile.h"

#include "hhnoutput.h"
#include "runman.h"
#ifdef __MECHANICS__
#include "walker.h"
#endif // __MECHANICS__

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

//////////////////////////////////////////////////////////////////////
// class hhn_output
hhn_output::hhn_output( void )
	: Tup( 10.0 ), Tdown( 10.0 ), Threshold( 0.0 ), G( 0.0 ), Ginh( 0.0 )
{ 
	UnitId = _id_NNOutput;
 	ConnectType.add_connect( CConnect( CHhnConnect(), _id_NN_OUTPUT ));    // only target (type _id_NN_OUTPUT)
}

hhn_output::hhn_output( const string &name )
	: Tup( 10.0 ), Tdown( 10.0 ), Threshold( 0.0 ), G( 0.0 ), Ginh( 0.0 )
{
	Name = name;
	UnitId = _id_NNOutput;
 	ConnectType.add_connect( CConnect( CHhnConnect(), _id_NN_OUTPUT ));    // only target (type _id_NN_OUTPUT)
}

hhn_output::hhn_output( const hhn_output &output )
	: nn_unit( output ), hhn_process( output ),
	Tup( output.Tup ), Tdown( output.Tdown ), Threshold( output.Threshold ),
	G( output.G ), Ginh( output.Ginh )
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
	Threshold = output.Threshold;
	return *this;
}

#ifdef __MECHANICS__
void hhn_output::attach( zmuscle *muscle )
{
	muscle->MN = &Output;
}
#endif // __MECHANICS__

void hhn_output::init( void )
{
	Units.clear();
	State = Output = 0.0; CtrlOut = 1.0;
	Units.push_back( this );
}

void hhn_output::reg_unit( runman *man )
{
	::reg_unit( this, hhn_output::reset, _id_PASS_RESET, -1, man ); 
	::reg_unit( this, hhn_output::calc_out, _id_PASS_NNUNIT, -1, man );
	for( inputs_::iterator it = Inputs.begin(); it != Inputs.end(); ++it ){
		it->second.reg_unit( man );
	} 
}

void hhn_output::reset( double step )
{
	G = Ginh = 0.0;
}

void hhn_output::calc_out( double step )
{
	double thresh = Threshold;
	double pre_out = State;
	double sum = G/( 1.0+Ginh );
	double stateup = EXP_EULER( pre_out, sum, step, Tup );
	double state = ( stateup >= pre_out )? stateup: ( EXP_EULER( pre_out, sum, step, Tdown ));
	Output = ( state >= thresh )? ( state-thresh )*CtrlOut: 0.;
	State = state;
}

void hhn_output::add_inputs( const vector<wconnect> &inputs )
{
	Inputs.insert( make_pair( size_t( &G ), hhn_input( &G, inputs.size()/2 )));
	Inputs.insert( make_pair( size_t( &Ginh ), hhn_input( &Ginh, inputs.size()/2 )));
	for( size_t i = 0; i < inputs.size(); ++i ){
		hhn_process *inp = inputs[i].Source;
		int sid = inputs[i].SynapseID;
		bool sigma = inputs[i].Sigma;
		bool depr = inputs[i].Depression;
		bool pre_inh = inputs[i].PreInh;
		inp->retrace( sid, ( sigma )? sid:-1, ( depr )? sid:-1 );

		double weight = inputs[i].Weight;
		double *src = ( depr )? inp->get_hy( sid ): inp->get_y( sid );
		double *trg = ( pre_inh )? &Ginh: &G;
		Inputs[size_t( trg )].add_src( src, weight );
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
			for( size_t id = _id_ExSyn; id < _id_InhASyn; ++id ){
				CHhnConnect weight = weights[i].Connect;
				CStat WS = weight.Weights[id];
				WS.Value /= sources[i]->size();
				if( WS.Value != 0.0 ){
					wconnect wc;
					wc.SynapseID = id;
					wc.Sigma = weight.Sigma[id];
					wc.PreInh = weight.PreInh[id];
					wc.Depression = weight.Depression[id];
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
		}
		else if( str == "Tup"){
			file >> str >> Tup;
		}
		else if( str == "Tdown"){
			file >> str >> Tdown;
		}
		else if( str == "Threshold"){
			file >> str >> Threshold;
		}
		else if( str == "</Output>" || str == "</Object>" ){
			return true;
        }
		else{
			unknown_string( file, str);
		}
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
	file << "</Output>\n";
}
