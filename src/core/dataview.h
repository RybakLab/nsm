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
class  alignas( 16 ) hhn_viewbuffer : public hhn_process{
	public:
		hhn_viewbuffer( void ) : hhn_process(), CurrData( NULL ),  NumSteps( 0 ), ReservedSize( 0 ),
				ParCode(), ViewValue(), StatValue() {};
		hhn_viewbuffer( const hhn_viewbuffer &buffer ) : hhn_process( buffer ), CurrData( buffer.CurrData ), NumSteps( buffer.NumSteps ), ReservedSize( buffer.ReservedSize ), 
				ParCode( buffer.ParCode ), ViewValue( buffer.ViewValue ), StatValue( buffer.StatValue ){};
		~hhn_viewbuffer( void ){ free_buffer(); };
	public:
		hhn_viewbuffer &operator = ( const hhn_viewbuffer &buffer );
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
	public:
		void reg_unit( runman *man );
	private:
		void storedata_stat( size_t currstep, double step );
		void storedata_chart( size_t currstep, double step );
static		void storedata_stat( size_t currstep, double step, hhn_process **start );
static		void storedata_chart( size_t currstep, double step, hhn_process **start );
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
		long save_header( ostream &file, CSimulate *manager, long num, double *calibr );
		void save( ostream &file, const hhn_pair<int> &wnd, double step, size_t prec_t, size_t prec_a, CSimulate *manager );
		void set_buffer( void *currdata, const unit_code &code, size_t size );
		void alloc_buffer( size_t reserved_size );
	private:
		void free_buffer( void );
	private:
		void *CurrData;
		size_t NumSteps;
		size_t ReservedSize;
		unit_code ParCode;
		nsm_vector(float) ViewValue;
		nsm_vector(lvector)StatValue;
};

/////////////////////////////////////////////////////////////////////////////
// CBufferManager class
class CBufferManager{
	public:
		CBufferManager( CSimulate *manager ) : Manager( manager ), ViewStep( 0 ), CurrStep( 0 ){};
		CBufferManager( const CBufferManager &buffer ) : Manager( buffer.Manager ), ViewStep( buffer.ViewStep ), CurrStep( buffer.CurrStep ){};
virtual		~CBufferManager( void ){};
	public:
		CBufferManager &operator = ( const CBufferManager &buffer )
		{
			Manager = buffer.Manager;
			ViewStep = buffer.ViewStep;
			CurrStep = buffer.CurrStep;
			return *this;
		};
	public:
		double step( void ) const
		{
			return ViewStep;
		};
		size_t nsteps( void ) const
		{
			return CurrStep;
		};
	public:
virtual		void next_step( size_t currstep )
		{
			CurrStep = currstep+1;
		};
virtual		void init_all_buffers( double step, size_t numsteps, double freq )
		{
			release_all_buffers();
			ViewStep = step;
		};
virtual		void alloc_all_buffers( void ){};
virtual		void release_all_buffers( void )
		{
			ViewStep = 0;
			CurrStep = 0;
		};
virtual		size_t max_nsteps( void ) const = 0;
virtual		void reg_unit( runman *man ){};
	protected:
		CSimulate *Manager;
		double ViewStep;
		size_t CurrStep;
};

/////////////////////////////////////////////////////////////////////////////
// CChartBuffer class
class CChartBuffer : public CBufferManager{
	public:
		CChartBuffer( CSimulate *manager ) : CBufferManager( manager ), ReservedSize( 0 ){};
		CChartBuffer( const CChartBuffer &buffer ) : CBufferManager( buffer ), ReservedSize( buffer.ReservedSize ), TimeScale( buffer.TimeScale ), ParBuffer( buffer.ParBuffer ){};
virtual		~CChartBuffer( void ){	release_all_buffers(); };
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
		const nsm_vector(float) &timescale( void ) const
		{
			return TimeScale;
		};
		const void *get_buffer( const unit_code &code ) const;
		bool add_buffer( const unit_code &code );
	public:	// overrided
		void init_all_buffers( double step, size_t numsteps, double freq );
		void alloc_all_buffers( void );
		void release_all_buffers( void );
		size_t max_nsteps( void ) const
		{
			return ( size_t )TimeScale.size();
		};
		void reg_unit( runman *man );
		void next_step( size_t currstep );
	public:
		void save( ostream &file, const vector<unit_code> &buffers, hhn_pair<int> wnd, double prec_t, int prec_a , int format );
	private:
		size_t ReservedSize;
		nsm_vector(float) TimeScale;
		nsm_vector(hhn_viewbuffer) ParBuffer;
};

#if defined (__MECHANICS_2D__)
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
		hhn_pair<float> Walker[10];
};

/////////////////////////////////////////////////////////////////////////////
// CWalkerBuffer class
class CWalkerBuffer : public CBufferManager{
	public:
		CWalkerBuffer( CSimulate* manager ) : CBufferManager( manager ), CurrData( NULL ){};
		CWalkerBuffer( const CWalkerBuffer& buffer ) : CBufferManager( buffer ), Buffer( buffer.Buffer ), CurrData( buffer.CurrData ){};;
virtual		~CWalkerBuffer( void ){};
	public:
		CWalkerBuffer &operator = ( const CWalkerBuffer &buffer );
	public:
		const void *get_buffer( void ) const; 
	public:	// overrided
		void init_all_buffers( double step, size_t numsteps, double = 0.);
		void alloc_all_buffers( void );
		void release_all_buffers( void );
		size_t max_nsteps( void ) const
		{
			return Buffer.size();
		};
		void next_step( size_t currstep );
	private:
		void *CurrData;
		nsm_vector(CWalkerVertex) Buffer;
};
#elif defined (__MECHANICS_3D__)
// TODO implementation for 3d model
#endif // __MECHANICS_2D__
#endif // __DATA_VIEW_H
