/////////////////////////////////////////////////////////////////////////////
// MainFrm.cpp : implementation of the CMainFrame class
#include "precompile.h"

#ifndef __CONSOLE__

#include "MainFrm.h"
#include "afxpriv.h"
#include "SpcordDoc.h"
#include "ChildFrm.h"
#include "SpcordView.h"
#include "networksetup.h"
#include "ControlSetup.h"
#include "NetworkView.h"
#include "biomechsetup.h"
#include "SimulateSetup.h"
#include "ConnectionSetup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump( CDumpContext &dc ) const
{
	CMDIFrameWnd::Dump( dc );
}
#endif // _DEBUG

extern CHhnNetwork *GlobalNet;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame
IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_WINDOW_NEW, OnWindowNew)
	ON_COMMAND(ID_SAVE_VIEW, OnSaveView)
	ON_COMMAND(ID_NETWORK_SETUP, OnNetworkSetup)
	ON_COMMAND(ID_CONNECTION_SETUP, OnConnectionSetup)
	ON_COMMAND(ID_NETWORK_CONTROL, OnNetworkControl)
	ON_COMMAND(ID_NETWORK_INIT, OnNetworkInit)
	ON_COMMAND(ID_NETWORK_DIFF, OnNetworkDiff)
	ON_COMMAND(ID_MODEL_PARAM, OnModelParam)
	ON_COMMAND(ID_MODEL_START, OnModelStart)
	ON_COMMAND(ID_MODEL_STOP, OnModelStop)
	ON_COMMAND(ID_MODEL_PAUSE, OnModelPause)
	ON_COMMAND(ID_REDRAW_RESULTS, OnRedrawResults)
	ON_COMMAND(ID_SAVE_RESULTS, OnSaveResults)
	ON_COMMAND(ID_NETWORK_VIEW, OnNetworkView)

	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateMenuItem)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateMenuItem)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateMenuItem)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateMenuItem)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateMenuItem)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateMenuItem)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateMenuItem)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_SETUP, OnUpdateMenuItem)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_NEW, OnUpdateMenuItem)
	ON_UPDATE_COMMAND_UI(ID_SAVE_VIEW, OnUpdateMenuItem)
	ON_UPDATE_COMMAND_UI(ID_NETWORK_INIT, OnUpdateMenuItem)
	ON_UPDATE_COMMAND_UI(ID_NETWORK_DIFF, OnUpdateMenuItem)
	ON_UPDATE_COMMAND_UI(ID_NETWORK_SETUP, OnUpdateMenuItem)
	ON_UPDATE_COMMAND_UI(ID_MODEL_PARAM, OnUpdateMenuItem)
	ON_UPDATE_COMMAND_UI(ID_MODEL_START, OnUpdateMenuItem)
	ON_UPDATE_COMMAND_UI(ID_NETWORK_CONTROL, OnUpdateMenuItem)
	ON_UPDATE_COMMAND_UI(ID_REDRAW_RESULTS, OnUpdateMenuItem)
	ON_UPDATE_COMMAND_UI(ID_SAVE_RESULTS, OnUpdateMenuItem)
	ON_UPDATE_COMMAND_UI(ID_CONNECTION_SETUP, OnUpdateMenuItem)
	ON_UPDATE_COMMAND_UI(ID_MODEL_STOP, OnUpdateModelStop)
	ON_UPDATE_COMMAND_UI(ID_MODEL_PAUSE, OnUpdateModelPause)
	ON_UPDATE_COMMAND_UI(ID_NETWORK_VIEW, OnUpdateNetworkView)

	ON_WM_TIMER()
	ON_MESSAGE( WM_UPDATE_MAIN_WND, OnUpdateResults )
	ON_MESSAGE( WM_STOP_MAIN_WND_TIMER, OnStopTimer )
	ON_MESSAGE( WM_SETMESSAGESTRING, OnSetMessageString )
#ifdef __MECHANICS__
	ON_COMMAND( IDM_NEW_LIMB_WINDOW, OnNewlimbwindow )
	ON_COMMAND( ID_BIOMECHANICS, OnBiomechanics )
	ON_COMMAND( ID_IS_STICK, OnIsStick )
	ON_UPDATE_COMMAND_UI( IDM_NEW_LIMB_WINDOW, OnUpdateNewlimbwindow )
	ON_UPDATE_COMMAND_UI( ID_BIOMECHANICS, OnUpdateMenuItem )
	ON_UPDATE_COMMAND_UI( ID_IS_STICK, OnUpdateIsStick )
#endif // __MECHANICS__
END_MESSAGE_MAP()

static UINT BASED_CODE indicators[] = {
	ID_SEPARATOR,           // status line indicator
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction
CMainFrame::CMainFrame()
{
	CurrStep = 0;
	TimerHandle = 0;
}

CMainFrame::~CMainFrame()
{
}

void CMainFrame::createFrameView( CFrameView *view )
{
	CMDIChildWnd *childWnd = MDIGetActive();
	CDocument *doc;
	if( childWnd == NULL || ( doc = childWnd->GetActiveDocument()) == NULL ){
		TRACE( traceAppMsg, 0, "Warning: No active document for WindowNew command.\n" );
		AfxMessageBox( AFX_IDP_COMMAND_FAILURE );
	}
	else{
		if( !view->create_view( doc, childWnd )){
			if( view->ShowCmd == SW_SHOWMAXIMIZED ){
				childWnd->MDIActivate();		
				childWnd->SetActiveView( childWnd->GetActiveView());
			}
		}
		else{
			TRACE( traceAppMsg, 0, "Warning: failed to create new frame.\n" );
		}
	}
}

CDocument* CMainFrame::GetActiveDoc() 
{
	CMDIChildWnd *pActiveChild = MDIGetActive();
	CDocument *pDoc = NULL;
	if( pActiveChild == NULL || ( pDoc = pActiveChild->GetActiveDocument()) == NULL ){
		TRACE0( "Warning: No active document for WindowNew command\n" );       // command failed
	}
	return pDoc;
}

int CMainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if( CMDIFrameWnd::OnCreate( lpCreateStruct ) == -1 )
		return -1;
#ifdef __MECHANICS__
	if( !defaultToolBar.CreateEx( this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!defaultToolBar.LoadToolBar( IDR_LOCOTYPE ) ){
		TRACE0( "Failed to create toolbar\n" );
		return -1;      // fail to create
	}
#else
	if( !defaultToolBar.CreateEx( this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!defaultToolBar.LoadToolBar(IDR_NSMTYPE) ){
		TRACE0( "Failed to create toolbar\n" );
		return -1;      // fail to create
	}
#endif // __MECHANICS__
	if( !m_wndStatusBar.Create( this ) ||
		!m_wndStatusBar.SetIndicators( indicators, sizeof(indicators)/sizeof(UINT)) ){
		TRACE0( "Failed to create status bar\n" );
		return -1;      // fail to create
	}
	defaultToolBar.EnableDocking( CBRS_ALIGN_ANY );
	EnableDocking( CBRS_ALIGN_ANY );
	DockControlBar( &defaultToolBar );
	m_mdiTabs.Create(this, MT_BOTTOM|MT_IMAGES|MT_HIDEWLT2VIEWS );
	return 0;
}

BOOL CMainFrame::DestroyWindow() 
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc();
	if( pDoc ){
		while( !pDoc->Model->IsStopped());
	}
	save_wndpl( this, "mainframe" ); 	// save mainframe window position
	return CMDIFrameWnd::DestroyWindow();
}

void CMainFrame::OnUpdateFrameTitle( BOOL bAddToTitle )
{
	m_mdiTabs.Update(); // sync the mditabctrl with all views
	CMDIFrameWnd::OnUpdateFrameTitle( bAddToTitle );
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnClose() 
{
#ifdef _DEBUG
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc();
	if( pDoc && pDoc->IsModified()){
		pDoc->SetModifiedFlag( FALSE ); // fuck ya
	}
#endif
	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnWindowNew() 
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ) return;
	CChartFrameView view;
	view.set_xrange( pDoc->SimData->BeginView, pDoc->SimData->EndView );
	pDoc->pushView( &view );
	createFrameView( &view );
	pDoc->SetModifiedFlag();
	pDoc->Model->InitViews();
	pDoc->ShowResults();
}

void CMainFrame::OnNetworkView( void )
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ) return;
	neurosim_view *pView = ( neurosim_view *)MDIGetActive()->GetActiveView();
	if( pView && pView->IsKindOf( RUNTIME_CLASS( CChartView ))){
		CNetworkView dlg;
		dlg.View = *(( CChartFrameView *)( pView->View ));
		dlg.Manager = pDoc->Model;
		if( dlg.DoModal() == IDOK ){
			dlg.View.set_name( dlg.NameView );
			*(( CChartFrameView *)( pView->View )) = dlg.View;
			(( CChartView * )pView )->SetView();
			MDIGetActive()->SetWindowText( pView->View->get_name() );
			pDoc->ShowResults( pView );
			pDoc->SetModifiedFlag();
			OnUpdateFrameTitle();
		}
	}
}

void CMainFrame::ViewProperies( void )
{
	OnNetworkView();
}

void CMainFrame::OnNetworkSetup() 
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ) return;
	CNetworkSetup dlg("Network Setup");
	dlg.Network = pDoc->Model->Network;
#ifdef __MECHANICS__
	dlg.BiomT = pDoc->Model->BiomechT;
#endif // __MECHANICS__
	GlobalNet = &( dlg.Network );
	if( dlg.DoModal() == IDOK ){
		dlg.Network.init( pDoc->Model->SimData.Seed );
		pDoc->Model->CreateNetwork( dlg.Network, pDoc->Model->SimData.Seed );
		for( neurosim_view *pView = NULL, *currView = ( neurosim_view *)MDIGetActive()->GetActiveView(); pView != currView; ){
			MDINext();
			pView = ( neurosim_view *)MDIGetActive()->GetActiveView();
			if( pView->IsKindOf( RUNTIME_CLASS( CChartView )) )
				(( CChartView * )pView )->SetView();
		}
		pDoc->ClearAllViews();
		pDoc->SetModifiedFlag();
	}
	GlobalNet = &( pDoc->Model->Network );
}
 
void CMainFrame::OnNetworkControl() 
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ) return;
	CNetworkControl dlg;
	dlg.Network = pDoc->Model->Network;
	dlg.Network.init( pDoc->Model->SimData.Seed );
	if( dlg.DoModal() == IDOK ){
		dlg.Network.init( pDoc->Model->SimData.Seed );
		pDoc->Model->CreateNetwork( dlg.Network, pDoc->Model->SimData.Seed ); 
		for( neurosim_view *pView = NULL, *currView = ( neurosim_view *)MDIGetActive()->GetActiveView(); pView != currView; ){
			MDINext();
			pView = ( neurosim_view *)MDIGetActive()->GetActiveView();
			if( pView->IsKindOf( RUNTIME_CLASS( CChartView )) )
				(( CChartView * )pView )->SetView();
		}
		pDoc->ClearAllViews();
		pDoc->SetModifiedFlag();
	}  
}

void CMainFrame::OnNetworkInit() 
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ) return;
	pDoc->SimData->Randomize();
	pDoc->Model->init( pDoc->SimData->Seed, true );
	pDoc->SetModifiedFlag();
}

void CMainFrame::OnNetworkDiff() 
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ) return;
	CNetDiff dlg;
	if( dlg.DoModal() == IDOK ){
		ofstream log( "log.log" );
		pDoc->Model->diff(( LPCTSTR )dlg.Pattern, log );
	}
}

void CMainFrame::OnModelParam() 
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ) return;
	CSimulateSetup dlg( "Simulation parameters" );
	dlg.Data = *pDoc->SimData;
	double beginview = pDoc->SimData->BeginView;
	double endview = pDoc->SimData->EndView;
	if( dlg.DoModal() == IDOK ){
		*pDoc->SimData = dlg.Data;
		if( dlg.IsCheckAllView() ){
			pDoc->Model->SetTimeScale( pDoc->SimData->BeginView, pDoc->SimData->EndView );
			pDoc->ShowResults();
		}
		else{
			pDoc->SimData->BeginView = beginview;
			pDoc->SimData->EndView = endview;
			neurosim_view *view = ( neurosim_view *)MDIGetActive()->GetActiveView();
			if( view && view->View ){
				view->View->set_xrange( dlg.Data.BeginView, dlg.Data.EndView );
				pDoc->ShowResults( view );
			}
		}
		pDoc->SetModifiedFlag();
	}
}

void CMainFrame::OnModelStart() 
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ) return;
	POSITION pos = pDoc->GetFirstViewPosition();
	while( pos ) {
		CView *pView = pDoc->GetNextView( pos );
		if( pView->IsKindOf( RUNTIME_CLASS( CChartView )) )
			(( CChartView * )pView )->SetView();
	}
	pDoc->Model->init( pDoc->SimData->Seed );
	pDoc->pause( pDoc->SimData->IsSimPaused );
	pDoc->start();
}

void CMainFrame::OnModelStop() 
{
	if( TimerHandle )
		SendMessage( WM_STOP_MAIN_WND_TIMER, 0, 0 );
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ) return;
	pDoc->stop();
}

void CMainFrame::OnModelPause() 
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ) return;
	pDoc->SimData->IsSimPaused = !pDoc->SimData->IsSimPaused;
	pDoc->pause( pDoc->SimData->IsSimPaused );
}

void CMainFrame::OnRedrawResults() 
{
	if( TimerHandle ){
		KillTimer( TimerHandle );
		TimerHandle = 0;
	}
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ) return;
	pDoc->InitViews();
	pDoc->SimData->IsSimComplete = false;
	CurrStep = 0;
	TimerHandle = SetTimer( 1, UINT( pDoc->SimData->UpdatingTime*pDoc->SimData->TimeFactor ), NULL );
	pDoc->SimData->Promt = "Redrawing the results... ";
}

void CMainFrame::OnSaveResults() 
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ) return;
	CFormatDialog formatdialog;
	for( unsigned int i = 0; i < pDoc->Model->GetChartBuffer().nbuffs(); i++ ){
		unit_code parcode = pDoc->Model->GetChartBuffer()[i].get_code();
		string name;
		parcode.get_fullname( pDoc->Model, name );
		formatdialog.AllNames.push_back( name );
		if( pDoc->Model->IsSelectedItem( parcode ))
			formatdialog.SelectedItems.push_back( i );
	}
	formatdialog.FormatType = pDoc->Model->GetFormatType();
	if( formatdialog.DoModal() == IDOK ){
		vector<unit_code> buffers;
		for( unsigned int i = 0; i < formatdialog.SelectedItems.size(); i++ ){
			unit_code parcode = pDoc->Model->GetChartBuffer()[formatdialog.SelectedItems[i]].get_code();
			buffers.push_back( parcode );
		} 
		pDoc->Model->PreSaveData( buffers, formatdialog.FormatType );
		CString title = "Save data of the simulation";
		CString fileFilter = "Data Files (*.dat)|*.dat|All Files (*.*)|*.*||";
		CFileDialog saveDialog( FALSE, "dat", "results", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,	( LPCTSTR )fileFilter );
		saveDialog.m_ofn.lpstrTitle = title;
		if( saveDialog.DoModal() == IDOK ){
			BeginWaitCursor();
			string info = "Saving the data to ";
			info +=  ( LPCSTR )saveDialog.GetPathName();
			info += ". Please wait... ";
			(( CMainFrame* )AfxGetMainWnd())->StatusBarMessage( info.c_str() );
			pDoc->Model->SaveData( ( LPCSTR )saveDialog.GetPathName(), hhn_pair<int>( int( formatdialog.Wnd1), int( formatdialog.Wnd2) ) ); /*insert window*/
			(( CMainFrame* )AfxGetMainWnd())->StatusBarMessage( "Ready" );
			EndWaitCursor();
		}
		pDoc->SetModifiedFlag();
	}
}

void CMainFrame::OnConnectionSetup() 
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ) return;
	CConnectionSetup dlg("Connection setup");
	dlg.CM = pDoc->Model->Network.nnconnect();
	dlg.pNetwork = &pDoc->Model->Network;
	dlg.pNetwork->init( pDoc->Model->SimData.Seed );
	if (dlg.DoModal() == IDOK){
		pDoc->Model->Network.nnconnect() = dlg.CM;
		pDoc->SetModifiedFlag();
	}	
}

void CMainFrame::OnSaveView() 
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ) return;
	neurosim_view *pView = ( neurosim_view *)MDIGetActive()->GetActiveView();
	if( pView ){
		CString str; MDIGetActive()->GetWindowText( str );
		CString name = pDoc->Model->SimData.ModelName.c_str();
		CString fname = name.Left( name.ReverseFind( '.' ));
		fname += "_";
		fname += str;
		fname += ".bmp";
		CRect rect;
		pView->GetClientRect( &rect );
		CImageSize dlg;
		dlg.Width = rect.Width();
		dlg.Height = rect.Height();
		if( dlg.DoModal() == IDOK ){
			CSize size( dlg.Width, dlg.Height );
			BeginWaitCursor();
			pView->SaveImage( LPCTSTR( fname ), size );
			EndWaitCursor();
		}
	}
}

void CMainFrame::OnUpdateNetworkView(CCmdUI* pCmdUI) 
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ){ pCmdUI->Enable( false ); return; }
	bool enable = false;
	CView *pView = MDIGetActive()->GetActiveView();
	if( pView->IsKindOf( RUNTIME_CLASS( CChartView )) )
		enable = true;
	enable &= pDoc->SimData->IsSimComplete;
	pCmdUI->Enable( enable );
}

void CMainFrame::OnUpdateModelStop(CCmdUI* pCmdUI) 
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ){ pCmdUI->Enable( false ); return; }
	pCmdUI->Enable( !pDoc->SimData->IsSimComplete );
}

void CMainFrame::OnUpdateModelPause(CCmdUI* pCmdUI) 
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ){ pCmdUI->Enable( false ); return; }
	pCmdUI->SetCheck( pDoc->SimData->IsSimPaused );
	if( !pDoc->SimData->IsSimComplete && pDoc->SimData->IsSimPaused )
		(( CMainFrame* )AfxGetMainWnd())->StatusBarMessage( "Simulation is paused" );
	else if( !pDoc->SimData->IsSimComplete )
		(( CMainFrame* )AfxGetMainWnd())->StatusBarMessage( pDoc->SimData->Promt.c_str() );
	else
		(( CMainFrame* )AfxGetMainWnd())->StatusBarMessage( "Ready" );
}

void CMainFrame::OnUpdateMenuItem(CCmdUI* pCmdUI)
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ){ pCmdUI->Enable( false ); return; }
   	pCmdUI->Enable( pDoc->SimData->IsSimComplete );
}

void CMainFrame::StatusBarMessage( const char *info )
{
	GetDescendantWindow(AFX_IDW_STATUS_BAR);
	m_sStatusBarString = info;
	SetMessageText((LPCTSTR)m_sStatusBarString);
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	neurosim_doc *pDoc = (neurosim_doc *)GetActiveDoc();
	if( pDoc && TimerHandle ){
		if( !pDoc->SimData->IsSimPaused ){
			CurrStep += unsigned long( pDoc->SimData->UpdatingTime/pDoc->Model->GetSimStep());
			if( CurrStep >= pDoc->Model->GetNumSteps() ){
				CurrStep = pDoc->Model->GetNumSteps();
				PostMessage( WM_STOP_MAIN_WND_TIMER, 0, 0 );
			}
			PostMessage( WM_UPDATE_MAIN_WND, false, CurrStep );
		}
	}
	CMDIFrameWnd::OnTimer(nIDEvent);
}

LRESULT CMainFrame::OnStopTimer( WPARAM hint, LPARAM counter )
{
	if( TimerHandle ){
		KillTimer( TimerHandle );
		PostMessage( WM_UPDATE_MAIN_WND, true, CurrStep );
		TimerHandle = 0;
		return 1;
	}
	return 0;
}

LRESULT CMainFrame::OnUpdateResults( WPARAM hint, LPARAM counter )
{
	neurosim_doc *pDoc = (neurosim_doc *)GetActiveDoc();
	if( pDoc ){
		pDoc->UpdateViews( hint != FALSE, counter );
		return 1;
	}
	return 0;
}

LRESULT CMainFrame::OnSetMessageString(WPARAM wParam, LPARAM lParam)
{
	UINT nIDLast = m_nIDLastMessage;
	m_nFlags &= ~WF_NOPOPMSG;
	CWnd* pMessageBar = GetMessageBar();
	if( pMessageBar != NULL ){
		CString sMsg;
		CString strMessage;
		// set the message bar text
		if( lParam != 0 ){
			ASSERT( wParam == 0 );    // can't have both an ID and a string
			m_sStatusBarString = (LPCTSTR)lParam;
			sMsg = m_sStatusBarString;
		}
		else if( wParam != 0 ){
			// map SC_CLOSE to PREVIEW_CLOSE when in print preview mode
			if( wParam == AFX_IDS_SCCLOSE && m_lpfnCloseProc != NULL ){
				wParam = AFX_IDS_PREVIEW_CLOSE;
			}
			// get message associated with the ID indicated by wParam
			if( wParam == AFX_IDS_IDLEMESSAGE ){
				sMsg = m_sStatusBarString;
			}
			else{
				GetMessageString(wParam, strMessage);
				sMsg = strMessage;
			}
		}
		pMessageBar->SetWindowText(sMsg);
		// update owner of the bar in terms of last message selected
		CFrameWnd* pFrameWnd = pMessageBar->GetParentFrame();
		if( pFrameWnd != NULL ){
			m_nIDLastMessage = (UINT)wParam;
			m_nIDTracking = (UINT)wParam;
		}
	}
	m_nIDLastMessage = (UINT)wParam;    	// new ID (or 0)
	m_nIDTracking = (UINT)wParam;       	// so F1 on toolbar buttons work
	return nIDLast;
}

#ifdef __MECHANICS__
bool CMainFrame::IsLimbWnd( void )
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ) return false;
	for( POSITION pos = pDoc->GetFirstViewPosition(); pos; ){
		if( pDoc->GetNextView( pos )->IsKindOf( RUNTIME_CLASS( CLimbView ))) return true;
	}
	return false;
}

bool CMainFrame::OpenLimbWindow( void )
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ) return false;
	CWalkerFrameView view;
	view.set_xrange( pDoc->SimData->BeginView, pDoc->SimData->EndView );
	pDoc->pushView( &view );
	createFrameView( &view );
	pDoc->SetModifiedFlag();
	pDoc->Model->InitViews();
	pDoc->ShowResults();
	return true;
}

bool CMainFrame::CloseLimbWindow( void )
{
	for( CMDIChildWnd *MDIChild = MDIGetActive(); MDIChild; MDIChild = ( CMDIChildWnd * )MDIChild->GetNextWindow()){
		if( MDIChild->GetActiveView()->IsKindOf( RUNTIME_CLASS( CLimbView ))){
			MDIChild->MDIDestroy();
			return true;
		}
	}
	return false;
}

void CMainFrame::OnIsStick() 
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ) return;
	pDoc->SimData->IsStickDiagram = !pDoc->SimData->IsStickDiagram;
	pDoc->ShowResults();
}

void CMainFrame::OnNewlimbwindow() 
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ) return;
	if( !IsLimbWnd() ){
		CWalkerFrameView view;
		view.set_xrange( pDoc->SimData->BeginView, pDoc->SimData->EndView );
		pDoc->pushView( &view );
		createFrameView( &view );
	}
	else{
		CloseLimbWindow();
	}
	pDoc->Model->InitViews();
	pDoc->ShowResults();
}

void CMainFrame::OnBiomechanics()
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ) return;
	CBiomechSetup dlg;
	pDoc->Model->upd_outlist();
	dlg.BiomechT = pDoc->Model->BiomechT;
	if( dlg.DoModal() == IDOK ){
		pDoc->Model->BiomechT = dlg.BiomechT;
		pDoc->ClearAllViews();
		pDoc->SetModifiedFlag();
	}
}

void CMainFrame::OnUpdateNewlimbwindow(CCmdUI* pCmdUI) 
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ){ pCmdUI->Enable( false ); return; }
	pCmdUI->Enable( pDoc->SimData->IsSimComplete );
	pCmdUI->SetCheck( IsLimbWnd() );
}

void CMainFrame::OnUpdateIsStick(CCmdUI* pCmdUI) 
{
	neurosim_doc *pDoc = ( neurosim_doc *)GetActiveDoc(); if( !pDoc ){ pCmdUI->Enable( false ); return; }
	pCmdUI->SetCheck( pDoc->SimData->IsStickDiagram );
	pCmdUI->Enable( pDoc->SimData->IsSimComplete );
}

#endif // __MECHANICS__
#endif // __CONSOLE__
