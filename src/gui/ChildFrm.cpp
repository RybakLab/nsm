/////////////////////////////////////////////////////////////////////////////
// ChildFrm.cpp : implementation of the CChildFrame class
#include "precompile.h"

#ifndef __CONSOLE__

#include "ChildFrm.h"
#include "SpcordDoc.h"
#include "SpcordView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics
#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame
IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	ON_WM_MOVE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// construction/destruction
CChildFrame::CChildFrame()
{
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow( CREATESTRUCT& cs )
{
	cs.style |= WS_CLIPSIBLINGS;
	if( !CMDIChildWnd::PreCreateWindow( cs ))
		return FALSE;
	return TRUE;
}

void CChildFrame::setWndPos( CFrameView *view )
{
	if( !view )
		return;
	WINDOWPLACEMENT wp;
	wp.length = sizeof wp;
	if( view->is_resize() ){
		wp.flags = 0;
		wp.showCmd = view->ShowCmd;
		wp.ptMaxPosition.x = -1;
		wp.ptMaxPosition.y = -1;
		wp.ptMinPosition.x = -1;
		wp.ptMinPosition.y = -1;
		wp.rcNormalPosition = view->WindowRect;
		SetWindowPlacement( &wp );
	}
	else{
		if( GetWindowPlacement( &wp )){
			view->ShowCmd = wp.showCmd;
			view->WindowRect = wp.rcNormalPosition;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers
void CChildFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	CMDIChildWnd::OnUpdateFrameTitle( bAddToTitle );
	neurosim_view *pActiveView = ( neurosim_view *)GetActiveView();
	if( pActiveView ){
		char *title = ( char *)pActiveView->getViewName();
		if( !title ){
			SetWindowText( "empty" );
		} else{ SetWindowText( title ); }
		
	}
}

void CChildFrame::OnMove(int x, int y) 
{
	CMDIChildWnd::OnMove( x, y );
	neurosim_view *pActiveView = ( neurosim_view * )GetActiveView();
	if( pActiveView ){
	    pActiveView->changeViewSize();
	}
}

BOOL CChildFrame::OnCreateClient( LPCREATESTRUCT, CCreateContext* pContext )
{
	if( pContext != NULL && pContext->m_pNewViewClass != NULL ){
		neurosim_view *gui_view = ( neurosim_view * )CreateView( pContext, AFX_IDW_PANE_FIRST );
		if( gui_view == NULL )
			return FALSE;
		neurosim_doc *doc = gui_view->GetDocument();
		CFrameView *frame_view = doc->get_view();
		if( frame_view != NULL ){
			char *view_name = ( char *)frame_view->get_name();
			if( !doc->Model->is_unique_name( view_name ))
				view_name = ( char *)doc->Model->find_unique_name( view_name, frame_view->prefix());
			frame_view->set_name( view_name );
			doc->Model->add_view( frame_view );
			frame_view = doc->Model->get_view( view_name );
			gui_view->attachView( frame_view );
			setWndPos( frame_view );
			doc->popView();
		}
	}
	return TRUE;
}

#endif // __CONSOLE__
