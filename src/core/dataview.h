/////////////////////////////////////////////////////////////////////////////
// dataview.h
#ifndef __DATA_VIEW_H
#define __DATA_VIEW_H

#include "hhnunit.h"

struct vertex;
class hhn_network;
class CSimulate;

/////////////////////////////////////////////////////////////////////////////
// hhn_viewbuffer class
class hhn_viewbuffer : public hhn_process{
	public:
		hhn_viewbuffer( void );
		hhn_viewbuffer( const hhn_viewbuffer &buffer );
		~hhn_viewbuffer( void );
	public:
		hhn_viewbuffer &operator = ( const hhn_viewbuffer &buffer );
	public:
		void reg_unit( runman *man = NULL );
	private:
		void storedata_stat( size_t currstep, double step );
		void storedata_chart( size_t currstep, double step );
static	void storedata_stat( size_t currstep, double step, hhn_process **start );
static	void storedata_chart( size_t currstep, double step, hhn_process **start );

	public:
		float operator[]( size_t time )
		{
			return float(( ParCode.is_stat() )?  0.: ViewValue[time] );
		};
		bool operator == ( const unit_code &code ) const
		{
			return ( code.is_stat() && ParCode.is_stat())? 
				( code.NNIndex == ParCode.NNIndex ): ( ParCode == code );
		}
	public:
		const unit_code &get_code( void ) const
		{
			return ParCode;
		};
		const void *get_buffer( void ) const
		{
			return ( ParCode.is_stat() )? ( const void *)&StatValue: ( const void *)&ViewValue;
		}
		void alloc( void *currdata, const unit_code &code, size_t numsteps, double step );
		void reserve( size_t size );
		long save_header( ostream &file, CSimulate *manager, long num, double *calibr );
		void save( ostream &file, const hhn_pair<int> &wnd, double step, size_t precstep, CSimulate *manager );
	private:
		void free( void );
	private:
		void *CurrData;
		unit_code ParCode;
		size_t ReservedSize;
		vector<float> ViewValue;
		vector<lvector>StatValue;
};

/////////////////////////////////////////////////////////////////////////////
// CBufferManager class
class CBufferManager{
	public:
		CBufferManager( CSimulate *manager ) : Manager( manager ), ViewStep( 0 ), NumSteps( 0 ){};
		CBufferManager( const CBufferManager &buffer ) : Manager( buffer.Manager ), ViewStep( buffer.ViewStep ), NumSteps( buffer.NumSteps ){};
virtual	~CBufferManager( void ){};
	public:
		CBufferManager &operator = ( const CBufferManager &buffer )
		{
			Manager = buffer.Manager;
			ViewStep = buffer.ViewStep;
			NumSteps = buffer.NumSteps;
			return *this;
		};
	public:
		double step( void ) const
		{
			return ViewStep;
		};
		size_t nsteps( void ) const
		{
			return NumSteps;
		};
	public:
virtual	void next_step( size_t currstep )
		{
			NumSteps = currstep+1;
		};
virtual	void init_views( double step, size_t numsteps, double freq )
		{
			release_views();
			ViewStep = step;
		};
virtual	void release_views( void )
		{
			ViewStep = 0;
			NumSteps = 0;
		};
virtual	size_t max_nsteps( void ) const = 0;
virtual	void reg_unit( runman *man = NULL ){};
	protected:
		double ViewStep;
		size_t NumSteps;
		CSimulate *Manager;
};

/////////////////////////////////////////////////////////////////////////////
// CChartBuffer class
class CChartBuffer : public CBufferManager{
	public:
		CChartBuffer( CSimulate *manager );
		CChartBuffer( const CChartBuffer &buffer );
virtual	~CChartBuffer( void );
	public:
		CChartBuffer &operator = ( const CChartBuffer &buffer );
		hhn_viewbuffer &operator[]( size_t index )
		{
			return ParBuffer[index];
		};
		const hhn_viewbuffer &operator[]( size_t index ) const
		{
			return ParBuffer[index];
		};
	public:
		size_t nbuffs( void ) const
		{
			return ( size_t )ParBuffer.size();
		};
		const vector<float> &timescale( void ) const
		{
			return TimeScale;
		};
		void reserve( void )
		{
			for( size_t i = 0; i < ParBuffer.size(); i++ )
				ParBuffer[i].reserve( ReservedSize );
		};
		bool add_view( const unit_code &code );
		const void *get_buffer( const unit_code &code ) const;
	public:	// overrided
		void init_views( double step, size_t numsteps, double freq );
		size_t max_nsteps( void ) const
		{
			return ( size_t )TimeScale.size();
		};
		void reg_unit( runman *man = NULL );
		void next_step( size_t currstep );
		void release_views( void );
	public:
		void save( ostream &file, const vector<unit_code> &buffers, hhn_pair<int> wnd, double prec, int format );
	private:
		size_t ReservedSize;
		vector<float> TimeScale;
		vector<hhn_viewbuffer> ParBuffer;
};

#ifdef __MECHANICS__
/////////////////////////////////////////////////////////////////////////////
// CWalkerVertex class
class CWalkerVertex{
	public:
		CWalkerVertex( void );
		CWalkerVertex( const CWalkerVertex &buffer );
		~CWalkerVertex( void ){};
	public:
		CWalkerVertex &operator = ( const CWalkerVertex &buffer );
		CWalkerVertex &operator = ( const vertex *ver );
	public:
		void move_pos( float x, float y );
	public:
		vector<hhn_pair<float> > Left;
		vector<hhn_pair<float> > Right;
};

/////////////////////////////////////////////////////////////////////////////
// CWalkerBuffer class
class CWalkerBuffer : public CBufferManager{
	public:
		CWalkerBuffer( CSimulate *manager );
		CWalkerBuffer( const CWalkerBuffer &buffer );
virtual	~CWalkerBuffer( void );
	public:
		CWalkerBuffer &operator = ( const CWalkerBuffer &buffer );
	public:
		const void *get_buffer( void ) const; 
	public:	// overrided
		void init_views( double step, size_t numsteps, double = 0.);
		size_t max_nsteps( void ) const;
		void next_step( size_t currstep );

		void release_views( void );
	private:
		void *CurrData;
		vector<CWalkerVertex> Buffer;
};
#endif // __MECHANICS__
#endif // __DATA_VIEW_H
