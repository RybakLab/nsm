/////////////////////////////////////////////////////////////////////////////
// simulate.cpp : implementation file
#include "precompile.h"

#include "simulate.h"
#include "frameview.h"

#ifdef __CONSOLE__
#include <iostream>
using std::cout;
#else  
#include "../gui/spcord.h"
#endif // __CONSOLE__

#ifdef __LINUX__
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

/////////////////////////////////////////////////////////////////////////////
// class CSimParameters
/////////////////////////////////////////////////////////////////////////////
// constructors/destructor
CSimParameters::CSimParameters( void )
{
	Freq = 0;
	SimStep = 0;
	NNSteps = 0;
	UpdatingTime = 0;
#ifndef __CONSOLE__
	NotifyWindow = NULL;
#endif // __CONSOLE__
}

CSimParameters::CSimParameters( const CSimParameters &par )
{
	Freq = par.Freq;
	SimStep = par.SimStep;
	NNSteps = par.NNSteps;
	UpdatingTime = par.UpdatingTime;
#ifndef __CONSOLE__
	NotifyWindow = par.NotifyWindow;
#endif // __CONSOLE__
}

/////////////////////////////////////////////////////////////////////////////
// public methods
/////////////////////////////////////////////////////////////////////////////
// operators
CSimParameters &CSimParameters::operator = ( const CSimParameters &par )
{
	Freq = par.Freq;
	SimStep = par.SimStep;
	NNSteps = par.NNSteps;
	UpdatingTime = par.UpdatingTime;
#ifndef __CONSOLE__
	NotifyWindow = par.NotifyWindow;
#endif // __CONSOLE__
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// functions
bool CSimParameters::IsValidate( void ) const
{
#ifndef __CONSOLE__
	if( NotifyWindow != NULL )
		return true;
	return false;
#else
    return true;
#endif // __CONSOLE__
}

/////////////////////////////////////////////////////////////////////////////
// class CSimulateData
/////////////////////////////////////////////////////////////////////////////
// constructors/destructor
CSimulateData::CSimulateData( void )
             : Promt(), ModelName()
{
	Seed = long( -time( NULL ));
	NumThreads = 0;
	SimStep = 0.05;
	SimTime  = 1000.;
	UpdatingTime = 10;
	Freq = 0.3;          //the average frequency of spikes firing is about 0.3 kHz 
	HistBin = 100.;
	HistNorm = 1000.;
	BeginView = 0;
	EndView = 1000;
	TimeFactor = 20.;
	IsSimPaused = false;
	IsSimComplete = true;
	IsRedrawResults = false;
#ifdef __MECHANICS__
	LimbSkip = 1;
	LimbScale = 5;
	LimbOriginX = 0.5;
	LimbOriginY = 0.5;
	IsStickDiagram = false;
	Slope = 0.;
#endif //__MECHANICS__
}

CSimulateData::CSimulateData( const CSimulateData &data )
	: Promt( data.Promt ), ModelName( data.ModelName )
{
	Seed = data.Seed;
	NumThreads = data.NumThreads;
	SimStep = data.SimStep;
	SimTime  = data.SimTime;
	UpdatingTime = data.UpdatingTime;
	Freq = data.Freq;
	HistBin = data.HistBin;
	HistNorm = data.HistNorm;
	BeginView = data.BeginView;
	EndView = data.EndView;
	TimeFactor = data.TimeFactor;
	IsSimPaused = data.IsSimPaused;
	IsSimComplete = data.IsSimComplete;
	IsRedrawResults = data.IsRedrawResults;
#ifdef __MECHANICS__
	LimbSkip = data.LimbSkip;
	LimbScale = data.LimbScale;
	LimbOriginX = data.LimbOriginX;
	LimbOriginY = data.LimbOriginY;
	IsStickDiagram = data.IsStickDiagram;
	Slope = data.Slope;
#endif //__MECHANICS__
}

CSimulateData::~CSimulateData( void )
{
}

/////////////////////////////////////////////////////////////////////////////
// public methods
/////////////////////////////////////////////////////////////////////////////
// operators
CSimulateData &CSimulateData::operator = ( const CSimulateData &data )
{
	Seed = data.Seed;
	NumThreads = data.NumThreads;
	SimStep = data.SimStep;
	SimTime  = data.SimTime;
	UpdatingTime = data.UpdatingTime;
	Freq = data.Freq;
	HistBin = data.HistBin;
	HistNorm = data.HistNorm;
	BeginView = data.BeginView;
	EndView = data.EndView;
	TimeFactor = data.TimeFactor;
	IsSimPaused = data.IsSimPaused;
	IsSimComplete = data.IsSimComplete;
	IsRedrawResults = data.IsRedrawResults;
	Promt = data.Promt;
	ModelName = data.ModelName;
#ifdef __MECHANICS__
	LimbSkip = data.LimbSkip;
	LimbScale = data.LimbScale;
	LimbOriginX = data.LimbOriginX;
	LimbOriginY = data.LimbOriginY;
	IsStickDiagram = data.IsStickDiagram;
	Slope = data.Slope;
#endif //__MECHANICS__
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// functions
#ifdef __CONSOLE__
void CSimulateData::SetSimParameters( CSimParameters *par )
{
	par->Freq = Freq;
	par->SimStep = SimStep;
	par->UpdatingTime = UpdatingTime;
	par->NNSteps = ( unsigned long )( SimTime/SimStep+0.1 );
	par->Validate();
}
#else
void CSimulateData::SetSimParameters( CSimParameters *par, HWND notifywnd )
{
	par->Freq = Freq;
	par->SimStep = SimStep;
	par->UpdatingTime = UpdatingTime;
	par->NotifyWindow = notifywnd;
	par->NNSteps = ( unsigned long )( SimTime/SimStep );
	par->Validate();
}
#endif // __CONSOLE__

bool CSimulateData::LoadData( istream &file )
{
	string str;
	while( file >> str){
		if( str == "Seed"){
			file >> str >> Seed;
		}
		else if( str == "SimStep"){
			file >> str >> SimStep;
		}
		else if( str == "SimNNStep"){
			file >> str >> SimStep;
		}
		else if( str == "SimTime"){
			file >> str >> SimTime;
		}
		else if( str == "UpdatingTime"){
			file >> str >> UpdatingTime;
		}
		else if( str == "Freq"){
			file >> str >> Freq;
		}
		else if( str == "HistBin"){
			file >> str >> HistBin;
		}
		else if( str == "HistNorm"){
			file >> str >> HistNorm;
		}
		else if( str == "BeginView"){
			file >> str >> BeginView;
		}
		else if( str == "EndView"){
			file >> str >> EndView;
		}
		else if( str == "TimeFactor"){
			file >> str >> TimeFactor;
		}
		else if( str == "NumThreads"){
			file >> str >> NumThreads;
		}
#ifdef __MECHANICS__
		else if( str == "LimbSkip"){
			file >> str >> LimbSkip;
		}
		else if( str == "LimbScale"){
			file >> str >> LimbScale;
		}
		else if( str == "LimbOriginX"){
			file >> str >> LimbOriginX;
		}
		else if( str == "LimbOriginY"){
			file >> str >> LimbOriginY;
		}
		else if( str == "Slope"){
			file >> str >> Slope;
		}
#endif // __MECHANICS__
		else if( str == "</SimulateData>"){
			return true;
		}
		else{
			unknown_string( file, str);
		}
	}
	message( "Missing tag </SimulateData>!", "Warning!" );
	return false;
}

void CSimulateData::SaveData( ostream &file )
{
	file << endl << "<SimulateData>" << endl;
	file << "Seed = " << Seed << endl;
	file << "SimStep = " << SimStep << endl;
	file << "SimTime = " << SimTime << endl;
	file << "UpdatingTime = " << UpdatingTime << endl;
	file << "Freq = " << Freq << endl;
	file << "HistBin = " << HistBin << endl;
	file << "HistNorm = " << HistNorm << endl;
	file << "BeginView = " << BeginView << endl;
	file << "EndView = " << EndView << endl;
	file << "TimeFactor = " << TimeFactor << endl;
	file << "NumThreads = " << NumThreads << endl;
#ifdef __MECHANICS__
	file << "LimbSkip = " << LimbSkip << endl;
	file << "LimbScale = " << LimbScale << endl;
	file << "LimbOriginX = " << LimbOriginX << endl;
	file << "LimbOriginY = " << LimbOriginY << endl;
	file << "Slope = " << Slope << endl;
#endif // __MECHANICS__
	file << "</SimulateData>" << endl;
}

/////////////////////////////////////////////////////////////////////////////
// class CSimulate
/////////////////////////////////////////////////////////////////////////////
// constructors/destructor
CSimulate::CSimulate( void ) 
	: SimPar(),	DataFormat( 0 )
#ifdef __MECHANICS__
	, BiomechT( NULL ), Walker( NULL )
#endif // __MECHANICS__
{
#ifndef __CONSOLE__
	IsHandleLocked = ::CreateMutex( NULL, true, NULL );
	NumSteps = 0;
	CurrStep = 0;
	IsShutdown = ::CreateEvent( NULL, true, false, NULL );  // manual-reset event
	IsStop = ::CreateEvent( NULL, true, true, NULL );       // manual-reset event
	Timer = ::CreateWaitableTimer( NULL, false, NULL ); //Send a notify message
	IsSimulate = ::CreateEvent( NULL, true, false, NULL );  // manual-reset event
	SimThread = NULL;
#endif // __CONSOLE__
    ChartBuffer = new CChartBuffer( this );
#ifdef __MECHANICS__
    WalkerBuffer = new CWalkerBuffer( this );
#endif // __MECHANICS__
#ifndef __CONSOLE__
	::ReleaseMutex( IsHandleLocked );
#endif // __CONSOLE__
}

CSimulate::~CSimulate( void )
{
#ifndef __CONSOLE__
	::WaitForSingleObject( IsHandleLocked, INFINITE );
	::SetEvent( IsShutdown );
	::SetEvent( IsStop );
	::WaitForSingleObject( SimThread, INFINITE );
	::CloseHandle( SimThread );
	::CloseHandle( IsSimulate );
	::CloseHandle( IsStop );
	::CloseHandle( Timer );
	::CloseHandle( IsShutdown );
#endif // __CONSOLE__
	delete ChartBuffer;
#ifdef __MECHANICS__
	if( Walker )
		delete Walker;
	delete WalkerBuffer;
#endif // __MECHANICS__
	for( size_t i = 0; i < Views.size(); i++ )
		delete Views[i];
#ifndef __CONSOLE__
	::ReleaseMutex( IsHandleLocked );
	::CloseHandle( IsHandleLocked );
#endif // __CONSOLE__
}

/////////////////////////////////////////////////////////////////////////////
// public methods
void CSimulate::init( long seed, bool rand )
{
	Network.init( seed, NULL, rand );
#ifdef __MECHANICS__
	if( Walker )
		delete Walker;
	BiomechT.copy_to( &Walker );
	Walker->attach( &Network );
	Walker->set_slope( SimData.Slope );
	Walker->reg_unit( NULL );
#endif	// __MECHANICS__
}

void CSimulate::InitViews( void )
{
	for( size_t i = 0; i < Views.size(); i++ )
		Views[i]->init_view( this );
	ChartBuffer->reserve();
	ChartBuffer->reg_unit();
}

void CSimulate::ReleaseViews( void )
{
	ChartBuffer->release_views();
#ifdef __MECHANICS__
	WalkerBuffer->release_views();
#endif // __MECHANICS__
}

void CSimulate::SetTimeScale( double begin, double end)
{
	for( size_t i = 0; i < Views.size(); i++ )
		Views[i]->set_xrange( begin, end );
}

void CSimulate::SaveModel( const char *filename )
{
    ofstream file( filename, ios_base::trunc );
	if( file.is_open() ){
		file << "Generated by " << _ProjectName << endl;
		SimData.ModelName = filename;
		SimData.SaveData( file );
		Network.save( file, this );
#ifdef __MECHANICS__
		BiomechT.save( file );
#endif
		SaveViews( file );
		SaveRecords( file );
		return;
	}
	message( "Cannot save file", "Error" );
}

void CSimulate::PreSaveData( const vector<unit_code> &buffers, int format )
{
    DataFormat = format;
    Results = buffers;
}

void CSimulate::SaveData( const char *filename, const hhn_pair<int> &wnd, double prec ) /*insert window*/
{
	ofstream *file = NULL;
	switch( DataFormat ){
		case _id_ASCII_GEN: // ASCII (generic)
			file = new ofstream( filename, ios_base::trunc );
			break;
		case _id_BIN_SCRC: // SCRC (columns)
			file = new ofstream( filename, ios_base::trunc | ios_base::binary );
			break;
		default:
			return;
	}
	if( file ){
		ChartBuffer->save( *file, Results, wnd, prec, DataFormat );
		delete file;
	}
}

/////////////////////////////////////////////////////////////////////////////
// private methods
void CSimulate::SaveViews( ostream &file )
{
	file << endl << "<Views>" << endl;
	for( size_t i=0; i < Views.size(); i++ )
		Views[i]->save( file, this );
	file << endl << "</Views>" << endl;
}

bool CSimulate::LoadRecords( istream &file )
{
	string str;
	while( file >> str){
		if( str == "Format" ){
			file >> str >> DataFormat;
		}
		else if( str == "<Record>"){
			unit_code code;
			file >> str;
			if( str == "<Code>")
				if( code.load( file, this ))
					Results.push_back( code );
		}
		else if( str == "</Record>"){
		}
		else if( str == "</Records>"){
			return true;
		}
		else{
			unknown_string( file, str);
		}
	}
	message( "Missing tag </Views>!", "Warning!" );
	return false;
}

void CSimulate::SaveRecords( ostream &file )
{
	file << endl << "<Records>" << endl << endl;
	file << "Format = " << DataFormat << endl;
	for( size_t i=0; i < Results.size(); i++ ){
		file << endl << "<Record>";
		Results[i].save( file, this );
		file << "</Record>" << endl;
	}
	file << endl << "</Records>" << endl;

}

/////////////////////////////////////////////////////////////////////////////
// private methods
void CSimulate::Initialize( double freq )
{
	if( freq <= 0 )
		freq = SimPar.Freq;
	ChartBuffer->init_views( SimPar.SimStep, NumSteps, freq );
#ifdef __MECHANICS__
	WalkerBuffer->init_views( SimPar.SimStep, NumSteps );
#endif // __MECHANICS__
	InitViews();
	CurrStep = 0;
#ifdef __OMP__
	NumThreads = omp_get_num_procs();
#else
	NumThreads = 1; // debug version
#endif //__OMP__
	::schedule( NumThreads );
}

void CSimulate::Shutdown( void )
{
	CurrStep = 0;
}

/////////////////////////////////////////////////////////////////////////////
// private methods
void CSimulate::CheckViews( CHhnNetwork &network )
{
	size_t i = 0;
	for( i = 0; i < Network.size_pop(); i++ ){
		CheckView( network, Network.get_pop( i ).get_name() );
	}
	for( i = 0; i < Network.size_drv(); i++ ){
		CheckView( network, Network.get_drv( i ).get_name() );
	}
	for( i = 0; i < Network.size_out(); i++ ){
		CheckView( network, Network.get_out( i ).get_name() );
	}
#ifdef __MECHANICS__
	for( i = 0; i < Network.size_fbk(); i++ ){
		CheckView( network, Network.get_fbk( i ).get_name() );
	}
#endif // __MECHANICS__
	for( i = 0; i < Network.size_ctr(); i++ ){
		CheckView( network, Network.get_ctr( i ).get_name() );
	}
	for( i = 0; i < Views.size(); i++ ){
		for( size_t j = 0; j < Views[i]->npars(); j++ ){
			unit_code viewcode = Views[i]->get_par(j)->ParCode;
			if( viewcode.UnitId == _id_NNControl ){
				unsigned index = viewcode.NNIndex;
				for( size_t k = 0; k < network.size_ctr(); k++ ){
					string ctrlname = network.get_ctr( k ).get_name();
					if( ctrlname == Network.get_ctr( index ).get_name() ){
						viewcode.NNIndex = k;
						break;
					}
				}
			}
		}
	}
}

void CSimulate::CheckView( CHhnNetwork &network, const string &name )
{   
	const nn_unit *nnunit = network.get_nnunit( name );
	if( nnunit == NULL ){
		unit_code nncode;
		if( Network.get_nnunit( name )){
			nncode.UnitId = Network.get_nnunit( name )->uid();
			nncode.NNIndex = Network.get_nnunit( name )->nn_inx();
			for( size_t i = 0; i < Views.size(); i++ ){
				for( size_t j = 0; j < Views[i]->npars(); j++ ){
					unit_code viewcode = Views[i]->get_par(j)->ParCode;
					if( viewcode.UnitId == nncode.UnitId && viewcode.NNIndex == nncode.NNIndex ){
						Views[i]->del_view( viewcode );
						j = 0;
					}
				}
			}
		}
	}
	else{
		unit_code newcode;
		newcode.UnitId = network.get_nnunit( name )->uid();
		newcode.NNIndex = network.get_nnunit( name )->nn_inx();
		unit_code oldcode;
		if( Network.get_nnunit( name )){
			oldcode.UnitId = Network.get_nnunit( name )->uid();
			oldcode.NNIndex = Network.get_nnunit( name )->nn_inx();
			for( size_t i = 0; i < Views.size(); i++ ){
				for( size_t j = 0; j < Views[i]->npars(); j++ ){
					unit_code *viewcode = &( Views[i]->get_par(j)->ParCode );
					if( viewcode->UnitId == oldcode.UnitId && viewcode->NNIndex == oldcode.NNIndex ){
						viewcode->UnitId = newcode.UnitId;
						viewcode->NNIndex = newcode.NNIndex;
						if( !IsParPresent( network, *viewcode )){
							Views[i]->del_view( *viewcode );
							j = 0;
						}
					}
				}
			}
		}
	}
	const hhn_control *ctrlunit = network.get_crtunit( name );
	if( ctrlunit == NULL ){
		for( size_t k = 0; k < Network.size_ctr(); k++ ){
			if( Network.get_ctr( k ).get_name() == name ){
				unit_code nncode;
				nncode.UnitId = _id_NNControl;
				nncode.NNIndex = k;
				for( size_t i = 0; i < Views.size(); i++ ){
					for( size_t j = 0; j < Views[i]->npars(); j++ ){
						unit_code viewcode = Views[i]->get_par(j)->ParCode;
						if( viewcode.UnitId == nncode.UnitId && viewcode.NNIndex == nncode.NNIndex ){
							Views[i]->del_view( viewcode );
							j = 0;
						}
					}
				}
			}
		}
	}
}

void CSimulate::CheckRecords( CHhnNetwork &network )
{
	for( size_t i = 0; i < Network.size_pop(); i++ ){
		CheckRecord( network, Network.get_pop( i ).get_name() );
	}
	for( size_t i = 0; i < Network.size_drv(); i++ ){
		CheckRecord( network, Network.get_drv( i ).get_name() );
	}
	for( size_t i = 0; i < Network.size_out(); i++ ){
		CheckRecord( network, Network.get_out( i ).get_name() );
	}
#ifdef __MECHANICS__
	for( size_t i = 0; i < Network.size_fbk(); i++ ){
		CheckRecord( network, Network.get_fbk( i ).get_name() );
	}
#endif // __MECHANICS__
	for( size_t i = 0; i < Network.size_ctr(); i++ ){
		CheckRecord( network, Network.get_ctr( i ).get_name() );
	}
	for( size_t i = 0; i < Results.size(); i++ ){
		if( Results[i].UnitId == _id_NNControl ){
			size_t index = Results[i].NNIndex;
			for( size_t k = 0; k < network.size_ctr(); k++ ){
				string ctrlname = network.get_ctr( k ).get_name();
				if( ctrlname == Network.get_ctr( index ).get_name() ){
					Results[i].NNIndex = k;
					break;
				}
			}
		}
	}
}

void CSimulate::CheckRecord( CHhnNetwork &network, const string &name )
{   
	const nn_unit *nnunit = network.get_nnunit( name );
	if( nnunit == NULL ){
		unit_code nncode;
		if( Network.get_nnunit( name )){
			nncode.UnitId = Network.get_nnunit( name )->uid();
			nncode.NNIndex = Network.get_nnunit( name )->nn_inx();
			for( size_t i = 0; i < Results.size(); i++ ){
				if(  Results[i].UnitId == nncode.UnitId && Results[i].NNIndex == nncode.NNIndex ){
					Results.erase( Results.begin()+i );
					i = 0;
				}
			}
		}
	}
	else{
		unit_code newcode;
		newcode.UnitId = network.get_nnunit( name )->uid();
		newcode.NNIndex = network.get_nnunit( name )->nn_inx();
		unit_code oldcode;
		if( Network.get_nnunit( name )){
			oldcode.UnitId = Network.get_nnunit( name )->uid();
			oldcode.NNIndex = Network.get_nnunit( name )->nn_inx();
			for( size_t i = 0; i < Results.size(); i++ ){
				if( Results[i].UnitId == oldcode.UnitId && Results[i].NNIndex == oldcode.NNIndex ){
					Results[i].UnitId = newcode.UnitId;
					Results[i].NNIndex = newcode.NNIndex;
					if( !IsParPresent( network, Results[i] )){
						Results.erase( Results.begin()+i );
						i = 0;
					}
				}
			}
		}
	}
	const hhn_control *ctrlunit = network.get_crtunit( name );
	if( ctrlunit == NULL ){
		for( size_t k = 0; k < Network.size_ctr(); k++ ){
			if( Network.get_ctr( k ).get_name() == name ){
				unit_code nncode;
				nncode.UnitId = _id_NNControl;
				nncode.NNIndex = k;
				for( size_t i = 0; i < Results.size(); i++ ){
					if( Results[i].UnitId == nncode.UnitId && Results[i].NNIndex == nncode.NNIndex ){
						Results.erase( Results.begin()+i );
						i = 0;
					}
				}
			}
		}
	}
}

bool CSimulate::IsParPresent( CHhnNetwork &network, const unit_code &code )
{
	void *data = NULL;
	nn_unit *object = ( nn_unit *)network.get_nnunit( code );
	if( object )
		data = ( void *)( object->select(( unit_code *)&code ));
	else{
		hhn_control *control = ( hhn_control *)network.get_crtunit( code );
		if( control )
			data = ( void *)( control->select(( unit_code *)&code ));
#ifdef __MECHANICS__
		else
			data = ( void *)Walker->select(( unit_code *)&code );
#endif // __MECHANICS__
	}
	if( data )
		return true;
	return false;
}

#ifdef __CONSOLE__
bool CSimulate::LoadModel( const char *filename )
{
#ifdef __LINUX__
	remove_char( filename, 0x0D );
#endif
#if defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	_FileVersion = 1.0;
#else
	_FileVersion = 2.05;
#endif // defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	bool LoadOK=false;
	string str;
	CSimulate manager;
	CChartFrameView view;
	manager.add_view( &view );

	ifstream file( filename );
	while( file >> str ){
		if( str == "Generated" ){
			getline( file, str );
			_FileVersion = 2.05;
		}
		else if(str == "<SimulateData>" ){
			LoadOK = manager.SimData.LoadData( file );
		}
		else if( str == "<Network" ){
			LoadOK = manager.Network.load( file, &manager );
		}
		else if( str == "<Views>" ){
			LoadOK = manager.LoadViews( file );
		}
		else if( str == "<Records>" ){
			LoadOK = manager.LoadRecords( file );
		}
#ifdef __MECHANICS__
		else if( str == "<Biomechanics" ){
			file >> ws;
			getline( file, str, '>') ;
			manager.BiomechT.load( file );
		}
#endif
		else
			unknown_string( file, str );
	}
	if( manager.Views.size() < 2 ){
		view.SetTimeScale( SimData.BeginView, SimData.EndView );
		manager.add_view( &view );
	}
	if( LoadOK ){
		SimData = manager.SimData;
		Network = manager.Network;
		for( size_t i = 0; i < manager.Views.size(); i++ )
			add_view( manager.Views[i] );
		DataFormat = manager.DataFormat;
		Results = manager.Results;
		init( SimData.Seed );
#ifdef __MECHANICS__
//?????
#endif
		SimData.ModelName = filename;
	}
	return LoadOK;
}

bool CSimulate::LoadViews( istream &file )
{
	string str;
	while( file >> str){
		if( str == "<View"){
			file >> ws;
			getline( file, str, '>' );
			if( str == "Chart"){
				CChartFrameView view;
				if( view.LoadView( file, this ) )
					add_view( &view );
			}
#ifdef __MECHANICS__
			else if( str == "Walker"){
				CWalkerFrameView view;
				if( view.LoadView( file, this ) )
					add_view( &view );
			}
#endif // __MECHANICS__
		}
		else if( str == "</Views>"){
			return true;
		}
		else{
			unknown_string( file, str);
		}
	}
	message( "Missing tag </Views>!", "Warning!" );
	return false;
}

bool CSimulate::StartSimulation( const char *ifilename, const char *ofilename )
{
	cout << "Loading the model " << ifilename << ". Please wait... " << endl;
	if( LoadModel( ifilename )){
	SimData.Promt = "Single simulation: the model ";
	SimData.Promt += ifilename;
	SimData.Promt += " is running. Please wait...";
	LockData();
	ChartBuffer->FreeBuffers();
#ifdef __MECHANICS__
	WalkerBuffer->FreeBuffers();
#endif // __MECHANICS__
	UnlockData();
	SimData.SetSimParameters( &SimPar );
	NumSteps = SimPar.NNSteps;
	Initialize( 1. );
	cout << endl;
	long slice = NumSteps/100;
	long percent = 0;
	long t0 = long( time( NULL ));
	for( CurrStep = 0; CurrStep < NumSteps; next_step(), ++CurrStep )
		if( CurrStep%slice == 0 ){
			cout << SimData.Promt << std::setw( 3 ) << percent << "%\r";
			++percent;
		}
	cout << endl << "The simulation is complete. Total duration is " << long( time( NULL ))-t0 << " sec" << endl;
	SaveData( ofilename );
	Shutdown();
	return true;
	}
	return false;
}
#else
CFrameView *CSimulate::get_view( const char *name ) const
{
	string view_name = name;
	for( size_t i = 0; i < Views.size(); ++i ){
		if( view_name == Views[i]->get_name())
			return Views[i];
	}
	return NULL;
}

bool CSimulate::add_view( const CFrameView *view )
{
	CFrameView *new_view = NULL;
	string view_name = view->get_name();
	for( size_t i = 0; i < Views.size(); ++i ){
		if( view_name == Views[i]->get_name())
			return false;
	}
	view->copy_to( &new_view );
	Views.push_back( new_view );
	return true;
}

bool CSimulate::del_view( const char *name )
{
	string view_name = name;
	for( size_t i = 0; i < Views.size(); ++i ){
		if( view_name == Views[i]->get_name()){
    		delete Views[i];
			Views.erase( Views.begin()+i );
			return true;
			}
	}
	return false;
}

bool CSimulate::is_unique_name( const char *name ) const
{
	CFrameView empty_view;
	string empty_name = empty_view.get_name();
	if( empty_name != name && !get_view( name ))
		return true;
	return false;
}

const char *CSimulate::find_unique_name( const char *name, const char *prefix ) const
{
	CFrameView empty_view;
	string empty_name = empty_view.get_name();
	CFrameView *view = get_view( name );
	if( view || name == empty_name ){
static	string unique_name = "";
		int unique_idx = 1;
		char name_idx[80];
		while( 1 ){
			unique_name = prefix;
			unique_name += " View [";
			sprintf( name_idx, "%d", unique_idx );
			unique_name += name_idx;
			unique_name += "]";
			if( !get_view( unique_name.c_str()))
				break;
			unique_idx++;
		}
		return unique_name.c_str();
	}
	return name;
}

void CSimulate::CreateNetwork( CHhnNetwork &network, long seed )
{
	CheckViews( network );
	CheckRecords( network );
	Network = network;
	Network.init( seed );
}

bool CSimulate::diff( const string &pattern, ostream &log, char delim1, char delim2 )
{
	typedef string::size_type size_type;
	size_type pos = 0, next_pos = pattern.find( delim1, pos );
	string p1 = pattern.substr( pos, next_pos-pos );
	pos = next_pos+1; next_pos = pattern.find( delim2, pos );
	string p2 = pattern.substr( pos, next_pos-pos );
	string name = pattern.substr( next_pos+1 );
	if( !p1.empty() && !p2.empty() && !name.empty()){
		log << "looking for the pattern '" << pattern << "'" << endl << "through the model '" << SimData.ModelName << "'" << endl;
		log << "==============================" << endl;
		Network.diff( p1, p2, name, log );
		return true;
	}
	return false;
}

bool CSimulate::LoadModel( const char *filename, queue<CFrameView *> &viewfifo )
{
#ifdef __LINUX__
    remove_char( filename, 0x0D );
#endif
#if defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
    _FileVersion = 1.0;
#else
    _FileVersion = 2.05;
#endif // defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )

	ifstream file( filename );
	if( !file.is_open() ){
		message( "Cannot open file", "Error" );
		return false;
	}

	file.seekg( 0, ios::end );
	size_t size = file.tellg(); 
	file.seekg( 0, ios::beg );
	char *buffer = new char[size];	// allocate memory to store file data
	memset( buffer, 0, size );
	file.rdbuf()->sgetn( buffer, size );		// load data from file to buffer
	file.close();

	istringstream inp;
	inp.clear();
	inp.str( string( buffer, size ));

	string str;
	CSimulate manager;
	bool LoadOK = false;
	while( inp >> str ){
		if( str == "Generated" ){
			getline( inp, str );
			_FileVersion = 2.05;
		}
		else if(str == "<SimulateData>" ){
			LoadOK = SimData.LoadData( inp );
		}
		else if( str == "<Network" ){
			LoadOK = Network.load( inp, this );
		}
		else if( str == "<Views>" ){
			LoadOK = LoadViews( inp, viewfifo );
		}
		else if( str == "<Records>" ){
 			LoadOK = LoadRecords( inp );
		}
#ifdef __MECHANICS__
		else if( str == "<Biomechanics" ){
			inp >> ws;
			getline( inp, str, '>') ;
			BiomechT.load( inp );
		}
#endif	// __MECHANICS__
		else if( !str.empty() && str[0] != 0x00 ){
			unknown_string( inp, str );
		}
	}
	delete[] buffer;

	if( LoadOK ){
		init( SimData.Seed );
		SimData.ModelName = filename;
	}
	return LoadOK;
}

bool CSimulate::LoadViews( istream &file, queue<CFrameView *> &viewfifo )
{
	string str;
	while( file >> str){
		if( str == "<View"){
            file >> ws;
            getline( file, str, '>' );
			if( str == "Chart"){
				CChartFrameView view;
				if( view.load( file, this ) ){
					CFrameView *new_view = NULL;
					view.copy_to( &new_view );
					viewfifo.push( new_view );
				}
			}
#ifdef __MECHANICS__
			else if( str == "Walker"){
				CWalkerFrameView view;
				if( view.load( file, this )){
					CFrameView *new_view = NULL;
					view.copy_to( &new_view );
					viewfifo.push( new_view );
				}
			}
#endif // __MECHANICS__
		}
		else if( str == "</Views>"){
			return true;
		}
		else{
			unknown_string( file, str);
		}
	}
	message( "Missing tag </Views>!", "Warning!" );
	return false;
}

#ifdef __MECHANICS__
void CSimulate::upd_outlist( void )
{
	vector<string> outlist;
	for( size_t i = 0; i < Network.size_out(); ++i ){
		outlist.push_back( Network.get_out( i ).get_name());
	}
	BiomechT.upd_outlist( outlist );
}

#endif // __MECHANICS__

/////////////////////////////////////////////////////////////////////////////
// multi-threading implementation
/////////////////////////////////////////////////////////////////////////////
// public methods
bool CSimulate::StartSimulation( const CSimParameters &par )
{
	bool exit_code = false;
	WaitForSingleObject( IsHandleLocked, INFINITE );
	if( WaitForSingleObject( IsStop, 0 ) == WAIT_OBJECT_0 && par.IsValidate() ){
		SimPar = par;
		NumSteps = SimPar.NNSteps;
		ResetEvent( IsStop );
		SimData.Promt = "Single simulation: the model ";
		SimData.Promt += SimData.ModelName;
		SimData.Promt += " is running. Please wait...";
		SimThread = ( HANDLE )_beginthreadex( NULL, 0, SimulationThread, this, 0, NULL );
		exit_code = true;
	}
	ReleaseMutex( IsHandleLocked );
	return exit_code;
}

bool CSimulate::StopSimulation( void )
{
	bool exit_code = false;
	WaitForSingleObject( IsHandleLocked, INFINITE );
	if( WaitForSingleObject( IsStop, 0 ) != WAIT_OBJECT_0 ){
		SetEvent( IsStop );
		::WaitForSingleObject( SimThread, INFINITE );
		CloseHandle( SimThread );
		SimThread = NULL;
		exit_code = true;
	}
	ReleaseMutex( IsHandleLocked );
	return exit_code;
}

void CSimulate::EnableTimer( bool enabled )
{
	if( enabled ){
		LARGE_INTEGER start_time;
		start_time.QuadPart = -long( SimPar.UpdatingTime*10000 );
		SetWaitableTimer( Timer, &start_time, SimPar.UpdatingTime, NULL, NULL, FALSE );
	}
	else{
		CancelWaitableTimer( Timer );
	}
}

/////////////////////////////////////////////////////////////////////////////
// thread
bool CSimulate::startsim( void )
{
	bool exit_code = true;
	Initialize();
	EnableTimer( true );
	UpdateResults( false );     	// to send a notify message to the main window
	return exit_code;
}

void CSimulate::stopsim( void )
{
	EnableTimer( false );
	Shutdown();
	UpdateResults( true );     	// to send a notify message to the main window
}

size_t CSimulate::runsim( void )
{
	size_t error_code = _SIM_NOERRORS;
	error_code = ::run( SimPar.SimStep, CurrStep, NumThreads, this );
	return error_code;
}

//size_t CSimulate::ctrl_handler( bool *run, bool *quit, size_t *currstep )
size_t CSimulate::ctrl_handler( bool *quit, size_t *currstep )
{
	size_t error_code = _SIM_NOERRORS;
	HANDLE synch_objects[4] = { IsShutdown, IsStop,	Timer, IsSimulate, };
	ChartBuffer->next_step( CurrStep );
	#ifdef __MECHANICS__
		WalkerBuffer->next_step( CurrStep );
	#endif // __MECHANICS__
	long code = WaitForMultipleObjects( 4, synch_objects, false, INFINITE );
//	*run = false;
	switch( code ){
		case WAIT_OBJECT_0:                // the shutdown event is occured
			error_code = _SIM_SHUTDOWN;
			*quit = true;
			break;
		case WAIT_OBJECT_0+1:              // the stop event is occured
			error_code = _SIM_NOERRORS;
			*quit = true;
			break;
		case WAIT_OBJECT_0+2:              // to send a notify message to the main window
			if( IsStopped())
				break;
			if( IsPaused())
				break;
			if( !UpdateResults( false )){
				error_code = _SIM_UPDATE_ERROR;
				*quit = true;
				break;
			}
		case WAIT_OBJECT_0+3:              // to continue the simulation
//			*run = true;
			break;
		default:
			error_code = _SIM_SYNCH_ERROR;
			*quit = true;
	}
	if( ++CurrStep >= NumSteps-1 ){
		SetEvent( IsStop );
	}
	*currstep = CurrStep;
	return error_code;
}

unsigned int WINAPI CSimulate::SimulationThread( void *param )
{
	CSimulate *the_sim = ( CSimulate *)param;
	size_t error_code = _SIM_INIT_ERROR;
	if( the_sim->startsim()){
		error_code = the_sim->runsim();
		the_sim->stopsim();
	}
	return error_code;
}

#endif /* ! __CONSOLE__*/
