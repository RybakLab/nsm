/////////////////////////////////////////////////////////////////////////////
// ConnectionSetup.cpp : implementation file
#include "precompile.h"

#ifndef __CONSOLE__

#include "ConnectionSetup.h"
#include "neuronsetup.h"
#include "networksetup.h"
#include "../Spcord.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG

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
	: CDialog(CWeightSetup::IDD, pParent), pCM( NULL ), IsChained( FALSE )
{
	for( int i = 0; i < _id_MAX_SYN; ++i ){
		Weights[i] = 0.;
		SynSpec[i] = 0;
		WeightVars[i] = 0.;
	}
	OldIndex = -1;
	Probability = 1.;
}

void CWeightSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_CONNECT_TYPE, ConnectType );
	DDX_Control( pDX, IDC_CONNECT_MODULATE, PresynMod );

	DDX_Text( pDX, IDC_EDIT_EX_WEIGHT, Weights[_id_ExSyn] );
	DDX_Text( pDX, IDC_EDIT_EX_WEIGHT2, Weights[_id_ExBSyn] );
	DDX_Text( pDX, IDC_EDIT_INH1_WEIGHT, Weights[_id_InhASyn] );
	DDX_Text( pDX, IDC_EDIT_INH2_WEIGHT, Weights[_id_InhBSyn] );
	DDX_Text( pDX, IDC_EDIT_SYN1_WEIGHT, Weights[_id_InhBSyn+1] );
	DDX_Text( pDX, IDC_EDIT_SYN2_WEIGHT, Weights[_id_InhBSyn+2] );
	DDX_Text( pDX, IDC_EDIT_SYN3_WEIGHT, Weights[_id_InhBSyn+3] );
	DDX_Text( pDX, IDC_EDIT_SYN4_WEIGHT, Weights[_id_InhBSyn+4] );

	DDX_Text( pDX, IDC_EDIT_EX_WEIGHT_VAR, WeightVars[_id_ExSyn] );
	DDX_Text( pDX, IDC_EDIT_EX_WEIGHT_VAR2, WeightVars[_id_ExBSyn] );
	DDX_Text( pDX, IDC_EDIT_INH1_WEIGHT_VAR, WeightVars[_id_InhASyn] );
	DDX_Text( pDX, IDC_EDIT_INH2_WEIGHT_VAR, WeightVars[_id_InhBSyn] );
	DDX_Text( pDX, IDC_EDIT_SYN1_WEIGHT_VAR, WeightVars[_id_InhBSyn+1] );
	DDX_Text( pDX, IDC_EDIT_SYN2_WEIGHT_VAR, WeightVars[_id_InhBSyn+2] );
	DDX_Text( pDX, IDC_EDIT_SYN3_WEIGHT_VAR, WeightVars[_id_InhBSyn+3] );
	DDX_Text( pDX, IDC_EDIT_SYN4_WEIGHT_VAR, WeightVars[_id_InhBSyn+4] );

	DDX_CBIndex( pDX, IDC_EX_SPEC, SynSpec[_id_ExSyn] );
	DDX_CBIndex( pDX, IDC_EX2_SPEC, SynSpec[_id_ExBSyn] );
	DDX_CBIndex( pDX, IDC_INH1_SPEC, SynSpec[_id_InhASyn] );
	DDX_CBIndex( pDX, IDC_INH2_SPEC, SynSpec[_id_InhBSyn] );
	DDX_CBIndex( pDX, IDC_SYN1_SPEC, SynSpec[_id_InhBSyn+1] );
	DDX_CBIndex( pDX, IDC_SYN2_SPEC, SynSpec[_id_InhBSyn+2] );
	DDX_CBIndex( pDX, IDC_SYN3_SPEC, SynSpec[_id_InhBSyn+3] );
	DDX_CBIndex( pDX, IDC_SYN4_SPEC, SynSpec[_id_InhBSyn+4] );

	DDV_MinMaxDouble( pDX, WeightVars[_id_ExSyn], 0., 50. );
	DDV_MinMaxDouble( pDX, WeightVars[_id_ExBSyn], 0, 50 );
	DDV_MinMaxDouble( pDX, WeightVars[_id_InhASyn], 0, 50 );
	DDV_MinMaxDouble( pDX, WeightVars[_id_InhBSyn], 0, 50 );
	DDV_MinMaxDouble( pDX, WeightVars[_id_InhBSyn+1], 0, 50 );
	DDV_MinMaxDouble( pDX, WeightVars[_id_InhBSyn+2], 0, 50 );
	DDV_MinMaxDouble( pDX, WeightVars[_id_InhBSyn+3], 0, 50 );
	DDV_MinMaxDouble( pDX, WeightVars[_id_InhBSyn+4], 0, 50 );


	DDX_Control( pDX, IDC_EDIT_EX_WEIGHT, EditWeights[_id_ExSyn] );
	DDX_Control( pDX, IDC_SPIN_EX_WEIGHT, SpinWeights[_id_ExSyn] );
	DDX_Control( pDX, IDC_EDIT_EX_WEIGHT_VAR, EditWeightVars[_id_ExSyn] );
	DDX_Control( pDX, IDC_SPIN_EX_WEIGHT_VAR, SpinWeightVars[_id_ExSyn] );

	DDX_Control( pDX, IDC_EDIT_EX_WEIGHT2, EditWeights[_id_ExBSyn] );
	DDX_Control( pDX, IDC_SPIN_EX_WEIGHT2, SpinWeights[_id_ExBSyn] );
	DDX_Control( pDX, IDC_EDIT_EX_WEIGHT_VAR2, EditWeightVars[_id_ExBSyn] );
	DDX_Control( pDX, IDC_SPIN_EX_WEIGHT_VAR2, SpinWeightVars[_id_ExBSyn] );

	DDX_Control( pDX, IDC_EDIT_INH1_WEIGHT, EditWeights[_id_InhASyn] );
	DDX_Control( pDX, IDC_SPIN_INH1_WEIGHT, SpinWeights[_id_InhASyn] );
	DDX_Control( pDX, IDC_EDIT_INH1_WEIGHT_VAR, EditWeightVars[_id_InhASyn] );
	DDX_Control( pDX, IDC_SPIN_INH1_WEIGHT_VAR, SpinWeightVars[_id_InhASyn] );

	DDX_Control( pDX, IDC_EDIT_INH2_WEIGHT, EditWeights[_id_InhBSyn] );
	DDX_Control( pDX, IDC_SPIN_INH2_WEIGHT, SpinWeights[_id_InhBSyn] );
	DDX_Control( pDX, IDC_EDIT_INH2_WEIGHT_VAR, EditWeightVars[_id_InhBSyn] );
	DDX_Control( pDX, IDC_SPIN_INH2_WEIGHT_VAR, SpinWeightVars[_id_InhBSyn] );

	DDX_Control( pDX, IDC_EDIT_SYN1_WEIGHT, EditWeights[_id_InhBSyn+1] );
	DDX_Control( pDX, IDC_SPIN_SYN1_WEIGHT, SpinWeights[_id_InhBSyn+1] );
	DDX_Control( pDX, IDC_EDIT_SYN1_WEIGHT_VAR, EditWeightVars[_id_InhBSyn+1] );
	DDX_Control( pDX, IDC_SPIN_SYN1_WEIGHT_VAR, SpinWeightVars[_id_InhBSyn+1] );

	DDX_Control( pDX, IDC_EDIT_SYN2_WEIGHT, EditWeights[_id_InhBSyn+2] );
	DDX_Control( pDX, IDC_SPIN_SYN2_WEIGHT, SpinWeights[_id_InhBSyn+2] );
	DDX_Control( pDX, IDC_EDIT_SYN2_WEIGHT_VAR, EditWeightVars[_id_InhBSyn+2] );
	DDX_Control( pDX, IDC_SPIN_SYN2_WEIGHT_VAR, SpinWeightVars[_id_InhBSyn+2] );

	DDX_Control( pDX, IDC_EDIT_SYN3_WEIGHT, EditWeights[_id_InhBSyn+3] );
	DDX_Control( pDX, IDC_SPIN_SYN3_WEIGHT, SpinWeights[_id_InhBSyn+3] );
	DDX_Control( pDX, IDC_EDIT_SYN3_WEIGHT_VAR, EditWeightVars[_id_InhBSyn+3] );
	DDX_Control( pDX, IDC_SPIN_SYN3_WEIGHT_VAR, SpinWeightVars[_id_InhBSyn+3] );

	DDX_Control( pDX, IDC_EDIT_SYN4_WEIGHT, EditWeights[_id_InhBSyn+4] );
	DDX_Control( pDX, IDC_SPIN_SYN4_WEIGHT, SpinWeights[_id_InhBSyn+4] );
	DDX_Control( pDX, IDC_EDIT_SYN4_WEIGHT_VAR, EditWeightVars[_id_InhBSyn+4] );
	DDX_Control( pDX, IDC_SPIN_SYN4_WEIGHT_VAR, SpinWeightVars[_id_InhBSyn+4] );

	DDX_Control( pDX, IDC_EDIT_PROBABILITY, EditProbability );
	DDX_Control( pDX, IDC_SPIN_PROBABILITY, SpinProbability );
	DDX_Text( pDX, IDC_EDIT_PROBABILITY, Probability );
	DDV_MinMaxDouble( pDX, Probability, 0., 1. );

	DDX_Check( pDX, IDC_CHAINED, IsChained );
	DDX_Control( pDX, IDC_CHECK_WEIGHTEXTEND, ExtendWeight );
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

	ON_EN_UPDATE(IDC_EDIT_EX_WEIGHT, &CWeightSetup::OnUpdateEditExWeight)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EX_WEIGHT, &CWeightSetup::OnDeltaposSpinExWeight)
	ON_EN_UPDATE(IDC_EDIT_EX_WEIGHT_VAR, &CWeightSetup::OnUpdateEditExWeightVar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EX_WEIGHT_VAR, &CWeightSetup::OnDeltaposSpinExWeightVar)

	ON_EN_UPDATE(IDC_EDIT_EX_WEIGHT2, &CWeightSetup::OnUpdateEditExWeight2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EX_WEIGHT2, &CWeightSetup::OnDeltaposSpinExWeight2)
	ON_EN_UPDATE(IDC_EDIT_EX_WEIGHT_VAR2, &CWeightSetup::OnUpdateEditExWeightVar2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EX_WEIGHT_VAR2, &CWeightSetup::OnDeltaposSpinExWeightVar2)
	
	ON_EN_UPDATE(IDC_EDIT_INH1_WEIGHT, &CWeightSetup::OnUpdateEditInhAWeight)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_INH1_WEIGHT, &CWeightSetup::OnDeltaposSpinInhAWeight)
	ON_EN_UPDATE(IDC_EDIT_INH1_WEIGHT_VAR, &CWeightSetup::OnUpdateEditInhAWeightVar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_INH1_WEIGHT_VAR, &CWeightSetup::OnDeltaposSpinInhAWeightVar)

	ON_EN_UPDATE(IDC_EDIT_INH2_WEIGHT, &CWeightSetup::OnUpdateEditInhBWeight)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_INH2_WEIGHT, &CWeightSetup::OnDeltaposSpinInhBWeight)
	ON_EN_UPDATE(IDC_EDIT_INH2_WEIGHT_VAR, &CWeightSetup::OnUpdateEditInhBWeightVar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_INH2_WEIGHT_VAR, &CWeightSetup::OnDeltaposSpinInhBWeightVar)

	ON_EN_UPDATE(IDC_EDIT_SYN1_WEIGHT, &CWeightSetup::OnUpdateEditSyn1Weight)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SYN1_WEIGHT, &CWeightSetup::OnDeltaposSpinSyn1Weight)
	ON_EN_UPDATE(IDC_EDIT_SYN1_WEIGHT_VAR, &CWeightSetup::OnUpdateEditSyn1WeightVar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SYN1_WEIGHT_VAR, &CWeightSetup::OnDeltaposSpinSyn1WeightVar)

	ON_EN_UPDATE(IDC_EDIT_SYN2_WEIGHT, &CWeightSetup::OnUpdateEditSyn2Weight)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SYN2_WEIGHT, &CWeightSetup::OnDeltaposSpinSyn2Weight)
	ON_EN_UPDATE(IDC_EDIT_SYN2_WEIGHT_VAR, &CWeightSetup::OnUpdateEditSyn2WeightVar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SYN2_WEIGHT_VAR, &CWeightSetup::OnDeltaposSpinSyn2WeightVar)

	ON_EN_UPDATE(IDC_EDIT_SYN3_WEIGHT, &CWeightSetup::OnUpdateEditSyn3Weight)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SYN3_WEIGHT, &CWeightSetup::OnDeltaposSpinSyn3Weight)
	ON_EN_UPDATE(IDC_EDIT_SYN3_WEIGHT_VAR, &CWeightSetup::OnUpdateEditSyn3WeightVar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SYN3_WEIGHT_VAR, &CWeightSetup::OnDeltaposSpinSyn3WeightVar)

	ON_EN_UPDATE(IDC_EDIT_SYN4_WEIGHT, &CWeightSetup::OnUpdateEditSyn4Weight)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SYN4_WEIGHT, &CWeightSetup::OnDeltaposSpinSyn4Weight)
	ON_EN_UPDATE(IDC_EDIT_SYN4_WEIGHT_VAR, &CWeightSetup::OnUpdateEditSyn4WeightVar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SYN4_WEIGHT_VAR, &CWeightSetup::OnDeltaposSpinSyn4WeightVar)

	ON_CBN_SELCHANGE(IDC_CONNECT_TYPE, &CWeightSetup::OnSelchangeConnectType)
	ON_EN_UPDATE(IDC_EDIT_PROBABILITY, &CWeightSetup::OnUpdateEditProbability)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PROBABILITY, &CWeightSetup::OnDeltaposSpinProbability)
	ON_BN_CLICKED(ID_WEIGHT_DEP, &CWeightSetup::OnWeightDep)
	ON_BN_CLICKED( IDC_CHECK_WEIGHTEXTEND, &CWeightSetup::OnBnClickedExtend )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWeightSetup message handlers
BOOL CWeightSetup::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowText( Title );
	for( size_t i = 0; i < Connect.size(); i++ ){
		if( Connect[i].Type >= 0 && Connect[i].Type < _id_NN_LAST_CONNECT ){
			ConnectType.AddString( _ConnectionTypes[Connect[i].Type] );
		}
	}
	PresynMod.AddString("None");
	PresynMod.AddString("Inhibition");
	PresynMod.AddString("Facilitation");
	for( int sid = 0; sid < _id_MAX_SYN; ++sid ){
		SpinWeights[sid].SetBuddy( &EditWeights[sid] );
		SpinWeights[sid].SetRange(UD_MINVAL, UD_MAXVAL);
		SpinWeightVars[sid].SetRange(0, 50);
	}

	SpinProbability.SetBuddy(&EditProbability);
	SpinProbability.SetRange(0,1);
	ConnectType.EnableWindow( FALSE );
	PresynMod.EnableWindow( TRUE );
	if( ConnectType.GetCount() >= 1 && !Connect.empty() ){
		ConnectType.EnableWindow( TRUE );
		ConnectType.SetCurSel( ConnectType.GetCount()-1 );
		GetWeight();
	}
	if( !Connect.ChainS.empty() && !Connect.ChainT.empty() ){
		IsChained = TRUE;
	} else{	IsChained = FALSE; }
	UpdateData( FALSE );
	return TRUE;
}

void CWeightSetup::OnWeightDep()
{
	assert( pCM );
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
		} else{
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

void CWeightSetup::OnBnClickedExtend()
{
	int state = ExtendWeight.GetCheck();
	if( state == BST_UNCHECKED ){
		CRect wnd; GetWindowRect( wnd );
		MoveWindow( wnd.left, wnd.top+170, wnd.Width(), wnd.Height()-340 );
	} else if( state == BST_CHECKED ){
		CRect wnd; GetWindowRect( wnd );
		MoveWindow( wnd.left, wnd.top-170, wnd.Width(), wnd.Height()+340 );
	}
}

void CWeightSetup::OnUpdateEditExWeight() 
{
	CString strWeight;
	EditWeights[_id_ExSyn].GetWindowText( strWeight );
	Weights[_id_ExSyn] = atof( LPCTSTR( strWeight ));
}

void CWeightSetup::OnUpdateEditExWeight2() 
{
	CString strWeight;
	EditWeights[_id_ExBSyn].GetWindowText( strWeight );
	Weights[_id_ExBSyn] = atof( LPCTSTR( strWeight ));
}

void CWeightSetup::OnUpdateEditInhAWeight() 
{
	CString strWeight;
	EditWeights[_id_InhASyn].GetWindowText( strWeight );
	Weights[_id_InhASyn] = atof( LPCTSTR( strWeight ));
}

void CWeightSetup::OnUpdateEditInhBWeight() 
{
	CString strWeight;
	EditWeights[_id_InhBSyn].GetWindowText( strWeight );
	Weights[_id_InhBSyn] = atof( LPCTSTR( strWeight ));
}

void CWeightSetup::OnDeltaposSpinExWeight(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	Weights[_id_ExSyn] += pNMUpDown->iDelta*0.1;;
	CString strWeight;
	strWeight.Format( "%lg", Weights[_id_ExSyn] );
	EditWeights[_id_ExSyn].SetWindowText(( LPCTSTR ) strWeight );
	*pResult = 0;
}

void CWeightSetup::OnDeltaposSpinExWeight2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	Weights[_id_ExBSyn] += pNMUpDown->iDelta*0.1;;
	CString strWeight;
	strWeight.Format( "%lg", Weights[_id_ExBSyn] );
	EditWeights[_id_ExBSyn].SetWindowText(( LPCTSTR ) strWeight );
	*pResult = 0;
}

void CWeightSetup::OnDeltaposSpinInhAWeight(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	Weights[_id_InhASyn] += pNMUpDown->iDelta*0.1;;
	CString strWeight;
	strWeight.Format( "%lg", Weights[_id_InhASyn] );
	EditWeights[_id_InhASyn].SetWindowText(( LPCTSTR ) strWeight );
	*pResult = 0;
}

void CWeightSetup::OnDeltaposSpinInhBWeight(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	Weights[_id_InhBSyn] += pNMUpDown->iDelta*0.1;;
	CString strWeight;
	strWeight.Format( "%lg", Weights[_id_InhBSyn] );
	EditWeights[_id_InhBSyn].SetWindowText(( LPCTSTR ) strWeight );
	*pResult = 0;
}

void CWeightSetup::OnUpdateEditExWeightVar() 
{
	CString strWeightVar;
	EditWeightVars[_id_ExSyn].GetWindowText( strWeightVar );
	WeightVars[_id_ExSyn] = atof(LPCTSTR(strWeightVar));
}

void CWeightSetup::OnDeltaposSpinExWeightVar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	WeightVars[_id_ExSyn] += pNMUpDown->iDelta*0.1;
	CString strWeightVar;
	strWeightVar.Format( "%lg", WeightVars[_id_ExSyn] );
	EditWeightVars[_id_ExSyn].SetWindowText(( LPCTSTR )strWeightVar);
	*pResult = 0;
}

void CWeightSetup::OnUpdateEditExWeightVar2() 
{
	CString strWeightVar;
	EditWeightVars[_id_ExBSyn].GetWindowText( strWeightVar );
	WeightVars[_id_ExBSyn] = atof(LPCTSTR(strWeightVar));
}

void CWeightSetup::OnDeltaposSpinExWeightVar2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	WeightVars[_id_ExBSyn] += pNMUpDown->iDelta*0.1;
	CString strWeightVar;
	strWeightVar.Format( "%lg", WeightVars[_id_ExBSyn] );
	EditWeightVars[_id_ExBSyn].SetWindowText(( LPCTSTR )strWeightVar);
	*pResult = 0;
}

void CWeightSetup::OnUpdateEditInhAWeightVar() 
{
	CString strWeightVar;
	EditWeightVars[_id_InhASyn].GetWindowText( strWeightVar );
	WeightVars[_id_InhASyn] = atof(LPCTSTR(strWeightVar));
}

void CWeightSetup::OnDeltaposSpinInhAWeightVar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	WeightVars[_id_InhASyn] += pNMUpDown->iDelta*0.1;
	CString strWeightVar;
	strWeightVar.Format( "%lg", WeightVars[_id_InhASyn] );
	EditWeightVars[_id_InhASyn].SetWindowText(( LPCTSTR )strWeightVar);
	*pResult = 0;
}

void CWeightSetup::OnUpdateEditInhBWeightVar() 
{
	CString strWeightVar;
	EditWeightVars[_id_InhBSyn].GetWindowText( strWeightVar );
	WeightVars[_id_InhBSyn] = atof(LPCTSTR(strWeightVar));
}

void CWeightSetup::OnDeltaposSpinInhBWeightVar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	WeightVars[_id_InhBSyn] += pNMUpDown->iDelta*0.1;
	CString strWeightVar;
	strWeightVar.Format( "%lg", WeightVars[_id_InhBSyn] );
	EditWeightVars[_id_InhBSyn].SetWindowText(( LPCTSTR )strWeightVar);
	*pResult = 0;
}

void CWeightSetup::OnUpdateEditSyn1Weight()
{
	CString strWeight;
	EditWeights[_id_Syn1].GetWindowText( strWeight );
	Weights[_id_Syn1] = atof( LPCTSTR( strWeight ));
}

void CWeightSetup::OnDeltaposSpinSyn1Weight(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	Weights[_id_Syn1] += pNMUpDown->iDelta*0.1;;
	CString strWeight;
	strWeight.Format( "%lg", Weights[_id_Syn1] );
	EditWeights[_id_Syn1].SetWindowText(( LPCTSTR ) strWeight );
	*pResult = 0;
}

void CWeightSetup::OnUpdateEditSyn1WeightVar()
{
	CString strWeightVar;
	EditWeightVars[_id_Syn1].GetWindowText( strWeightVar );
	WeightVars[_id_Syn1] = atof(LPCTSTR(strWeightVar));
}

void CWeightSetup::OnDeltaposSpinSyn1WeightVar(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	WeightVars[_id_Syn1] += pNMUpDown->iDelta*0.1;
	CString strWeightVar;
	strWeightVar.Format( "%lg", WeightVars[_id_Syn1] );
	EditWeightVars[_id_Syn1].SetWindowText(( LPCTSTR )strWeightVar);
	*pResult = 0;
}

void CWeightSetup::OnUpdateEditSyn2Weight()
{
	CString strWeight;
	EditWeights[_id_Syn2].GetWindowText( strWeight );
	Weights[_id_Syn2] = atof( LPCTSTR( strWeight ));
}

void CWeightSetup::OnDeltaposSpinSyn2Weight(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	Weights[_id_Syn2] += pNMUpDown->iDelta*0.1;;
	CString strWeight;
	strWeight.Format( "%lg", Weights[_id_Syn2] );
	EditWeights[_id_Syn2].SetWindowText(( LPCTSTR ) strWeight );
	*pResult = 0;
}

void CWeightSetup::OnUpdateEditSyn2WeightVar()
{
	CString strWeightVar;
	EditWeightVars[_id_Syn2].GetWindowText( strWeightVar );
	WeightVars[_id_Syn2] = atof(LPCTSTR(strWeightVar));
}

void CWeightSetup::OnDeltaposSpinSyn2WeightVar(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	WeightVars[_id_Syn2] += pNMUpDown->iDelta*0.1;
	CString strWeightVar;
	strWeightVar.Format( "%lg", WeightVars[_id_Syn2] );
	EditWeightVars[_id_Syn2].SetWindowText(( LPCTSTR )strWeightVar);
	*pResult = 0;
}

void CWeightSetup::OnUpdateEditSyn3Weight()
{
	CString strWeight;
	EditWeights[_id_Syn3].GetWindowText( strWeight );
	Weights[_id_Syn3] = atof( LPCTSTR( strWeight ));
}

void CWeightSetup::OnDeltaposSpinSyn3Weight(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	Weights[_id_Syn3] += pNMUpDown->iDelta*0.1;;
	CString strWeight;
	strWeight.Format( "%lg", Weights[_id_Syn3] );
	EditWeights[_id_Syn3].SetWindowText(( LPCTSTR ) strWeight );
	*pResult = 0;
}

void CWeightSetup::OnUpdateEditSyn3WeightVar()
{
	CString strWeightVar;
	EditWeightVars[_id_Syn3].GetWindowText( strWeightVar );
	WeightVars[_id_Syn3] = atof(LPCTSTR(strWeightVar));
}

void CWeightSetup::OnDeltaposSpinSyn3WeightVar(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	WeightVars[_id_Syn3] += pNMUpDown->iDelta*0.1;
	CString strWeightVar;
	strWeightVar.Format( "%lg", WeightVars[_id_Syn3] );
	EditWeightVars[_id_Syn3].SetWindowText(( LPCTSTR )strWeightVar);
	*pResult = 0;
}

void CWeightSetup::OnUpdateEditSyn4Weight()
{
	CString strWeight;
	EditWeights[_id_Syn4].GetWindowText( strWeight );
	Weights[_id_Syn4] = atof( LPCTSTR( strWeight ));
}

void CWeightSetup::OnDeltaposSpinSyn4Weight(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	Weights[_id_Syn4] += pNMUpDown->iDelta*0.1;;
	CString strWeight;
	strWeight.Format( "%lg", Weights[_id_Syn4] );
	EditWeights[_id_Syn4].SetWindowText(( LPCTSTR ) strWeight );
	*pResult = 0;
}

void CWeightSetup::OnUpdateEditSyn4WeightVar()
{
	CString strWeightVar;
	EditWeightVars[_id_Syn4].GetWindowText( strWeightVar );
	WeightVars[_id_Syn4] = atof(LPCTSTR(strWeightVar));
}

void CWeightSetup::OnDeltaposSpinSyn4WeightVar(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	WeightVars[_id_Syn4] += pNMUpDown->iDelta*0.1;
	CString strWeightVar;
	strWeightVar.Format( "%lg", WeightVars[_id_Syn4] );
	EditWeightVars[_id_Syn4].SetWindowText(( LPCTSTR )strWeightVar);
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
	if(Probability < 0.){ Probability = 0.; }
	if(Probability > 1.){ Probability = 1.; }
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
	if( index < 0 ){ return; }
	CString name;
	ConnectType.GetLBText( index, name );
	for( int type = 0; type < _id_NN_LAST_CONNECT; type++ )
		if( name == _ConnectionTypes[type] ){
			CStat ws;
			CHhnConnect weight( Connect.get_connect( type ).Connect );
			PresynMod.SetCurSel( weight.Modulate%3 );
			Probability = weight.Probability;
			for( int sid = 0; sid < _id_MAX_SYN; ++sid ){
				ws = weight.weight( sid );
				Weights[sid] = ws.Value;
				WeightVars[sid] = ws.Variance*100.;
				if( weight.Sigma[sid] ){ SynSpec[sid] = 1; }
				else if( weight.ModSigma[sid] ){ SynSpec[sid] = 2; }
				else if( weight.Linear[sid] ){ SynSpec[sid] = 3; }
				else{ SynSpec[sid] = 0; }
			}
			UpdateData( FALSE );
			break;
		}
	OldIndex = index;
}

void CWeightSetup::SetWeight( void )
{
	if( OldIndex < 0 ){ return; }
	CString name; ConnectType.GetLBText( OldIndex, name );
	for( int type = 0; type < _id_NN_LAST_CONNECT; type++ )
		if( name == _ConnectionTypes[type] ){
			CHhnConnect weight( Connect.get_connect( type ).Connect );
			weight.reset();
			weight.Modulate = PresynMod.GetCurSel();
			weight.Probability = Probability;
			for( int sid = 0; sid < _id_MAX_SYN; ++sid ){
				weight.weight( sid ) = CStat( Weights[sid], WeightVars[sid]/100.);
				switch( SynSpec[sid] ){
					case 1: 
						weight.Sigma[sid] = true;
						break;
					case 2:
						weight.ModSigma[sid] = true;
						break;
					case 3:
						weight.Linear[sid] = true;
						break;
				}
			}
			Connect.add_connect( CConnect( weight, type ), true );
			break;
		}
}

void CWeightSetup::Change( void )
{
	if( Connect.empty()){
		for( int sid = 0; sid < _id_MAX_SYN; ++sid ){
			EditWeights[sid].EnableWindow( FALSE );
			EditWeightVars[sid].EnableWindow( FALSE );
			SpinWeights[sid].EnableWindow( FALSE );
			SpinWeightVars[sid].EnableWindow( FALSE );
		}
	} else{
		for( int sid = 0; sid < _id_MAX_SYN; ++sid ){
			EditWeights[sid].EnableWindow( TRUE );
			EditWeightVars[sid].EnableWindow( TRUE );
			SpinWeights[sid].EnableWindow( TRUE );
			SpinWeightVars[sid].EnableWindow( TRUE );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CConnectionSourceSetup dialog
IMPLEMENT_DYNCREATE(CConnectionSourceSetup, CPropertyPage)

CConnectionSourceSetup::CConnectionSourceSetup()
	: CPropertyPage(CConnectionSourceSetup::IDD), pCM( NULL ), pNetwork( NULL )
{
}

CConnectionSourceSetup::~CConnectionSourceSetup()
{
}

void CConnectionSourceSetup::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectionSourceSetup)
	DDX_Control(pDX, IDC_WEIGHTS, Button_Weights);
	DDX_Control(pDX, IDC_LIST_USED_SOURCES, List_UsedSources);
	DDX_Control(pDX, IDC_LIST_TARGETS, List_Targets);
	DDX_Control(pDX, IDC_LIST_ALL_TARGETS, List_AllTargets);
	DDX_Control(pDX, IDC_EDIT_SOURCE, Edit_Source);
	DDX_Control(pDX, IDC_DELETE, Button_Delete);
	DDX_Control(pDX, IDC_COMBO_SOURCES, Combo_Source);
	DDX_Control(pDX, IDC_ADD, Button_Add);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConnectionSourceSetup, CPropertyPage)
	//{{AFX_MSG_MAP(CConnectionSourceSetup)
	ON_BN_CLICKED(IDC_ADD, &CConnectionSourceSetup::OnAdd)
	ON_BN_CLICKED(IDC_DELETE, &CConnectionSourceSetup::OnDelete)
	ON_BN_CLICKED(IDC_WEIGHTS, &CConnectionSourceSetup::OnWeights)
	ON_CBN_SELCHANGE(IDC_COMBO_SOURCES, &CConnectionSourceSetup::OnSelchangeComboSources)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TARGETS, &CConnectionSourceSetup::OnDblclkListTargets)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ALL_TARGETS, &CConnectionSourceSetup::OnDblclkListAllTargets)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectionSourceSetup message handlers
BOOL CConnectionSourceSetup::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	List_Targets.InsertColumn( 0, "");
	List_AllTargets.InsertColumn( 0, "");
	return TRUE;	// return TRUE unless you set the focus to a control
			// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CConnectionSourceSetup::OnSetActive() 
{
	assert( pCM );
	Combo_Source.ResetContent();
	for( unsigned int i = 0 ; i < pCM->size_src(); i++ ){
		Combo_Source.AddString(pCM->name_src(i).c_str());
	}
	if( pCM->size_src() > 0){
		Combo_Source.SetCurSel(0);
		CString source_name; 
		Combo_Source.GetLBText( 0, source_name );
		Edit_Source.SetWindowText(( LPCTSTR ) source_name );
	}
	ShowListUsedSources();
	ShowListTargets();
	return CPropertyPage::OnSetActive();
}

void CConnectionSourceSetup::ShowListUsedSources( void )
{
	assert( pCM );
	List_UsedSources.ResetContent();
	for( unsigned int i = 0 ; i < pCM->size_src(); i++ ){
		bool used = false;
		for( unsigned int j =0; j < pCM->size_trg(); j++){
			if(!(*pCM)(j,i).empty()) {
				used = true;
				break;
			}
		}
		if( used )
			List_UsedSources.AddString(pCM->name_src(i).c_str());
	}
}

void CConnectionSourceSetup::ShowListTargets( void )
{
	assert( pCM );
	List_AllTargets.DeleteAllItems();
	List_Targets.DeleteAllItems();
	int index = Combo_Source.GetCurSel();
	if( index == CB_ERR ) return;
	CString source_name; 
	Combo_Source.GetLBText( index, source_name );
	vector<string> all_targets = pCM->name_trg();
	for( size_t i = 0, j = 0, k = 0; i < all_targets.size(); ++i ){
		if((*pCM)( all_targets[i], ( LPCTSTR )source_name ).empty()){
			List_AllTargets.InsertItem( j, all_targets[i].c_str() );
			j++;
		}
		else{
			List_Targets.InsertItem( k, all_targets[i].c_str() );
			k++;
		}
	}
	List_Targets.SetColumnWidth(0, LVSCW_AUTOSIZE );
	List_AllTargets.SetColumnWidth(0, LVSCW_AUTOSIZE );
}

void CConnectionSourceSetup::OnAdd() 
{
	assert( pNetwork && pCM );
	int index_source = Combo_Source.GetCurSel();
	if( index_source == CB_ERR ) return;
	CString source_name; 
	CString target_name; 
	Combo_Source.GetLBText( index_source, source_name );
	int numSelected = List_AllTargets.GetSelectedCount();
	if( numSelected == 0 ) return;
	int *SelectedItems = new int [numSelected];
	POSITION pos = List_AllTargets.GetFirstSelectedItemPosition();
	if( pos != NULL ){
		int i = 0;
		for( i = 0; pos && i < numSelected; i++ ){
			SelectedItems[i] = List_AllTargets.GetNextSelectedItem( pos );
		}
		for( i = numSelected-1; i >= 0; i-- ){
			target_name = List_AllTargets.GetItemText(SelectedItems[i], 0);
			CNNConnect connect = pNetwork->get_nnunit( (LPCTSTR)target_name )->connect_type();
			pCM->connect( connect, ( LPCTSTR )target_name, ( LPCTSTR )source_name );
		}
	}
	delete []SelectedItems;
	ShowListUsedSources();
	ShowListTargets();
}

void CConnectionSourceSetup::OnDelete() 
{
	assert( pCM );
	int index_source = Combo_Source.GetCurSel();
	if( index_source == CB_ERR ) return;
	CString source_name; 
	CString target_name; 
	Combo_Source.GetLBText( index_source, source_name );
	int numSelected = List_Targets.GetSelectedCount();
	if( numSelected == 0 ) return;
	int *SelectedItems = new int [numSelected];
	POSITION pos = List_Targets.GetFirstSelectedItemPosition();
	if( pos != NULL ){
		int i = 0;
		for( i = 0; pos && i < numSelected; i++ ){
			SelectedItems[i] = List_Targets.GetNextSelectedItem( pos );
		}
		for( i = numSelected-1; i >= 0; i-- ){
			target_name = List_Targets.GetItemText(SelectedItems[i], 0);
			pCM->del(( LPCTSTR )target_name, ( LPCTSTR )source_name );
		}
	}
	delete []SelectedItems;
	ShowListUsedSources();
	ShowListTargets();
}

void CConnectionSourceSetup::OnWeights() 
{
	assert( pCM );
	int index_source = Combo_Source.GetCurSel();
	if( index_source == CB_ERR )
		return;
	CString source_name; 
	CString target_name; 
	Combo_Source.GetLBText( index_source, source_name );
	int numSelected = List_Targets.GetSelectedCount();
	if( numSelected == 0 )
		return;
	int index_target = List_Targets.GetSelectionMark();
	if( index_target < 0 )
		return;
	target_name = List_Targets.GetItemText(index_target, 0);
	CNNConnect connect = (*pCM)(( LPCTSTR )target_name, ( LPCTSTR )source_name );
	CWeightSetup dlg;
	dlg.pCM = pCM;
	dlg.Connect = connect;
	dlg.TrgName = target_name;
	dlg.SrcName = source_name;
	CString title = source_name; title += " --> "; title += target_name;
	if( dlg.DoModal( title ) == IDOK ){
		connect = dlg.Connect;
		int *SelectedItems = new int [numSelected];
		POSITION pos = List_Targets.GetFirstSelectedItemPosition();
		if( pos != NULL ){
			for( int i = 0; pos && i < numSelected; i++ ){
				SelectedItems[i] = List_Targets.GetNextSelectedItem( pos );
			}
			for( int i = 0; i< numSelected; i++ ){
				target_name = List_Targets.GetItemText(SelectedItems[i], 0);
				pCM->connect( connect, ( LPCTSTR )target_name, ( LPCTSTR )source_name );
				pCM->modify( make_pair( string(( LPCTSTR )target_name ), string(( LPCTSTR )source_name )));
			}
		}
		delete []SelectedItems;
	}
}

void CConnectionSourceSetup::OnSelchangeComboSources() 
{
	int index = Combo_Source.GetCurSel();
	if( index == CB_ERR )
		return;
	CString source_name; 
	Combo_Source.GetLBText( index, source_name );
	Edit_Source.SetWindowText(( LPCTSTR ) source_name );
	ShowListUsedSources();
	ShowListTargets();
}

void CConnectionSourceSetup::OnDblclkListTargets(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnWeights();
	*pResult = 0;
}

void CConnectionSourceSetup::OnDblclkListAllTargets(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnAdd();
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CConnectionTargetSetup property page
IMPLEMENT_DYNCREATE(CConnectionTargetSetup, CPropertyPage)

CConnectionTargetSetup::CConnectionTargetSetup() 
	: CPropertyPage(CConnectionTargetSetup::IDD), pCM( NULL ), pNetwork( NULL )
{
}

CConnectionTargetSetup::~CConnectionTargetSetup()
{
}

void CConnectionTargetSetup::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_USED_TARGETS, List_UsedTargets);
	DDX_Control(pDX, IDC_WEIGHTS, Button_Weights);
	DDX_Control(pDX, IDC_LIST_SOURCES, List_Sources);
	DDX_Control(pDX, IDC_LIST_ALL_SOURCES, List_AllSources);
	DDX_Control(pDX, IDC_EDIT_TARGET, Edit_Target);
	DDX_Control(pDX, IDC_COMBO_TARGETS, Combo_Targets);
	DDX_Control(pDX, IDC_DELETE, Button_Delete);
	DDX_Control(pDX, IDC_ADD, Button_Add);
}

BEGIN_MESSAGE_MAP(CConnectionTargetSetup, CPropertyPage)
	ON_BN_CLICKED(IDC_ADD, &CConnectionTargetSetup::OnAdd)
	ON_CBN_SELCHANGE(IDC_COMBO_TARGETS, &CConnectionTargetSetup::OnSelchangeComboTargets)
	ON_BN_CLICKED(IDC_DELETE, &CConnectionTargetSetup::OnDelete)
	ON_BN_CLICKED(IDC_WEIGHTS, &CConnectionTargetSetup::OnWeights)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SOURCES, &CConnectionTargetSetup::OnDblclkListSources)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ALL_SOURCES, &CConnectionTargetSetup::OnDblclkListAllSources)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectionTargetSetup message handlers
BOOL CConnectionTargetSetup::OnSetActive() 
{
	assert( pCM );
	Combo_Targets.ResetContent();
	for( unsigned int i = 0 ; i < pCM->size_trg(); i++ ){
		Combo_Targets.AddString(pCM->name_trg(i).c_str());
	}
	if( pCM->size_trg() > 0){
		Combo_Targets.SetCurSel(0);
		CString target_name; 
		Combo_Targets.GetLBText( 0, target_name );
		Edit_Target.SetWindowText(( LPCTSTR ) target_name );
	}
	ShowListUsedTargets();
	ShowListSources();
	return CPropertyPage::OnSetActive();
}

BOOL CConnectionTargetSetup::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	List_Sources.InsertColumn( 0, "");//, LVCFMT_LEFT, rc.Width()/4 );
	List_AllSources.InsertColumn( 0, "");//, LVCFMT_LEFT, rc.Width()/4 );
	return TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}

void CConnectionTargetSetup::ShowListUsedTargets( void )
{
	assert( pCM );
	List_UsedTargets.ResetContent();
	for( unsigned int i = 0 ; i < pCM->size_trg(); i++ ){
		bool used = false;
		for( unsigned int j =0; j < pCM->size_src(); j++){
			if(!(*pCM)(i,j).empty()) {
				used = true;
				break;
			}
		}
		if( used )
			List_UsedTargets.AddString(pCM->name_trg(i).c_str());
	}
}

void CConnectionTargetSetup::ShowListSources( void )
{
	assert( pCM );
	List_AllSources.DeleteAllItems();
	List_Sources.DeleteAllItems();
	int index = Combo_Targets.GetCurSel();
	if( index == CB_ERR ) return;
	CString target_name; 
	Combo_Targets.GetLBText( index, target_name );
	vector<string> all_sources = pCM->name_src();
	for( size_t i = 0, j = 0, k = 0; i < all_sources.size(); ++i ){
		if((*pCM)( ( LPCTSTR )target_name, all_sources[i] ).empty()){
			List_AllSources.InsertItem( j, all_sources[i].c_str() );
			j++;
		}
		else{
			List_Sources.InsertItem( k, all_sources[i].c_str() );
			k++;
		}
	}
	List_Sources.SetColumnWidth(0, LVSCW_AUTOSIZE );
	List_AllSources.SetColumnWidth(0, LVSCW_AUTOSIZE );
}

void CConnectionTargetSetup::OnAdd() 
{
	assert( pNetwork && pCM);
	int index_target = Combo_Targets.GetCurSel();
	if( index_target == CB_ERR ) return;
	CString source_name; 
	CString target_name; 
	Combo_Targets.GetLBText( index_target, target_name );
	CNNConnect connect = pNetwork->get_nnunit( (LPCTSTR)target_name )->connect_type();
	int numSelected = List_AllSources.GetSelectedCount();
	if( numSelected == 0 ) return;
	int *SelectedItems = new int [numSelected];
	POSITION pos = List_AllSources.GetFirstSelectedItemPosition();
	if( pos != NULL ){
		int i = 0;
		for( i = 0; pos && i < numSelected; i++ ){
			SelectedItems[i] = List_AllSources.GetNextSelectedItem( pos );
		}
		for( i = numSelected-1; i >= 0; i-- ){
			source_name = List_AllSources.GetItemText(SelectedItems[i], 0);
			pCM->connect( connect, ( LPCTSTR )target_name, ( LPCTSTR )source_name );
		}
	}
	delete []SelectedItems;
	ShowListUsedTargets();	
	ShowListSources();
}

void CConnectionTargetSetup::OnDelete() 
{
	assert( pCM );
	int index_target = Combo_Targets.GetCurSel();
	if( index_target == CB_ERR ) return;
	CString source_name; 
	CString target_name; 
	Combo_Targets.GetLBText( index_target, target_name );
	int numSelected = List_Sources.GetSelectedCount();
	if( numSelected == 0 ) return;
	int *SelectedItems = new int [numSelected];
	POSITION pos = List_Sources.GetFirstSelectedItemPosition();
	if( pos != NULL ){
		int i = 0;
		for( i = 0; pos && i < numSelected; i++ ){
			SelectedItems[i] = List_Sources.GetNextSelectedItem( pos );
		}
		for( i = numSelected-1; i >= 0; i-- ){
			source_name = List_Sources.GetItemText(SelectedItems[i], 0);
			pCM->del(( LPCTSTR )target_name, ( LPCTSTR )source_name );
		}
	}
	delete []SelectedItems;
	ShowListUsedTargets();		
	ShowListSources();
}

void CConnectionTargetSetup::OnWeights() 
{
	assert( pCM );
	int index_target = Combo_Targets.GetCurSel();
	if( index_target == CB_ERR )
		return;
	CString source_name; 
	CString target_name; 
	Combo_Targets.GetLBText( index_target, target_name );
	int numSelected = List_Sources.GetSelectedCount();
	if( numSelected == 0 )
		return;
	int index_source = List_Sources.GetSelectionMark();
	if( index_source < 0 )
		return;
	source_name = List_Sources.GetItemText( index_source, 0 );
	CNNConnect connect = (*pCM)(( LPCTSTR )target_name, ( LPCTSTR )source_name );
	CWeightSetup dlg;
	dlg.pCM = pCM;
	dlg.Connect = connect;
	dlg.TrgName = target_name;
	dlg.SrcName = source_name;
	CString title = target_name; title += " <-- "; title += source_name;
	if( dlg.DoModal( title ) == IDOK ){
		connect = dlg.Connect;
		int *SelectedItems = new int [numSelected];
		POSITION pos = List_Sources.GetFirstSelectedItemPosition();
		if( pos != NULL ){
			int i = 0;
			for( i = 0; pos && i < numSelected; i++ ){
				SelectedItems[i] = List_Sources.GetNextSelectedItem( pos );
			}
			for( i = 0; i< numSelected; i++ ){
				pCM->connect( connect, ( LPCTSTR )target_name, ( LPCTSTR )source_name );
				pCM->modify( make_pair( string(( LPCTSTR )target_name ), string(( LPCTSTR )source_name )));
				source_name = List_Sources.GetItemText(SelectedItems[i], 0);
			}
		}
		delete []SelectedItems;
	}	
}

void CConnectionTargetSetup::OnSelchangeComboTargets() 
{
	int index = Combo_Targets.GetCurSel();
	if( index == CB_ERR ) return;
	CString target_name; 
	Combo_Targets.GetLBText( index, target_name );
	Edit_Target.SetWindowText(( LPCTSTR ) target_name );
	ShowListUsedTargets();
	ShowListSources();
}

void CConnectionTargetSetup::OnDblclkListSources(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnWeights();
	*pResult = 0;
}

void CConnectionTargetSetup::OnDblclkListAllSources(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnAdd();	
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CConnectionSetup
IMPLEMENT_DYNAMIC(CConnectionSetup, CPropertySheet)

CConnectionSetup::CConnectionSetup(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage), pNetwork( NULL )
{
	ConnectionSourceSetup.pCM = &CM;
	ConnectionTargetSetup.pCM = &CM;
	AddPage( &ConnectionSourceSetup );
	AddPage( &ConnectionTargetSetup );
}

CConnectionSetup::CConnectionSetup(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage), pNetwork( NULL )
{
	ConnectionSourceSetup.pCM = &CM;
	ConnectionTargetSetup.pCM = &CM;
	AddPage( &ConnectionSourceSetup );
	AddPage( &ConnectionTargetSetup );
}

CConnectionSetup::~CConnectionSetup()
{
}

/////////////////////////////////////////////////////////////////////////////
// CConnectionSetup message handlers
int CConnectionSetup::DoModal() 
{
	ConnectionSourceSetup.pNetwork = pNetwork;
	ConnectionTargetSetup.pNetwork = pNetwork;
	return CPropertySheet::DoModal();
}

#endif // __CONSOLE__
