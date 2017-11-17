/////////////////////////////////////////////////////////////////////////////
// SimulateSetup.cpp : implementation file
#include "precompile.h"
#include "resource.h"
#include "SimulateSetup.h"

#ifndef __CONSOLE__

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSimParamSetup property page
IMPLEMENT_DYNCREATE(CSimParamSetup, CPropertyPage)

BEGIN_MESSAGE_MAP(CSimParamSetup, CPropertyPage)
	ON_EN_UPDATE(IDC_EDIT_SIM_TIME, OnUpdateEditSimTime)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SIM_TIME, OnDeltaposSpinSimTime)
	ON_EN_KILLFOCUS(IDC_EDIT_SIM_TIME, OnKillfocusEditSimTime)
	ON_EN_UPDATE(IDC_EDIT_SIM_NN_STEP, OnUpdateEditSimNnStep)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SIM_NN_STEP, OnDeltaposSpinSimNnStep)
	ON_EN_KILLFOCUS(IDC_EDIT_SIM_NN_STEP, OnKillfocusEditSimNnStep)
	ON_EN_UPDATE(IDC_EDIT_TIME_FACTOR, OnUpdateEditTimeFactor)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TIME_FACTOR, OnDeltaposSpinTimeFactor)
	ON_EN_KILLFOCUS(IDC_EDIT_TIME_FACTOR, OnKillfocusEditTimeFactor)
END_MESSAGE_MAP()

CSimParamSetup::CSimParamSetup()
	: CPropertyPage(CSimParamSetup::IDD)
	, NumThreads(_T(""))
{
	SimTime = 0.0;
	SimNNStep = 0.0;
	TimeFactor = 0.0;
}

CSimParamSetup::~CSimParamSetup()
{
}

void CSimParamSetup::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_TIME_FACTOR, Spin_TimeFactor);
	DDX_Control(pDX, IDC_EDIT_TIME_FACTOR, Edit_TimeFactor);
	DDX_Control(pDX, IDC_SPIN_SIM_TIME, Spin_SimTime);
	DDX_Control(pDX, IDC_EDIT_SIM_TIME, Edit_SimTime);
	DDX_Control(pDX, IDC_SPIN_SIM_NN_STEP, Spin_SimNNStep);
	DDX_Control(pDX, IDC_EDIT_SIM_NN_STEP, Edit_SimNNStep);
	DDX_Text(pDX, IDC_EDIT_SIM_NN_STEP, SimNNStep);
	DDX_Text(pDX, IDC_EDIT_SIM_TIME, SimTime);
	DDX_Text(pDX, IDC_EDIT_TIME_FACTOR, TimeFactor);
	DDX_Control(pDX, IDC_NUM_THREADS, NumThreadsCombo);
	DDX_CBString(pDX, IDC_NUM_THREADS, NumThreads);
}

/////////////////////////////////////////////////////////////////////////////
// CSimParamSetup message handlers
BOOL CSimParamSetup::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	Spin_SimNNStep.SetBuddy(&Edit_SimNNStep);
	Spin_SimNNStep.SetRange(0,UD_MAXVAL);
	Spin_SimTime.SetBuddy(&Edit_SimTime);
	Spin_SimTime.SetRange(0,UD_MAXVAL);
	Spin_TimeFactor.SetBuddy(&Edit_TimeFactor);
	Spin_TimeFactor.SetRange(0,UD_MAXVAL);
#ifdef __OMP__
	int numThreads = omp_get_num_procs();
	if( numThreads > 1 ){
		CString num = "1";
		NumThreadsCombo.AddString( num );
		for( int i = 2; i < numThreads; i += 2 ){
			num.Format( "%d", i );
			NumThreadsCombo.AddString( num );
		}
	}
#endif
	NumThreadsCombo.SetCurSel( NumThreadsCombo.GetCount()-1 );
	return TRUE;
}

BOOL CSimParamSetup::OnSetActive() 
{
	CString nt = ""; nt.Format( "%d", pData->NumThreads );
	SimNNStep = pData->SimStep;
	SimTime = pData->SimTime;
	TimeFactor = pData->TimeFactor;
	NumThreadsCombo.SetCurSel( NumThreadsCombo.GetCount()-1 );
	int i = NumThreadsCombo.FindStringExact( 0, nt );
	if( i != CB_ERR ){
		NumThreadsCombo.SetCurSel( i );
	}
	UpdateData( FALSE );
	return CPropertyPage::OnSetActive();
}

BOOL CSimParamSetup::OnKillActive() 
{
	UpdateData( TRUE );
	pData->SimStep = SimNNStep;
	pData->SimTime = SimTime;
	pData->SimStep = SimNNStep;
	pData->TimeFactor = TimeFactor;
	if( NumThreads == "Auto" ){
		pData->NumThreads = 0;
	}
	else{
		pData->NumThreads = atoi( NumThreads.GetString() );
	}
	return CPropertyPage::OnKillActive();
}

void CSimParamSetup::OnUpdateEditSimNnStep() 
{
    CString str;
	Edit_SimNNStep.GetWindowText( str );
    SimNNStep = atof( LPCTSTR( str ));
}

void CSimParamSetup::OnKillfocusEditSimNnStep() 
{
    CString str;
	if(SimNNStep<0.001){
		SimNNStep = 0.001;
		str.Format( "%lg", SimNNStep );
		Edit_SimNNStep.SetWindowText( LPCTSTR( str ));
	}
}

void CSimParamSetup::OnDeltaposSpinSimNnStep(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    CString str;
    SimNNStep += pNMUpDown->iDelta*0.1;
	if( SimNNStep < 0.1 )
		SimNNStep = 0.1;
    str.Format( "%lg", SimNNStep );
    Edit_SimNNStep.SetWindowText( LPCTSTR( str ));
	*pResult = 0;
}

void CSimParamSetup::OnUpdateEditSimTime() 
{
    CString str;
	Edit_SimTime.GetWindowText( str );
    SimTime = atof( LPCTSTR( str ));
}

void CSimParamSetup::OnKillfocusEditSimTime() 
{
    CString str;
	if(SimTime < 0.){
		SimTime = 0.;
		str.Format( "%lg", SimTime );
		Edit_SimTime.SetWindowText( LPCTSTR( str ));
	}
}

void CSimParamSetup::OnDeltaposSpinSimTime(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    SimTime += pNMUpDown->iDelta*1000.;
	if( SimTime < 0.)
		SimTime = 0.;
    CString str;
    str.Format( "%lg", SimTime );
    Edit_SimTime.SetWindowText( LPCTSTR( str ));
	*pResult = 0;
}

void CSimParamSetup::OnUpdateEditTimeFactor() 
{
    CString str;
	Edit_TimeFactor.GetWindowText( str );
    TimeFactor = atof( LPCTSTR( str ));
}

void CSimParamSetup::OnKillfocusEditTimeFactor() 
{
    CString str;
	if(TimeFactor < 0.01){
		TimeFactor = 0.01;
		str.Format( "%lg", TimeFactor );
		Edit_TimeFactor.SetWindowText( LPCTSTR( str ));
	}
}

void CSimParamSetup::OnDeltaposSpinTimeFactor(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    TimeFactor += pNMUpDown->iDelta*0.1;
	if( TimeFactor < 0.01)
		TimeFactor = 0.01;
    CString str;
    str.Format( "%lg", TimeFactor );
    Edit_TimeFactor.SetWindowText( LPCTSTR( str ));
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CSimViewSetup property page
IMPLEMENT_DYNCREATE(CSimViewSetup, CPropertyPage)

CSimViewSetup::CSimViewSetup()
	: CPropertyPage(CSimViewSetup::IDD)
{
	BeginView = 0.0;
	EndView = 0.0;
	UpdateTime = 0;
	All_View = FALSE;
	HistBin = 0.0;
	HistNorm = 0.0;
}

CSimViewSetup::~CSimViewSetup( void )
{
}

void CSimViewSetup::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_HIST_NORM, Spin_HistNorm);
	DDX_Control(pDX, IDC_EDIT_HIST_NORM, Edit_HistNorm);
	DDX_Control(pDX, IDC_SPIN_HIST_BIN, Spin_HistBin);
	DDX_Control(pDX, IDC_EDIT_HIST_BIN, Edit_HistBin);
	DDX_Control(pDX, IDC_CHECK_ALL_VIEWS, Check_All_View);
	DDX_Control(pDX, IDC_SPIN_UPDATE_TIME, Spin_UpdateTime);
	DDX_Control(pDX, IDC_SPIN_END_VIEW, Spin_EndView);
	DDX_Control(pDX, IDC_EDIT_END_VIEW, Edit_EndView);
	DDX_Control(pDX, IDC_SPIN_BEGIN_VIEW, Spin_BeginView);
	DDX_Control(pDX, IDC_EDIT_BEGIN_VIEW, Edit_BeginView);
	DDX_Text(pDX, IDC_EDIT_BEGIN_VIEW, BeginView);
	DDX_Text(pDX, IDC_EDIT_END_VIEW, EndView);
	DDX_Text(pDX, IDC_EDIT_UPDATE_TIME, UpdateTime);
	DDV_MinMaxLong(pDX, UpdateTime, 0, 1000000);
	DDX_Check(pDX, IDC_CHECK_ALL_VIEWS, All_View);
	DDX_Text(pDX, IDC_EDIT_HIST_BIN, HistBin);
	DDX_Text(pDX, IDC_EDIT_HIST_NORM, HistNorm);
}


BEGIN_MESSAGE_MAP(CSimViewSetup, CPropertyPage)
	ON_EN_UPDATE(IDC_EDIT_BEGIN_VIEW, OnUpdateEditBeginView)
	ON_EN_UPDATE(IDC_EDIT_END_VIEW, OnUpdateEditEndView)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BEGIN_VIEW, OnDeltaposSpinBeginView)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_END_VIEW, OnDeltaposSpinEndView)
	ON_EN_KILLFOCUS(IDC_EDIT_BEGIN_VIEW, OnKillfocusEditBeginView)
	ON_EN_KILLFOCUS(IDC_EDIT_END_VIEW, OnKillfocusEditEndView)
	ON_EN_KILLFOCUS(IDC_EDIT_HIST_BIN, OnKillfocusEditHistBin)
	ON_EN_UPDATE(IDC_EDIT_HIST_BIN, OnUpdateEditHistBin)
	ON_EN_KILLFOCUS(IDC_EDIT_HIST_NORM, OnKillfocusEditHistNorm)
	ON_EN_UPDATE(IDC_EDIT_HIST_NORM, OnUpdateEditHistNorm)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HIST_BIN, OnDeltaposSpinHistBin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HIST_NORM, OnDeltaposSpinHistNorm)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimViewSetup message handlers

BOOL CSimViewSetup::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	Spin_BeginView.SetBuddy(&Edit_BeginView);
	Spin_BeginView.SetRange(0,UD_MAXVAL);
	Spin_EndView.SetBuddy(&Edit_EndView);
	Spin_EndView.SetRange(0,UD_MAXVAL);
	Spin_UpdateTime.SetRange(0,UD_MAXVAL);
	Spin_HistBin.SetBuddy(&Edit_HistBin);
	Spin_HistBin.SetRange(0,UD_MAXVAL);
	Spin_HistNorm.SetBuddy(&Edit_HistNorm);
	Spin_HistNorm.SetRange(0,UD_MAXVAL);
	All_View = TRUE;
	return TRUE;
}

BOOL CSimViewSetup::OnSetActive() 
{
	BeginView = pData->BeginView;
	EndView = pData->EndView;
	UpdateTime = pData->UpdatingTime;
	HistBin = pData->HistBin;
	HistNorm = pData->HistNorm;
	UpdateData( FALSE );
	return CPropertyPage::OnSetActive();
}

BOOL CSimViewSetup::OnKillActive() 
{
	UpdateData( TRUE );
	pData->BeginView = BeginView;
	pData->EndView = EndView;
	pData->UpdatingTime = UpdateTime;
	pData->HistBin = HistBin;
	pData->HistNorm = HistNorm;
	return CPropertyPage::OnKillActive();
}

void CSimViewSetup::OnUpdateEditBeginView() 
{
    CString str;
	Edit_BeginView.GetWindowText( str );
    BeginView = atof( LPCTSTR( str ));
}

void CSimViewSetup::OnKillfocusEditBeginView() 
{
    CString str;
	if(BeginView < 0.0){
		BeginView = 0.0;
		str.Format( "%lg", BeginView );
		Edit_BeginView.SetWindowText( LPCTSTR( str ));
	}
	if(EndView < BeginView){
		EndView = BeginView;
		str.Format( "%lg", EndView );
		Edit_EndView.SetWindowText( LPCTSTR( str ));
	}
}

void CSimViewSetup::OnDeltaposSpinBeginView(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    BeginView += pNMUpDown->iDelta*100.;
	if(BeginView < 0.0)	BeginView = 0.0;
    CString str;
    str.Format( "%lg", BeginView );
    Edit_BeginView.SetWindowText( LPCTSTR( str ));
	if(EndView < BeginView){
		EndView = BeginView;
		str.Format( "%lg", EndView );
		Edit_EndView.SetWindowText( LPCTSTR( str ));
	}
	*pResult = 0;
}

void CSimViewSetup::OnUpdateEditEndView() 
{
    CString str;
	Edit_EndView.GetWindowText( str );
    EndView = atof( LPCTSTR( str ));
}

void CSimViewSetup::OnKillfocusEditEndView() 
{
    CString str;
	if(EndView < BeginView){
		EndView = BeginView;
		str.Format( "%lg", EndView );
		Edit_EndView.SetWindowText(( LPCTSTR ) str );
	}
}

void CSimViewSetup::OnDeltaposSpinEndView(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    EndView += pNMUpDown->iDelta*100.;
	if(EndView < BeginView)	EndView = BeginView;
    CString str;
    str.Format( "%lg", EndView );
    Edit_EndView.SetWindowText(( LPCTSTR ) str );
	*pResult = 0;
}

void CSimViewSetup::OnUpdateEditHistBin() 
{
    CString str;
	Edit_HistBin.GetWindowText( str );
    HistBin = atof( LPCTSTR( str ));
}

void CSimViewSetup::OnKillfocusEditHistBin() 
{
    CString str;
	if(HistBin < 0.0){
		HistBin = 0.0;
		str.Format( "%lg", HistBin );
		Edit_HistBin.SetWindowText( LPCTSTR( str ));
	}
}

void CSimViewSetup::OnDeltaposSpinHistBin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    HistBin += pNMUpDown->iDelta*1.;
	if(HistBin < 0.0)	HistBin = 0.0;
    CString str;
    str.Format( "%lg", HistBin );
    Edit_HistBin.SetWindowText( LPCTSTR( str ));
	*pResult = 0;
}

void CSimViewSetup::OnUpdateEditHistNorm() 
{
    CString str;
	Edit_HistNorm.GetWindowText( str );
    HistNorm = atof( LPCTSTR( str ));
}

void CSimViewSetup::OnKillfocusEditHistNorm() 
{
    CString str;
	if( HistNorm < 0.0 ){
		HistNorm = 0.0;
		str.Format( "%lg", HistNorm );
		Edit_HistNorm.SetWindowText( LPCTSTR( str ));
	}
}

void CSimViewSetup::OnDeltaposSpinHistNorm(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    HistNorm += pNMUpDown->iDelta*1.;
	if( HistNorm < 0.0 ) HistNorm = 0.0;
    CString str;
    str.Format( "%lg", HistNorm );
    Edit_HistNorm.SetWindowText( LPCTSTR( str ));
	*pResult = 0;
}


#ifdef __MECHANICS__
/////////////////////////////////////////////////////////////////////////////
// CSimLimbSetup property page
IMPLEMENT_DYNCREATE(CSimLimbSetup, CPropertyPage)

CSimLimbSetup::CSimLimbSetup()
	: CPropertyPage(CSimLimbSetup::IDD)
{
	LimbSkip = 0;
	LimbScale = 0;
    LimbOriginX = 0.;
    LimbOriginY = 0.;
}

CSimLimbSetup::~CSimLimbSetup()
{
}

void CSimLimbSetup::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SPIN_LIMB_SCALE, Spin_LimbScale);
    DDX_Control(pDX, IDC_EDIT_LIMB_SCALE, Edit_LimbScale);
    DDX_Control(pDX, IDC_SPIN_SIM_LIMB_SKIP, Spin_LimbSkip);
    DDX_Text(pDX, IDC_EDIT_SIM_LIMB_SKIP, LimbSkip);
    DDV_MinMaxLong(pDX, LimbSkip, 0, 1000000);
    DDX_Text(pDX, IDC_EDIT_LIMB_SCALE, LimbScale);
    DDX_Control(pDX, IDC_SPIN_LIMB_ORIGIN_X, Spin_LimbOriginX);
    DDX_Control(pDX, IDC_SPIN_LIMB_ORIGIN_Y, Spin_LimbOriginY);
    DDX_Control(pDX, IDC_EDIT_LIMB_ORIGIN_X, Edit_LimbOriginX);
    DDX_Control(pDX, IDC_EDIT_LIMB_ORIGIN_Y, Edit_LimbOriginY);
    DDX_Text(pDX, IDC_EDIT_LIMB_ORIGIN_X, LimbOriginX);
    DDX_Text(pDX, IDC_EDIT_LIMB_ORIGIN_Y, LimbOriginY);
}


BEGIN_MESSAGE_MAP(CSimLimbSetup, CPropertyPage)
    ON_EN_UPDATE(IDC_EDIT_LIMB_ORIGIN_X, OnUpdateEditLimbOriginX)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LIMB_ORIGIN_X, OnDeltaposSpinLimbOriginX)
    ON_EN_UPDATE(IDC_EDIT_LIMB_ORIGIN_Y, OnUpdateEditLimbOriginY)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LIMB_ORIGIN_Y, OnDeltaposSpinLimbOriginY)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimLimbSetup message handlers
BOOL CSimLimbSetup::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	Spin_LimbSkip.SetRange(0,UD_MAXVAL);
	Spin_LimbScale.SetRange(0,UD_MAXVAL);
    Spin_LimbOriginX.SetBuddy(&Edit_LimbOriginX);
    Spin_LimbOriginX.SetRange(UD_MINVAL,UD_MAXVAL);
    Spin_LimbOriginY.SetBuddy(&Edit_LimbOriginY);
    Spin_LimbOriginY.SetRange(UD_MINVAL,UD_MAXVAL);
	return TRUE;
}

BOOL CSimLimbSetup::OnSetActive() 
{
	LimbSkip = pData->LimbSkip;
	LimbScale = pData->LimbScale;
    LimbOriginX = pData->LimbOriginX;
    LimbOriginY = pData->LimbOriginY;
	UpdateData( FALSE );
	return CPropertyPage::OnSetActive();
}

BOOL CSimLimbSetup::OnKillActive() 
{
	UpdateData( TRUE );
	pData->LimbSkip = LimbSkip;
	pData->LimbScale = LimbScale;
    pData->LimbOriginX = LimbOriginX;
    pData->LimbOriginY = LimbOriginY;
	return CPropertyPage::OnKillActive();
}

void CSimLimbSetup::OnUpdateEditLimbOriginX()
{
    CString strLimbOriginX; 
	Edit_LimbOriginX.GetWindowText( strLimbOriginX );
    LimbOriginX = atof( LPCTSTR( strLimbOriginX ));
}

void CSimLimbSetup::OnDeltaposSpinLimbOriginX(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    if(( LimbOriginX += pNMUpDown->iDelta*0.1 ) < 0 )
         LimbOriginX = 0.0;
    else if( LimbOriginX > 2.0 )
         LimbOriginX = 2.0;
    CString strLimbOriginX;
    strLimbOriginX.Format( "%g", LimbOriginX );
    Edit_LimbOriginX.SetWindowText(( LPCTSTR ) strLimbOriginX );
    *pResult = 0;
}

void CSimLimbSetup::OnUpdateEditLimbOriginY()
{
    CString strLimbOriginY; 
	Edit_LimbOriginY.GetWindowText( strLimbOriginY );
    LimbOriginY = atof( LPCTSTR( strLimbOriginY ));
}

void CSimLimbSetup::OnDeltaposSpinLimbOriginY(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    if(( LimbOriginY += pNMUpDown->iDelta*0.1 ) < 0 )
         LimbOriginY = 0.0;
    else if( LimbOriginY > 2.0 )
         LimbOriginY = 2.0;
    CString strLimbOriginY;
    strLimbOriginY.Format( "%g", LimbOriginY );
    Edit_LimbOriginY.SetWindowText(( LPCTSTR ) strLimbOriginY );
    *pResult = 0;
}
#endif __MECHANICS__

/////////////////////////////////////////////////////////////////////////////
// CSimulateSetup
IMPLEMENT_DYNAMIC(CSimulateSetup, CPropertySheet)

CSimulateSetup::CSimulateSetup(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	AddPage( &SimParamSetup );
	AddPage( &SimViewSetup );
	SimParamSetup.pData = &Data;
	SimViewSetup.pData = &Data;
#ifdef __MECHANICS__
	AddPage( &SimLimbSetup );
	SimLimbSetup.pData = &Data;
#endif // __MECHANICS__
}

CSimulateSetup::CSimulateSetup(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage( &SimParamSetup );
	AddPage( &SimViewSetup );
	SimParamSetup.pData = &Data;
	SimViewSetup.pData = &Data;
#ifdef __MECHANICS__
	AddPage( &SimLimbSetup );
	SimLimbSetup.pData = &Data;
#endif // __MECHANICS__
}

CSimulateSetup::~CSimulateSetup()
{
}

#endif // __CONSOLE__
