/////////////////////////////////////////////////////////////////////////////
// mpi_layer.cpp
#include "precompile.h"
#include "mpi_layer.h"
#include "runman.h"
#include "simulate.h"

#ifdef __MPI__

/////////////////////////////////////////////////////////////////////////////
// class mpi_proc
mpi_proc::mpi_proc( void )
	: hhn_process(), nn_unit()
{
	ConnectType.del_connect();	// any mpi_proc object can only tranfer 
}

/////////////////////////////////////////////////////////////////////////////
// class mpi_in
mpi_in mpi_in::RegMethod;

mpi_in::mpi_in( bool )
{
	::reg_mpiin_method( mpi_in::calculate );
}

mpi_in::mpi_in( void )
	: mpi_proc()
{
	for( size_t i = 0; i < _id_MAX_SYN; Y[i] = 0, H[i] = 1., Proc[i] = IsY[i] = IsH[i] = false, ++i );
}

mpi_in::mpi_in( const mpi_in &in )
	: mpi_proc( in )
{
	for( size_t i = 0; i < _id_MAX_SYN; Y[i] = in.Y[i], H[i] = in.H[i], Proc[i] = in.Proc[i], IsY[i] = in.IsY[i], IsH[i] = in.IsH[i], ++i ){
		Syn[i] = in.Syn[i];
		DSyn[i] = in.DSyn[i];
	}
}

mpi_in::~mpi_in( void )
{
}

mpi_in &mpi_in::operator = ( const mpi_in &in )
{
	mpi_proc::operator = ( in );
	for( size_t i = 0; i < _id_MAX_SYN; Y[i] = in.Y[i], H[i] = in.H[i], Proc[i] = in.Proc[i], IsY[i] = in.IsY[i], IsH[i] = in.IsH[i], ++i ){
		Syn[i] = in.Syn[i];
		DSyn[i] = in.DSyn[i];
	}
	return *this;
}

void mpi_in::init( nn_unit *trg )
{
	Units.clear(); Units.push_back( this );
	for( size_t i = 0; i < _id_MAX_SYN; ++i ){
		Proc[i] = IsY[i] = IsH[i] = false;
		if( trg->get_syn( i )){
			Syn[i] = *( trg->get_syn( i ));
		}
		if( trg->get_dsyn( i )){
			DSyn[i] = *( trg->get_dsyn( i ));
		}
		Y[i] = 0.0; H[i] = 1.; 
		Output[i] = Y[i]; OutputD[i] = Y[i]*H[i];
	}
	hhn_process::Output = 0.0;
}

void mpi_in::reg_unit( void )
{
	::reg_mpiin( this );
}

void mpi_in::retrace( int yid, int /*sid*/, int hid )
{
	if( yid >= 0 && yid < _id_MAX_SYN && Syn[yid].Gmax > 0.0 )
		IsY[yid] = true;
	if( hid >= 0 && hid < _id_MAX_SYN && Syn[yid].Gmax > 0.0 )
		IsY[yid] = IsH[hid] = true;
}

/////////////////////////////////////////////////////////////////////////////
// class mpi_out
mpi_out mpi_out::RegMethod;

mpi_out::mpi_out( bool )
{
	::reg_mpiout_method( mpi_out::calculate );
}

mpi_out::mpi_out( void )
	: mpi_proc(), Proc( false ), Input( &Input_ ), Input_( 0. )
{
}

mpi_out::mpi_out( const mpi_out &out )
	: mpi_proc( out ), Proc( out.Proc ), Input( &Input_ ), Input_( out.Input_ )
{
}

mpi_out::~mpi_out( void )
{
}

mpi_out &mpi_out::operator = ( const mpi_out &out )
{
	mpi_proc::operator = ( out );
	Proc = out.Proc;
	Input_ = out.Input_;
	return *this;
}

void mpi_out::init( void )
{
	Units.clear(); Units.push_back( this );
	Proc = false;
	Input = &Input_;
	Input_ = hhn_process::Output = 0.0;
}

void mpi_out::reg_unit( void )
{
	::reg_mpiout( this );
}

void mpi_out::set_connection( const vector<nn_unit *> sources, const vector<CConnect> &/*weights*/ )
{
	if( !sources.empty() && sources[0]->is_proc() != NULL ){
		Input = &( sources[0]->is_proc()->Output );
	}
}

#endif // __MPI__
