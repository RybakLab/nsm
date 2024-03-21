/////////////////////////////////////////////////////////////////////////////
// simulate.cpp : implementation file
#include "precompile.h"

#include "simulate.h"
#include "frameview.h"

#ifdef __CONSOLE__
#include <iostream>
using std::cout;
#else  
#include "../gui/Spcord.h"
#endif // __CONSOLE__

#ifdef __LINUX__
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
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
	TimeFactor = 1.;
	IsSimPaused = false;
	IsSimComplete = true;
	IsRedrawResults = false;
#if defined (__MECHANICS_2D__)
	LimbSkip = 1;
	LimbScale = 5;
	LimbOriginX = 0.5;
	LimbOriginY = 0.5;
	IsStickDiagram = false;
	Slope = 0.;
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif //__MECHANICS_2D__
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
#if defined (__MECHANICS_2D__)
	LimbSkip = data.LimbSkip;
	LimbScale = data.LimbScale;
	LimbOriginX = data.LimbOriginX;
	LimbOriginY = data.LimbOriginY;
	IsStickDiagram = data.IsStickDiagram;
	Slope = data.Slope;
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif //__MECHANICS_2D__
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
#if defined (__MECHANICS_2D__)
	LimbSkip = data.LimbSkip;
	LimbScale = data.LimbScale;
	LimbOriginX = data.LimbOriginX;
	LimbOriginY = data.LimbOriginY;
	IsStickDiagram = data.IsStickDiagram;
	Slope = data.Slope;
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif //__MECHANICS_2D__
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
#if defined (__MECHANICS_2D__)
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
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
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
#if defined (__MECHANICS_2D__)
	file << "LimbSkip = " << LimbSkip << endl;
	file << "LimbScale = " << LimbScale << endl;
	file << "LimbOriginX = " << LimbOriginX << endl;
	file << "LimbOriginY = " << LimbOriginY << endl;
	file << "Slope = " << Slope << endl;
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	file << "</SimulateData>" << endl;
}

/////////////////////////////////////////////////////////////////////////////
// class CSimulate
/////////////////////////////////////////////////////////////////////////////
// constructors/destructor
CSimulate::CSimulate( void ) 
/*, SimCfg( NULL ), NetworkT( NULL ) TODO alex*/
{
	/* TODO alex
	Control_values.reserve( NumCheck );
	std::generate_n( std::back_inserter( Control_values ), NumCheck, []{ return 0.; });
	//	Control_values.resize(NumCheck);//AK 
	view = 2;
	NumControl = 10;
	#ifdef __CONSOLE__
	ContFileName = "TestCons.txt";
	#else
	ContFileName = "TestGUI.txt";
	#endif // __CONSOLE__#ifndef __CONSOLE__
	*/
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
	DataFormat = 0;
	NumThreads = 1;
	NumSteps = 0;
	CurrStep = 0;
	RunMan = new runman(); // -->>> mod
	ChartBuffer = new CChartBuffer( this );
#if defined (__MECHANICS_2D__)
	BiomechT = NULL;
	Walker = NULL;
	WalkerBuffer = new CWalkerBuffer( this );
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
#ifndef __CONSOLE__
	::ReleaseMutex( IsHandleLocked );
#endif // __CONSOLE__
}
/* TODO alex
CSimulate & CSimulate ::operator = (const CSimulate & SS)	//AK
{
	SimData = SS.SimData; //AK there is  operator =
	*RunMan = *SS.RunMan;			//AK there is  operator =
	Network = SS.Network;	//AK there is  operator =
#if defined (__MECHANICS_2D__)
	//	*Walker = *SS.Walker;			//AK there was NO!!! operator = !!added by AK
	BiomechT = SS.BiomechT;		//AK there is  operator =
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	// *ChartBuffer = *SS.ChartBuffer;
	// *WalkerBuffer = *SS.WalkerBuffer;
	return *this;
}
*/
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
	delete RunMan;
	delete ChartBuffer;
#if defined (__MECHANICS_2D__)
	if( Walker )
		delete Walker;
	delete WalkerBuffer;
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
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
	Network.init( seed, RunMan, rand ); // -->>> mod
#if defined (__MECHANICS_2D__)
	if( Walker )
		delete Walker;
//	BiomechT.SetSlope(SimData.Slope); //TODO alex
	BiomechT.copy_to( &Walker );
	Walker->attach( &Network );
	Walker->set_slope( SimData.Slope );
	Walker->reg_unit( RunMan ); // -->>> mod
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif	// __MECHANICS_2D__
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
#if defined (__MECHANICS_2D__)
		BiomechT.save( file );
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
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

void CSimulate::SaveData( const char *filename, const hhn_pair<int> &wnd, double prec_t, int prec_a )
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
		ChartBuffer->save( *file, Results, wnd, prec_t, prec_a, DataFormat );
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
	ChartBuffer->init_all_buffers( SimPar.SimStep, NumSteps, freq );
#if defined (__MECHANICS_2D__)
	WalkerBuffer->init_all_buffers( SimPar.SimStep, NumSteps );
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	InitViews();
	CurrStep = 0;
#ifdef __OMP__
	NumThreads = ( SimData.NumThreads == 0 )? omp_get_num_procs(): SimData.NumThreads;
#else
	NumThreads = 1; // debug version
#endif //__OMP__
	::schedule( NumThreads, RunMan ); // -->>> mod
}

void CSimulate::InitViews( void )
{
	for( size_t i = 0; i < Views.size(); i++ )
		Views[i]->init_view( this );
	for( size_t i = 0; i < Views.size(); i++ )
		Views[i]->alloc_view();
	ChartBuffer->reg_unit(RunMan);
}

void CSimulate::ReleaseViews( void )
{
	ChartBuffer->release_all_buffers();
#if defined (__MECHANICS_2D__)
	WalkerBuffer->release_all_buffers();
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
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
#if defined (__MECHANICS_2D__)
	for( i = 0; i < Network.size_fbk(); i++ ){
		CheckView( network, Network.get_fbk( i ).get_name() );
	}
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	for( i = 0; i < Network.size_ctr(); i++ ){
		CheckView( network, Network.get_ctr( i ).get_name() );
	}
	for( i = 0; i < Views.size(); i++ ){
		for( size_t j = 0; j < Views[i]->npars(); j++ ){
			unit_code viewcode = Views[i]->get_par(j)->ParCode;
			if( viewcode.UnitId == _id_NNControl ){
				DWORD index = viewcode.NNIndex;
				for( size_t k = 0; k < network.size_ctr(); k++ ){
					string ctrlname = network.get_ctr( k ).get_name();
					if( ctrlname == Network.get_ctr( index ).get_name() ){
						viewcode.NNIndex = ( DWORD )k;
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
			nncode.UnitId = (DWORD)Network.get_nnunit( name )->uid();
			nncode.NNIndex = (DWORD)Network.get_nnunit( name )->nn_inx();
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
		newcode.UnitId = (DWORD)network.get_nnunit( name )->uid();
		newcode.NNIndex = (DWORD)network.get_nnunit( name )->nn_inx();
		unit_code oldcode;
		if( Network.get_nnunit( name )){
			oldcode.UnitId = (DWORD)Network.get_nnunit( name )->uid();
			oldcode.NNIndex = (DWORD)Network.get_nnunit( name )->nn_inx();
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
				nncode.NNIndex = (DWORD)k;
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
#if defined (__MECHANICS_2D__)
	for( size_t i = 0; i < Network.size_fbk(); i++ ){
		CheckRecord( network, Network.get_fbk( i ).get_name() );
	}
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	for( size_t i = 0; i < Network.size_ctr(); i++ ){
		CheckRecord( network, Network.get_ctr( i ).get_name() );
	}
	for( size_t i = 0; i < Results.size(); i++ ){
		if( Results[i].UnitId == _id_NNControl ){
			size_t index = Results[i].NNIndex;
			for( size_t k = 0; k < network.size_ctr(); k++ ){
				string ctrlname = network.get_ctr( k ).get_name();
				if( ctrlname == Network.get_ctr( index ).get_name() ){
					Results[i].NNIndex = (DWORD)k;
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
			nncode.UnitId = (DWORD)Network.get_nnunit( name )->uid();
			nncode.NNIndex = (DWORD)Network.get_nnunit( name )->nn_inx();
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
		newcode.UnitId = (DWORD)network.get_nnunit( name )->uid();
		newcode.NNIndex = (DWORD)network.get_nnunit( name )->nn_inx();
		unit_code oldcode;
		if( Network.get_nnunit( name )){
			oldcode.UnitId = (DWORD)Network.get_nnunit( name )->uid();
			oldcode.NNIndex = (DWORD)Network.get_nnunit( name )->nn_inx();
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
				nncode.NNIndex = (DWORD)k;
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
#if defined (__MECHANICS_2D__)
		else
			data = ( void *)Walker->select(( unit_code *)&code );
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	}
	if( data )
		return true;
	return false;
}

size_t CSimulate::ctrl_handler( bool *quit, size_t *currstep )
{
#ifndef __CONSOLE__
	size_t error_code = _SIM_NOERRORS;
	HANDLE synch_objects[4] = { IsShutdown, IsStop,	Timer, IsSimulate, };
	ChartBuffer->next_step( CurrStep );
#if defined (__MECHANICS_2D__)
	WalkerBuffer->next_step( CurrStep );
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	long code = WaitForMultipleObjects( 4, synch_objects, false, INFINITE );
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
#else
	size_t error_code = _SIM_NOERRORS;
	if( ++CurrStep >= NumSteps-1 ){
		*quit = true;
	}
	*currstep = CurrStep;
	return error_code;
#endif /*__CONSOLE__*/
}

#ifdef __CONSOLE__
bool CSimulate::LoadModel( const char *filename )
{
#ifdef __LINUX__
	remove_char( filename, 0x0D );
#endif
	_FileVersion = 2.05;
	
	ifstream file( filename );
	if( !file.is_open() ){
		message( "Cannot open file", "Error" );
		return false;
	}
	file.seekg(0, std::ios::end);
	std::streamsize fsize = file.tellg();
	file.seekg( 0, ios::beg );
	vector<char> buffer( fsize );
	file.read( buffer.data(), fsize );
	istringstream inp( string( buffer.begin(), buffer.begin()+file.gcount()));
	file.close();

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
			LoadOK = LoadViews( inp );
		}
		else if( str == "<Records>" ){
 			LoadOK = LoadRecords( inp );
		}
#if defined (__MECHANICS_2D__)
		else if( str == "<Biomechanics" ){
			inp >> ws;
			getline( inp, str, '>') ;
			BiomechT.load( inp );
		}
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif	// __MECHANICS_2D__
		else if( !str.empty() && str[0] != 0x00 ){
			unknown_string( inp, str );
		}
	}
	if( LoadOK ){
		init( SimData.Seed );
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
				if( view.load( file, this ) ){
					CFrameView *new_view = NULL;
					view.copy_to( &new_view );
					Views.push_back( new_view );
				}
		}
#if defined (__MECHANICS_2D__)
			else if( str == "Walker"){
				CWalkerFrameView view;
				if( view.load( file, this )){
					CFrameView *new_view = NULL;
					view.copy_to( &new_view );
					Views.push_back( new_view );
				}
			}
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
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
		lock_data();
		ChartBuffer->release_all_buffers();
#if defined (__MECHANICS_2D__)
		WalkerBuffer->release_all_buffers();
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
		unlock_data();
		SimData.SetSimParameters( &SimPar );
		NumSteps = SimPar.NNSteps;
		Initialize();
		cout << endl;
		long slice = NumSteps/100;
		long percent = 0;
		long t0 = long( time( NULL ));
		for( CurrStep = 0; CurrStep < NumSteps; ){
			::next_step( SimPar.SimStep, CurrStep, NumThreads, this, RunMan );// -->>> mod
			if( CurrStep%slice == 0 ){
				cout << SimData.Promt << std::setw( 3 ) << percent << "%\r";
				++percent;
			}
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

void CSimulate::InitNetwork( CHhnNetwork &network, long seed )
{
	CheckViews( network );
	CheckRecords( network );
	Network = network;
	Network.init( seed, RunMan ); // -->>> mod
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
	_FileVersion = 2.05;
	ifstream file( filename );
	if( !file.is_open() ){
		message( "Cannot open file", "Error" );
		return false;
	}
	file.seekg(0, std::ios::end);
	std::streamsize fsize = file.tellg();
	file.seekg( 0, ios::beg );
	vector<char> buffer( fsize );
	file.read( buffer.data(), fsize );
	istringstream inp( string( buffer.begin(), buffer.begin()+file.gcount()));
	file.close();
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
#if defined (__MECHANICS_2D__)
		else if( str == "<Biomechanics" ){
			inp >> ws;
			getline( inp, str, '>') ;
			BiomechT.load( inp );
		}
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif	// __MECHANICS_2D__
		else if( !str.empty() && str[0] != 0x00 ){
			unknown_string( inp, str );
		}
	}

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
#if defined (__MECHANICS_2D__)
			else if( str == "Walker"){
				CWalkerFrameView view;
				if( view.load( file, this )){
					CFrameView *new_view = NULL;
					view.copy_to( &new_view );
					viewfifo.push( new_view );
				}
			}
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
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

#if defined (__MECHANICS_2D__)
void CSimulate::upd_outlist( void )
{
	vector<string> outlist;
	for( size_t i = 0; i < Network.size_out(); ++i ){
		outlist.push_back( Network.get_out( i ).get_name());
	}
	BiomechT.upd_outlist( outlist );
}
#elif defined (__MECHANICS_3D__)
// TODO implementation 3d model
#endif // __MECHANICS_2D__

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
		SetWaitableTimer( Timer, &start_time, (LONG)SimPar.UpdatingTime, NULL, NULL, FALSE );
	} else{
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
	UpdateResults( false );		// to send a notify message to the main window
	return exit_code;
}

void CSimulate::stopsim( void )
{
	EnableTimer( false );
	Shutdown();
	UpdateResults( true );		// to send a notify message to the main window
}

size_t CSimulate::runsim( void )
{
	size_t error_code = _SIM_NOERRORS;
	Network.prerun( SimPar.SimStep );
	error_code = ::run( SimPar.SimStep, CurrStep, NumThreads, this, RunMan );
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
	return ( unsigned int )error_code;
}

#endif /* ! __CONSOLE__*/
