//////////////////////////////////////////////////////////////////////
// hhnunit.h
#ifndef __UNIT_H_
#define __UNIT_H_

#include "sptypes.h"
#include "hhnconnect.h"
#include "nsmsys.h"

class runman;
class hhn_synapse; 
class CHhnControlled;
class CSimulate;

/////////////////////////////////////////////////////////////////////////////
// unit_code class
class alignas( 16 ) unit_code{
	public:
		unit_code();
		unit_code( DWORD code );
		unit_code( const unit_code &code );
		~unit_code( void ){};
	public:
		unit_code &operator = (const unit_code &code );
		bool operator == ( const unit_code &code ) const;
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
	public:
		void decode( DWORD code );
		DWORD encode( void ) const;
		bool get_fullname( CSimulate *manager, string &str );
		bool is_stat( void ) const;
		bool load( istream &file, CSimulate *manager );
		void save( ostream &file, CSimulate *manager );
	public:
		DWORD UnitId;
		DWORD NNIndex;
		DWORD HhnIndex;
		DWORD PartIndex;
		DWORD ChanIndex;
		DWORD Param;
};

///////////////////////////////////////////////////////////////////////////////
// class hhn_process
class alignas( 16 ) hhn_process{
	public:
		hhn_process( void ) : Output( 1. ){};
		hhn_process( const hhn_process &unit ) : Output( unit.Output ){};
virtual		~hhn_process( void ){};
	public:
		hhn_process &operator = ( const hhn_process &unit ){ Output = unit.Output; return *this; };
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); };
	public:
virtual		double *get_y( int /*id*/ = -1 ){ return &Output; };
virtual		int retrace( int /*synid*/ = -1, bool /*sigma*/ = false, bool /*modsigma*/ = false, bool /*linear*/ = false){ return -1; };
virtual		void reg_unit( runman *man ){};
virtual		bool init( void ){ return true; };
virtual		void prerun( double step ){};
	public:
		double Output;
};

///////////////////////////////////////////////////////////////////////////////
// class hhn_input
class alignas( 16 ) hhn_input : public hhn_process{
	class alignas( 16 ) winp_{
		public:
			winp_( void ) : Inp( NULL ), W( 0. ){};
			winp_( double *inp, double w ) : Inp( inp ), W( w ){};
			winp_( const winp_ &wp ) : Inp( wp.Inp ), W( wp.W ){};
			~winp_( void ){};
		public:
			winp_ &operator = ( const winp_ &wp ){ Inp = wp.Inp; W = wp.W; return *this; };
			operator double( void ){ return *Inp*W; };
		public:
			void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
			void operator delete( void * p ){ nsm_free( p ); };
		public:
			double *Inp;
			double W;
	};
	public:
		hhn_input( void );
		hhn_input( double *trg, size_t src_sz );
		hhn_input( const hhn_input &inp );
		~hhn_input( void );
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
	public:
		hhn_input &operator = ( const hhn_input &input );
	public:
		void add_src( double *s, double w );
		void reg_unit( runman *man = NULL );
		void calc_g( double step );
	private:
static		void calc_g( size_t currstep, double step, hhn_process **start );
	public:
		nsm_vector(winp_) Src;
		double *Trg;		// pointer to target unit (synapse)  !!!POINTER ONLY
};

///////////////////////////////////////////////////////////////////////////////
// class wconnect
class alignas( 16 ) wconnect{
	public:
		wconnect( void ) : Source( NULL ), Target( NULL ), SynapseID( -1 ), Sigma( false ), ModSigma( false ), Linear( false ), Modulate( 0 ), Weight( 0. ){};
		wconnect( const wconnect &w ) : Source( w.Source ), Target( w.Target ), SynapseID( w.SynapseID ), Sigma( w.Sigma ), ModSigma( w.ModSigma ), Linear( w.Linear ), Modulate( w.Modulate ), Weight( w.Weight ){};
		~wconnect( void ){};
	public:
		wconnect &operator = ( const wconnect &w )
		{
			Source = w.Source;
			Target = w.Target;
			SynapseID = w.SynapseID;
			Sigma = w.Sigma;
			ModSigma = w.ModSigma;
			Linear = w.Linear;
			Modulate = w.Modulate;
			Weight = w.Weight;
			return *this;
		};
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
	public:
		hhn_process *Source;
		hhn_process *Target;
		int SynapseID;
		bool Sigma;
		bool ModSigma;
		bool Linear;
		int Modulate;
		double Weight;
};

///////////////////////////////////////////////////////////////////////////////
// class nn_unit
class nn_unit{
typedef		map<size_t, hhn_input> inputs_;
friend		class hhn_populat;
friend		class hhn_output;
	public:
		nn_unit( void );
		nn_unit( const nn_unit &unit );
virtual		~nn_unit( void );
	public:
		nn_unit &operator = ( const nn_unit &unit );
	public:
		size_t uid( void ) const{ return UnitId; };
		size_t &nn_inx( void ){ return NNIndex; };
		CNNConnect &connect_type( void ){ return ConnectType; };

virtual		size_t size( void ) const{ return 1; };
		const string &get_name( void ) const{ return Name; };
		void set_name( const string &name ){ Name = name; };
		void connect( const vector<nn_unit *> &source, const vector<CNNConnect> &connect );

		bool load( istream &file );
		void save( ostream &file );
	public:
virtual		void *select( CHhnControlled *ctrl ){ return NULL; };
virtual		void *select( unit_code *view ){ return NULL; };
	protected:
virtual		void add_connection( const vector<nn_unit *> sources, const vector<CConnect> &weights ){};
	protected:
		string Name;
		size_t UnitId; 
		size_t NNIndex;
		CNNConnect ConnectType;
		inputs_ Inputs;
		vector<hhn_process *> Units;	// all atomic units belong to nn_unit like population, output, drive, etc
};

extern const char *cr_name( string &name, unit_code &code );

#endif // __UNIT_H_
