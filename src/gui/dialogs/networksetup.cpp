/////////////////////////////////////////////////////////////////////////////
// networksetup.cpp : implementation file
#include "precompile.h"
#include "networksetup.h"
#if defined (__MECHANICS_2D__)
#include "biomtemplate.h"
#elif defined (__MECHANICS_3D__)
// TODO implementation 3d model
#endif //__MECHANICS_2D__

#ifndef __CONSOLE__

#include "../Spcord.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
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
// CNetworkParam property page
IMPLEMENT_DYNCREATE(CNetworkParam, CPropertyPage)

CNetworkParam::CNetworkParam(void)
	: CPropertyPage(CNetworkParam::IDD)
{
}

void CNetworkParam::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID, NetParView );
}

BEGIN_MESSAGE_MAP(CNetworkParam, CPropertyPage)
	ON_NOTIFY( NM_CLICK, IDC_GRID, &CNetworkParam::OnGridClick )
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, &CNetworkParam::OnEndCellEdit )
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
	} else{	UpdateView( true ); }
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
		} else{ OnFlyUpdate( parent_path ); }
	}
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

bool CNetworkParam::DisplayUnit( uni_template *unit, const string &start_path, bool collapse )
{
	if( unit ){
		bool is_success = DisplayUnitPar( unit, start_path );
		return  DisplayUnitChildren( unit, start_path, collapse ) || is_success;
	}
	return false;
}

bool CNetworkParam::DisplayUnitPar( uni_template *unit, const string &start_path )
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

bool CNetworkParam::DisplayUnitChildren( uni_template *unit, const string &start_path, bool collapse )
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
	ON_BN_CLICKED(IDC_ADD_POPULAT, &CPopulatSetup::OnAddPopulat)
	ON_BN_CLICKED(IDC_DEL_POPULAT, &CPopulatSetup::OnDelPopulat)
	ON_BN_CLICKED(IDC_PROP_POPULAT, &CPopulatSetup::OnPropPolulat)
	ON_LBN_DBLCLK(IDC_LIST_POPULAT, &CPopulatSetup::OnDblclkListPopulat)
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
	ON_BN_CLICKED(IDC_ADD_DRIVE, &CDriveSetup::OnAddDrive)
	ON_BN_CLICKED(IDC_DEL_DRIVE, &CDriveSetup::OnDelDrive)
	ON_BN_CLICKED(IDC_PROP_DRIVE, &CDriveSetup::OnPropDrive)
	ON_LBN_DBLCLK(IDC_LIST_DRIVE, &CDriveSetup::OnDblclkListDrive)
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
	if( i == -1 ){ return; }
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
	, Name( _T("") )
	, T1( 0 )
	, T2( 0 )
	, Threshold( 0 )
	, Bias( 0 )
	, SlpA( 0 )
	, PowA( 0 )
	, SlpT( 0 )
	, PowT( 0 )
{
}

void COutputProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT_OUTPUT_NAME, Name );

	DDX_Control( pDX, IDC_EDIT_THRESHOLD, EditThreshold );
	DDX_Control( pDX, IDC_EDIT_T2, EditT2 );
	DDX_Control( pDX, IDC_EDIT_T1, EditT1 );
	DDX_Control( pDX, IDC_EDIT_BIAS, EditBias );
	DDX_Control( pDX, IDC_EDIT_SLOPEA, EditSlpA );
	DDX_Control( pDX, IDC_EDIT_POWA, EditPowA );
	DDX_Control( pDX, IDC_EDIT_SLOPET, EditSlpT );
	DDX_Control( pDX, IDC_EDIT_POWT, EditPowT );
	DDX_Control( pDX, IDC_EDIT_CAP, EditCap );

	DDX_Control( pDX, IDC_SPIN_T2, SpinT2 );
	DDX_Control( pDX, IDC_SPIN_T1, SpinT1 );
	DDX_Control( pDX, IDC_SPIN_THRESHOLD, SpinThreshold );
	DDX_Control( pDX, IDC_SPIN_BIAS, SpinBias );
	DDX_Control( pDX, IDC_SPIN_SLOPEA, SpinSlpA );
	DDX_Control( pDX, IDC_SPIN_POWA, SpinPowA );
	DDX_Control( pDX, IDC_SPIN_SLOPET, SpinSlpT );
	DDX_Control( pDX, IDC_SPIN_POWT, SpinPowT );
	DDX_Control( pDX, IDC_SPIN_CAP, SpinCap );

	DDX_Text( pDX, IDC_EDIT_T1, T1 );
	DDX_Text( pDX, IDC_EDIT_T2, T2 );
	DDX_Text( pDX, IDC_EDIT_THRESHOLD, Threshold );
	DDX_Text( pDX, IDC_EDIT_BIAS, Bias );
	DDX_Text( pDX, IDC_EDIT_SLOPEA, SlpA );
	DDX_Text( pDX, IDC_EDIT_POWA, PowA );
	DDX_Text( pDX, IDC_EDIT_SLOPET, SlpT );
	DDX_Text( pDX, IDC_EDIT_POWT, PowT );
	DDX_Text( pDX, IDC_EDIT_CAP, Cap );
}

BEGIN_MESSAGE_MAP(COutputProp, CDialog)
	ON_EN_UPDATE(IDC_EDIT_T1, &COutputProp::OnUpdateEditT1)
	ON_EN_UPDATE(IDC_EDIT_T2, &COutputProp::OnUpdateEditT2)
	ON_EN_UPDATE(IDC_EDIT_THRESHOLD, &COutputProp::OnUpdateEditThreshold)
	ON_EN_UPDATE(IDC_EDIT_BIAS, &COutputProp::OnUpdateBias)
	ON_EN_UPDATE(IDC_EDIT_SLOPEA, &COutputProp::OnUpdateSlpA)
	ON_EN_UPDATE(IDC_EDIT_POWA, &COutputProp::OnUpdatePowA)
	ON_EN_UPDATE(IDC_EDIT_SLOPET, &COutputProp::OnUpdateSlpT)
	ON_EN_UPDATE(IDC_EDIT_POWT, &COutputProp::OnUpdatePowT)
	ON_EN_UPDATE(IDC_EDIT_CAP, &COutputProp::OnUpdateCap)

	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_T1, &COutputProp::OnDeltaposSpinT1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_T2, &COutputProp::OnDeltaposSpinT2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_THRESHOLD, &COutputProp::OnDeltaposSpinThreshold)

	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BIAS, &COutputProp::OnDeltaposSpinBias)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SLOPEA, &COutputProp::OnDeltaposSpinSlpA)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_POWA, &COutputProp::OnDeltaposSpinPowA)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SLOPET, &COutputProp::OnDeltaposSpinSlpT)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_POWT, &COutputProp::OnDeltaposSpinPowT)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CAP, &COutputProp::OnDeltaposSpinCap)
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
	SpinThreshold.SetBuddy(&EditThreshold);
	SpinThreshold.SetRange(0,UD_MAXVAL);
	SpinBias.SetBuddy(&EditBias);
	SpinBias.SetRange(0,UD_MAXVAL);
	SpinSlpA.SetBuddy(&EditSlpA);
	SpinSlpA.SetRange(0,UD_MAXVAL);
	SpinPowA.SetBuddy(&EditPowA);
	SpinPowA.SetRange(0,UD_MAXVAL);
	SpinSlpT.SetBuddy(&EditSlpT);
	SpinSlpT.SetRange(0,UD_MAXVAL);
	SpinPowT.SetBuddy(&EditPowT);
	SpinPowT.SetRange(0,UD_MAXVAL);
	SpinCap.SetBuddy(&EditCap);
	SpinCap.SetRange(0,UD_MAXVAL);

	UpdateData();
	return TRUE;
}

void COutputProp::OnUpdateEditT1() 
{
	CString strT1;
	EditT1.GetWindowText( strT1 );
	T1 = ( T1 < 0. )? 0.: T1;
	T1 = atof( LPCTSTR( strT1 ));
}

void COutputProp::OnUpdateEditT2() 
{
	CString strT2;
	EditT2.GetWindowText( strT2 );
	T2 = ( T2 < 0. )? 0.: T2;
	T2 = atof( LPCTSTR( strT2 ));
}

void COutputProp::OnUpdateEditThreshold() 
{
	CString str;
	EditThreshold.GetWindowText( str );
	Threshold = atof( LPCTSTR( str ));
}

void COutputProp::OnUpdateBias()
{
	CString str;
	EditBias.GetWindowText( str );
	Bias = atof( LPCTSTR( str ));
}

void COutputProp::OnUpdateSlpA()
{
	CString str;
	EditSlpA.GetWindowText( str );
	SlpA = atof( LPCTSTR( str ));
}

void COutputProp::OnUpdatePowA()
{
	CString str;
	EditPowA.GetWindowText( str );
	PowA = atof( LPCTSTR( str ));
}

void COutputProp::OnUpdateSlpT()
{
	CString str;
	EditSlpT.GetWindowText( str );
	SlpT = atof( LPCTSTR( str ));
}

void COutputProp::OnUpdatePowT()
{
	CString str;
	EditPowT.GetWindowText( str );
	PowT = atof( LPCTSTR( str ));
}

void COutputProp::OnUpdateCap()
{
	CString str;
	EditCap.GetWindowText( str );
	Cap = atof( LPCTSTR( str ));
}

void COutputProp::OnDeltaposSpinT1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	T1 += pNMUpDown->iDelta*0.1;
	T1 = ( T1 < 0. )? 0.: T1;
	CString strT1;
	strT1.Format( "%lg", T1 );
	EditT1.SetWindowText(( LPCTSTR ) strT1 );
	*pResult = 0;
}

void COutputProp::OnDeltaposSpinT2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	T2 += pNMUpDown->iDelta*0.1;
	T2 = ( T2 < 0. )? 0.: T2;
	CString strT2;
	strT2.Format( "%lg", T2 );
	EditT2.SetWindowText(( LPCTSTR ) strT2 );
	*pResult = 0;
}

void COutputProp::OnDeltaposSpinThreshold(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	Threshold += pNMUpDown->iDelta*0.1;
	CString str;
	str.Format( "%lg", Threshold );
	EditThreshold.SetWindowText( LPCTSTR( str ));
	*pResult = 0;
}

void COutputProp::OnDeltaposSpinBias(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	Bias += pNMUpDown->iDelta*0.1;
	CString str;
	str.Format( "%lg", Bias );
	EditBias.SetWindowText( LPCTSTR( str ));
	*pResult = 0;
}

void COutputProp::OnDeltaposSpinSlpA(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	SlpA += pNMUpDown->iDelta*0.1;
	CString str;
	str.Format( "%lg", SlpA );
	EditSlpA.SetWindowText( LPCTSTR( str ));
	*pResult = 0;
}

void COutputProp::OnDeltaposSpinPowA(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	PowA += pNMUpDown->iDelta*0.1;
	CString str;
	str.Format( "%lg", PowA );
	EditPowA.SetWindowText( LPCTSTR( str ));
	*pResult = 0;
}

void COutputProp::OnDeltaposSpinSlpT(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	SlpT += pNMUpDown->iDelta*0.1;
	CString str;
	str.Format( "%lg", SlpT );
	EditSlpT.SetWindowText( LPCTSTR( str ));
	*pResult = 0;
}

void COutputProp::OnDeltaposSpinPowT(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	PowT += pNMUpDown->iDelta*0.1;
	CString str;
	str.Format( "%lg", PowT );
	EditPowT.SetWindowText( LPCTSTR( str ));
	*pResult = 0;
}

void COutputProp::OnDeltaposSpinCap(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	Cap += pNMUpDown->iDelta*0.1;
	Cap = ( Cap < 0. )? 0.: Cap;
	CString str;
	str.Format( "%lg", Cap );
	EditCap.SetWindowText( LPCTSTR( str ));
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
	ON_BN_CLICKED(IDC_ADD_OUTPUT, &COutputNetwork::OnAddOutput)
	ON_BN_CLICKED(IDC_DEL_OUTPUT, &COutputNetwork::OnDelOutput)
	ON_BN_CLICKED(IDC_PROP_OUTPUT, &COutputNetwork::OnPropOutput)
	ON_LBN_DBLCLK(IDC_LIST_OUTPUT, &COutputNetwork::OnDblclkListOutput)
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
	dlg.Bias = output.bias();
	dlg.SlpA = output.slp_a();
	dlg.PowA = output.pow_a();
	dlg.SlpT = output.slp_t();
	dlg.PowT = output.pow_t();
	dlg.Cap = output.cap();
	if( dlg.DoModal() == IDOK ){
		output.set_name(( LPCTSTR )dlg.Name );
		output.tup() = dlg.T1;
		output.tdown() = dlg.T2;
		output.threshold() = dlg.Threshold;
		output.bias() = dlg.Bias;
		output.slp_a() = dlg.SlpA;
		output.pow_a() = dlg.PowA;
		output.slp_t() = dlg.SlpT;
		output.pow_t() = dlg.PowT;
		output.cap() = dlg.Cap;
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
	} else{
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
	for( size_t i = 0 ; i < Network->size_out(); i++ ){
		ListOutput.AddString( Network->get_out(i).get_name().c_str() );
	}
	ListOutput.SetCurSel( index );
}

BOOL COutputNetwork::OnSetActive() 
{
	Change();
	return CPropertyPage::OnSetActive();
}

#if defined (__MECHANICS_2D__)
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
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &CFBProp::OnSelchangeComboType)
	ON_CBN_SELCHANGE(IDC_COMBO_LISTFB, &CFBProp::OnSelchangeListFB)
	ON_EN_UPDATE(IDC_KV, &CFBProp::OnUpdateKV)
	ON_EN_UPDATE(IDC_PV, &CFBProp::OnUpdatePV)
	ON_EN_UPDATE(IDC_KL, &CFBProp::OnUpdateKL)
	ON_EN_UPDATE(IDC_KEMG, &CFBProp::OnUpdateKEMG)
	ON_EN_UPDATE(IDC_KF, &CFBProp::OnUpdateKF)
	ON_EN_UPDATE(IDC_KFTR, &CFBProp::OnUpdateKFTR)
	ON_EN_UPDATE(IDC_KC, &CFBProp::OnUpdateKC)
	ON_EN_UPDATE(IDC_KGR, &CFBProp::OnUpdateKG)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_KV, &CFBProp::OnDeltaposSpinKV)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PV, &CFBProp::OnDeltaposSpinPV)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_KL, &CFBProp::OnDeltaposSpinKL)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_KEMG, &CFBProp::OnDeltaposSpinKEMG)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_KF, &CFBProp::OnDeltaposSpinKF)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_KFTR, &CFBProp::OnDeltaposSpinKFTR)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_KC, &CFBProp::OnDeltaposSpinKC)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_KGR, &CFBProp::OnDeltaposSpinKG)
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
	ON_BN_CLICKED(IDC_DEL_FEEDBACK, &CFeedbackSetup::OnDelFeedback)
	ON_BN_CLICKED(IDC_ADD_FEEDBACK, &CFeedbackSetup::OnAddFeedback)
	ON_BN_CLICKED(IDC_PROP_FEEDBACK, &CFeedbackSetup::OnPropFeedback)
	ON_LBN_DBLCLK(IDC_LIST_FEEDBACK, &CFeedbackSetup::OnDblclkListFeedback)
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
#elif defined (__MECHANICS_3D__)
// TODO implementation 3d model
#endif // __MECHANICS_2D__

/////////////////////////////////////////////////////////////////////////////
// CNetworkSetup
IMPLEMENT_DYNAMIC(CNetworkSetup, CPropertySheet)

CNetworkSetup::CNetworkSetup(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
#if defined (__MECHANICS_2D__)
	, BiomT( NULL )
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
{
	AddPage( &NetworkParam );
	AddPage( &PopulatSetup );
	AddPage( &DriveSetup );
	AddPage( &OutputSetup );
	NetworkParam.Network = &Network;
	PopulatSetup.Network = &Network;
	DriveSetup.Network = &Network;
	OutputSetup.Network = &Network;
#if defined (__MECHANICS_2D__)
	AddPage( &FeedbackSetup );
	FeedbackSetup.Network = &Network;
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
}

CNetworkSetup::CNetworkSetup(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
#if defined (__MECHANICS_2D__)
	, BiomT( NULL )
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
{
	AddPage( &NetworkParam );
	AddPage( &PopulatSetup );
	AddPage( &DriveSetup );
	AddPage( &OutputSetup );
	NetworkParam.Network = &Network;
	PopulatSetup.Network = &Network;
	DriveSetup.Network = &Network;
	OutputSetup.Network = &Network;
#if defined (__MECHANICS_2D__)
	AddPage( &FeedbackSetup );
	FeedbackSetup.Network = &Network;
	FeedbackSetup.BiomT = &BiomT;
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
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
