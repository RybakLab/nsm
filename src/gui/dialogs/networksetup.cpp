/////////////////////////////////////////////////////////////////////////////
// networksetup.cpp : implementation file
#include "precompile.h"
#include "networksetup.h"
#ifdef __MECHANICS__
#include "biomtemplate.h"
#endif //__MECHANICS__

#ifndef __CONSOLE__

#include "../spcord.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG

/////////////////////////////////////////////////////////////////////////////
// nn_unitSel dialog
nn_unitSel::nn_unitSel(CWnd* pParent /*=NULL*/)
	: CDialog(nn_unitSel::IDD, pParent)
{
	NumUnit = -1;
}

void nn_unitSel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAG_SEL, PopulatSel);
	DDX_CBIndex(pDX, IDC_TAG_SEL, NumUnit);
}

int nn_unitSel::DoModal( CStringArray &unitsList ) 
{
	UnitsList.RemoveAll();
	UnitsList.Append( unitsList );
	return CDialog::DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// nn_unitSel message handlers
BOOL nn_unitSel::OnInitDialog() 
{
	CDialog::OnInitDialog();
    for( int i = PopulatSel.GetCount()-1; i >= 0; i-- )
         PopulatSel.DeleteString(i);
    for( int i = 0; i < UnitsList.GetSize(); i++ )
         PopulatSel.AddString(UnitsList[i]);
	PopulatSel.SetCurSel(0);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CWeightClone dialog
IMPLEMENT_DYNAMIC( CWeightClone, CDialog )

CWeightClone::CWeightClone( CWnd* pParent /*=NULL*/ )
	: CDialog( CWeightClone::IDD, pParent )
	, IsClone( FALSE )
	, SelSrc(_T(""))
	, SelTrg(_T(""))
{
}

CWeightClone::~CWeightClone( void )
{
}

void CWeightClone::ShowContent( void )
{
	if( IsClone ){
		ListAllSrc.EnableWindow( TRUE );
		ListAllTrg.EnableWindow( TRUE );
	}
	else{
		ListAllSrc.EnableWindow( FALSE );
		ListAllTrg.EnableWindow( FALSE );
	}
}

void CWeightClone::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_SRC_CLONE, ListAllSrc );
	DDX_Control( pDX, IDC_TRG_CLONE, ListAllTrg );
	DDX_Check( pDX, IDC_CHECK_CLONE, IsClone );
	DDX_CBString(pDX, IDC_SRC_CLONE, SelSrc);
	DDX_CBString(pDX, IDC_TRG_CLONE, SelTrg);
}

BEGIN_MESSAGE_MAP( CWeightClone, CDialog )
	ON_BN_CLICKED( IDC_CHECK_CLONE, &CWeightClone::OnClone )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWeightClone message handlers
BOOL CWeightClone::OnInitDialog( void )
{
	CDialog::OnInitDialog();

	size_t trg_n = 0, src_n = 0;
	ListAllTrg.AddString( "None" );
	for( size_t i = 0 ; i < TrgNames.size(); ++i ){
		ListAllTrg.AddString( TrgNames[i].c_str());
		if( SelTrg == TrgNames[i].c_str() ){
			trg_n = i+1;
		}
	}
	ListAllSrc.AddString( "None" );
	for( size_t i = 0 ; i < SrcNames.size(); ++i ){
		ListAllSrc.AddString( SrcNames[i].c_str());
		if( SelSrc == SrcNames[i].c_str() ){
			src_n = i+1;
		}
	}
	if( trg_n == 0 || src_n == 0 ){
		trg_n = src_n = 0;
		SelTrg = "None";
		SelSrc = "None";
		IsClone = FALSE;
	}
	ListAllTrg.SetCurSel( trg_n );
	ListAllSrc.SetCurSel( src_n );
	UpdateData( FALSE );
	ShowContent();
	return TRUE;
}

void CWeightClone::OnClone()
{
	UpdateData();
	ShowContent();
}

/////////////////////////////////////////////////////////////////////////////
// CWeightSetup dialog
CString CWeightSetup::SelSrc = "None";
CString CWeightSetup::SelTrg = "None";

CWeightSetup::CWeightSetup(CWnd* pParent /*=NULL*/)
	: CDialog(CWeightSetup::IDD, pParent), IsChained(FALSE)
{
	ExWeight = 0.0;
	ExWeightVar = 0;
	ExSpec = 0;
	Inh1Spec = 0;
	Inh2Spec = 0;
#if defined( __RESPIRATION__ )
	ExWeight2 = 0.0;
	ExWeightVar2 = 0;
	Ex2Spec = 0;
#endif // defined( __RESPIRATION__ )
	InhAWeight = 0.0;
	InhAWeightVar = 0.;
	InhBWeight = 0.0;
	InhBWeightVar = 0.;
	OldIndex = -1;
	Probability = 1.;
	ActiveSynapse = -1;
}

void CWeightSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONNECT_TYPE, ConnectType);
	DDX_Control(pDX, IDC_EDIT_EX_WEIGHT_VAR, EditExWeightVar);
	DDX_Control(pDX, IDC_SPIN_EX_WEIGHT_VAR, SpinExWeightVar);
	DDX_Control(pDX, IDC_SPIN_EX_WEIGHT, SpinExWeight);
	DDX_Control(pDX, IDC_EDIT_EX_WEIGHT, EditExWeight);
	DDX_Text(pDX, IDC_EDIT_EX_WEIGHT, ExWeight);
	DDX_Text(pDX, IDC_EDIT_EX_WEIGHT_VAR, ExWeightVar);
	DDV_MinMaxDouble(pDX, ExWeightVar, 0., 50.);
#if defined( __RESPIRATION__ )
	DDX_Control(pDX, IDC_EDIT_EX_WEIGHT_VAR2, EditExWeightVar2);
	DDX_Control(pDX, IDC_SPIN_EX_WEIGHT_VAR2, SpinExWeightVar2);
	DDX_Control(pDX, IDC_SPIN_EX_WEIGHT2, SpinExWeight2);
	DDX_Control(pDX, IDC_EDIT_EX_WEIGHT2, EditExWeight2);
	DDX_Text(pDX, IDC_EDIT_EX_WEIGHT2, ExWeight2);
	DDX_Text(pDX, IDC_EDIT_EX_WEIGHT_VAR2, ExWeightVar2);
	DDV_MinMaxDouble(pDX, ExWeightVar2, 0, 50);
#endif // defined( __RESPIRATION__ )
	DDX_Control(pDX, IDC_EDIT_INH1_WEIGHT_VAR, EditInhAWeightVar);
	DDX_Control(pDX, IDC_SPIN_INH1_WEIGHT_VAR, SpinInhAWeightVar);
	DDX_Control(pDX, IDC_SPIN_INH1_WEIGHT, SpinInhAWeight);
	DDX_Control(pDX, IDC_EDIT_INH1_WEIGHT, EditInhAWeight);
	DDX_Text(pDX, IDC_EDIT_INH1_WEIGHT, InhAWeight);
	DDX_Text(pDX, IDC_EDIT_INH1_WEIGHT_VAR, InhAWeightVar);
	DDX_Control(pDX, IDC_EDIT_PROBABILITY, EditProbability);
	DDX_Control(pDX, IDC_SPIN_PROBABILITY, SpinProbability);
	DDX_Text(pDX, IDC_EDIT_PROBABILITY, Probability);
	DDV_MinMaxDouble(pDX, Probability, 0., 1.);
	DDV_MinMaxDouble(pDX, InhAWeightVar, 0, 50);
	DDX_Control(pDX, IDC_EDIT_INH2_WEIGHT_VAR, EditInhBWeightVar);
	DDX_Control(pDX, IDC_SPIN_INH2_WEIGHT_VAR, SpinInhBWeightVar);
	DDX_Control(pDX, IDC_SPIN_INH2_WEIGHT, SpinInhBWeight);
	DDX_Control(pDX, IDC_EDIT_INH2_WEIGHT, EditInhBWeight);
	DDX_Text(pDX, IDC_EDIT_INH2_WEIGHT, InhBWeight);
	DDX_Text(pDX, IDC_EDIT_INH2_WEIGHT_VAR, InhBWeightVar);
	DDV_MinMaxDouble(pDX, InhBWeightVar, 0, 50);
	DDX_CBIndex(pDX, IDC_EX_SPEC, ExSpec);
#if defined( __RESPIRATION__ )
	DDX_CBIndex(pDX, IDC_EX2_SPEC, Ex2Spec);
#endif // defined( __RESPIRATION__ )
	DDX_CBIndex(pDX, IDC_INH1_SPEC, Inh1Spec);
	DDX_CBIndex(pDX, IDC_INH2_SPEC, Inh2Spec);
	DDX_Check(pDX, IDC_CHAINED, IsChained);
}

int CWeightSetup::DoModal( const char *title ) 
{
	Title = title;
	return CDialog::DoModal();
} 

BOOL CWeightSetup::DestroyWindow() 
{
    OldIndex = ConnectType.GetCurSel();
    SetWeight();
	return CDialog::DestroyWindow();
}

BEGIN_MESSAGE_MAP(CWeightSetup, CDialog)
	ON_EN_UPDATE(IDC_EDIT_EX_WEIGHT, OnUpdateEditExWeight)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EX_WEIGHT, OnDeltaposSpinExWeight)
	ON_EN_UPDATE(IDC_EDIT_EX_WEIGHT_VAR, OnUpdateEditExWeightVar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EX_WEIGHT_VAR, OnDeltaposSpinExWeightVar)
#if defined( __RESPIRATION__ )
	ON_EN_UPDATE(IDC_EDIT_EX_WEIGHT2, OnUpdateEditExWeight2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EX_WEIGHT2, OnDeltaposSpinExWeight2)
	ON_EN_UPDATE(IDC_EDIT_EX_WEIGHT_VAR2, OnUpdateEditExWeightVar2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EX_WEIGHT_VAR2, OnDeltaposSpinExWeightVar2)
#endif // defined( __RESPIRATION__ )
	ON_EN_UPDATE(IDC_EDIT_INH1_WEIGHT, OnUpdateEditInhAWeight)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_INH1_WEIGHT, OnDeltaposSpinInhAWeight)
	ON_EN_UPDATE(IDC_EDIT_INH1_WEIGHT_VAR, OnUpdateEditInhAWeightVar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_INH1_WEIGHT_VAR, OnDeltaposSpinInhAWeightVar)
	ON_EN_UPDATE(IDC_EDIT_INH2_WEIGHT, OnUpdateEditInhBWeight)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_INH2_WEIGHT, OnDeltaposSpinInhBWeight)
	ON_EN_UPDATE(IDC_EDIT_INH2_WEIGHT_VAR, OnUpdateEditInhBWeightVar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_INH2_WEIGHT_VAR, OnDeltaposSpinInhBWeightVar)
	ON_CBN_SELCHANGE(IDC_CONNECT_TYPE, OnSelchangeConnectType)
	ON_EN_UPDATE(IDC_EDIT_PROBABILITY, OnUpdateEditProbability)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PROBABILITY, OnDeltaposSpinProbability)
	ON_BN_CLICKED(ID_WEIGHT_DEP, &CWeightSetup::OnWeightDep)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWeightSetup message handlers
BOOL CWeightSetup::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowText( Title );
	for( unsigned int i = 0; i < Connect.size(); i++ ){
		if( Connect[i].Type >= 0 && Connect[i].Type < _id_NN_LAST_CONNECT )
			ConnectType.AddString( _ConnectionTypes[Connect[i].Type] );
	}
	SpinExWeight.SetBuddy(&EditExWeight);
	SpinExWeight.SetRange(UD_MINVAL,UD_MAXVAL);
	SpinExWeightVar.SetRange(0,50);
#if defined( __RESPIRATION__ )
	SpinExWeight2.SetBuddy(&EditExWeight2);
	SpinExWeight2.SetRange(UD_MINVAL,UD_MAXVAL);
	SpinExWeightVar2.SetRange(0,50);
#endif // defined( __RESPIRATION__ )
	SpinInhAWeight.SetBuddy(&EditInhAWeight);
	SpinInhAWeight.SetRange(UD_MINVAL,UD_MAXVAL);
	SpinInhAWeightVar.SetRange(0,50);
	SpinInhBWeight.SetBuddy(&EditInhBWeight);
	SpinInhBWeight.SetRange(UD_MINVAL,UD_MAXVAL);
	SpinInhBWeightVar.SetRange(0,50);
	SpinProbability.SetBuddy(&EditProbability);
	SpinProbability.SetRange(0,1);
	ConnectType.EnableWindow( FALSE );
	if( ConnectType.GetCount() >= 1 && !Connect.empty() ){
		ConnectType.EnableWindow( TRUE );
		ConnectType.SetCurSel( ConnectType.GetCount()-1 );
		GetWeight();
	}
	if( !Connect.ChainS.empty() && !Connect.ChainT.empty() ){
		IsChained = TRUE;
	}
	else{
		IsChained = FALSE;
	}
	UpdateData( FALSE );
	return TRUE;
}

void CWeightSetup::OnWeightDep()
{
	CWeightClone dlg;
	dlg.TrgNames = pCM->name_trg();
	dlg.SrcNames = pCM->name_src();
	dlg.SelTrg = CWeightSetup::SelTrg;
	dlg.SelSrc = CWeightSetup::SelSrc;
	dlg.IsClone = FALSE;
	if( !Connect.ChainS.empty() && !Connect.ChainT.empty()){
		dlg.SelTrg = Connect.ChainT.c_str();
		dlg.SelSrc = Connect.ChainS.c_str();
		dlg.IsClone = TRUE;
	}
	if( dlg.DoModal() == IDOK ){
		if( dlg.IsClone && dlg.SelSrc != "None" && dlg.SelTrg != "None" ){
			pCM->chain( make_pair( string(( LPCSTR )dlg.SelTrg ), string(( LPCSTR )dlg.SelSrc )),
						make_pair( string(( LPCSTR )TrgName ), string(( LPCSTR )SrcName )));
			IsChained = TRUE;
			CWeightSetup::SelSrc = dlg.SelSrc;
			CWeightSetup::SelTrg = dlg.SelTrg;
		}
		else{
			pCM->unchain( make_pair( string(( LPCSTR )TrgName ), string(( LPCSTR )SrcName )));
			IsChained = FALSE;
			CWeightSetup::SelSrc = "None";
			CWeightSetup::SelTrg = "None";
		}
		Connect = ( *pCM )(( LPCSTR )TrgName, ( LPCSTR )SrcName );
	    GetWeight();
		Change();
	}
}

void CWeightSetup::OnUpdateEditExWeight() 
{
    CString strWeight;
	EditExWeight.GetWindowText( strWeight );
    if( ExWeight < 0. )
        ExWeight = 0.;
    ExWeight = atof( LPCTSTR( strWeight ));
}

void CWeightSetup::OnDeltaposSpinExWeight(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    ExWeight += pNMUpDown->iDelta*0.1;
    if( ExWeight < 0. )
        ExWeight = 0.;
    CString strWeight;
    strWeight.Format( "%lg", ExWeight );
    EditExWeight.SetWindowText(( LPCTSTR ) strWeight );
	*pResult = 0;
}

void CWeightSetup::OnUpdateEditExWeightVar() 
{
    CString strWeightVar;
	EditExWeightVar.GetWindowText( strWeightVar );
    if( ExWeightVar < 0. )
        ExWeightVar = 0.;
    ExWeightVar = atof( LPCTSTR( strWeightVar ));
}

void CWeightSetup::OnDeltaposSpinExWeightVar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    ExWeightVar += pNMUpDown->iDelta*0.1;
    if( ExWeightVar < 0. )
        ExWeightVar = 0.;
    CString strWeightVar;
    strWeightVar.Format( "%lg", ExWeightVar );
    EditExWeightVar.SetWindowText(( LPCTSTR ) strWeightVar );
	*pResult = 0;
}

#if defined( __RESPIRATION__ )
void CWeightSetup::OnUpdateEditExWeight2() 
{
    CString strWeight2;
	EditExWeight2.GetWindowText( strWeight2 );
    if( ExWeight2 < 0. )
        ExWeight2 = 0.;
    ExWeight2 = atof( LPCTSTR( strWeight2 ));
}

void CWeightSetup::OnDeltaposSpinExWeight2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    ExWeight2 += pNMUpDown->iDelta*0.1;
    if( ExWeight2 < 0. )
        ExWeight2 = 0.;
    CString strWeight2;
    strWeight2.Format( "%lg", ExWeight2 );
    EditExWeight2.SetWindowText(( LPCTSTR ) strWeight2 );
	*pResult = 0;
}

void CWeightSetup::OnUpdateEditExWeightVar2() 
{
    CString strWeightVar2;
	EditExWeightVar2.GetWindowText( strWeightVar2 );
    if( ExWeightVar2 < 0. )
        ExWeightVar2 = 0.;
    ExWeightVar2 = atof( LPCTSTR( strWeightVar2 ));
}

void CWeightSetup::OnDeltaposSpinExWeightVar2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    ExWeightVar2 += pNMUpDown->iDelta*0.1;
    if( ExWeightVar2 < 0. )
        ExWeightVar2 = 0.;
    CString strWeightVar2;
    strWeightVar2.Format( "%lg", ExWeightVar2 );
    EditExWeightVar2.SetWindowText(( LPCTSTR ) strWeightVar2 );
	*pResult = 0;
}
#endif // defined( __RESPIRATION__ )

void CWeightSetup::OnUpdateEditInhAWeight() 
{
    CString strWeight;
	EditInhAWeight.GetWindowText( strWeight );
    if( InhAWeight < 0. )
        InhAWeight = 0.;
    InhAWeight = atof( LPCTSTR( strWeight ));
}

void CWeightSetup::OnDeltaposSpinInhAWeight(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    InhAWeight += pNMUpDown->iDelta*0.1;
    if( InhAWeight < 0. )
        InhAWeight = 0.;
    CString strWeight;
    strWeight.Format( "%lg", InhAWeight );
    EditInhAWeight.SetWindowText(( LPCTSTR ) strWeight );
	*pResult = 0;
}

void CWeightSetup::OnUpdateEditInhAWeightVar() 
{
    CString strWeightVar;
	EditInhAWeightVar.GetWindowText( strWeightVar );
    if( InhAWeightVar < 0. )
        InhAWeightVar = 0.;
    InhAWeightVar = atof( LPCTSTR( strWeightVar ));
}

void CWeightSetup::OnDeltaposSpinInhAWeightVar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    InhAWeightVar += pNMUpDown->iDelta*0.1;
    if( InhAWeightVar < 0. )
        InhAWeightVar = 0.;
    CString strWeightVar;
    strWeightVar.Format( "%lg", InhAWeightVar );
    EditInhAWeightVar.SetWindowText(( LPCTSTR )strWeightVar );
	*pResult = 0;
}

void CWeightSetup::OnUpdateEditInhBWeight() 
{
    CString strWeight;
	EditInhBWeight.GetWindowText( strWeight );
    if( InhBWeight < 0. )
        InhBWeight = 0.;
    InhBWeight = atof( LPCTSTR( strWeight ));
}

void CWeightSetup::OnDeltaposSpinInhBWeight(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    InhBWeight += pNMUpDown->iDelta*0.1;
    if( InhBWeight < 0. )
        InhBWeight = 0.;
    CString strWeight;
    strWeight.Format( "%lg", InhBWeight );
    EditInhBWeight.SetWindowText(( LPCTSTR ) strWeight );
	*pResult = 0;
}

void CWeightSetup::OnUpdateEditInhBWeightVar() 
{
    CString strWeightVar;
	EditInhBWeightVar.GetWindowText( strWeightVar );
    if( InhBWeightVar < 0. )
        InhBWeightVar = 0.;
    InhBWeightVar = atof( LPCTSTR( strWeightVar ));
}

void CWeightSetup::OnDeltaposSpinInhBWeightVar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    InhBWeightVar += pNMUpDown->iDelta*0.1;
    if( InhBWeightVar < 0. )
        InhBWeightVar = 0.;
    CString strWeightVar;
    strWeightVar.Format( "%lg", InhBWeightVar );
    EditInhBWeightVar.SetWindowText(( LPCTSTR ) strWeightVar );
	*pResult = 0;
}

void CWeightSetup::OnUpdateEditProbability() 
{
    CString str;
	EditProbability.GetWindowText( str );
    Probability = atof( LPCTSTR( str ));
}

void CWeightSetup::OnDeltaposSpinProbability(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    Probability += pNMUpDown->iDelta*0.05;
	if(Probability < 0.) Probability = 0.;
	if(Probability > 1.) Probability = 1.;
    CString str;
    str.Format( "%g", Probability );
    EditProbability.SetWindowText(( LPCTSTR ) str );
	*pResult = 0;
}

void CWeightSetup::OnSelchangeConnectType() 
{
    SetWeight();
    GetWeight();
	Change();
}

void CWeightSetup::GetWeight( void )
{
    int index = ConnectType.GetCurSel();
    if( index < 0 )
        return;
    CString name;
    ConnectType.GetLBText( index, name );
    for( int type = 0; type < _id_NN_LAST_CONNECT; type++ )
         if( name == _ConnectionTypes[type] ){
             CConnect connect_type( Connect.get_connect( type ));
             CHhnConnect connect( connect_type.Connect );
             Probability = connect.Probability;
             CStat ex = connect.weight( _id_ExSyn );
             ExWeight = ex.Value;
             ExWeightVar = ex.Variance*100.;
             ExSpec = 0;
             if( connect.Sigma[_id_ExSyn] )
                 ExSpec = 1;
             if( connect.Depression[_id_ExSyn] )
                 ExSpec = 2;
             if( connect.PreInh[_id_ExSyn] )
                 ExSpec = 3;
#if defined( __RESPIRATION__ )
             CStat ex2 = connect.weight( _id_ExBSyn );
             ExWeight2 = ex2.Value;
             ExWeightVar2 = ex2.Variance*100.;
             Ex2Spec = 0;
             if( connect.Sigma[_id_ExBSyn] )
                 Ex2Spec = 1;
             if( connect.Depression[_id_ExBSyn] )
                 Ex2Spec = 2;
             if( connect.PreInh[_id_ExBSyn] )
                 Ex2Spec = 3;
#endif // defined( __RESPIRATION__ )
             CStat inha = connect.weight( _id_InhASyn );
             InhAWeight = inha.Value;
             InhAWeightVar = inha.Variance*100.;
             Inh1Spec = 0;
             if( connect.Sigma[_id_InhASyn] )
                 Inh1Spec = 1;
             if( connect.Depression[_id_InhASyn] )
                 Inh1Spec = 2;
             if( connect.PreInh[_id_InhASyn] )
                 Inh1Spec = 3;
             CStat inhb = connect.weight( _id_InhBSyn );
             InhBWeight = inhb.Value;
             InhBWeightVar = inhb.Variance*100.;
             Inh2Spec = 0;
             if( connect.Sigma[_id_InhBSyn] )
                 Inh2Spec = 1;
             if( connect.Depression[_id_InhBSyn] )
                 Inh2Spec = 2;
             if( connect.PreInh[_id_InhBSyn] )
                 Inh2Spec = 3;
             UpdateData( FALSE );
             break;
             }
    OldIndex = index;
}

void CWeightSetup::SetWeight( void )
{
    if( OldIndex < 0 )
        return;
    CString name;
    ConnectType.GetLBText( OldIndex, name );
    for( int type = 0; type < _id_NN_LAST_CONNECT; type++ )
         if( name == _ConnectionTypes[type] ){
             CHhnConnect weight( Connect.get_connect( type ).Connect );
             weight.Probability = Probability;
             weight.weight( _id_ExSyn ) = CStat( ExWeight, ExWeightVar/100. );
             weight.Sigma[_id_ExSyn] = false;
             weight.Depression[_id_ExSyn] = false;
             weight.PreInh[_id_ExSyn] = false;
             if( ExSpec == 1 )
                 weight.Sigma[_id_ExSyn] = true;
             if( ExSpec == 2 )
                 weight.Depression[_id_ExSyn] = true;
             else if( ExSpec == 3 )
                 weight.PreInh[_id_ExSyn] = true;
#if defined( __RESPIRATION__ )
             weight.weight( _id_ExBSyn ) = CStat( ExWeight2, ExWeightVar2/100. );
             weight.Sigma[_id_ExBSyn] = false;
             weight.Depression[_id_ExBSyn] = false;
             weight.PreInh[_id_ExBSyn] = false;
             if( Ex2Spec == 1 )
                 weight.Sigma[_id_ExBSyn] = true;
             if( Ex2Spec == 2)
                 weight.Depression[_id_ExBSyn] = true;
             else if( Ex2Spec == 3 )
                 weight.PreInh[_id_ExBSyn] = true;
#endif // defined( __RESPIRATION__ )
             weight.weight( _id_InhASyn ) = CStat( InhAWeight, InhAWeightVar/100. );
             weight.Sigma[_id_InhASyn] = false;
             weight.Depression[_id_InhASyn] = false;
             weight.PreInh[_id_InhASyn] = false;
             if( Inh1Spec == 1 )
                 weight.Sigma[_id_InhASyn] = true;
             if( Inh1Spec == 2 )
                 weight.Depression[_id_InhASyn] = true;
             else if( Inh1Spec == 3 )
                 weight.PreInh[_id_InhASyn] = true;
             weight.weight( _id_InhBSyn ) = CStat( InhBWeight, InhBWeightVar/100. );
             weight.Sigma[_id_InhBSyn] = false;
             weight.Depression[_id_InhBSyn] = false;
             weight.PreInh[_id_InhBSyn] = false;
             if( Inh2Spec == 1 )
                 weight.Sigma[_id_InhBSyn] = true;
             if( Inh2Spec == 2 )
                 weight.Depression[_id_InhBSyn] = true;
             else if( Inh2Spec == 3 )
                 weight.PreInh[_id_InhBSyn] = true;
             Connect.add_connect( CConnect( weight, type ), true );
             break;
             }
}

void CWeightSetup::Change( void )
{
	SpinExWeightVar.EnableWindow( FALSE );
	SpinExWeight.EnableWindow( FALSE );
	EditExWeight.EnableWindow( FALSE );
	EditExWeightVar.EnableWindow( FALSE );
#if defined( __RESPIRATION__ )
	SpinExWeightVar2.EnableWindow( FALSE );
	SpinExWeight2.EnableWindow( FALSE );
	EditExWeight2.EnableWindow( FALSE );
    EditExWeightVar2.EnableWindow( FALSE );
#endif // defined( __RESPIRATION__ )
	SpinInhAWeightVar.EnableWindow( FALSE );
	SpinInhAWeight.EnableWindow( FALSE );
	EditInhAWeight.EnableWindow( FALSE );
	EditInhAWeightVar.EnableWindow( FALSE );
	SpinInhBWeightVar.EnableWindow( FALSE );
	SpinInhBWeight.EnableWindow( FALSE );
	EditInhBWeight.EnableWindow( FALSE );
	EditInhBWeightVar.EnableWindow( FALSE );
    if( Connect.empty())
        return;
    switch( ActiveSynapse ){
            case _id_ExSyn:
                 SpinExWeightVar.EnableWindow( TRUE );
                 SpinExWeight.EnableWindow( TRUE );
                 EditExWeight.EnableWindow( TRUE );
                 EditExWeightVar.EnableWindow( TRUE );
                 break;
#if defined( __RESPIRATION__ )
            case _id_ExBSyn:
                 SpinExWeightVar2.EnableWindow( TRUE );
                 SpinExWeight2.EnableWindow( TRUE );
                 EditExWeight2.EnableWindow( TRUE );
                 EditExWeightVar2.EnableWindow( TRUE );
                 break;
#endif // defined( __RESPIRATION__ )
            case _id_InhASyn:
	             SpinInhAWeightVar.EnableWindow( TRUE );
	             SpinInhAWeight.EnableWindow( TRUE );
	             EditInhAWeight.EnableWindow( TRUE );
                 EditInhAWeightVar.EnableWindow( TRUE );
                 break;
            case _id_InhBSyn:
                 SpinInhBWeightVar.EnableWindow( TRUE );
                 SpinInhBWeight.EnableWindow( TRUE );
                 EditInhBWeight.EnableWindow( TRUE );
                 EditInhBWeightVar.EnableWindow( TRUE );
                 break;
            default:
                 SpinExWeightVar.EnableWindow( TRUE );
                 SpinExWeight.EnableWindow( TRUE );
                 EditExWeight.EnableWindow( TRUE );
                 EditExWeightVar.EnableWindow( TRUE );
#if defined( __RESPIRATION__ )
                 SpinExWeightVar2.EnableWindow( TRUE );
                 SpinExWeight2.EnableWindow( TRUE );
                 EditExWeight2.EnableWindow( TRUE );
                 EditExWeightVar2.EnableWindow( TRUE );
#endif // defined( __RESPIRATION__ )
                 SpinInhAWeightVar.EnableWindow( TRUE );
                 SpinInhAWeight.EnableWindow( TRUE );
                 EditInhAWeight.EnableWindow( TRUE );
                 EditInhAWeightVar.EnableWindow( TRUE );
                 SpinInhBWeightVar.EnableWindow( TRUE );
                 SpinInhBWeight.EnableWindow( TRUE );
                 EditInhBWeight.EnableWindow( TRUE );
                 EditInhBWeightVar.EnableWindow( TRUE );
            }
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkParam property page
IMPLEMENT_DYNCREATE(CNetworkParam, CPropertyPage)

CNetworkParam::CNetworkParam(void)
	: CPropertyPage(CNetworkParam::IDD)
{
}

void CNetworkParam::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SYNAPSES_PROP, NetParView );
}

BEGIN_MESSAGE_MAP(CNetworkParam, CPropertyPage)
	ON_NOTIFY( NM_CLICK, IDC_SYNAPSES_PROP, OnGridClick )
    ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnEndCellEdit )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkParam message handlers
void CNetworkParam::OnGridClick( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	NetParView.OnGridClick( pNotifyStruct, pResult );
	if( *pResult & GRID_PROCESS ){ 
		string path;
		NetParView.GetActiveBranch( path );
		OnFlyUpdate( path );
	}
	*pResult = 0;
};

void CNetworkParam::OnEndCellEdit( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	NetParView.OnEndCellEdit( pNotifyStruct, pResult );
	string path;
	if( *pResult & GRID_PROCESS ){ 
		NetParView.GetActiveBranch( path );
		OnFlyUpdate( path );
	}
	else{
		UpdateView( true );
	}
	*pResult = 0;
}

void CNetworkParam::OnFlyUpdate( const string &path )
{
	size_t pos = path.rfind( '\t' );
	if( pos != string::npos ){
		string parent_path = path.substr( 0, pos );
		string var_path = path.substr( pos+1 );
		uni_template *unit = Network->NetParam.get_unit( parent_path.c_str() );
		if( unit ){
			UpdateView( true );
			if( unit->process( var_path.c_str() )){
				DisplayUnit( &( Network->NetParam ), string( Network->NetParam.get_type()), false );
				NetParView.Update( false );
			}
		}
		else{
			OnFlyUpdate( parent_path );
		}
	}
}

void CNetworkParam::UpdateView( bool save )
{
	NetParView.UpdateData( save );
	map<_Grid_Key,_Grid_Element> *all_cells = NetParView.GetMap();
	typedef map<_Grid_Key,_Grid_Element>::iterator it;
	for( it pos = all_cells->begin(); pos != all_cells->end(); ++pos ){
		Network->NetParam.update( pos->first.Path.c_str(), pos->first.Name.c_str(), pos->second.Data, save );
	}
	NetParView.UpdateData( save );
}

bool CNetworkParam::DisplayUnit( uni_template *unit, string &start_path, bool collapse )
{
	if( unit ){
		bool is_success = DisplayUnitPar( unit, start_path );
		return  DisplayUnitChildren( unit, start_path, collapse ) || is_success;
	}
	return false;
}

bool CNetworkParam::DisplayUnitPar( uni_template *unit, string &start_path )
{
	vector<pair<_Grid_Key,_Grid_Element> > par_list;
	unit->get_allpar( start_path.c_str(), par_list );
	unsigned int pos = 0;
	for( pos = 0; pos < par_list.size(); ++pos ){
		if( !NetParView.InsertElement( par_list[pos].first.Path, par_list[pos].first.Name,
										 par_list[pos].second.InitStr, par_list[pos].second.Type )){
			NetParView.SetElement( par_list[pos].first.Path, par_list[pos].first.Name, par_list[pos].second );
		}
	}
	return ( pos > 0? true:false );
}

bool CNetworkParam::DisplayUnitChildren( uni_template *unit, string &start_path, bool collapse )
{
	vector<pair<string,int> > path;
	unit->get_childrenpath( start_path.c_str(), path );
	size_t pos = 0;
	for( pos = 0; pos < path.size(); ++pos ){
		if( !DisplayUnit( Network->NetParam.get_unit( path[pos].first.c_str()), path[pos].first, collapse )){
			NetParView.InsertElement( path[pos].first, "Data", "", path[pos].second );
			NetParView.SetActiveElement( path[pos].first, path[pos].second );
		}
	}
	pair<string,int> unit_prop;
	unit->get_properties( start_path.c_str(), unit_prop );
	NetParView.SetActiveElement( unit_prop.first, unit_prop.second );
	if( unit_prop.second != GRID_NONE  && collapse ){
		NetParView.CollapseAllSubLevels( unit_prop.first );
	}
	return ( pos > 0? true:false );
}

BOOL CNetworkParam::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	NetParView.InitGridCtrl( "Name" );
	DisplayUnit( &( Network->NetParam ), string( Network->NetParam.get_type()));
	NetParView.Update();
	UpdateView( false );
	return TRUE;
}

BOOL CNetworkParam::OnSetActive() 
{
	UpdateView( false );
	return CPropertyPage::OnSetActive();
}

BOOL CNetworkParam::OnKillActive() 
{
	UpdateView( true );
	return CPropertyPage::OnKillActive();
}

/////////////////////////////////////////////////////////////////////////////
// CPopulatSetup property page
IMPLEMENT_DYNCREATE(CPopulatSetup, CPropertyPage)

CPopulatSetup::CPopulatSetup()
	: CPropertyPage(CPopulatSetup::IDD)
{
	NewPopulate = 0;
}

CPopulatSetup::~CPopulatSetup()
{
}

void CPopulatSetup::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NETWORK_SIZE, StringSize);
	DDX_Control(pDX, IDC_ADD_POPULAT, add_pop);
	DDX_Control(pDX, IDC_DEL_POPULAT, DelPopulat);
	DDX_Control(pDX, IDC_PROP_POPULAT, PropPopulat);
	DDX_Control(pDX, IDC_LIST_POPULAT, ListPopulat);
}

BEGIN_MESSAGE_MAP(CPopulatSetup, CPropertyPage)
	ON_BN_CLICKED(IDC_ADD_POPULAT, OnAddPopulat)
	ON_BN_CLICKED(IDC_DEL_POPULAT, OnDelPopulat)
	ON_BN_CLICKED(IDC_PROP_POPULAT, OnPropPolulat)
	ON_LBN_DBLCLK(IDC_LIST_POPULAT, OnDblclkListPopulat)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPopulatSetup message handlers
BOOL CPopulatSetup::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
    NewPopulate = Network->size_pop();
    Change();
	if(Network->size_pop() > 0) ListPopulat.SetCurSel(0);
	return TRUE;
}

void CPopulatSetup::OnAddPopulat() 
{
    CString name;
    name.Format("Population %d",NewPopulate);
	Network->add_pop( hhn_populat( string( LPCTSTR( name ))));
    NewPopulate++;
    Change();
	ListPopulat.SetCurSel( Network->size_pop()-1 );
}

void CPopulatSetup::OnDelPopulat() 
{
    int i = ListPopulat.GetCurSel();
    if( i == -1 ) return;
    Network->del_pop(i);
    Change();
	if( i >= ( int )Network->size_pop()) --i;
	if( i >= 0 ) ListPopulat.SetCurSel(i);
}

void CPopulatSetup::OnPropPolulat() 
{
    int i = ListPopulat.GetCurSel();
    if( i == -1 )
        return;
    hhn_populat populat( Network->get_pop( i ));
// temporary solution ------- begin
	CPopulatProperties dlg;
	dlg.PopulateTemplate.set_parent( &Network->NetParam );
    dlg.PopulateTemplate.CopyFrom( &populat );
/*
    dlg.PopulateTemplate.InNa = Network->get_naions()->In;
    dlg.PopulateTemplate.OutNa = Network->get_naions()->Out;
    dlg.PopulateTemplate.InK = Network->get_kions()->In;
    dlg.PopulateTemplate.OutK = Network->get_kions()->Out;
    dlg.PopulateTemplate.InCl = Network->get_clions()->In;
    dlg.PopulateTemplate.OutCl = Network->get_clions()->Out;
    dlg.PopulateTemplate.RTF = Network->get_naions()->RTF;
*/
	if( dlg.DoModal() == IDOK ){
        populat.set_name(dlg.PopulateTemplate.get_name());
        populat.create(Network,*dlg.PopulateTemplate.get_neuronT(),dlg.PopulateTemplate.GetSize());
        Network->set_pop( populat, i );
        Change();
	}
// temporary solution ------- end
	ListPopulat.SetCurSel( i );
}

void CPopulatSetup::OnDblclkListPopulat() 
{
	OnPropPolulat();
}

void CPopulatSetup::Change( void )
{
    add_pop.EnableWindow(TRUE);
    if( Network->size_pop() == 0 ){
        DelPopulat.EnableWindow(FALSE);
        PropPopulat.EnableWindow(FALSE);
        }
	else{
		DelPopulat.EnableWindow(TRUE);
		PropPopulat.EnableWindow(TRUE);
		}
    CString str;
    str.Format("Number of populations - %d", Network->size_pop());
    StringSize.SetWindowText(str);
    ListPopulat.ResetContent();
    for( unsigned int i = 0; i < Network->size_pop(); i++ )
         ListPopulat.AddString( Network->get_pop(i).get_name().c_str() );
}

BOOL CPopulatSetup::OnSetActive() 
{
    Change();
	return CPropertyPage::OnSetActive();
}

/////////////////////////////////////////////////////////////////////////////
// CDriveName dialog
CDriveName::CDriveName(CWnd* pParent /*=NULL*/)
	: CDialog(CDriveName::IDD, pParent)
{
	DriveName = _T("");
}

void CDriveName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DRIVE_NAME, DriveName);
}

/////////////////////////////////////////////////////////////////////////////
// CDriveSetup property page
IMPLEMENT_DYNCREATE(CDriveSetup, CPropertyPage)

CDriveSetup::CDriveSetup() : CPropertyPage(CDriveSetup::IDD)
{
    NewDrive = 0;
}

void CDriveSetup::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROP_DRIVE, PropDrive);
	DDX_Control(pDX, IDC_DRIVE_SIZE, StringSize);
	DDX_Control(pDX, IDC_ADD_DRIVE, add_drv);
	DDX_Control(pDX, IDC_DEL_DRIVE, DelDrive);
	DDX_Control(pDX, IDC_LIST_DRIVE, ListDrive);
}

BEGIN_MESSAGE_MAP(CDriveSetup, CPropertyPage)
	ON_BN_CLICKED(IDC_ADD_DRIVE, OnAddDrive)
	ON_BN_CLICKED(IDC_DEL_DRIVE, OnDelDrive)
	ON_BN_CLICKED(IDC_PROP_DRIVE, OnPropDrive)
	ON_LBN_DBLCLK(IDC_LIST_DRIVE, OnDblclkListDrive)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDriveSetup message handlers
BOOL CDriveSetup::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
    NewDrive = Network->size_drv();
    Change();
	return TRUE;
}

void CDriveSetup::OnAddDrive() 
{
    CString name;
    name.Format( "Drive %d", NewDrive );	
	Network->add_drv( hhn_drive( ( LPCTSTR )name ));
    NewDrive++;
    Change();
}

void CDriveSetup::OnDelDrive() 
{
    unsigned int i = ListDrive.GetCurSel();
    if( i == -1 )
        return;
    Network->del_drv(i);
    Change();
}

void CDriveSetup::OnPropDrive() 
{
    int i = ListDrive.GetCurSel();
    if( i == -1 )
        return;
	CDriveName dlg;
    hhn_drive drive( Network->get_drv(i) );
    dlg.DriveName = drive.get_name().c_str();
    if( dlg.DoModal() == IDOK ){
        drive.set_name( LPCTSTR( dlg.DriveName ));
        Network->set_drv( drive, i );
        Change();
        } 
}

void CDriveSetup::OnDblclkListDrive() 
{
	OnPropDrive();
}

void CDriveSetup::Change( void )
{
    add_drv.EnableWindow(TRUE);
    if( Network->size_drv() == 0 ){
        DelDrive.EnableWindow(FALSE);
		PropDrive.EnableWindow(FALSE);
        }
	else{
        DelDrive.EnableWindow(TRUE);
		PropDrive.EnableWindow(TRUE);
		}

    CString str;
    str.Format("Number of drives - %d", Network->size_drv());
    StringSize.SetWindowText(str);

    unsigned int index = ListDrive.GetCurSel();
    if( index < 0 ) index = 0;
    if( index >= Network->size_drv() ) index = Network->size_drv()-1;

    ListDrive.ResetContent();
    for( unsigned int i = 0 ; i < Network->size_drv(); i++ ){
         ListDrive.AddString( Network->get_drv(i).get_name().c_str() );
         }
    ListDrive.SetCurSel( index );
}

BOOL CDriveSetup::OnSetActive() 
{
    Change();
	return CPropertyPage::OnSetActive();
}

/////////////////////////////////////////////////////////////////////////////
// COutputProp dialog
COutputProp::COutputProp(CWnd* pParent /*=NULL*/)
	: CDialog(COutputProp::IDD, pParent)
{
	Name = _T("");
	T1 = 0.0;
	T2 = 0.0;
	Threshold = 0.0;
}

void COutputProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_THRESHOLD, Spin_Threshold);
	DDX_Control(pDX, IDC_EDIT_THRESHOLD, Edit_Threshold);
	DDX_Control(pDX, IDC_SPIN_T2, SpinT2);
	DDX_Control(pDX, IDC_SPIN_T1, SpinT1);
	DDX_Control(pDX, IDC_EDIT_T2, EditT2);
	DDX_Control(pDX, IDC_EDIT_T1, EditT1);
	DDX_Text(pDX, IDC_EDIT_OUTPUT_NAME, Name);
	DDX_Text(pDX, IDC_EDIT_T1, T1);
	DDV_MinMaxDouble(pDX, T1, 0.0, 100000.);
	DDX_Text(pDX, IDC_EDIT_T2, T2);
	DDV_MinMaxDouble(pDX, T2, 0.0, 100000.);
	DDX_Text(pDX, IDC_EDIT_THRESHOLD, Threshold);
}

BEGIN_MESSAGE_MAP(COutputProp, CDialog)
	ON_EN_UPDATE(IDC_EDIT_T1, OnUpdateEditT1)
	ON_EN_UPDATE(IDC_EDIT_T2, OnUpdateEditT2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_T1, OnDeltaposSpinT1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_T2, OnDeltaposSpinT2)
	ON_EN_UPDATE(IDC_EDIT_THRESHOLD, OnUpdateEditThreshold)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_THRESHOLD, OnDeltaposSpinThreshold)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputProp message handlers
BOOL COutputProp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SpinT1.SetBuddy(&EditT1);
	SpinT1.SetRange(0,UD_MAXVAL);
	SpinT2.SetBuddy(&EditT2);
	SpinT2.SetRange(0,UD_MAXVAL);
	Spin_Threshold.SetBuddy(&Edit_Threshold);
	Spin_Threshold.SetRange(0,UD_MAXVAL);
	UpdateData();
	return TRUE;
}

void COutputProp::OnUpdateEditT1() 
{
	CString strT1;
	EditT1.GetWindowText( strT1 );
	T1 = atof( LPCTSTR( strT1 ));
}

void COutputProp::OnUpdateEditT2() 
{
	CString strT2;
	EditT2.GetWindowText( strT2 );
	T2 = atof( LPCTSTR( strT2 ));
}

void COutputProp::OnDeltaposSpinT1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	T1 += pNMUpDown->iDelta*1.;
	CString strT1;
	strT1.Format( "%lg", T1 );
	EditT1.SetWindowText(( LPCTSTR ) strT1 );
	*pResult = 0;
}

void COutputProp::OnDeltaposSpinT2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	T2 += pNMUpDown->iDelta*1.;
	CString strT2;
	strT2.Format( "%lg", T2 );
	EditT2.SetWindowText(( LPCTSTR ) strT2 );
	*pResult = 0;
}

void COutputProp::OnUpdateEditThreshold() 
{
	CString str;
	Edit_Threshold.GetWindowText( str );
	Threshold = atof( LPCTSTR( str ));
}

void COutputProp::OnDeltaposSpinThreshold(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	Threshold += pNMUpDown->iDelta*0.1;
	CString str;
	str.Format( "%lg", Threshold );
	Edit_Threshold.SetWindowText( LPCTSTR( str ));
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// COutputNetwork property page
IMPLEMENT_DYNCREATE(COutputNetwork, CPropertyPage)

COutputNetwork::COutputNetwork() : CPropertyPage(COutputNetwork::IDD)
{
    NewOutput = 0;
}

void COutputNetwork::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROP_OUTPUT, PropOutput);
	DDX_Control(pDX, IDC_OUTPUT_SIZE, StringSize);
	DDX_Control(pDX, IDC_LIST_OUTPUT, ListOutput);
	DDX_Control(pDX, IDC_DEL_OUTPUT, DelOutput);
	DDX_Control(pDX, IDC_ADD_OUTPUT, add_out);
}

BEGIN_MESSAGE_MAP(COutputNetwork, CPropertyPage)
	ON_BN_CLICKED(IDC_ADD_OUTPUT, OnAddOutput)
	ON_BN_CLICKED(IDC_DEL_OUTPUT, OnDelOutput)
	ON_BN_CLICKED(IDC_PROP_OUTPUT, OnPropOutput)
	ON_LBN_DBLCLK(IDC_LIST_OUTPUT, OnDblclkListOutput)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputNetwork message handlers
BOOL COutputNetwork::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
    NewOutput = Network->size_out();
    Change();
	return TRUE;
}

void COutputNetwork::OnAddOutput() 
{
    CString name;
#if defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
    name.Format("Object %d",NewOutput);
#else
    name.Format("Output %d",NewOutput);
#endif //defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
	Network->add_out( hhn_output( string(( LPCTSTR )name )));
    NewOutput++;
    Change();
}

void COutputNetwork::OnDelOutput() 
{
    unsigned int i = ListOutput.GetCurSel();
    if( i == -1 )
        return;
    Network->del_out(i);
    Change();
}

void COutputNetwork::OnPropOutput() 
{
    unsigned int i = ListOutput.GetCurSel();
    if( i < 0 )
        return;
	COutputProp dlg;
    hhn_output output( Network->get_out( i ));
    dlg.Name = output.get_name().c_str();
    dlg.T1 = output.tup();
    dlg.T2 = output.tdown();
	dlg.Threshold = output.threshold();
    if( dlg.DoModal() == IDOK ){
        output.set_name(( LPCTSTR )dlg.Name );
        output.tup() = dlg.T1;
        output.tdown() = dlg.T2;
		output.threshold() = dlg.Threshold;
        Network->set_out( output, i );
        Change();
        }
}

void COutputNetwork::OnDblclkListOutput() 
{
	OnPropOutput();
}

void COutputNetwork::Change( void )
{
    add_out.EnableWindow(TRUE);
    if( Network->size_out() == 0 ){
        PropOutput.EnableWindow(FALSE);
        DelOutput.EnableWindow(FALSE);
        }
	else{
		DelOutput.EnableWindow(TRUE);
		PropOutput.EnableWindow(TRUE);
		}

    unsigned int index = ListOutput.GetCurSel();
    if( index == -1 ) index = 0;
    if( index >= Network->size_out() ) index = Network->size_out()-1;

    CString str;
#if defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
    str.Format("Number of objects - %d", Network->size_out());
#else

    str.Format("Number of outputs - %d", Network->size_out());
#endif //defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )

    StringSize.SetWindowText(str);
    ListOutput.ResetContent();
    for( unsigned int i = 0 ; i < Network->size_out(); i++ ){
         ListOutput.AddString( Network->get_out(i).get_name().c_str() );
         }
    ListOutput.SetCurSel( index );
}

BOOL COutputNetwork::OnSetActive() 
{
    Change();
	return CPropertyPage::OnSetActive();
}

#ifdef __MECHANICS__
/////////////////////////////////////////////////////////////////////////////
// CFBProp dialog
CFBProp::CFBProp(CWnd* pParent /*=NULL*/)
	: CDialog(CFBProp::IDD, pParent)
{
	Pv = 0.6;
	Kv = 0.0;
	Kl = 0.0;
	Kemg = 0.0;
	Kf = 0.0;
	Ftr = 0.0;
	Kc = 0.0;
	Kg = 0.0;
	TypeFB = 0;
	FBName = _T("");
	MechName = _T("");
}

void CFBProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FB_NAME, FBName);
	DDX_Control(pDX, IDC_COMBO_TYPE, ComboType);
	DDX_Control(pDX, IDC_COMBO_LISTFB, ListFB);
	DDX_CBIndex(pDX, IDC_COMBO_TYPE, TypeFB);
	DDX_Control(pDX, IDC_KV, EditKV);
	DDX_Control(pDX, IDC_PV, EditPV);
	DDX_Control(pDX, IDC_KL, EditKL);
	DDX_Control(pDX, IDC_KEMG, EditKEMG);
	DDX_Control(pDX, IDC_KF, EditKF);
	DDX_Control(pDX, IDC_KFTR, EditKFTR);
	DDX_Control(pDX, IDC_KC, EditKC);
	DDX_Control(pDX, IDC_KGR, EditKG);

	DDX_Control(pDX, IDC_SPIN_KV, SpinKV);
	DDX_Control(pDX, IDC_SPIN_PV, SpinPV);
	DDX_Control(pDX, IDC_SPIN_KL, SpinKL);
	DDX_Control(pDX, IDC_SPIN_KEMG, SpinKEMG);
	DDX_Control(pDX, IDC_SPIN_KF, SpinKF);
	DDX_Control(pDX, IDC_SPIN_KFTR, SpinKFTR);
	DDX_Control(pDX, IDC_SPIN_KC, SpinKC);
	DDX_Control(pDX, IDC_SPIN_KGR, SpinKG);

	DDX_Text(pDX, IDC_KV, Kv);
	DDX_Text(pDX, IDC_PV, Pv);
	DDX_Text(pDX, IDC_KL, Kl);
	DDX_Text(pDX, IDC_KEMG, Kemg);
	DDX_Text(pDX, IDC_KF, Kf);
	DDX_Text(pDX, IDC_KFTR, Ftr);
	DDX_Text(pDX, IDC_KC, Kc);
	DDX_Text(pDX, IDC_KGR, Kg);
}

BEGIN_MESSAGE_MAP(CFBProp, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnSelchangeComboType)
	ON_CBN_SELCHANGE(IDC_COMBO_LISTFB, OnSelchangeListFB)
	ON_EN_UPDATE(IDC_KV, OnUpdateKV)
	ON_EN_UPDATE(IDC_PV, OnUpdatePV)
	ON_EN_UPDATE(IDC_KL, OnUpdateKL)
	ON_EN_UPDATE(IDC_KEMG, OnUpdateKEMG)
	ON_EN_UPDATE(IDC_KF, OnUpdateKF)
	ON_EN_UPDATE(IDC_KFTR, OnUpdateKFTR)
	ON_EN_UPDATE(IDC_KC, OnUpdateKC)
	ON_EN_UPDATE(IDC_KGR, OnUpdateKG)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_KV, OnDeltaposSpinKV)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PV, OnDeltaposSpinPV)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_KL, OnDeltaposSpinKL)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_KEMG, OnDeltaposSpinKEMG)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_KF, OnDeltaposSpinKF)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_KFTR, OnDeltaposSpinKFTR)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_KC, OnDeltaposSpinKC)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_KGR, OnDeltaposSpinKG)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFBProp message handlers
BOOL CFBProp::OnInitDialog() 
{
	CDialog::OnInitDialog();
    SpinKV.SetBuddy(&EditKV);
    SpinKV.SetRange(UD_MINVAL,UD_MAXVAL);
    SpinPV.SetBuddy(&EditPV);
    SpinPV.SetRange(UD_MINVAL,UD_MAXVAL);
    SpinKL.SetBuddy(&EditKL);
    SpinKL.SetRange(UD_MINVAL,UD_MAXVAL);
    SpinKEMG.SetBuddy(&EditKEMG);
    SpinKEMG.SetRange(UD_MINVAL,UD_MAXVAL);
    SpinKF.SetBuddy(&EditKF);
    SpinKF.SetRange(UD_MINVAL,UD_MAXVAL);
    SpinKFTR.SetBuddy(&EditKFTR);
    SpinKFTR.SetRange(UD_MINVAL,UD_MAXVAL);
    SpinKC.SetBuddy(&EditKC);
    SpinKC.SetRange(UD_MINVAL,UD_MAXVAL);
    SpinKG.SetBuddy(&EditKG);
    SpinKG.SetRange(UD_MINVAL,UD_MAXVAL);

	int fb_inx = 0;
	ListFB.ResetContent();
	ListFB.AddString( "None" );
	vector<string> fb_list;
	BiomT->get_fblist( fb_list );
	for( size_t i = 0 ; i < fb_list.size(); ++i ){
		ListFB.AddString( fb_list[i].c_str());
		if( fb_list[i] == ( LPCTSTR )MechName )
			fb_inx = i+1;
	}
	ListFB.SetCurSel( fb_inx );
    Change(); 
	return TRUE;
}

void CFBProp::Change( void )
{
	EditKV.EnableWindow(FALSE);
	EditPV.EnableWindow(FALSE);
	EditKL.EnableWindow(FALSE);
	EditKEMG.EnableWindow(FALSE);
	EditKF.EnableWindow(FALSE);
	EditKFTR.EnableWindow(FALSE);
	EditKG.EnableWindow(FALSE);
	EditKC.EnableWindow(FALSE);
	SpinKV.EnableWindow(FALSE);
	SpinPV.EnableWindow(FALSE);
	SpinKL.EnableWindow(FALSE);
	SpinKEMG.EnableWindow(FALSE);
	SpinKF.EnableWindow(FALSE);
	SpinKFTR.EnableWindow(FALSE);
	SpinKC.EnableWindow(FALSE);
	SpinKG.EnableWindow(FALSE);
	switch( TypeFB ){
		case 1:		// Ia
			EditKV.EnableWindow(TRUE);
			EditPV.EnableWindow(TRUE);
			EditKL.EnableWindow(TRUE);
			EditKEMG.EnableWindow(TRUE);
			EditKC.EnableWindow(TRUE);
			SpinKV.EnableWindow(TRUE);
			SpinPV.EnableWindow(TRUE);
			SpinKL.EnableWindow(TRUE);
			SpinKEMG.EnableWindow(TRUE);
			SpinKC.EnableWindow(TRUE);
			break;
		case 2:		// II
			EditKL.EnableWindow(TRUE);
			EditKEMG.EnableWindow(TRUE);
			EditKC.EnableWindow(TRUE);
			SpinKL.EnableWindow(TRUE);
			SpinKEMG.EnableWindow(TRUE);
			SpinKC.EnableWindow(TRUE);
			break;
		case 3:		// Ib
			EditKF.EnableWindow(TRUE);
			EditKFTR.EnableWindow(TRUE);
			SpinKF.EnableWindow(TRUE);
			SpinKFTR.EnableWindow(TRUE);
			break;
		case 4:		// GRF
			EditKG.EnableWindow(TRUE);
			SpinKG.EnableWindow(TRUE);
			break;
		default:;	// none
	}
}

void CFBProp::OnSelchangeComboType() 
{
	TypeFB = ComboType.GetCurSel();
    Change();
}

void CFBProp::OnSelchangeListFB() 
{
	int fb_num = ListFB.GetCurSel();
    if( fb_num < 0 ) return;
    ListFB.GetLBText( fb_num, MechName );
}

void CFBProp::OnUpdateKV() 
{
    CString str;
	EditKV.GetWindowText( str );
    Kv = atof( LPCTSTR( str ));
}

void CFBProp::OnUpdatePV() 
{
    CString str;
	EditPV.GetWindowText( str );
    Pv = atof( LPCTSTR( str ));
}

void CFBProp::OnUpdateKL()
{
    CString str;
	EditKL.GetWindowText( str );
    Kl = atof( LPCTSTR( str ));
}

void CFBProp::OnUpdateKEMG() 
{
    CString str;
	EditKEMG.GetWindowText( str );
    Kemg = atof( LPCTSTR( str ));
}

void CFBProp::OnUpdateKF()
{
    CString str;
	EditKF.GetWindowText( str );
    Kf = atof( LPCTSTR( str ));
}

void CFBProp::OnUpdateKC()
{
    CString str;
	EditKC.GetWindowText( str );
    Kc = atof( LPCTSTR( str ));
}

void CFBProp::OnUpdateKG()
{
    CString str;
	EditKG.GetWindowText( str );
    Kg = atof( LPCTSTR( str ));
}

void CFBProp::OnUpdateKFTR()
{
    CString str;
	EditKFTR.GetWindowText( str );
    Ftr = atof( LPCTSTR( str ));
}

void CFBProp::OnDeltaposSpinKV(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    Kv += pNMUpDown->iDelta*0.1;
    CString str;
    str.Format( "%lg", Kv );
    EditKV.SetWindowText(( LPCTSTR ) str );
	*pResult = 0;
}

void CFBProp::OnDeltaposSpinPV(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    Pv += pNMUpDown->iDelta*0.1;
    CString str;
    str.Format( "%lg", Pv );
    EditPV.SetWindowText(( LPCTSTR ) str );
	*pResult = 0;
}

void CFBProp::OnDeltaposSpinKL(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    Kl += pNMUpDown->iDelta*0.1;
    CString str;
    str.Format( "%lg", Kl );
    EditKL.SetWindowText(( LPCTSTR ) str );
	*pResult = 0;
}

void CFBProp::OnDeltaposSpinKEMG(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    Kemg += pNMUpDown->iDelta*0.1;
    CString str;
    str.Format( "%lg", Kemg );
    EditKEMG.SetWindowText(( LPCTSTR ) str );
	*pResult = 0;
}

void CFBProp::OnDeltaposSpinKF(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    Kf += pNMUpDown->iDelta*0.1;
    CString str;
    str.Format( "%lg", Kf );
    EditKF.SetWindowText(( LPCTSTR ) str );
	*pResult = 0;
}

void CFBProp::OnDeltaposSpinKC(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    Kc += pNMUpDown->iDelta*0.1;
    CString str;
    str.Format( "%lg", Kc );
    EditKC.SetWindowText(( LPCTSTR ) str );
	*pResult = 0;
}

void CFBProp::OnDeltaposSpinKG(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    Kg += pNMUpDown->iDelta*0.1;
    CString str;
    str.Format( "%lg", Kg );
    EditKG.SetWindowText(( LPCTSTR ) str );
	*pResult = 0;
}

void CFBProp::OnDeltaposSpinKFTR(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    Ftr += pNMUpDown->iDelta*0.1;
    CString str;
    str.Format( "%lg", Ftr );
    EditKFTR.SetWindowText(( LPCTSTR ) str );
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CFeedbackSetup property page
IMPLEMENT_DYNCREATE( CFeedbackSetup, CPropertyPage )

CFeedbackSetup::CFeedbackSetup() : CPropertyPage( CFeedbackSetup::IDD )
{
    NewFeedback = 0;
}

CFeedbackSetup::~CFeedbackSetup()
{
}

void CFeedbackSetup::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FEEDBACK_SIZE, FeedbackSize);
	DDX_Control(pDX, IDC_ADD_FEEDBACK, add_fbk);
	DDX_Control(pDX, IDC_DEL_FEEDBACK, DelFeedback);
	DDX_Control(pDX, IDC_PROP_FEEDBACK, PropFeedback);
	DDX_Control(pDX, IDC_LIST_FEEDBACK, ListFeedback);
}

BEGIN_MESSAGE_MAP(CFeedbackSetup, CPropertyPage)
	ON_BN_CLICKED(IDC_DEL_FEEDBACK, OnDelFeedback)
	ON_BN_CLICKED(IDC_ADD_FEEDBACK, OnAddFeedback)
	ON_BN_CLICKED(IDC_PROP_FEEDBACK, OnPropFeedback)
	ON_LBN_DBLCLK(IDC_LIST_FEEDBACK, OnDblclkListFeedback)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFeedbackSetup message handlers
BOOL CFeedbackSetup::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
    NewFeedback = Network->size_fbk();
    Change();
	return TRUE;
}

void CFeedbackSetup::OnAddFeedback() 
{
    CString name;
    name.Format("Feedback %d",NewFeedback);
	Network->add_fbk( hhn_feedback( string(( LPCTSTR )name )));
    NewFeedback++;
    Change();
}

void CFeedbackSetup::OnDelFeedback() 
{
    size_t i = ListFeedback.GetCurSel();
    if( i == -1 ) return;
    Network->del_fbk( i );
    Change();
}

void CFeedbackSetup::OnPropFeedback() 
{
	size_t i = ListFeedback.GetCurSel();
	if( i == -1 ) return;
	CFBProp dlg;
	hhn_feedback feedback( Network->get_fbk( i ));
	dlg.BiomT = BiomT;
	dlg.FBName = feedback.get_name().c_str();
	dlg.MechName = feedback.MechName.c_str();
	if( feedback.Kv > 0.0 ){
		dlg.Kv = feedback.Kv;
		dlg.Pv = feedback.Pv;
		dlg.Kl = feedback.Kl;
		dlg.Kemg = feedback.Kemg;
		dlg.Kc = feedback.Kc;
		dlg.TypeFB = 1;		// Ia
	}
	else if( feedback.Kl > 0.0 ){
		dlg.Kl = feedback.Kl;
		dlg.Kemg = feedback.Kemg;
		dlg.Kc = feedback.Kc;
		dlg.TypeFB = 2;		// II
	}
	else if( feedback.Kf > 0 ){
		dlg.Kf = feedback.Kf;
		dlg.Ftr = feedback.Ftr;
		dlg.TypeFB = 3;		// Ib
	}
	else if( feedback.Kg > 0 ){
		dlg.Kg = feedback.Kg;
		dlg.TypeFB = 4;		// Ib
	}
	if( dlg.DoModal() == IDOK ){
		feedback.set_name( LPCTSTR( dlg.FBName ));
		feedback.MechName = LPCTSTR( dlg.MechName );
		switch( dlg.TypeFB ){
			case 1:		// Ia
				feedback.Pv = dlg.Pv;
				feedback.Kv = dlg.Kv;
				feedback.Kl = dlg.Kl;
				feedback.Kemg = dlg.Kemg;
				feedback.Kc = dlg.Kc;
				feedback.Kf = 0.0;
				feedback.Ftr = 0.0;
				feedback.Kg = 0.0;
				break;
			case 2:		// II
				feedback.Kl = dlg.Kl;
				feedback.Kemg = dlg.Kemg;
				feedback.Kc = dlg.Kc;
				feedback.Pv = 0.0;
				feedback.Kv = 0.0;
				feedback.Kf = 0.0;
				feedback.Ftr = 0.0;
				feedback.Kg = 0.0;
				break;
			case 3:		// Ib
				feedback.Kf = dlg.Kf;
				feedback.Ftr = dlg.Ftr;
				feedback.Kl = 0.0;
				feedback.Kemg = 0.0;
				feedback.Kc = 0.0;
				feedback.Pv = 0.0;
				feedback.Kv = 0.0;
				feedback.Kg = 0.0;
				break;
			case 4:		// GRF
				feedback.Kg = dlg.Kg;
				feedback.Kl = 0.0;
				feedback.Kemg = 0.0;
				feedback.Kc = 0.0;
				feedback.Pv = 0.0;
				feedback.Kv = 0.0;
				feedback.Kf = 0.0;
				feedback.Ftr = 0.0;
				break;
			default:;	// none
		}
		Network->set_fbk( feedback, i );
		Change();
	}
}

void CFeedbackSetup::OnDblclkListFeedback() 
{
	OnPropFeedback();
}

void CFeedbackSetup::Change( void )
{
	add_fbk.EnableWindow( TRUE );
	if( Network->size_fbk() == 0 ){
		DelFeedback.EnableWindow( FALSE );
		PropFeedback.EnableWindow( FALSE );
	}
	else{
		DelFeedback.EnableWindow(TRUE);
		PropFeedback.EnableWindow(TRUE);
	}
	CString str; str.Format("Number of feedbacks - %d", Network->size_fbk());
	FeedbackSize.SetWindowText( str );
	size_t index = ListFeedback.GetCurSel();
	if( index == -1 ) index = 0;
	if( index >= Network->size_fbk() ) index = Network->size_fbk()-1;
	ListFeedback.ResetContent();
	for( size_t i = 0 ; i < Network->size_fbk(); i++ ){
		ListFeedback.AddString( Network->get_fbk(i).get_name().c_str() );
	}
	ListFeedback.SetCurSel( index );
}

BOOL CFeedbackSetup::OnSetActive() 
{
	Change();
	return CPropertyPage::OnSetActive();
}
#endif // __MECHANICS__

/////////////////////////////////////////////////////////////////////////////
// CNetworkSetup
IMPLEMENT_DYNAMIC(CNetworkSetup, CPropertySheet)

CNetworkSetup::CNetworkSetup(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
#ifdef __MECHANICS__
	, BiomT( NULL )
#endif // __MECHANICS__
{
	AddPage( &NetworkParam );
	AddPage( &PopulatSetup );
	AddPage( &DriveSetup );
	AddPage( &OutputSetup );
	NetworkParam.Network = &Network;
	PopulatSetup.Network = &Network;
	DriveSetup.Network = &Network;
	OutputSetup.Network = &Network;
#ifdef __MECHANICS__
	AddPage( &FeedbackSetup );
	FeedbackSetup.Network = &Network;
#endif // __MECHANICS__
}

CNetworkSetup::CNetworkSetup(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
#ifdef __MECHANICS__
	, BiomT( NULL )
#endif // __MECHANICS__
{
	AddPage( &NetworkParam );
	AddPage( &PopulatSetup );
	AddPage( &DriveSetup );
	AddPage( &OutputSetup );
	NetworkParam.Network = &Network;
	PopulatSetup.Network = &Network;
	DriveSetup.Network = &Network;
	OutputSetup.Network = &Network;
#ifdef __MECHANICS__
	AddPage( &FeedbackSetup );
	FeedbackSetup.Network = &Network;
	FeedbackSetup.BiomT = &BiomT;
#endif // __MECHANICS__
}

CNetworkSetup::~CNetworkSetup()
{
}

/////////////////////////////////////////////////////////////////////////////
// CNetDiff dialog
CNetDiff::CNetDiff(CWnd* pParent /*=NULL*/)
	: CDialog(CNetDiff::IDD, pParent)
{
	Pattern = _T("^l/^r|.*");
}

void CNetDiff::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NETDIFF, Pattern);
}

#endif // __CONSOLE__
