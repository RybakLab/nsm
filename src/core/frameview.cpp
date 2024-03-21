/////////////////////////////////////////////////////////////////////////////
// frameview.cpp
#include "precompile.h"
#include "frameview.h"
#include "simulate.h"
#include "../gui/Spcord.h"

#ifndef __LINUX__ 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG
#endif // __LINUX__

/////////////////////////////////////////////////////////////////////////////
// class CViewParam
bool CViewParam::load( istream &file, CSimulate *manager )
{
	string str;
	bool bOk = false;
	while( file >> str){
		if( str == "Ymax"){
			file >> str >> Ymax;
		}
		else if( str == "Ymin"){
			file >> str >> Ymin;
		}
		else if( str == "<Code>"){
			bOk = ParCode.load( file, manager );
		}
		else if( str == "</Parameter>"){
			return bOk;
		}
		else{
			unknown_string( file, str);
		}
	}
	message( "Missing tag </Parameter>!", "Warning!" );
	return false;
}

void CViewParam::save( ostream &file, CSimulate *manager )
{
	file << "\n<Parameter>\n";
	file << "Ymax = " << Ymax << "\n";
	file << "Ymin = " << Ymin << "\n";
	ParCode.save( file, manager );
	file << "</Parameter>\n";
}

/////////////////////////////////////////////////////////////////////////////
// CFrameView class
CFrameView::CFrameView( void )
	: ViewName("empty"), BufferManager( NULL ), LimitView(), LimitStep()
{
#ifndef __CONSOLE__
	ShowCmd = SW_SHOWNORMAL;
#else
	ShowCmd = 0;
#endif // __CONSOLE__
	WindowRect.left = WindowRect.top = WindowRect.right = WindowRect.bottom = -1;
}

CFrameView::CFrameView( const CFrameView &view )
	: ShowCmd( view.ShowCmd ), WindowRect( view.WindowRect ), ViewName( view.ViewName ), BufferManager( view.BufferManager ), LimitView( view.LimitView ), LimitStep( view.LimitStep )
{
}

CFrameView &CFrameView::operator = ( const CFrameView &view)
{
	ViewName = view.ViewName;
	BufferManager = view.BufferManager;
	ShowCmd = view.ShowCmd;
	WindowRect = view.WindowRect;
	LimitView = view.LimitView;
	LimitStep = view.LimitStep;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// public methods
void CFrameView::set_xrange( double begin, double end )
{
	LimitView.X = begin;
	LimitView.Y = end;
	set_range();
}

bool CFrameView::validate( hhn_pair<size_t> &limits )
{
	if( limits.Y < LimitStep.X || limits.X > LimitStep.Y )
		return false;
	if( limits.X < LimitStep.X )
		limits.X = LimitStep.X;
	if( limits.Y > LimitStep.Y )
		limits.Y = LimitStep.Y;
	return true;
}

bool CFrameView::load_pos( istream &file )
{
	string str;
	while( file >> str){
		if( str == "ShowCmd"){
			file >> str >> ShowCmd;
		}
		else if( str == "WindowRect"){
			file >> str >> WindowRect;
		}
		else if( str == "X0"){
			file >> str >> WindowRect.left;
		}
		else if( str == "X1"){
			file >> str >> WindowRect.right;
		}
		else if( str == "Y0"){
			file >> str >> WindowRect.top;
		}
		else if( str == "Y1"){
			file >> str >> WindowRect.bottom;
		}
		else if( str == "</Position>"){
			return true;
		}
		else{
			unknown_string( file, str);
		}
	}
	message( "Missing tag </Position>!", "Warning!" );
	return false;
}

void CFrameView::save_pos( ostream &file )
{
	file << endl << "<Position>" << endl;
	file << "ShowCmd = " << ShowCmd << endl;
	file << "WindowRect = " << WindowRect << endl;
	file << "</Position>" << endl;
}

void CFrameView::set_range( void )
{
	if( BufferManager && BufferManager->step() > 0.){
		LimitStep.X = ( unsigned long )( LimitView.X/BufferManager->step());
		LimitStep.Y = ( unsigned long )( LimitView.Y/BufferManager->step());
		if( LimitStep.Y > BufferManager->max_nsteps() )
			LimitStep.Y = BufferManager->max_nsteps();
		if( LimitStep.X > BufferManager->max_nsteps() )
			LimitStep.X = BufferManager->max_nsteps();
	}
}

bool CFrameView::load( istream &file, CSimulate * )
{
	string str;
	while( file >> str){
		if( str == "LimitView"){
			file >> str >> LimitView;
		}
		else if( str == "ViewName"){
			file >> str; file >> ws;
			getline( file, ViewName );
		}
		else if( str == "<Position>"){
			load_pos( file );
		}
		else if( str == "</View>"){
			return true;
		}
		else{
			unknown_string( file, str);
		}
	}
	message( "Missing tag </View>!", "Warning!" );
	return false;
}

void CFrameView::save( ostream &file, CSimulate * )
{
	file << "LimitView = " << LimitView << endl;
	file << "ViewName = " << ViewName << endl;
	save_pos( file );
	file << "</View>" << endl;
}

#ifndef __CONSOLE__
bool CFrameView::create_view( CDocTemplate *tpl, CDocument* doc, CMDIChildWnd *child )
{
	ASSERT_VALID( tpl );
	CFrameWnd *nnView = tpl->CreateNewFrame( doc, child );	// fuck me it's here
	if( nnView ){
		tpl->InitialUpdateFrame( nnView, doc );
		return true;
	}
	return false;        // command failed
}
#endif /*__CONSOLE__*/

/////////////////////////////////////////////////////////////////////////////
// class CChartFrameView
CChartFrameView &CChartFrameView::operator = ( CChartFrameView &view)
{
	CFrameView::operator = ( view );
	NumRow = view.NumRow;
	NumCol = view.NumCol;
	NNParam = view.NNParam;
	return *this;
}

void CChartFrameView::init_view( CSimulate *manager )
{
	CFrameView::init_buf( &manager->GetChartBuffer() );
	if( BufferManager ){
		for( size_t i = 0; i < NNParam.size(); ++i ){
			if( (( CChartBuffer *)BufferManager )->add_buffer( NNParam[i].ParCode ) == false ){
				remove( i ); // Variable is not exist;
			}
		}
	}
}

void CChartFrameView::alloc_view( void )
{
	if( BufferManager ){
		CFrameView::alloc_view();
		BufferManager->alloc_all_buffers();
	}
}

void CChartFrameView::copy_to( CFrameView **view ) const
{
	*view = new CChartFrameView( *this );
}

void CChartFrameView::add_view( const CViewParam &param )
{
	NNParam.push_back( param );
	set_geometry( npars(), 1 ); 
}

void CChartFrameView::remove( size_t inx )
{
	if( inx >= NNParam.size())
		return;
	NNParam.erase( NNParam.begin()+inx );
	if( npars() > 0 ){
		set_geometry( npars(), 1 ); 
	}
	else{
		set_geometry( 0, 0 ); 
	}
}

void CChartFrameView::del_view( unit_code &code )
{
	for( size_t i = 0; i < npars(); i++ ){
		if( NNParam[i].ParCode == code ){
			remove( i );
			return; 
		} 
	}
}

void CChartFrameView::clear( void )
{
	NNParam.clear();
	set_geometry( 0, 0 ); 
}

void CChartFrameView::swap( size_t inx1, size_t inx2 )
{
	std::swap( NNParam[inx1], NNParam[inx2] );
}

#ifndef __CONSOLE__
bool CChartFrameView::create_view( CDocument* doc, CMDIChildWnd *child )
{
	CDocTemplate *tpl = (( neurosim_app *)AfxGetApp())->m_pView;
	return CFrameView::create_view( tpl, doc, child );
}
#endif // __CONSOLE__

bool CChartFrameView::load( istream &file, CSimulate *manager )
{
	string str;
	bool wrong = false;
	clear();
	while( file >> str){
		if( str == "NumRow"){
			file >> str >> NumRow;
		}
		else if( str == "NumCol"){
			file >> str >> NumCol;
		}
		else if( str == "<Parameter>"){
			CViewParam param;
			if(param.load( file, manager ))
				NNParam.push_back( param );
			else
				wrong = true;
		}
		else if( str == "ViewName"){
			file >> str; file >> ws;
			getline( file, ViewName );
		}
		else if( str == "LimitView"){
			file >> str >> LimitView;
			set_range();
		}
		else if( str == "<Position>"){
			load_pos( file );
		}
		else if( str == "</View>"){
			if( wrong )
				set_geometry( npars(), 1 ); 
			return true;
		}
		else{
			unknown_string( file, str);
		}
	}
	message( "Missing tag </View>!", "Warning!" );
	return false;
}

void CChartFrameView::save( ostream &file, CSimulate *manager )
{
	file << endl << "<View Chart>" << endl;
	file << "NumRow = " << NumRow << endl;
	file << "NumCol = " << NumCol << endl;
	for( unsigned int i = 0; i < npars(); i++){
		NNParam[i].save( file, manager );
	}
	CFrameView::save( file, manager );
}

#if defined (__MECHANICS_2D__)
/////////////////////////////////////////////////////////////////////////////
// CWalkerFrameView class
void CWalkerFrameView::init_view( CSimulate *manager )
{
	CFrameView::init_buf( &manager->GetWalkerBuffer() );
}

void CWalkerFrameView::alloc_view( void )
{
	if( BufferManager ){
		CFrameView::alloc_view();
		BufferManager->alloc_all_buffers();
	}
}

void CWalkerFrameView::copy_to( CFrameView **view ) const
{
	*view = new CWalkerFrameView( *this );
}

#ifndef __CONSOLE__
bool CWalkerFrameView::create_view( CDocument* doc, CMDIChildWnd *child )
{
	CDocTemplate *tpl = (( neurosim_app *)AfxGetApp())->m_pLimb;
	return CFrameView::create_view( tpl, doc, child );
}
#endif // __CONSOLE__

void CWalkerFrameView::save( ostream &file, CSimulate *manager )
{
	file << endl << "<View Walker>" << endl;
	CFrameView::save( file, manager );
}
#elif defined (__MECHANICS_3D__)
// TOD implementation for 3d model
#endif // __MECHANICS_2D__
