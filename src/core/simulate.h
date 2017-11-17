/////////////////////////////////////////////////////////////////////////////
// file: simulate.h
#ifndef _SIMULATE_H_
#define _SIMULATE_H_

#include "../usermessage.h"
#include "./nsm/hhnnetwork.h"

#include "sptypes.h"
#include "biomtemplate.h"

class CFrameView;
class CSimulateData;
/////////////////////////////////////////////////////////////////////////////
// class CSimParameters
class CSimParameters{
	public:
		CSimParameters( void );
		CSimParameters( const CSimParameters &par );
		~CSimParameters( void ){};
	public:
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
class CSimulateData{
	public:
		CSimulateData( void );
		CSimulateData( const CSimulateData &data );
		~CSimulateData( void );
	public:
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
#ifdef __MECHANICS__
		bool IsStickDiagram;		// interface flags
		double LimbOriginX;			// [0:1]
		double LimbOriginY;			// [0:1]
		double Slope;				// in deg [-90:90] 
		unsigned long LimbSkip;
		unsigned long LimbScale;
#endif // __MECHANICS__
};

/////////////////////////////////////////////////////////////////////////////
// class CSimulate
class CSimulate{
	public:
		CSimulate( void );
		~CSimulate( void );
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
		#ifdef __MECHANICS__
			if( unit.X == -1 )
				unit = BiomechT.get_uid( name );
		#endif // __MECHANICS__
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
		#ifdef __MECHANICS__
				case _id_Body:
				case _id_Vertex:
				case _id_Link:
				case _id_Joint:
				case _id_Muscle:
				case _id_Ground:
					return Walker->get_name( code );	//make sure Walker returns apropriate name
		#endif // __MECHANICS__
				default:
					name = "";
			}
			return name;
		};
		int GetFormatType( void ) const{ return DataFormat;	};
		const vector<unit_code> &GetResults( void ) const{	return Results;	};
		double GetSimStep( void ) const{ return SimPar.SimStep; };
		unsigned long GetNumSteps( void ) const{ return NumSteps; };
		const vector<float> &GetTimeScale( void ) const{ return ChartBuffer->timescale(); };
		CChartBuffer &GetChartBuffer( void ) const{ return *ChartBuffer; };
#ifdef __MECHANICS__
		void upd_outlist( void );
		CWalkerBuffer &GetWalkerBuffer( void ) const{ return *WalkerBuffer; };
#endif // __MECHANICS__
		void SetTimeScale( double begin, double end);
		void InitViews( void );
		void ReleaseViews( void );
		void SaveModel( const char *filename );
		void PreSaveData( const vector<unit_code> &buffers, int format = 0 );
		void SaveData( const char *filename, const hhn_pair<int> &wnd = hhn_pair<int>( -1, -1 ), double prec = 0. /*msec*/ ); /*insert window*/
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
#ifdef __MECHANICS__
		walker *Walker;
		t_biomech BiomechT;
#endif // __MECHANICS__
	private:
	////////////////////////////////////////////////////
	// view buffers
		CChartBuffer *ChartBuffer;
#ifdef __MECHANICS__
		CWalkerBuffer *WalkerBuffer;
#endif // __MECHANICS__
	////////////////////////////////////////////////////
	// views framework
		vector<CFrameView *> Views;
		int DataFormat;
		vector<unit_code> Results;

		CSimParameters SimPar;
		unsigned long NumSteps;
		unsigned long CurrStep;
	public:
#ifdef __CONSOLE__
		bool LoadModel( const char *filename );
		bool StartSimulation( const char *ifilename, const char *ofilename );
	private:
		bool LoadViews( istream &file );
#else
	public:
		bool diff( const string &pattern, ostream &log, char delim1 = '/', char delim2 = '|' );
		bool LoadModel( const char *filename, queue<CFrameView *> &viewfifo );
		bool UpdateResults( bool complete = true )
		{
			unsigned long counter = 0;
			if( IsStopped()){
				if( complete )
					counter = NumSteps;
			}
			else{
				if( !complete )
					counter = CurrStep;
			}
			if( ::PostMessage( SimPar.NotifyWindow, WM_UPDATE_MAIN_WND, complete, counter ))
				return true;
			return false;
		};
		void PauseSimulation( bool pause )
		{
			if( pause ){
				ResetEvent( IsSimulate );
			}
			else{
				SetEvent( IsSimulate );
			}
		};
		bool IsPaused( void )
		{
			if( WaitForSingleObject( IsSimulate, 0 ) != WAIT_OBJECT_0 )
				return true;
			return false;
		};
		bool IsStopped( void )
		{
			if( WaitForSingleObject( IsStop, 0 ) == WAIT_OBJECT_0 )
				return true;
			return false;
		};
		bool StartSimulation( const CSimParameters &par );
		bool StopSimulation( void );
		void CreateNetwork( CHhnNetwork &network, long seed );
//		size_t ctrl_handler( bool *run, bool *quit, size_t *currstep );
		size_t ctrl_handler( bool *quit, size_t *currstep );
	private:
		bool LoadViews( istream &file, queue<CFrameView *> &viewfifo );
		void EnableTimer( bool enabled );

		bool startsim( void );
		void stopsim( void );
		size_t runsim( void );
	////////////////////////////////////////////////////
	// thread
static	unsigned int WINAPI SimulationThread( void *param );
	////////////////////////////////////////////////////
	// synchronization objects
	private:
		size_t NumThreads;
		HANDLE IsHandleLocked;          // mutex
		HANDLE IsShutdown;				// manual-reset event
		HANDLE IsStop;					// manual-reset event
		HANDLE Timer;                   // waitable timer
		HANDLE IsSimulate;				// manual-reset event
		HANDLE SimThread;				// handle of simulation thread
#endif // __CONSOLE__
};

#endif // _SIMULATE_H_
