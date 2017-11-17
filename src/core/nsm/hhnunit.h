//////////////////////////////////////////////////////////////////////
// hhnunit.h
#ifndef __UNIT_H_
#define __UNIT_H_

#include "sptypes.h"
#include "hhnconnect.h"

class runman;
class hhn_synapse; 
class CHhnControlled;
class CSimulate;

/////////////////////////////////////////////////////////////////////////////
// unit_code class
class unit_code{
      public:
            unit_code();
            unit_code( DWORD code );
            unit_code( const unit_code &code );
           ~unit_code( void ){};
      public:
            unit_code &operator = (const unit_code &code );
            bool operator == ( const unit_code &code ) const;
      public:
			DWORD encode( void ) const;
			bool get_fullname( CSimulate *manager, string &str );
			bool is_stat( void ) const;
            bool load( istream &file, CSimulate *manager );
            void save( ostream &file, CSimulate *manager );
      public:
            size_t UnitId;
            size_t NNIndex;
            size_t HhnIndex;
            size_t PartIndex;
            size_t ChanIndex;
            size_t Param;
      };

///////////////////////////////////////////////////////////////////////////////
// class hhn_process
class hhn_process{
	public:
		hhn_process( void ) : Output( 1. ){};
		hhn_process( const hhn_process &unit ) : Output( unit.Output ){};
virtual	~hhn_process( void ){};
	public:
		hhn_process &operator = ( const hhn_process &unit ){ Output = unit.Output; return *this; };
	public:
virtual	double *get_y( int /*id*/ = -1 ){ return &Output; };
virtual	double *get_hy( int /*id*/ = -1 ){ return &Output; };
virtual	void retrace( int /*yid*/ = -1, int /*sid*/ = -1, int /*hid*/ = -1 ){};
virtual	void reg_unit( runman *man = NULL ){};
	public:
		double Output;
};

///////////////////////////////////////////////////////////////////////////////
// class hhn_input
class hhn_input : public hhn_process{
///////////////////////////////////////////////////////////////////////////////
// class winp_
	class winp_{
		public:
			winp_( void ) : Inp( NULL ), W( 0. ){};
			winp_( double *inp, double w ) : Inp( inp ), W( w ){};
			winp_( const winp_ &wp ) : Inp( wp.Inp ), W( wp.W ){};
			~winp_( void ){};
		public:
			winp_ &operator = ( const winp_ &wp ){ Inp = wp.Inp; W = wp.W; return *this; };
			operator double( void ){ return *Inp*W; };
		public:
			double W;
			double *Inp;
	};
	public:
		hhn_input( void );
		hhn_input( double *trg, size_t src_sz );
		hhn_input( const hhn_input &inp );
		~hhn_input( void );
	public:
		hhn_input &operator = ( const hhn_input &input );
	public:
		void add_src( double *s, double w );
		void reg_unit( runman *man = NULL );
		void calc_g( double step );
	private:
static	void calc_g( size_t currstep, double step, hhn_process **start );
	public:
		vector<winp_> Src;
		double *Trg;		// pointer to target unit (synapse)  !!!POINTER ONLY
};

///////////////////////////////////////////////////////////////////////////////
// class wconnect
class wconnect{
	public:
		wconnect( void ) : Source( NULL ), SynapseID( -1 ), Sigma( false ), Depression( false ), PreInh( false ), Weight( 0. ){};
		wconnect( const wconnect &w ) : Source( w.Source ), Target( w.Target ), SynapseID( w.SynapseID ), Sigma( w.Sigma ), Depression( w.Depression ), PreInh( w.PreInh ), Weight( w.Weight ){};
		~wconnect( void ){};
	public:
		wconnect &operator = ( const wconnect &w )
		{
			SynapseID = w.SynapseID;
			Source = w.Source;
			Target = w.Target;
			Weight = w.Weight;
			Sigma = w.Sigma;
			PreInh = w.PreInh;
			Depression = w.Depression;
			return *this;
		};
	public:
		hhn_process *Source;
		hhn_process *Target;
		int SynapseID;
		bool Sigma;
		bool Depression;
		bool PreInh;
		double Weight;
};

///////////////////////////////////////////////////////////////////////////////
// class nn_unit
class nn_unit{
typedef	map<size_t, hhn_input> inputs_;

friend	class hhn_populat;
friend	class hhn_output;
	public:
		nn_unit( void );
		nn_unit( const nn_unit &unit );
virtual	~nn_unit( void );
	public:
		nn_unit &operator = ( const nn_unit &unit );
	public:
		size_t uid( void ) const{ return UnitId; };
		size_t &nn_inx( void ){ return NNIndex; };
		CNNConnect &connect_type( void ){ return ConnectType; };

virtual	size_t size( void ) const{ return 1; };
		const string &get_name( void ) const{ return Name; };
		void set_name( const string &name ){ Name = name; };
		void connect( const vector<nn_unit *> &source, const vector<CNNConnect> &connect );

		bool load( istream &file );
		void save( ostream &file );
	public:
virtual	void *select( CHhnControlled *ctrl ){ return NULL; };
virtual	void *select( unit_code *view ){ return NULL; };
	protected:
virtual	void add_connection( const vector<nn_unit *> sources, const vector<CConnect> &weights ){};
	protected:
		string Name;
		size_t UnitId; 
		size_t NNIndex;
		CNNConnect ConnectType;
		inputs_ Inputs;
		vector<hhn_process *> Units;	// all atomic units belong to nn_unit like population, output, drive, etc
};

extern const char *vw_name( string &name, unit_code &code );
extern const char *cr_name( string &name, unit_code &code );

#endif // __UNIT_H_
