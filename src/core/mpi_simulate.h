/////////////////////////////////////////////////////////////////////////////
// mpi_simulate.h
#ifndef __MPI_SIMULATE_H
#define __MPI_SIMULATE_H

#include "simulate.h"
#include "mpi_layer.h"

#ifdef __MPI__

/////////////////////////////////////////////////////////////////////////////
// class mpi_subnet
class mpi_subnet{
	public:
		mpi_subnet( void );
		mpi_subnet( const mpi_subnet &net );
		~mpi_subnet( void );
	public:
		mpi_subnet &operator = ( const mpi_subnet &net );
	public:
		bool load_mod( const char *filename );
		void save_dat( const char *filename, double prec = 0. );
	public:
		vector<mpi_map> &get_mpimap( void ){ return IOMap; };
		void in( size_t id, double signal, size_t synid, bool proc = true )
		{
			if( id < IProc.size()){
				IProc[id].in( signal, synid, proc );
			}
		};
		void out( size_t id, double &signal, bool proc = true )
		{
			signal = 0.0;
			if( id < OProc.size()){
				signal = OProc[id].out( proc ); 
			}
		};
		int idin( const string &name, int typenn );
		int idout( const string &name );
		void init( long seed );
	private:
		bool add_hlayers( void );
		void add_mpiout( vector<nn_unit *> src );
		void add_mpiin( vector<nn_unit *> trg );
	public:
		simulate Subnet;
	private:
		vector<mpi_in> IProc;
		vector<mpi_out> OProc;
		vector<mpi_map> IOMap;
};

/////////////////////////////////////////////////////////////////////////////
// class mpi_simulate
class mpi_simulate{
	public:
		mpi_simulate( void );
		~mpi_simulate( void );
	public:
		bool load_mod( const char *mpi_model );
		void start_sim( void );
		void save_dat( const char *filename, double prec = 0. );
	private:
		bool get_param( istream &file );
		bool get_subnet( istream &file );
		bool get_ipconnect( istream &file );
		bool get_source( const string &name, istream &file );
		bool get_target( ipc_connect connect, const string &name, istream &file );
		bool get_connect( double &weight, int &type, int &sid, int &proc, istream &file );
		ipc_link getl_from( const string &name );
		ipc_link getl_to( const string &name, int type );
	private:
		void init_mod( void );
		void shut_mod( void );
		void next_step( void );
	private:
		double get_from( size_t id );
		void put_to( size_t id, double signal );
	private: // data
		vector<mpi_subnet> Subnets;
		vector<ipc_connect> IPCMap;
		string Promt;
		string ModelName;
		long Seed;
		double SimStep;
		double SimTime;
		unsigned long NumSteps;
		unsigned long CurrStep;
};

#endif // __MPI__
#endif // __MPI_SIMULATE_H
