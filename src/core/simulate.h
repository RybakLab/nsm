#include "config.h"

/////////////////////////////////////////////////////////////////////////////
// file: simulate.h
#ifndef _SIMULATE_H_
#define _SIMULATE_H_

#ifndef __CONSOLE__
	#include <WinDef.h>
#endif // __CONSOLE__

#include "../usermessage.h"
#include "./nsm/hhnnetwork.h"

#include "sptypes.h"
#include "biomtemplate.h"

class CFrameView;
class CSimulateData;
/////////////////////////////////////////////////////////////////////////////
// class CSimParameters
class alignas( 16 ) CSimParameters{
	public:
		CSimParameters( void );
		CSimParameters( const CSimParameters &par );
		~CSimParameters( void ){};
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
		CSimParameters &operator = ( const CSimParameters &par );
	public:
		bool IsValidate( void ) const;
		void Validate( void ){};
	public:
#ifndef __CONSOLE__
		HWND NotifyWindow;
#endif // __CONSOLE__
		double Freq;
		double SimStep;
		unsigned long NNSteps;
		unsigned long UpdatingTime;  /*msec*/
};

/////////////////////////////////////////////////////////////////////////////
// class CSimulateData
class alignas( 16 ) CSimulateData{
	public:
		CSimulateData( void );
		CSimulateData( const CSimulateData &data );
		~CSimulateData( void );
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
		CSimulateData &operator = ( const CSimulateData &data );
	public:
#ifdef __CONSOLE__
		void SetSimParameters( CSimParameters *par );
#else
		void SetSimParameters( CSimParameters *par, HWND notifywnd );
#endif // __CONSOLE__
		bool LoadData( istream &file );
		void SaveData( ostream &file );
		void Randomize( void ){Seed = long( -time( NULL )); };
	public:
		long Seed;
		// parameters of a simulation proc.
		int NumThreads;
		double SimStep;
		double SimTime;              //msec
		unsigned long UpdatingTime;  //msec
		// parameters of visualization proc.
		double Freq;                 //kHz 
		double HistBin;              //msec
		double HistNorm;             //msec
		double BeginView;            //msec
		double EndView;              //msec
		double TimeFactor;
		// interface flags
		bool IsSimPaused;
		bool IsSimComplete;
		bool IsRedrawResults;
		string Promt;
		string ModelName;
#if defined (__MECHANICS_2D__)
		bool IsStickDiagram;		// interface flags
		double LimbOriginX;			// [0:1]
		double LimbOriginY;			// [0:1]
		double Slope;				// in deg [-90:90] 
		unsigned long LimbSkip;
		unsigned long LimbScale;
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
};

/////////////////////////////////////////////////////////////////////////////
// class CSimulate
class alignas( 16 ) CSimulate{
	public:
		CSimulate( void );
virtual		~CSimulate( void );
	public:
		void *operator new( size_t size ){ return nsm_alloc( 16, size ); };
		void operator delete( void * p ){ nsm_free( p ); }; 
	public:
		void init( long seed, bool rand = false );

		////////////////////////////////////////////////////
		// single-threading implementation
		bool is_unique_name( const char *name ) const;
		const char *find_unique_name( const char *name, const char *prefix ) const;
		CFrameView *get_view( const char *name ) const;
		bool add_view( const CFrameView *view );
		bool del_view( const char *name );
		bool IsSelectedItem( const unit_code &code ) const
		{
			for( unsigned int i = 0; i < Results.size(); i++ )
				 if( Results[i] == code )
					 return true;
			return false;
		};
		hhn_pair<int> get_uid(const string &name ) const
		{
			hhn_pair<int> unit = Network.get_uid( name );
		#if defined (__MECHANICS_2D__)
			if( unit.X == -1 )
				unit = BiomechT.get_uid( name );
		#elif defined (__MECHANICS_3D__)
			// TODO implementation 3d model
		#endif // __MECHANICS_2D__
			return unit;
		};
		const string &GetUnitName( const unit_code &code )
		{
			static string name;
			switch( code.UnitId ){
				case _id_Network:
				case _id_NNPopulat:
				case _id_NNFeedback:
				case _id_NNDrive:
				case _id_NNOutput:
					 {
					  if( Network.get_nnunit( code ))
						  return Network.get_nnunit( code )->get_name();
					 }
				case _id_NNControl:
					 {
					  if( Network.get_crtunit( code ) )
						  return Network.get_crtunit( code )->get_name();
					 }
		#if defined (__MECHANICS_2D__)
				case _id_Body:
				case _id_Vertex:
				case _id_Link:
				case _id_Joint:
				case _id_Muscle:
				case _id_Ground:
					return Walker->get_name( code );	//make sure Walker returns apropriate name
		#elif defined (__MECHANICS_3D__)
			// TODO implementation 3d model
		#endif // __MECHANICS_2D__
				default:
					name = "";
			}
			return name;
		};
		int GetFormatType( void ) const{ return DataFormat;	};
		const vector<unit_code> &GetResults( void ) const{	return Results;	};
		double GetSimStep( void ) const{ return SimPar.SimStep; };
		size_t GetNumSteps( void ) const{ return NumSteps; };
		const nsm_vector(float) &GetTimeScale( void ) const{ return ChartBuffer->timescale(); };
		CChartBuffer &GetChartBuffer( void ) const{ return *ChartBuffer; };
#if defined (__MECHANICS_2D__)
		void upd_outlist( void );
		CWalkerBuffer &GetWalkerBuffer( void ) const{ return *WalkerBuffer; };
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
		void SetTimeScale( double begin, double end);
		void InitViews( void );
		void ReleaseViews( void );
		void SaveModel( const char *filename );
		void PreSaveData( const vector<unit_code> &buffers, int format = 0 );
		void SaveData( const char *filename, const hhn_pair<int> &wnd = hhn_pair<int>( -1, -1 ), double prec_t = 0. /*msec*/, int prec_a = -1 /*number of digits afte the dec. point*/);
	private:
		void SaveViews( ostream &file );
		bool LoadRecords( istream &file );
		void SaveRecords( ostream &file );
	private:
		bool IsParPresent( CHhnNetwork &network, const unit_code &code );
		void CheckViews( CHhnNetwork &network );
		void CheckView( CHhnNetwork &network, const string &name );
		void CheckRecords( CHhnNetwork &network );
		void CheckRecord( CHhnNetwork &network, const string &name );

		void Initialize( double freq = -1. );
		void Shutdown( void );
	public:
		CSimulateData SimData;
		CHhnNetwork Network;
		runman *RunMan;
#if defined (__MECHANICS_2D__)
		walker *Walker;
		t_biomech BiomechT;
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	private:
	////////////////////////////////////////////////////
	// view buffers
		CChartBuffer *ChartBuffer;
#if defined (__MECHANICS_2D__)
		CWalkerBuffer *WalkerBuffer;
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	////////////////////////////////////////////////////
	// views framework
		vector<CFrameView *> Views;
		int DataFormat;
		vector<unit_code> Results;

		CSimParameters SimPar;
		size_t NumThreads;
		size_t NumSteps;
		size_t CurrStep;
#ifdef __CONSOLE__
	public:
		bool LoadModel( const char *filename );
		bool StartSimulation( const char *ifilename, const char *ofilename );
	private:
		bool LoadViews( istream &file );
#else
	public:
		bool LoadModel( const char *filename, queue<CFrameView *> &viewfifo );
		bool diff( const string &pattern, ostream &log, char delim1 = '/', char delim2 = '|' );
	private:
		bool LoadViews( istream &file, queue<CFrameView *> &viewfifo );
	public: // simulation management
		void PauseSimulation( bool pause ){ pause? ResetEvent( IsSimulate ): SetEvent( IsSimulate ); };
		bool UpdateResults( bool complete = true )
		{
			size_t counter = IsStopped()? ( complete? NumSteps:0 ): (!complete? CurrStep: 0 );
			if( ::PostMessage( SimPar.NotifyWindow, WM_UPDATE_MAIN_WND, complete, counter ) ){
				return true;
			}
			return false;
		};
		bool IsPaused( void )
		{
			if( WaitForSingleObject( IsSimulate, 0 ) != WAIT_OBJECT_0 ){
				return true;
			}
			return false;
		};
		bool IsStopped( void )
		{
			if( WaitForSingleObject( IsStop, 0 ) == WAIT_OBJECT_0 ){
				return true;
			}
			return false;
		};
		bool StartSimulation( const CSimParameters &par );
		bool StopSimulation( void );
		void InitNetwork( CHhnNetwork &network, long seed );
virtual		size_t ctrl_handler( bool *quit, size_t *currstep );
	private:
		void EnableTimer( bool enabled );

		bool startsim( void );
		void stopsim( void );
		size_t runsim( void );
	private: // thread & synchronization objects
static		unsigned int WINAPI SimulationThread( void *param );
		HANDLE IsHandleLocked;	// mutex
		HANDLE IsShutdown;	// manual-reset event
		HANDLE IsStop;		// manual-reset event
		HANDLE Timer;		// waitable timer
		HANDLE IsSimulate;	// manual-reset event
		HANDLE SimThread;	// handle of simulation thread
#endif // __CONSOLE__
};

#endif // _SIMULATE_H_
