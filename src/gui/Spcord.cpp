// Spcord.cpp : Defines the class behaviors for the application.
#include "precompile.h"

#ifndef __CONSOLE__

#include "Spcord.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "SpcordDoc.h"
#include "SpcordView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
class neurosim_about : public CDialog{
	public:
		neurosim_about();
		enum { IDD = IDD_ABOUTBOX };
	protected:
virtual	void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	protected:
};

neurosim_about::neurosim_about() : CDialog(neurosim_about::IDD)
{
}

void neurosim_about::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only neurosim_app object
neurosim_app theApp;

/////////////////////////////////////////////////////////////////////////////
// neurosim_app
/////////////////////////////////////////////////////////////////////////////
// initialization
BOOL neurosim_app::InitInstance( void )
{
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	// We create two doc template objects to orchestrate the creation of two
	// distinct MDI children-hosted views of the document: (1) the check
	// check view and (2) the book view.
	//
	// We register both doc templates with the CWinApp object, by calling
	// AddDocTemplate.  However, if we were to do this and nothing else, then
	// the framework would mistakenly believe that the application supports
	// two document types.  The framework would display a File New dialog
	// that lists two document types, both which would be "Check Book".
	// We avoid this problem by removing the third string from
	// the document template for the check frame/view.  Specifically,
	// the strings for documents IDR_SPCORDTYPE and IDR_SPCORDTYPE5 are,
	// respectively:
	//
	//       "Book\nchecks\nCheck Book\n
	//        Check Book File (*.chb)\n.chb\n
	//        ChkBookFileType\nCheck Book File Type"
	// and
	//       "Check\nchecks\n\n
	//        Check Book File (*.chb)\n.chb\n
	//        ChkBookFileType\nCheck Book File Type"
	//
	// Finding no GetDocString(CDocTemplate::fileNewName) for the
	// second document template, CWinApp concludes that there is only
	// one document type supported by the application (the "Check Book"
	// document type specified in the first document template; and
	// therefore does not represent the user with a File New dialog.
	InitCommonControls();
#if !defined __MECHANICS__
	m_pView = new CMultiDocTemplate(IDR_NSMTYPE,
		RUNTIME_CLASS(neurosim_doc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CChartView));
	AddDocTemplate(m_pView);
#else
	m_pView = new CMultiDocTemplate(IDR_LOCOTYPE,
		RUNTIME_CLASS(neurosim_doc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CChartView));
	AddDocTemplate(m_pView);
	m_pLimb = new CMultiDocTemplate(IDR_LIMBVIEW,
        RUNTIME_CLASS(neurosim_doc),
        RUNTIME_CLASS(CChildFrame),
        RUNTIME_CLASS(CLimbView));
	// create main MDI Frame window
	AddDocTemplate(m_pLimb);
#endif //__MECHANICS__
	CMainFrame* pMainFrame = new CMainFrame;
	if( !pMainFrame->LoadFrame( IDR_MAINFRAME ))
		return FALSE;
	m_pMainWnd = pMainFrame;
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	//Dispatch commands specified on the command line
	//this should not create empty document when application starts,
	//but should still process other commands (like if you drop a file into your
	//program's icon)
	if( !ProcessShellCommand( cmdInfo ))
		return FALSE;
	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	load_wndpl( pMainFrame, "mainframe" );	// load mainfraim window
	pMainFrame->UpdateWindow();
	return TRUE;
}


void neurosim_app::initViews( neurosim_doc *doc )
{
    (( CMainFrame* )m_pMainWnd )->StatusBarMessage( "Initializing views..." );
	for( CFrameView *view = doc->get_view(); view != NULL; view = doc->get_view() ){
		CMDIChildWnd *childWnd = (( CMainFrame* )m_pMainWnd )->MDIGetActive(); ASSERT( childWnd );
		ASSERT( view );	view->create_view( doc, childWnd );
		childWnd->MDIActivate();		
		childWnd->SetActiveView( childWnd->GetActiveView());
	}
//	top childframe has been already added to the main frame during 
//	document initialization so it has to be removed
	for( CWnd *child = (( CMainFrame* )m_pMainWnd )->MDIGetActive(); child != NULL; child = child->GetNextWindow( GW_HWNDNEXT )){
		CString title;
		child->GetWindowText( title );
		if( title == "empty" ){
			child->DestroyWindow();
			break;
		}
	}
}

CDocument *neurosim_app::OpenDocumentFile( LPCTSTR lpszFileName ) 
{
	m_pView->CloseAllDocuments( FALSE );
	BeginWaitCursor();
	string info = "Loading the model ";
	info +=  lpszFileName;
	info += ". Please wait... ";
	(( CMainFrame* )m_pMainWnd )->StatusBarMessage( info.c_str() );
	FILE *fl = fopen( lpszFileName, "r" ); 
	if( fl != NULL ){
		fclose( fl );
	}
	else{
		message( "Cannot open the file", "Warning" );
		lpszFileName = NULL;
	}
	neurosim_doc *doc = ( neurosim_doc *)m_pView->OpenDocumentFile( lpszFileName, TRUE ); ASSERT( doc );
	initViews( doc );
	doc->Model->InitViews();
	doc->ShowResults();
	(( CMainFrame* )m_pMainWnd )->StatusBarMessage( "Ready" );
	EndWaitCursor();
	return doc;
}

/////////////////////////////////////////////////////////////////////////////
// message handlers
BEGIN_MESSAGE_MAP(neurosim_app, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

void neurosim_app::OnFileNew() 
{
	m_pView->CloseAllDocuments( FALSE );
	neurosim_doc *doc = ( neurosim_doc *)m_pView->OpenDocumentFile( NULL ); ASSERT( doc );
	initViews( doc );
    (( CMainFrame* )m_pMainWnd )->StatusBarMessage( "Ready" );
}

void neurosim_app::OnAppAbout()
{
	neurosim_about about;
	about.DoModal();
}

#endif // __CONSOLE__
