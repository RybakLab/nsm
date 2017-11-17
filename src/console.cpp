#include "precompile.h"

#ifdef __CONSOLE__

#include "sptypes.h"

#include "simulate.h"
#include "parser.h"

#include <stdio.h>
#include <iostream>

#ifdef __MPI__

/*
void help( ??? )
{
}

int main( int argc, char *argv[] )
{
}

class mpi_simuate{
	public:
		mpi_simuate( void );
		~mpi_simuate( void );
	public:
		bool load( const char *mpi_model );
		{
			if( open( mpi_model )){
				read_next_block( file );
				if( next_block == "subnet" ){
					load_subnet( ??? );
				}
				else if( next_block == "interproc communication" ){
					load_interproc( ??? )'
				}
				else{
					message( "unknown command" );
				}
			}
			close();
		};
		void init( void );
		{
			initialize();
			for( i = 0; Subnet.size(); ++i )
				Subnet[i].init();
			Interproc.init();
		}
		void run( void );
		{
			for( i = 0; Subnets.size(); ++i )
				set_flag( i );
			wait_til_all_proc_finished();
			dispatch_data();
			
		}
		void stop( void );
		{
			
		}
	private:
		bool load_subnet( const char *subnet );
		bool load_map( const char *map );

		void run_subnet( ???? );
		void dispatch( void );
	private: // data
};
*/  

#else
using std::cout;

void help( const char *name )
{
	cout << "Usage: " << name << "  [-i file_name [-o filename]] [-s file_name] [-h help]" << endl;
	cout << "Options:" << endl;
	cout << "     -i                 specify the model to simulate" << endl;
	cout << "     -o                 specify the file to save the results of the simulation" << endl;
	cout << "     -s                 run script-base simulation" << endl;
	cout << "     -h                 display this help and exit" << endl;
}

int main( int argc, char *argv[] )
{
	string cmd_line;
	for( int i = 1; i < argc; ++i )
		cmd_line += argv[i];
	typedef parser::const_iterator p_pos;
	parser cmd( "-i:-o:-s", ':' );
	cmd.parse( cmd_line );
	string ifilename;
	string ofilename;
	string sfilename;
	for(  p_pos pos = cmd.begin(); pos != cmd.end(); ++pos ){
		if( pos->first == "-s" )
			sfilename = pos->second;
		else if( pos->first == "-i" )
			ifilename = pos->second;
		else if( pos->first == "-o" )
			ofilename = pos->second;
		else if( pos->first == "-h" ){
			help( argv[0] );
			return 1;
		}
	}

	CSimulate model;
	if( !ifilename.empty()){
		if( ofilename.empty())
		ofilename = ifilename+".dat";
		model.StartSimulation( ifilename.c_str(), ofilename.c_str());
	}
	else if( !sfilename.empty())
		model.StartScript( sfilename.c_str());
	else{
		help( argv[0] );
	}
	return 0;
}

#endif // __MPI__
#endif // __CONSOLE__
