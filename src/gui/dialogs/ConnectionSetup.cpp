/////////////////////////////////////////////////////////////////////////////
// ConnectionSetup.cpp : implementation file
#include "precompile.h"

#ifndef __CONSOLE__

#include "ConnectionSetup.h"
#include "neuronsetup.h"
#include "networksetup.h"
#include "../spcord.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CConnectionSourceSetup dialog
IMPLEMENT_DYNCREATE(CConnectionSourceSetup, CPropertyPage)

CConnectionSourceSetup::CConnectionSourceSetup()
	: CPropertyPage(CConnectionSourceSetup::IDD)
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
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_WEIGHTS, OnWeights)
	ON_CBN_SELCHANGE(IDC_COMBO_SOURCES, OnSelchangeComboSources)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TARGETS, OnDblclkListTargets)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ALL_TARGETS, OnDblclkListAllTargets)
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
		for( i = 0; pos; i++ )
			SelectedItems[i] = List_AllTargets.GetNextSelectedItem( pos );
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
		for( i = 0; pos; i++ )
			SelectedItems[i] = List_Targets.GetNextSelectedItem( pos );
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
			for( int i = 0; pos; i++ )
				SelectedItems[i] = List_Targets.GetNextSelectedItem( pos );
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

CConnectionTargetSetup::CConnectionTargetSetup() : CPropertyPage(CConnectionTargetSetup::IDD)
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
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_CBN_SELCHANGE(IDC_COMBO_TARGETS, OnSelchangeComboTargets)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_WEIGHTS, OnWeights)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SOURCES, OnDblclkListSources)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ALL_SOURCES, OnDblclkListAllSources)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectionTargetSetup message handlers
BOOL CConnectionTargetSetup::OnSetActive() 
{
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
		for( i = 0; pos; i++ )
			SelectedItems[i] = List_AllSources.GetNextSelectedItem( pos );
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
		for( i = 0; pos; i++ )
			SelectedItems[i] = List_Sources.GetNextSelectedItem( pos );
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
			for( i = 0; pos != NULL; i++ )
				SelectedItems[i] = List_Sources.GetNextSelectedItem( pos );
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
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	ConnectionSourceSetup.pCM = &CM;
	ConnectionTargetSetup.pCM = &CM;
	AddPage( &ConnectionSourceSetup );
	AddPage( &ConnectionTargetSetup );
}

CConnectionSetup::CConnectionSetup(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	ConnectionSourceSetup.pCM = &CM;
	ConnectionTargetSetup.pCM = &CM;
	AddPage( &ConnectionSourceSetup );
	AddPage( &ConnectionTargetSetup );
}

CConnectionSetup::~CConnectionSetup()
{
}

BEGIN_MESSAGE_MAP(CConnectionSetup, CPropertySheet)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectionSetup message handlers
int CConnectionSetup::DoModal() 
{
	ConnectionSourceSetup.pNetwork = pNetwork;
	ConnectionTargetSetup.pNetwork = pNetwork;
	return CPropertySheet::DoModal();
}

#endif // __CONSOLE__
