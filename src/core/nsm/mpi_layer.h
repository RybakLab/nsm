/////////////////////////////////////////////////////////////////////////////
// mpi_layer.h
#ifndef __MPI_LAYER_H
#define __MPI_LAYER_H

#include "hhnunit.h"
#include "hhnsynapse.h"
#include "hhnpopulat.h"

#ifdef __MPI__

/////////////////////////////////////////////////////////////////////////////
// class ipc_link
class ipc_link{
	public:
		ipc_link( void ) : SubnetID( -1 ), UnitID( -1 ), SynID( -1 ), Proc( false ){};
		ipc_link( const ipc_link &link ) : SubnetID( link.SubnetID ), UnitID( link.UnitID ), SynID( link.SynID ), Proc( link.Proc ){};
		~ipc_link( void ){};
	public:
		ipc_link &operator = ( const ipc_link &link ){ SubnetID = link.SubnetID; UnitID = link.UnitID; SynID = link.SynID; Proc = link.Proc; return *this; };
	public:
		int SubnetID;	// subnet ID
		int UnitID;		// mpi_in/mpi_out ID
		int SynID;		// synapse id
		bool Proc;
};

/////////////////////////////////////////////////////////////////////////////
// class ipc_connect
class ipc_connect{
	public:
		ipc_connect( void ) : From(), To(), Weight( 0.0 ){};
		ipc_connect( const ipc_connect &connect ) : From( connect.From ), To( connect.To ), Weight( connect.Weight ){};
		~ipc_connect( void ){};
	public:
		ipc_connect &operator = ( const ipc_connect &connect ){ From = connect.From; To = connect.To; Weight = connect.Weight; return *this; };
	public:
		double Weight;
		ipc_link From;
		ipc_link To;
};

/////////////////////////////////////////////////////////////////////////////
// class mpi_map
class mpi_map{
	public:
		mpi_map( void ) : Name( "empty" ), ID( -1 ), TypeNN( -1 ){};
		mpi_map( const mpi_map &m ) : Name( m.Name ), ID( m.ID ), TypeNN( m.TypeNN ){};
		~mpi_map( void ){};
	public:
		mpi_map &operator = ( const mpi_map &m )
		{
			Name = m.Name; ID = m.ID; TypeNN = m.TypeNN;
			return *this;
		};
	public:
		string Name;
		int ID;
		int TypeNN;
};

/////////////////////////////////////////////////////////////////////////////
// class mpi_proc
class mpi_proc : public nn_unit, public hhn_process{ 
	public:
		mpi_proc( void );
		mpi_proc( const mpi_proc &m ) : nn_unit( m ), hhn_process( m ){};
		~mpi_proc( void ){};
	public:
		mpi_proc &operator = ( const mpi_proc &m )
		{
			hhn_process::operator = ( m );
			nn_unit::operator = ( m );
			return *this;
		};
	public:
		hhn_process *is_proc( void ){ return ( hhn_process * )this; };
	public:		// mpi specific methods. Should be overrided and/or wrapped by c-style functions 
virtual	void in( size_t syn_id, double signal, bool proc ){};
virtual	double out( bool proc ){ return Output; };
};

/////////////////////////////////////////////////////////////////////////////
// class mpi_in
class mpi_in : public mpi_proc{ 
explicit	mpi_in( bool );
	public:
		mpi_in( void );
		mpi_in( const mpi_in &in );
		~mpi_in( void );
	public:
		mpi_in &operator = ( const mpi_in &in );
	public:
		void in( double signal, size_t syn_id, bool proc = true )
		{
			if( proc ){	// get a signal (average number of spikes per neuron) from source population(s) outside of the subnet. 
				Y[syn_id] += signal*Syn[syn_id].Vs;
			}
			else{		// get a signal from quasi-stationary source(s) (drive, output etc) outside of the subnet
				OutputD[syn_id] = Output[syn_id] = Y[syn_id] = signal;
			}
			Proc[syn_id] = proc;
		};
	public:
		double *get_y( int id ){ return &Output[id]; };
		double *get_hy( int id ){ return &OutputD[id]; };
		void init( nn_unit *trg );
		void reg_unit( void );
		void retrace( int yid = -1, int /*sid*/ = -1, int hid = -1 );
	private:
static	void calculate( int pass, double step, hhn_process *start );
	private:
static	mpi_in RegMethod;
		bool Proc[_id_MAX_SYN];
		bool IsY[_id_MAX_SYN];
		bool IsH[_id_MAX_SYN];			// 
		double Y[_id_MAX_SYN];			// output for other neurons
		double H[_id_MAX_SYN];			// output for other neurons
		base_synapse Syn[_id_MAX_SYN];
		base_dsynapse DSyn[_id_MAX_SYN];
	private:
		double Output[_id_MAX_SYN];
		double OutputD[_id_MAX_SYN];
};

/////////////////////////////////////////////////////////////////////////////
// class mpi_out
class mpi_out : public mpi_proc{ 
explicit	mpi_out( bool );
	public:
		mpi_out( void );
		mpi_out( const mpi_out &out );
		~mpi_out( void );
	public:
		mpi_out &operator = ( const mpi_out &out );
		double out( bool proc )
		{
			double o = Output;
			Output = ( proc )? 0.0: *Input;
			Proc = proc;
			return o;
		};
	public:
		void init( void );
		void reg_unit( void );
	protected:
		size_t set_inputs( const vector<wconnect> &inputs );
		void set_connection( const vector<nn_unit *> sources, const vector<CConnect> &weights );
	private:
static	void calculate( int pass, double step, hhn_process *start );
	public:
		bool Proc;		// if the signal must be processes then true, otherwise false
		double *Input;	// the signal that should be transfer out of the subnet
	private:
		double Input_;
static	mpi_out RegMethod;
};

#endif // __MPI__
#endif // __MPI_LAYER_H
