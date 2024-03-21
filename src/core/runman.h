///////////////////////////////////////////////////////////////////////////////
// runman.h
//-----------------------------------------------------------------------------
#ifndef __RUN_MAN
#define __RUN_MAN

#include "sptypes.h"
#include "hhnunit.h"

#define PROC_TYPE( func ) void (* func )( size_t currstep, double step, hhn_process **start )
#define proc_type void( * )( size_t currstep, double step, hhn_process **start )

enum _pass_id_{
// 1. start next step
	_id_PASS_RESET,		// reset all variables
// 2.  presynaptic calculations on neurons
	_id_PASS_PREY,		// presynaptic output
// 3.  calculate the intrinsic properties of a neurons
	_id_PASS_IEDS,		// calculate Nernst potentials for all ions (Na, K, Ca, Cl)
	_id_PASS_G,		// conductance and current for all ions channels and synapses
	_id_PASS_IPUMP,		// pump current
#ifndef __RK__
	_id_PASS_IDYN,		// ions dynamics
#else
	_id_PASS_IDYN_K1,
	_id_PASS_IPUMP_K2,
	_id_PASS_IDYN_K2,
#endif
	_id_PASS_PREVM,		// finalize all preliminary calucations before solving the diff. eq. for membrane potential
	_id_PASS_VM,		// solve diff. equation for membrane patential af any compartment but soma
	_id_PASS_SOMA_VM,	// solve diff. equation for membrane patential of soma and detect if a spike occur
// 4.  calculate output function of network units like population, outputs, feedbacks etc
	_id_PASS_NNUNIT,
	_id_PASS_POSTY,		// presynaptic output - smoothing
// 5.  calculate biomechanics if needed
#if defined (__MECHANICS_2D__)
	_id_PASS_WALKER,
#elif defined (__MECHANICS_3D__)
// TODO implementation 3d model
#endif /*__MECHANICS_2D__*/
// 6.  control behavior of the model.
#ifdef __MPI__
	_id_PASS_MPI,
#endif /* __MPI__ */
	_id_PASS_VIEW,
	_id_PASS_CONTROL,
	_id_MAX_PASS,
};

class hhn_process;
class CSimulate;

///////////////////////////////////////////////////////////////////////////////
// class methid_
template<typename priority_, typename method_>
class methid_{
	public:
		methid_( void ) : Method( NULL ), Priority( -1 ) {};
		methid_( method_ method, priority_ prior ) : Method( method ), Priority( prior ) {};
		methid_( const methid_ &mid ) : Method( mid.Method ), Priority( mid.Priority ) {};
		~methid_( void ){};
	public:
		methid_ &operator = ( const methid_ &mid )
		{
			Method = mid.Method;
			Priority = mid.Priority;
			return *this;
		}
		bool operator < ( const methid_ &mid ) const
		{ 
			if( Priority < mid.Priority )
				return true;
			else if( Priority == mid.Priority && Method < mid.Method )
				return true;
			return false;
		};
		bool operator == ( const methid_ &mid ) const
		{ 
			if( Priority == mid.Priority && Method == mid.Method ) return true;
			return false;
		};
	public:
		method_ Method;
		priority_ Priority;
};

///////////////////////////////////////////////////////////////////////////////
// class runman
class runman{
	typedef	methid_<size_t, void *> method_;
	typedef	multimap<size_t, hhn_process *> units_;
	typedef	map<method_, units_> uproc_;
	///////////////////////////////////////////////////////////////////////////////
	// class usched_
	class usched_{
	typedef units_::iterator iter_;
		public:
			usched_( void *method, size_t priority, units_ &u, size_t gran ) 
				: Method(( proc_type )method ), Priority( priority )
			{
				if( gran < 1 ) gran = 1;
				if( !u.empty() ) schedule( u, gran );
			};
			usched_( const usched_ &usch ) 
				: Method( usch.Method ), Priority( usch.Priority ), Units( usch.Units )
			{
			};
			~usched_( void ){};
		public:
			usched_ &operator = ( const usched_ &usch )
			{
				Method = usch.Method;
				Priority = usch.Priority;
				Units = usch.Units;
				init_starts();
				return *this;
			};
			void init_starts( void )
			{
				for( size_t i = 0; i < Units.size(); ++i ){
					Starts.push_back( &Units[i] );
					for( ; Units[i] != NULL; ++i );
				}
			}
		private:
			void schedule( units_ &u, size_t gran )
			{
				Units.reserve( u.size()+gran+1 );
				size_t num = ( u.size()%gran == 0 )? u.size()/gran: u.size()/gran+1; // average number of units per cluster
				for( iter_ it1 = u.begin(); it1 != u.end(); ){
					size_t mid = -1;
					for( size_t n = 0; it1 != u.end() && n < num; ++it1, ++n ){ 
						mid = it1->first;
						Units.push_back( it1->second );
					}
					if( mid != -1 && it1 != u.end() && mid == it1->first ){
						iter_ it2 = u.upper_bound( mid );
						for( ; it1 != it2; ++it1 ){
							Units.push_back( it1->second );
						}
					}
					Units.push_back( NULL );
				}
			};
		public:
			PROC_TYPE( Method );
			size_t Priority;
			vector<hhn_process **> Starts;
		private:
			vector<hhn_process *> Units;
			
	};
	typedef uproc_::iterator upiter_;
	typedef vector<usched_>::iterator uschiter_;
	public:
		runman( void );
		runman( const runman &rm );
		~runman( void );
	public:
		runman &operator = ( const runman &rm ){ Units = rm.Units; return *this; };
	public:
		size_t run( double step, size_t currstep, size_t granularity, CSimulate *sim );
		size_t next_step( double step, size_t currstep, size_t granularity, CSimulate *sim );
		bool reg_unit( hhn_process *unit, PROC_TYPE( method ), size_t priority, size_t uid = -1 );
		void unreg_units( void );
		void schedule( size_t granularity = 0 );
		int get_unuts_size( void ){ return  Units.size(); }; //AK
	private:
		void balance( size_t granularity );
	private:
		uproc_ Units;			// raw data to be process
		vector<usched_> SchUnits;	// scheduled data to be process
static		size_t Copies;
};

extern bool reg_unit( hhn_process *unit, PROC_TYPE( method ), size_t priority, size_t uid, runman *man );
extern void schedule( size_t granularity, runman *man );
extern size_t run( double step, size_t currstep, size_t granularity, CSimulate *sim, runman *man );
extern size_t next_step( double step, size_t currstep, size_t granularity, CSimulate *sim, runman *man );
extern void unreg_units( runman *man = NULL );

extern void lock_data( void );
extern void unlock_data( void );

#endif // __RUN_MAN

