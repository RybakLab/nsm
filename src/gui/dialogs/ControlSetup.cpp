/////////////////////////////////////////////////////////////////////////////
// ControlSetup.cpp : implementation file
#include "precompile.h"

#ifndef __CONSOLE__

#include "ControlSetup.h"
#include "networksetup.h"
#include "../Spcord.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CControlledSetup dialog
IMPLEMENT_DYNCREATE(CControlledSetup, CPropertyPage)

CControlledSetup::CControlledSetup() : CPropertyPage(CControlledSetup::IDD)
{
	pNetwork = NULL;
	pControl = NULL;
}

CControlledSetup::~CControlledSetup()
{
}

void CControlledSetup::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONTROLLED_PARAM, List_Controlled);
	DDX_Control(pDX, IDC_CONTROLLED_TREE, Tree_Controlled);
	DDX_Control(pDX, IDC_REMOVE_PARAM, Button_RemoveParam);
	DDX_Control(pDX, IDC_ADD_PARAM, Button_AddParam);
}

BEGIN_MESSAGE_MAP( CControlledSetup, CDialog )
	ON_BN_CLICKED( IDC_ADD_PARAM, &CControlledSetup::OnAddParam )
	ON_BN_CLICKED( IDC_REMOVE_PARAM, &CControlledSetup::OnRemoveParam )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlledSetup message handlers
BOOL CControlledSetup::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	if( !pNetwork )
		return TRUE;
	unit_code code;
	HTREEITEM mainItems[5];
	// Network
	code.UnitId = _id_Network; code.NNIndex = 0;
	mainItems[0] = Tree_Controlled.InsertItem( pNetwork->get_name().c_str(), 0, TVI_ROOT );
	Tree_Controlled.SetItemData( mainItems[0], code.encode() );
	HTREEITEM paramItem[_id_NUM_CONTROLLED];
	code.Param = _id_KIons;
	paramItem[0] = Tree_Controlled.InsertItem( "KIons (Outside)", mainItems[0], mainItems[0] );
	Tree_Controlled.SetItemData( paramItem[0], code.encode() );
	code.Param = -1;
	paramItem[1] = Tree_Controlled.InsertItem( "Synaptic conductances", mainItems[0], mainItems[0] );
	Tree_Controlled.SetItemData( paramItem[1], code.encode() );
	for( size_t i = 0; i < _id_MAX_SYN; ++i ){
		code.Param = _id_MAX_CHAN+i;
		paramItem[i+2] = Tree_Controlled.InsertItem( _SynapseNames[i], paramItem[1], paramItem[i+1] );
		Tree_Controlled.SetItemData( paramItem[i+2], code.encode() );
	}
	// Populations
	code = unit_code();
	code.UnitId = _id_NNPopulat;
	code.NNIndex = 0;
	mainItems[1] = Tree_Controlled.InsertItem("Populations", 0, TVI_ROOT);
	Tree_Controlled.SetItemData(mainItems[1], code.encode() );
	// Drives
	code.UnitId = _id_NNDrive;
	mainItems[2] = Tree_Controlled.InsertItem("External Drives", 0, TVI_ROOT);
	Tree_Controlled.SetItemData(mainItems[2], code.encode() );
	// Outputs
	code.UnitId = _id_NNOutput;
	mainItems[3] = Tree_Controlled.InsertItem("Outputs", 0, TVI_ROOT);
	Tree_Controlled.SetItemData(mainItems[3], code.encode() );
#if defined (__MECHANICS_2D__)
	// Feedbacks
	code = unit_code();
	code.UnitId = _id_NNFeedback;
	code.NNIndex = 0;
	mainItems[4] = Tree_Controlled.InsertItem("Feedbacks", 0, TVI_ROOT);
	Tree_Controlled.SetItemData(mainItems[4], code.encode() );
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	// Populations
	if( pNetwork->size_pop() ){
		HTREEITEM *items = new HTREEITEM[pNetwork->size_pop()];
		HTREEITEM pitems[3];

		code = unit_code();
		code.UnitId = _id_NNPopulat;
		code.NNIndex = 0;
		items[0] = Tree_Controlled.InsertItem(pNetwork->get_pop(0).get_name().c_str(), mainItems[1], mainItems[1] );
		Tree_Controlled.SetItemData(items[0], code.encode());
		for( size_t i = 1; i < min( pNetwork->size_pop(), 255 ); ++i ){
			code.NNIndex = i;
			items[i] = Tree_Controlled.InsertItem( pNetwork->get_pop(i).get_name().c_str(), mainItems[1], items[i-1] );
			Tree_Controlled.SetItemData(items[i], code.encode()); 	
		}
		for( size_t i = 0; i < min( pNetwork->size_pop(), 255 ); ++i ){
			code.NNIndex = i; code.Param = -1;
			pitems[0] = Tree_Controlled.InsertItem( "Synaptic conductances", items[i], items[i] );
			Tree_Controlled.SetItemData( pitems[0], code.encode() );

			code.Param = _id_MAX_CHAN;
			paramItem[1] = Tree_Controlled.InsertItem( _SynapseNames[0], pitems[0], pitems[0] );
			Tree_Controlled.SetItemData( paramItem[1], code.encode());
			for( size_t j = 1; j < _id_MAX_SYN; ++j ){
				code.Param = _id_MAX_CHAN+j;
				paramItem[j+1] = Tree_Controlled.InsertItem( _SynapseNames[j], pitems[0], paramItem[j] );
				Tree_Controlled.SetItemData( paramItem[j+1], code.encode() );
			}

//	conductance settings
			code.Param = -1;
			code.PartIndex = -1;
			pitems[1] = Tree_Controlled.InsertItem( "Channel conductances", items[i], items[i] );
			Tree_Controlled.SetItemData( pitems[1], code.encode() );
			const hhn_compart *cmp = ( pNetwork->get_pop( i ))[0].get_compart( 0 );
			int index = (* cmp )[0]->get_chanid();
			if( index == _id_generic_Chan ){ // new code 4 control
				index = (* cmp )[0]->get_gchanid();
				code.Param = _id_generic_Chan+_id_NUM_CONTROLLED+index;
				paramItem[_id_MAX_SYN] = Tree_Controlled.InsertItem( pNetwork->TUniList.getname( index ).c_str(), pitems[1], pitems[1] );
			}
			else{
				code.Param = _id_generic_Chan+index;
				paramItem[_id_MAX_SYN] = Tree_Controlled.InsertItem( _ChannelNames[_id_generic_Chan+index], pitems[1], pitems[1] );
			}
			Tree_Controlled.SetItemData( paramItem[_id_MAX_SYN], code.encode() );
			for( size_t j = 1; j < min((* cmp ).size(),63); j++){
			    index = (* cmp )[j]->get_chanid(); 
				if( index == _id_generic_Chan ){
					index = (* cmp )[j]->get_gchanid();
					code.Param = _id_generic_Chan+_id_NUM_CONTROLLED+index;
					paramItem[j+1+_id_MAX_SYN] = Tree_Controlled.InsertItem( pNetwork->TUniList.getname( index ).c_str(),  pitems[1], paramItem[j+_id_MAX_SYN]);
				}
				else{
					code.Param = _id_generic_Chan+index;
					paramItem[j+1+_id_MAX_SYN] = Tree_Controlled.InsertItem( _ChannelNames[_id_generic_Chan+index],  pitems[1], paramItem[j+_id_MAX_SYN]);
				}
				Tree_Controlled.SetItemData( paramItem[j+1+_id_MAX_SYN], code.encode() );
			}
//	reversasl potentials settings
			code.Param = -1;
			code.PartIndex = 0;
			pitems[2] = Tree_Controlled.InsertItem( "Channel reversasl potentials", items[i], items[i] );
			Tree_Controlled.SetItemData( pitems[2], code.encode() );
			index = (* cmp )[0]->get_chanid();
			if( index == _id_generic_Chan ){ // new code 4 control
				index = (* cmp )[0]->get_gchanid();
				code.Param = _id_generic_Chan+_id_NUM_CONTROLLED+index;
				paramItem[_id_MAX_SYN] = Tree_Controlled.InsertItem( pNetwork->TUniList.getname( index ).c_str(), pitems[2], pitems[2] );
			}
			else{
				code.Param = _id_generic_Chan+index;
				paramItem[_id_MAX_SYN] = Tree_Controlled.InsertItem( _ChannelNames[_id_generic_Chan+index], pitems[2], pitems[2] );
			}
			Tree_Controlled.SetItemData( paramItem[_id_MAX_SYN], code.encode() );
			for( size_t j = 1; j < min((* cmp ).size(),63); j++){
			    index = (* cmp )[j]->get_chanid(); 
				if( index == _id_generic_Chan ){
					index = (* cmp )[j]->get_gchanid();
					code.Param = _id_generic_Chan+_id_NUM_CONTROLLED+index;
					paramItem[j+1+_id_MAX_SYN] = Tree_Controlled.InsertItem( pNetwork->TUniList.getname( index ).c_str(),  pitems[2], paramItem[j+_id_MAX_SYN]);
				}
				else{
					code.Param = _id_generic_Chan+index;
					paramItem[j+1+_id_MAX_SYN] = Tree_Controlled.InsertItem( _ChannelNames[_id_generic_Chan+index],  pitems[2], paramItem[j+_id_MAX_SYN]);
				}
				Tree_Controlled.SetItemData( paramItem[j+1+_id_MAX_SYN], code.encode() );
			}
			code.PartIndex = -1;
		}
		delete []items;
	}
	// Drive
	if( pNetwork->size_drv() ){
		HTREEITEM *items = new HTREEITEM[pNetwork->size_drv()];
		code = unit_code(); code.UnitId = _id_NNDrive; code.NNIndex = 0; code.Param = 0;
		items[0] = Tree_Controlled.InsertItem( pNetwork->get_drv(0).get_name().c_str(), mainItems[2], mainItems[2] );
		Tree_Controlled.SetItemData( items[0], code.encode() );
		for( size_t i = 1; i < min( pNetwork->size_drv(), 255 ); ++i ){
			code.NNIndex = i;
			items[i] = Tree_Controlled.InsertItem( pNetwork->get_drv(i).get_name().c_str(), mainItems[2], items[i-1] );
			Tree_Controlled.SetItemData( items[i], code.encode() ); 	
		}
		delete []items;
	}
	// Output
	if( pNetwork->size_out() ){
		HTREEITEM *items = new HTREEITEM[pNetwork->size_out()];
		code = unit_code(); code.UnitId = _id_NNOutput; code.NNIndex = 0; code.Param = 0;
		items[0] = Tree_Controlled.InsertItem( pNetwork->get_out(0).get_name().c_str(), mainItems[3], mainItems[3] );
		Tree_Controlled.SetItemData( items[0], code.encode() );
		for( size_t i = 1; i < min( pNetwork->size_out(), 255 ); ++i ){
			code.NNIndex = i;
			items[i] = Tree_Controlled.InsertItem( pNetwork->get_out(i).get_name().c_str(), mainItems[3], items[i-1] );
			Tree_Controlled.SetItemData( items[i], code.encode() ); 	
		}
		delete []items;

	}
#if defined (__MECHANICS_2D__)
	// Feedbacks
	if( pNetwork->size_fbk() ){
		HTREEITEM *items = new HTREEITEM[pNetwork->size_fbk()];
		code = unit_code(); code.UnitId = _id_NNFeedback; code.NNIndex = 0;	code.Param = 0;
		items[0] = Tree_Controlled.InsertItem( pNetwork->get_fbk(0).get_name().c_str(), mainItems[4], mainItems[4] );
		Tree_Controlled.SetItemData( items[0], code.encode() );
		for( size_t i = 1; i < min( pNetwork->size_fbk(), 255 ); ++i ){
 			code.NNIndex = i;
			items[i] = Tree_Controlled.InsertItem( pNetwork->get_fbk(i).get_name().c_str(), mainItems[4], items[i-1] );
			Tree_Controlled.SetItemData( items[i], code.encode() ); 	
		}
		delete []items;
	}
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	List_Controlled.InsertColumn( 0, "");
	ShowListBox();
	return TRUE; 
}

void CControlledSetup::OnAddParam() 
{
	HTREEITEM hItem = Tree_Controlled.GetSelectedItem();
	if( hItem == NULL)
		return;
	unit_code code( Tree_Controlled.GetItemData( hItem ));
	if( code.Param == -1 )
		return;
	CHhnControlled ctrld( code );
	pControl->add_unit( ctrld );
	ShowListBox();
}

void CControlledSetup::OnRemoveParam() 
{
	int numSelected = List_Controlled.GetSelectedCount();
	if( numSelected == 0 )return;
	int *SelectedItems = new int [numSelected];
	POSITION pos = List_Controlled.GetFirstSelectedItemPosition();
	if( pos != NULL ){
		for( int i = 0; pos; i++ ){
		 SelectedItems[i] = List_Controlled.GetNextSelectedItem( pos );
		}
		for( int i = numSelected-1; i >= 0; i-- ){
			unit_code code( List_Controlled.GetItemData( SelectedItems[i] ));
			CHhnControlled ctrld( code );
//			nn_unit *object = pNetwork->get_nnunit( code );
			pControl->del_unit( ctrld );
		}
	}
	ShowListBox();
	if( !List_Controlled.GetItemCount() ){
		Button_RemoveParam.EnableWindow( FALSE );
	}
	delete []SelectedItems;
}

void CControlledSetup::ShowListBox() 
{
	List_Controlled.DeleteAllItems();
	CString str;
	LV_ITEM item;
	for( size_t i = 0; i < pControl->ncunits(); i++ ){
		nn_unit *object = pNetwork->get_nnunit( pControl->ctrl_unit(i).get_par() );
		if( !object )
			break;
		
		str = object->get_name().c_str();
		string name;
		str += " / ";
		str += cr_name( name, pControl->ctrl_unit( i ).get_par());
		item.iItem = i;
		item.mask = LVIF_TEXT | LVIF_PARAM;
		item.lParam = pControl->ctrl_unit(i).get_par().encode(); 
		item.iSubItem = 0;
		item.pszText = ( LPTSTR )( LPCTSTR )str;

		List_Controlled.InsertItem( &item );
		Button_RemoveParam.EnableWindow( TRUE );
	}
	List_Controlled.SetColumnWidth(0, LVSCW_AUTOSIZE );
}

/////////////////////////////////////////////////////////////////////////////
// CControlFunction property page
IMPLEMENT_DYNCREATE(CControlFunction, CPropertyPage)

CControlFunction::CControlFunction() : CPropertyPage(CControlFunction::IDD)
, IsStep(0)
{
	NameCtrl = _T("");
	NoiseLevel = 0;
	IsNoiseAdded = FALSE;
}

CControlFunction::~CControlFunction()
{
}

void CControlFunction::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NAME_CTR, EditNameCtrl);
	DDX_Control(pDX, IDC_SAVE, SaveButton);
	DDX_Control(pDX, IDC_PROP_POINT, PropPoint);
	DDX_Control(pDX, IDC_LIST_POINTS, ListPoints);
	DDX_Control(pDX, IDC_DEL_POINT, DelPoint);
	DDX_Control(pDX, IDC_ADD_POINT, AddPoint);
	DDX_Text(pDX, IDC_EDIT_NAME_CTR, NameCtrl);
	DDX_Text(pDX, IDC_EDIT_NOISE, NoiseLevel);
	DDX_Check(pDX, IDC_ADD_NOISE, IsNoiseAdded);
	DDX_Check(pDX, IDC_CRL_ITERPOL, IsStep);
}

BEGIN_MESSAGE_MAP( CControlFunction, CPropertyPage )
	ON_BN_CLICKED( IDC_ADD_POINT, &CControlFunction::OnAddPoint )
	ON_BN_CLICKED( IDC_DEL_POINT, &CControlFunction::OnDelPoint )
	ON_LBN_DBLCLK( IDC_LIST_POINTS, &CControlFunction::OnDblclkListPoints )
	ON_BN_CLICKED( IDC_LOAD, &CControlFunction::OnLoad )
	ON_BN_CLICKED( IDC_PROP_POINT, &CControlFunction::OnPropPoint )
	ON_BN_CLICKED( IDC_SAVE, &CControlFunction::OnSave )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlFunction message handlers
BOOL CControlFunction::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	NameCtrl = pControl->get_name().c_str();
	EditNameCtrl.SetWindowText( NameCtrl );
	NoiseLevel = pControl->noise();
	IsStep = int( pControl->step());
	if( NoiseLevel <= 0 ){
		IsNoiseAdded = FALSE;
		NoiseLevel = 0;
	} else{ IsNoiseAdded = TRUE; }
	Change();
	return TRUE;
}

BOOL CControlFunction::OnSetActive() 
{
	UpdateData( FALSE );
	return CPropertyPage::OnSetActive();
}

BOOL CControlFunction::OnKillActive() 
{
	UpdateData( TRUE );
	pControl->set_name( LPCTSTR( NameCtrl ));
	pControl->noise() =  IsNoiseAdded? NoiseLevel: -1;
	pControl->step() = IsStep? true: false;
	return CPropertyPage::OnKillActive();
}

void CControlFunction::OnAddPoint() 
{
	pControl->add_tab(hhn_pair<double>());
	Change();	
}

void CControlFunction::OnDelPoint() 
{
	unsigned int i = ListPoints.GetCurSel();
	if( i != -1 ){
		pControl->del_tab( i );
		Change();
	}
}

void CControlFunction::OnDblclkListPoints() 
{
	OnPropPoint();	
}

void CControlFunction::OnPropPoint() 
{
	unsigned int i = ListPoints.GetCurSel();
	if( i == -1 ) return;

	CPropPoint dlg;
	dlg.Time = pControl->tab_point(i).X;
	dlg.Mul = pControl->tab_point(i).Y;
	if( dlg.DoModal() == IDOK ){
		pControl->tab_point(i).Y = dlg.Mul;
		pControl->tab_point(i).X = dlg.Time;
	}
	Change();	
}

void CControlFunction::OnLoad() 
{
//	<insert> clear_tabs is useless. must be xcluded
	CString title = "Open control function";
	CString fileFilter = "tab image (*.tab)|*.tab|All Files (*.*)|*.*||";
	CFileDialog openDialog( TRUE, NULL, NULL, 
			OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, 
			( LPCTSTR )fileFilter );
	openDialog.m_ofn.lpstrTitle = title;
	if( openDialog.DoModal() == IDOK ){
		ifstream file(( LPCSTR )openDialog.GetPathName());
		string str;
		bool Ok = false;
		file >> str;
		if(str == "<Function>")
      		Ok = pControl->load_tab( file );
		if(!Ok){
			message("File format is not correct!","Reading error" );
			pControl->clear_tabs();
		}
    }
    Change();	
}

void CControlFunction::OnSave() 
{
	CString title = "Save control function";
	CString fileFilter = "Tabs function (*.tab)|*.tab|All Files (*.*)|*.*||";
	CFileDialog saveDialog( FALSE, "tab", "ctrlfunc", 
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
			( LPCTSTR )fileFilter );
	saveDialog.m_ofn.lpstrTitle = title;
	if( saveDialog.DoModal() == IDOK ){
		ofstream file(( LPCSTR )saveDialog.GetPathName(), ios_base::trunc);
      		pControl->save_tab( file );
	}	
}

void CControlFunction::Change( void )
{
	AddPoint.EnableWindow(TRUE);
	DelPoint.EnableWindow(TRUE);
	PropPoint.EnableWindow(TRUE);
	SaveButton.EnableWindow(TRUE);
	if( pControl->nctabs() == 0 ){
		DelPoint.EnableWindow(FALSE);
		PropPoint.EnableWindow(FALSE);
		SaveButton.EnableWindow(FALSE);
	}
	CString str; 
	pControl->sort_tabs();
	ListPoints.ResetContent();
	for( size_t i = 0 ; i < pControl->nctabs(); i++ ){
		str.Format("time = %lg, change = %lg",pControl->tab_point(i).X,pControl->tab_point(i).Y);
		ListPoints.AddString(str);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPropPoint dialog
CPropPoint::CPropPoint(CWnd* pParent /*=NULL*/)
	: CDialog(CPropPoint::IDD, pParent)
{
	Time = 0.0;
	Mul = 0.0;
}

void CPropPoint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_MUL, SpinMul);
	DDX_Control(pDX, IDC_EDIT_MUL, EditMul);
	DDX_Control(pDX, IDC_SPIN_TIME, SpinTime);
	DDX_Control(pDX, IDC_EDIT_TIME, EditTime);
	DDX_Text(pDX, IDC_EDIT_TIME, Time);
	DDX_Text(pDX, IDC_EDIT_MUL, Mul);
}

BEGIN_MESSAGE_MAP(CPropPoint, CDialog)
	ON_EN_UPDATE(IDC_EDIT_TIME, &CPropPoint::OnUpdateEditTime)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TIME, &CPropPoint::OnDeltaposSpinTime)
	ON_EN_UPDATE(IDC_EDIT_MUL, &CPropPoint::OnUpdateEditMul)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MUL, &CPropPoint::OnDeltaposSpinMul)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropPoint message handlers
BOOL CPropPoint::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SpinMul.SetBuddy(&EditMul);
	SpinMul.SetRange(UD_MINVAL,UD_MAXVAL);
	SpinTime.SetBuddy(&EditTime);
	SpinTime.SetRange(0,UD_MAXVAL);
	return TRUE;
}

void CPropPoint::OnUpdateEditTime() 
{
	CString strTime;
	EditTime.GetWindowText( strTime );
	Time = atof( LPCTSTR( strTime ));
}

void CPropPoint::OnDeltaposSpinTime(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	Time += pNMUpDown->iDelta*1.;
	if( Time < 0. )
		Time = 0.;
	CString strTime;
	strTime.Format( "%lg", Time );
	EditTime.SetWindowText(( LPCTSTR ) strTime );
	*pResult = 0;
}

void CPropPoint::OnUpdateEditMul() 
{
	CString strMul;
	EditMul.GetWindowText( strMul );
	Mul = atof( LPCTSTR( strMul ));
}

void CPropPoint::OnDeltaposSpinMul(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	Mul += pNMUpDown->iDelta*0.1;
	CString strMul;
	strMul.Format( "%lg", Mul );
	EditMul.SetWindowText(( LPCTSTR ) strMul );
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CControlProperties
IMPLEMENT_DYNAMIC(CControlProperties, CPropertySheet)

CControlProperties::CControlProperties(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	Control = NULL;
	AddPage( &ControlFunction );
	AddPage( &ControlledSetup );
}

CControlProperties::CControlProperties(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	Control = NULL;
	AddPage( &ControlFunction );
	AddPage( &ControlledSetup );
}

CControlProperties::~CControlProperties()
{
	if( Control ){
		delete Control;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CControlProperties message handlers
int CControlProperties::DoModal( CHhnNetwork * pnetwork, const hhn_control &control ) 
{
	if( Control ){
		delete Control;
	}
	Control = new hhn_control( control );

	ControlledSetup.pNetwork = pnetwork;
	ControlledSetup.pControl = Control;
	ControlFunction.pControl = Control;
	return CPropertySheet::DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkControl dialog
CNetworkControl::CNetworkControl( CWnd* pParent /*=NULL*/ )
	: CDialog( CNetworkControl::IDD, pParent )
{
	NewControl = 0;
}

void CNetworkControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONTROL_SIZE, ControlSize);
	DDX_Control(pDX, IDC_LIST_CONTROL, ListControl);
	DDX_Control(pDX, IDC_PROP_CONTROL, PropControl);
	DDX_Control(pDX, IDC_DEL_CONTROL, DelControl);
	DDX_Control(pDX, IDC_ADD_CONTROL, add_ctr);
}

BEGIN_MESSAGE_MAP(CNetworkControl, CDialog)
	ON_BN_CLICKED(IDC_ADD_CONTROL, &CNetworkControl::OnAddControl)
	ON_BN_CLICKED(IDC_DEL_CONTROL, &CNetworkControl::OnDelControl)
	ON_BN_CLICKED(IDC_PROP_CONTROL, &CNetworkControl::OnPropControl)
	ON_LBN_DBLCLK(IDC_LIST_CONTROL, &CNetworkControl::OnDblclkListControl)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkControl message handlers
BOOL CNetworkControl::OnInitDialog() 
{
	CDialog::OnInitDialog();
	NewControl = Network.size_ctr();
	Change();
	if(Network.size_ctr()){
		ListControl.SetCurSel( 0 );
	}
	return TRUE;
}

void CNetworkControl::OnAddControl() 
{
	CString name;
	name.Format("Control %d",NewControl);
	Network.add_ctr( hhn_control( string(( LPCTSTR )name )));
	NewControl++;
	Change();
	ListControl.SetCurSel( ListControl.GetCount()-1 );
}

void CNetworkControl::OnDelControl() 
{
	int i = ListControl.GetCurSel();
	if( i == -1 )
		return;
	Network.del_ctr(i);
	Change();
	if( i >= ListControl.GetCount() )
		i = ListControl.GetCount()-1;
	ListControl.SetCurSel( i );
}

void CNetworkControl::OnPropControl() 
{
	size_t i = ListControl.GetCurSel();
	if( i == -1 ) return;

	CControlProperties dlg("Control Properties");
	if( dlg.DoModal( &Network, Network.get_ctr(i) ) == IDOK ){
		Network.set_ctr(*dlg.Control,i);
		Change(); 
	}
	ListControl.SetCurSel(i);
}

void CNetworkControl::OnDblclkListControl() 
{
	OnPropControl();
}

void CNetworkControl::Change( void )
{
	add_ctr.EnableWindow( TRUE );
	DelControl.EnableWindow( TRUE );
	PropControl.EnableWindow( TRUE );
	if( Network.size_ctr() == 0 ){
		DelControl.EnableWindow(FALSE);
		PropControl.EnableWindow(FALSE);
	}
	ListControl.ResetContent();
	for( size_t i = 0 ; i < Network.size_ctr(); ++i ){
		ListControl.AddString( Network.get_ctr(i).get_name().c_str() );
	}
	CString str;
	str.Format("Number of control modules - %d", Network.size_ctr());
	ControlSize.SetWindowText(str);
}

#endif // __CONSOLE__
