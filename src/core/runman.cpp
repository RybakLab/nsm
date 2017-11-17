///////////////////////////////////////////////////////////////////////////////
// runman.cpp.	
//-----------------------------------------------------------------------------
// Sergey Markin <smarkin@drexelmed.edu>
// 2010, Drexel University, College of Medicine
//-----------------------------------------------------------------------------
#include "precompile.h"

#include "fp_control.h"
#include "runman.h"
#include "hhnunit.h"
#include "simulate.h"

#ifdef __OMP__
#include <omp.h>
#endif //__OMP__

#ifndef __CONSOLE__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG
#endif // __CONSOLE__

runman RunMan;

///////////////////////////////////////////////////////////////////////////////
// global methods
bool reg_unit( hhn_process *unit, PROC_TYPE( method ), size_t priority, size_t uid, runman *man )
{
	if( man != NULL ){
		return man->reg_unit( unit, method, priority, uid );
	}
	return RunMan.reg_unit( unit, method, priority, uid );
}

void schedule( size_t granularity, runman *man )
{
	if( man != NULL ){
		man->schedule( granularity );
	}
	else{
		RunMan.schedule( granularity );
	}
}

size_t run( double step, size_t currstep, size_t granularity, CSimulate *sim, runman *man )
{
	if( man != NULL ){
		return man->run( step, currstep, granularity, sim );
	}
	return RunMan.run( step, currstep, granularity, sim );
}

void unreg_units( runman *man )
{
	if( man != NULL ){
		man->unreg_units();
	}
	else{
		RunMan.unreg_units();
	}
}

#ifdef __OMP__
	omp_lock_t LockData;
#endif //__OMP__

void lock_data( void )
{
#ifdef __OMP__
	omp_set_lock( &LockData );
#endif //__OMP__
}

void unlock_data( void )
{
#ifdef __OMP__
	omp_unset_lock( &LockData );
#endif //__OMP__
}

///////////////////////////////////////////////////////////////////////////////
// class runman
runman::runman( void )
{
#ifdef __OMP__
	omp_init_lock( &LockData );
#endif //__OMP__
}

runman::runman( const runman &rm )
	: Units( rm.Units )
{
}

runman::~runman( void )
{
	unreg_units();
#ifdef __OMP__
	omp_destroy_lock( &LockData );
#endif //__OMP__
}

///////////////////////////////////////////////////////////////////////////////
// public methods
bool runman::reg_unit( hhn_process *unit, PROC_TYPE( method ), size_t priority, size_t uid )
{
	method_ meth_id(( void *)method, priority );
	upiter_ it = Units.find( meth_id );
	if( it != Units.end()){
		it->second.insert( make_pair( uid, unit ));
	}
	else{
		units_ umap;
		umap.insert( make_pair( uid, unit ));
		Units[meth_id] = umap;
	}
	return false;
}

void runman::unreg_units( void )
{
	Units.clear();
	SchUnits.clear();
}

void runman::schedule( size_t granularity )
{
	SchUnits.clear();
	SchUnits.reserve( Units.size() );
	for( upiter_ it = Units.begin(); it != Units.end(); ++it ){
		usched_ u( it->first.Method, it->first.Priority, it->second, granularity );
		SchUnits.push_back( u );
	}
	balance( granularity );
}

void runman::balance( size_t granularity )
{
	for( uschiter_ proc = SchUnits.begin(); proc != SchUnits.end(); ++proc ){
		proc->init_starts();
	}
}

#ifdef __OMP__
size_t runman::run( double step, size_t currstep, size_t granularity, CSimulate *sim )
{
	size_t error_code = _SIM_NOERRORS;
	bool isquit = false;
	omp_set_num_threads( granularity );
	omp_set_nested( TRUE );

	set_mxcsr_off( OVERFLOW_EXCEPTION_MASK );	//???

	set_mxcsr_on( FTZ_BIT );
	set_mxcsr_off( DAZ_BIT );
	set_mxcsr_on( DENORMAL_EXCEPTION_MASK );
	make_denormal();
	#pragma omp parallel
	{
		while( !isquit ){
			for( uschiter_ proc = SchUnits.begin(); proc != SchUnits.end(); ++proc ){ // loop for methods (substeps).
				PROC_TYPE( method )  = proc->Method;
				#pragma omp for schedule( static ) nowait
					for( int i = 0; i < int( proc->Starts.size()); ++i ){ // loop for units to be processed during each pass
						method( currstep, step, proc->Starts[i] ); // run calculations for correspondent pass
					}
				#pragma omp barrier	// pass synchronization
			}
			#pragma omp master	// control/reaction on external events
			{
				error_code = sim->ctrl_handler( &isquit, &currstep );
			}
			#pragma omp barrier // step synchronization
		}
	}

	clear_flags();
	return error_code;
}
#else
size_t runman::run( double step, size_t currstep, size_t granularity, CSimulate *sim )
{
	set_mxcsr_off( OVERFLOW_EXCEPTION_MASK );	//???
	set_mxcsr_on( FTZ_BIT );
	set_mxcsr_off( DAZ_BIT );
	set_mxcsr_on( DENORMAL_EXCEPTION_MASK );
	make_denormal();

	size_t error_code = _SIM_NOERRORS;
	bool isquit = false;
	PROC_TYPE( method ) = NULL;
	while( !isquit ){
		for( uschiter_ proc = SchUnits.begin(); proc != SchUnits.end(); ++proc ){ // loop for methods (substeps).
			method = proc->Method;
			for( size_t i = 0; i < proc->Starts.size(); ++i ){ // loop for units to be processed during each pass
				method( currstep, step, proc->Starts[i] ); // run calculations for correspondent pass
			}
		}
		error_code = sim->ctrl_handler( &isquit, &currstep );
	}
	clear_flags();
	return error_code;
}
#endif //__OMP__
