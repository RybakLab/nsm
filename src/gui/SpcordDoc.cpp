/////////////////////////////////////////////////////////////////////////////
// SpcordDoc.cpp : implementation of the neurosim_doc class
#include "precompile.h"

#ifndef __CONSOLE__
#include "SpcordDoc.h"
#include "SpcordView.h"
#include "Spcord.h"
#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG

extern CHhnNetwork *GlobalNet;

/////////////////////////////////////////////////////////////////////////////
// neurosim_doc diagnostics
#ifdef _DEBUG
void neurosim_doc::AssertValid() const
{
	CDocument::AssertValid();
}

void neurosim_doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// neurosim_doc
IMPLEMENT_DYNCREATE( neurosim_doc, CDocument )

BEGIN_MESSAGE_MAP( neurosim_doc, CDocument )
	ON_COMMAND( IDM_REFRESH, OnRefresh )
	ON_UPDATE_COMMAND_UI( ID_FILE_PRINT, OnUpdateFilePrint )
	ON_UPDATE_COMMAND_UI( ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// construction/destruction
neurosim_doc::neurosim_doc()
{
	IsPlotting = ::CreateMutex( NULL, false, NULL );
	Model = new CSimulate();
	SimData = &( Model->SimData );
	GlobalNet = &( Model->Network );
}

neurosim_doc::~neurosim_doc()
{
	::WaitForSingleObject( IsPlotting, INFINITE );
	::CloseHandle( IsPlotting );
	delete Model;
	SimData = NULL;
	while( get_view()){
		popView();
	}
}

/////////////////////////////////////////////////////////////////////////////
// manage views
CFrameView *neurosim_doc::get_view( void )
{
	if( ViewFIFO.empty()){
		return NULL;
	}
	return ViewFIFO.front();
}

void neurosim_doc::pushView( CFrameView *view )
{
	CFrameView *new_view = NULL;
	view->copy_to( &new_view );
	ViewFIFO.push( new_view );
}

void neurosim_doc::popView( void )
{
	if( !ViewFIFO.empty()){
		CFrameView *view = ViewFIFO.front();
		if( view )
			delete view;
		ViewFIFO.pop();
	}
}

bool neurosim_doc::start( void )
{
	CSimParameters par;
	SimData->SetSimParameters( &par, AfxGetMainWnd()->m_hWnd );
	if( SimData->IsSimComplete && Model->StartSimulation( par )){
		InitViews();
		SimData->IsSimComplete = false;
		return true;
	}
	return false;
}

bool neurosim_doc::stop( void )
{
	if( Model->StopSimulation()){
		return true;
	}
	ShowResults();
	return false;
}

void neurosim_doc::pause( bool p )
{
	Model->PauseSimulation( p );
}

bool neurosim_doc::InitViews( void )
{
	if( SimData->IsSimComplete ){
		POSITION pos = GetFirstViewPosition();
		while( pos != NULL ) {
			neurosim_view *pView = ( neurosim_view *)GetNextView( pos );
			pView->InitDraw();
		}
		return true;
	}
	return false;
}

void neurosim_doc::DrawTo( bool complete, double time )
{
	if( ::WaitForSingleObject( IsPlotting, 0 ) == WAIT_OBJECT_0 ){
		POSITION pos = GetFirstViewPosition();
		while( pos != NULL ){
			neurosim_view *pView = ( neurosim_view *)GetNextView( pos );
			if( pView->IsKindOf( RUNTIME_CLASS( CChartView )) ) 
				pView->DrawTo( complete, unsigned long( time/SimData->SimStep ));
#ifdef __MECHANICS__
			else if( pView->IsKindOf( RUNTIME_CLASS( CLimbView )))
				pView->DrawTo( complete, unsigned long( time/SimData->SimStep ));
#endif //__MECHANICS__
		}
		::ReleaseMutex( IsPlotting );
	}
}

void neurosim_doc::ShowResults( void )
{
	if( SimData->IsSimComplete )
		DrawTo( true, -1 );
}

void neurosim_doc::ShowResults( neurosim_view *view )
{
	view->DrawTo( true, -1 );
}

void neurosim_doc::UpdateViews( bool complete, unsigned long counter )
{
	if( !SimData->IsSimComplete )
		DrawTo( complete, counter*SimData->SimStep);
	if( complete )
		SimData->IsSimComplete = true;
}

void neurosim_doc::ClearAllViews( void )
{
	Model->ReleaseViews();
	Model->InitViews();
	ShowResults();
}

BOOL neurosim_doc::OnNewDocument()
{
	if( !CDocument::OnNewDocument())
		return FALSE;
	Model->init( SimData->Seed );
	CChartFrameView view;
	view.set_xrange( SimData->BeginView, SimData->EndView );
	pushView( &view );
	Model->InitViews();
	ShowResults();
	return TRUE;
}

BOOL neurosim_doc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if( !CDocument::OnOpenDocument( lpszPathName ))
		return FALSE;
	Model->LoadModel( lpszPathName, ViewFIFO );
	if( ViewFIFO.empty()){
		CChartFrameView view;
		view.set_xrange( SimData->BeginView, SimData->EndView );
		pushView( &view );
	}
	return TRUE;
}

BOOL neurosim_doc::OnSaveDocument( LPCTSTR lpszPathName ) 
{
	CDocument::OnSaveDocument( lpszPathName );
	BeginWaitCursor();
	string info = "Saving the model to ";
	info +=  lpszPathName;
	info += ". Please wait... ";
	(( CMainFrame* )AfxGetMainWnd())->StatusBarMessage( info.c_str() );
	Model->SaveModel( lpszPathName );
	(( CMainFrame* )AfxGetMainWnd())->StatusBarMessage( "Ready" );
	EndWaitCursor();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// neurosim_doc commands
void neurosim_doc::OnRefresh( void )
{
	UpdateAllViews( NULL );
}

void neurosim_doc::OnUpdateFilePrint( CCmdUI *pCmdUI ) 
{
	pCmdUI->Enable( SimData->IsSimComplete );
}

void neurosim_doc::OnUpdateFilePrintPreview( CCmdUI* pCmdUI ) 
{
	pCmdUI->Enable( SimData->IsSimComplete );
}

#endif // __CONSOLE__
