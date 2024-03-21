///////////////////////////////////////////////////////////////////////////////
// runman.cpp.	
//-----------------------------------------------------------------------------
// Sergey Markin <smarkin@drexelmed.edu>
// 2010, Drexel University, College of Medicine
//-----------------------------------------------------------------------------
#include "precompile.h"

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
//#define new DEBUG_NEW
#endif // _DEBUG
#endif // __CONSOLE__

static runman RunMan;
#ifdef __OMP__
	omp_lock_t LockData;
#endif //__OMP__


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

size_t next_step( double step, size_t currstep, size_t granularity, CSimulate *sim, runman *man )
{
	if( man != NULL ){
		return man->next_step( step, currstep, granularity, sim );
	}
	return RunMan.next_step( step, currstep, granularity, sim );
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
size_t runman::Copies = 0;

runman::runman( void )
{
#ifdef __OMP__
	if( Copies == 0 ){
		omp_init_lock( &LockData );
	}
#endif /*__OMP__*/
	Copies++;
}

runman::runman( const runman &rm )
	: Units( rm.Units )
{
	Copies++;
}

runman::~runman( void )
{
	unreg_units();
	if( Copies > 0 ){
		Copies--;
	}
#ifdef __OMP__
	if( Copies == 0 ){
		omp_destroy_lock( &LockData );
	}
#endif /*__OMP__*/
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
	if( granularity == 1 ){
		while( !isquit ){
			for( uschiter_ proc = SchUnits.begin(); proc != SchUnits.end(); ++proc ){ // loop for methods (substeps).
				PROC_TYPE( method ) = proc->Method;
				for( size_t i = 0; i < proc->Starts.size(); ++i ){ // loop for units to be processed during each pass
					method( currstep, step, proc->Starts[i] ); // run calculations for correspondent pass
				}
			}
			error_code = sim->ctrl_handler( &isquit, &currstep );
		}
	} else{
		omp_set_num_threads( granularity );
		omp_set_dynamic( FALSE );
		#if defined ( __INTEL_COMPILER )
			omp_set_max_active_levels( 1 ); // intel classical compiler
		#elif defined (__INTEL_LLVM_COMPILER )
			omp_set_max_active_levels( 1 ); // intel LLVM compiler
		#else
			omp_set_nested( 0 ); // visual c compiler
		#endif
		#pragma omp parallel
		{
			while( !isquit ){
				for( uschiter_ proc = SchUnits.begin(); proc != SchUnits.end(); ++proc ){ // loop for methods (substeps).
					PROC_TYPE( method ) = proc->Method;
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
	}
	return error_code;
}

size_t runman::next_step( double step, size_t currstep, size_t granularity, CSimulate *sim )
{
	size_t error_code = _SIM_NOERRORS;
	bool isquit = false;
	if( granularity == 1 ){
		for( uschiter_ proc = SchUnits.begin(); proc != SchUnits.end(); ++proc ){ // loop for methods (substeps).
			PROC_TYPE( method ) = proc->Method;
			for( size_t i = 0; i < proc->Starts.size(); ++i ){ // loop for units to be processed during each pass
				method( currstep, step, proc->Starts[i] ); // run calculations for correspondent pass
			}
		}
		error_code = sim->ctrl_handler( &isquit, &currstep );
	} else{
		omp_set_num_threads( granularity );
		omp_set_dynamic( FALSE );
		#if __INTEL_COMPILER 
			omp_set_max_active_levels( 1 ); // intel compiler
		#else
			omp_set_nested( FALSE ); // visual c compiler
		#endif
		#pragma omp parallel
		{
			for( uschiter_ proc = SchUnits.begin(); proc != SchUnits.end(); ++proc ){ // loop for methods (substeps).
				PROC_TYPE( method ) = proc->Method;
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
	return error_code;
}

#else
size_t runman::run( double step, size_t currstep, size_t granularity, CSimulate *sim )
{
	size_t error_code = _SIM_NOERRORS;
	bool isquit = false;
	while( !isquit ){
		for( uschiter_ proc = SchUnits.begin(); proc != SchUnits.end(); ++proc ){ // loop for methods (substeps).
			PROC_TYPE( method ) = proc->Method;
			for( size_t i = 0; i < proc->Starts.size(); ++i ){ // loop for units to be processed during each pass
				method( currstep, step, proc->Starts[i] ); // run calculations for correspondent pass
			}
		}
		error_code = sim->ctrl_handler( &isquit, &currstep );
	}
	return error_code;
}

size_t runman::next_step( double step, size_t currstep, size_t granularity, CSimulate *sim )
{
	size_t error_code = _SIM_NOERRORS;
	bool isquit = false;
	for( uschiter_ proc = SchUnits.begin(); proc != SchUnits.end(); ++proc ){ // loop for methods (substeps).
		PROC_TYPE( method ) = proc->Method;
		for( size_t i = 0; i < proc->Starts.size(); ++i ){ // loop for units to be processed during each pass
			method( currstep, step, proc->Starts[i] ); // run calculations for correspondent pass
		}
	}
	error_code = sim->ctrl_handler( &isquit, &currstep );
	return error_code;
}
#endif //__OMP__

/*
#include <iostream>
#include <atomic>
#include <thread>
using namespace std;
 
//#define ATOMIC_FENCE
//#define ATOMIC_RELEASE
//#define ATOMIC_CONSUME
 
#if defined ATOMIC_FENCE
#define FENCE_ACQUIRE atomic_thread_fence(memory_order_acquire)
#define FENCE_RELEASE atomic_thread_fence(memory_order_release)
#elif defined ATOMIC_RELEASE
atomic_bool f{false};
#define FENCE_ACQUIRE f.load(memory_order_acquire)
#define FENCE_RELEASE f.store(true, memory_order_release)
#elif defined ATOMIC_CONSUME
atomic_bool f{false};
#define FENCE_ACQUIRE f.load(memory_order_consume)
#define FENCE_RELEASE f.store(flag, memory_order_release)
#else
#define FENCE_ACQUIRE
#define FENCE_RELEASE
#endif
 
int main(int argc, char** argv)
{
	bool flag = false;
 
	thread t1([&]() {
		this_thread::sleep_for(100ms);
		cout << "t1 started" << endl;
		flag = true;
		FENCE_RELEASE;
		cout << "t1 signals and exits" << endl;
	});
 
	thread t2([&]() {
		cout << "t2 started" << endl;
		while(flag == false) FENCE_ACQUIRE;
		cout << "t2 got signaled and exits" << endl;
	});
 
	t1.join();
	t2.join();
 
	return 1;
}

int main() 
{
	std::vector<int> toSum = getSomeVector();
	std::atomic<int> sum(0);
	std::vector<std::thread> threads;

	int chunkSize = std::ceil(toSum.size() / (float)NUM_THREADS);
	for (int i = 0; i < NUM_THREADS; ++i) {
		auto chunkBegin = toSum.begin() + (i * chunkSize);
		auto chunkEnd = chunkBegin + chunkSize;
		threads.emplace_back([chunkBegin, chunkEnd, &sum]() mutable{
			int partialSum = 0;
			for (; chunkBegin != chunkEnd; ++chunkBegin) {
				partialSum += *chunkBegin;
			}
			// Since we don't care about the order that the threads update the sum,
			// we can use memory_order_relaxed.  This is a rabbit-hole I won't get
			// too deep into here though.
			sum.fetch_add(partialSum, std::memory_order_relaxed);
		});
	}

	for (std::thread& thd : threads) {
		thd.join();
	}
	std::cout << sum << '\n';
}
*/