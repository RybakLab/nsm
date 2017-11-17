/////////////////////////////////////////////////////////////////////////////
// NetworkView.cpp : implementation file
#include "precompile.h"

#ifndef __CONSOLE__

#include "simulate.h"
#include "NetworkView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif // _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGeomView dialog
CGeomView::CGeomView(CWnd* pParent /*=NULL*/)
	: CDialog(CGeomView::IDD, pParent)
{
	NumCol = 0;
	NumRow = 0;
    NumParam = 1;
}

void CGeomView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_ROW, SpinRow);
	DDX_Control(pDX, IDC_SPIN_COL, SpinCol);
	DDX_Text(pDX, IDC_EDIT_COL, NumCol);
	DDX_Text(pDX, IDC_EDIT_ROW, NumRow);
	DDV_MinMaxInt(pDX, NumCol, 1, NumParam);
	DDV_MinMaxInt(pDX, NumRow, 1, NumParam);
}

BEGIN_MESSAGE_MAP(CGeomView, CDialog)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ROW, OnDeltaposSpinRow)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_COL, OnDeltaposSpinCol)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeomView message handlers
BOOL CGeomView::OnInitDialog() 
{
	CDialog::OnInitDialog();
    NumParam = NumRow*NumCol;
 	SpinRow.SetRange(1,NumParam);
    SpinRow.SetPos(NumRow);
 	SpinCol.SetRange(1,NumParam);
    SpinCol.SetPos(NumCol);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGeomView::OnDeltaposSpinRow(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	*pResult = 0;
}

void CGeomView::OnDeltaposSpinCol(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CScaleView dialog
CScaleView::CScaleView(CWnd* pParent /*=NULL*/)
	: CDialog(CScaleView::IDD, pParent)
{
	Ymax = 0.0;
	Ymin = 0.0;
}

void CScaleView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_YMIN, SpinYmin);
	DDX_Control(pDX, IDC_SPIN_YMAX, SpinYmax);
	DDX_Control(pDX, IDC_EDIT_YMIN, EditYmin);
	DDX_Control(pDX, IDC_EDIT_YMAX, EditYmax);
	DDX_Text(pDX, IDC_EDIT_YMAX, Ymax);
	DDX_Text(pDX, IDC_EDIT_YMIN, Ymin);
}

BEGIN_MESSAGE_MAP(CScaleView, CDialog)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_YMAX, OnDeltaposSpinYmax)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_YMIN, OnDeltaposSpinYmin)
	ON_EN_UPDATE(IDC_EDIT_YMAX, OnUpdateEditYmax)
	ON_EN_UPDATE(IDC_EDIT_YMIN, OnUpdateEditYmin)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScaleView message handlers
BOOL CScaleView::OnInitDialog() 
{
	CDialog::OnInitDialog();
    SpinYmin.SetBuddy(&EditYmin);
    SpinYmin.SetRange(UD_MINVAL,UD_MAXVAL);
    SpinYmax.SetBuddy(&EditYmax);
    SpinYmax.SetRange(UD_MINVAL,UD_MAXVAL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CScaleView::OnDeltaposSpinYmax(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    Ymax += pNMUpDown->iDelta*0.1;
    CString strYmax;
    strYmax.Format( "%g", Ymax );
    EditYmax.SetWindowText(( LPCTSTR ) strYmax );
	*pResult = 0;
}

void CScaleView::OnDeltaposSpinYmin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    Ymin += pNMUpDown->iDelta*0.1;
    CString strYmin;
    strYmin.Format( "%g", Ymin );
    EditYmin.SetWindowText(( LPCTSTR ) strYmin );
	*pResult = 0;
}

void CScaleView::OnUpdateEditYmax() 
{
    CString strYmax;
	EditYmax.GetWindowText( strYmax );
    Ymax = atof( LPCTSTR( strYmax ));
}

void CScaleView::OnUpdateEditYmin() 
{
    CString strYmin;
	EditYmin.GetWindowText( strYmin );
    Ymin = atof( LPCTSTR( strYmin ));
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkView dialog
CNetworkView::CNetworkView(CWnd* pParent /*=NULL*/)
	: CDialog(CNetworkView::IDD, pParent)
{
	NumView = 0;
	NameView = _T("");
}

void CNetworkView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_VIEW_NAME, EditNameView);
	DDX_Control(pDX, IDC_GEOMETRY_VIEW, GeomView);
	DDX_Control(pDX, IDC_SCALE_VIEW, ScaleView);
	DDX_Control(pDX, IDC_REMOVE_VIEW, RemoveView);
	DDX_Control(pDX, IDC_ADD_VIEW, AddView);
	DDX_Control(pDX, IDC_LIST_VIEW, ListView);
	DDX_Control(pDX, IDC_VIEW_TREE, ViewTree);
	DDX_Text(pDX, IDC_EDIT_VIEW_NAME, NameView);
}

BEGIN_MESSAGE_MAP(CNetworkView, CDialog)
	ON_BN_CLICKED(IDC_ADD_VIEW, OnAddView)
	ON_BN_CLICKED(IDC_REMOVE_VIEW, OnRemoveView)
	ON_BN_CLICKED(IDC_SCALE_VIEW, OnScaleView)
	ON_BN_CLICKED(IDC_GEOMETRY_VIEW, OnGeometryView)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_VIEW, OnDblclkListView)
	ON_BN_CLICKED(IDC_MOVE_UP, OnMoveUp)
	ON_BN_CLICKED(IDC_MOVE_DOWN, OnMoveDown)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkView message handlers
int CNetworkView::DoModal() 
{
	NumView = 0;
	return CDialog::DoModal();
}

BOOL CNetworkView::OnInitDialog() 
{
	CDialog::OnInitDialog();
	NameView = View.get_name();
	EditNameView.SetWindowText( NameView );

	size_t i, j;

	int NETWORK = 0;
	int POPULAT = 1;
	int DRIVE = 2;
	int OUTPUT = 3;
	int CONTROL = 4;
	int MAXITEM = 5;
#ifdef __MECHANICS__
	int FEEDBACK = 5;
	int BODY = 6;
	MAXITEM = 7;
#endif // __MECHANICS__
	vector<HTREEITEM>mainItems( MAXITEM );
	mainItems[NETWORK] = ViewTree.InsertItem(Manager->Network.get_name().c_str(), 0, TVI_ROOT);
	mainItems[POPULAT] = ViewTree.InsertItem("Population", mainItems[NETWORK], mainItems[NETWORK]);
	mainItems[DRIVE] = ViewTree.InsertItem("External Drive", mainItems[NETWORK], mainItems[POPULAT]);
	mainItems[OUTPUT] = ViewTree.InsertItem("Output", mainItems[NETWORK], mainItems[DRIVE]);
	mainItems[CONTROL] = ViewTree.InsertItem("Control", mainItems[NETWORK], mainItems[OUTPUT]);
#ifdef __MECHANICS__
	mainItems[FEEDBACK] = ViewTree.InsertItem("Feedback", mainItems[NETWORK], mainItems[CONTROL]);
	mainItems[BODY] = ViewTree.InsertItem("Body", 0, TVI_ROOT);
#endif // __MECHANICS__
	unit_code code;
	code.UnitId = _id_Network;
	ViewTree.SetItemData(mainItems[NETWORK], code.encode());
	code.UnitId = _id_NNPopulat;
	ViewTree.SetItemData(mainItems[POPULAT], code.encode());
	code.UnitId = _id_NNDrive;
	ViewTree.SetItemData(mainItems[DRIVE], code.encode());
	code.UnitId = _id_NNOutput;
	ViewTree.SetItemData(mainItems[OUTPUT], code.encode());
	code.UnitId = _id_NNControl;
	ViewTree.SetItemData(mainItems[CONTROL], code.encode());
#ifdef __MECHANICS__
	code.UnitId = _id_NNFeedback;
	ViewTree.SetItemData(mainItems[FEEDBACK], code.encode());
	code.UnitId = _id_Body;
	ViewTree.SetItemData(mainItems[BODY], code.encode());
#endif // __MECHANICS__
	vector<HTREEITEM> populatItems( Manager->Network.size_pop() );
	vector<HTREEITEM> driveItems( Manager->Network.size_drv() );
	vector<HTREEITEM> outputItems( Manager->Network.size_out() );
	vector<HTREEITEM> controlItems( Manager->Network.size_ctr() );
#ifdef __MECHANICS__
	vector<HTREEITEM> feedbackItems( Manager->Network.size_fbk() );
#endif // __MECHANICS__
	if( Manager->Network.size_pop() ){ // Populations
		populatItems[0] = ViewTree.InsertItem(Manager->Network.get_pop(0).get_name().c_str(),mainItems[POPULAT],mainItems[POPULAT]);
		code = unit_code();
		code.UnitId = _id_NNPopulat;
		code.NNIndex = 0;
		ViewTree.SetItemData(populatItems[0], code.encode());
		for( i = 1; i < min(Manager->Network.size_pop(), 255); i++ ){
			code.NNIndex = i;
			populatItems[i] = ViewTree.InsertItem(Manager->Network.get_pop(i).get_name().c_str(),mainItems[POPULAT],populatItems[i-1]);
			ViewTree.SetItemData(populatItems[i], code.encode()); 	
		}
		for( i = 0; i < min(Manager->Network.size_pop(), 255); i++ ){
			HTREEITEM plotItem;
			HTREEITEM histItem;
			code.NNIndex = i;
			code.HhnIndex = -1;
			code.PartIndex = -1;
			code.ChanIndex = -1;
			code.Param	= CViewParam::Plot;
			plotItem = ViewTree.InsertItem("Raster plot",populatItems[i],populatItems[i]); 
			ViewTree.SetItemData(plotItem, code.encode()); 	
			code.Param	= CViewParam::Hist;
			histItem = ViewTree.InsertItem("Average histogram",populatItems[i],plotItem);
			ViewTree.SetItemData(histItem, code.encode()); 	
			code.Param = -1;
			vector<HTREEITEM> neuronItems( Manager->Network.get_pop(i).size() );
			CString name; 
			name.Format("Neuron 0");
			if( Manager->Network.get_pop(i).size() ){
				code.HhnIndex = 0;
				neuronItems[0] = ViewTree.InsertItem(name,populatItems[i],populatItems[i]);
				ViewTree.SetItemData(neuronItems[0], code.encode());
				for( j = 1; j < min(Manager->Network.get_pop(i).size(), 63); j++ ){
					code.HhnIndex = j;
					name.Format("Neuron %d", j);
					neuronItems[j] = ViewTree.InsertItem(name,populatItems[i],neuronItems[j-1]);
					ViewTree.SetItemData(neuronItems[j], code.encode());
				}
				for( j = 0; j < min(Manager->Network.get_pop(i).size(), 63); j++ ){
					vector<HTREEITEM> PartItem(( Manager->Network.get_pop(i))[j].size() );
					code.HhnIndex = j;
					code.PartIndex = 0;
					code.ChanIndex = -1;
					PartItem[0] = ViewTree.InsertItem(Manager->Network.get_pop(i)[j].get_compart(0)->get_name(),neuronItems[j],neuronItems[j]);
					ViewTree.SetItemData(PartItem[0], code.encode());
					size_t k =0;
					for( k = 1; k < min((Manager->Network.get_pop(i))[j].size(), 7); k++ ){
						code.PartIndex = k;
						PartItem[k] = ViewTree.InsertItem(Manager->Network.get_pop(i)[j].get_compart(k)->get_name(),neuronItems[j],PartItem[k-1]);
						ViewTree.SetItemData(PartItem[k], code.encode());
					}
					for( k = 0; k < min((Manager->Network.get_pop(i))[j].size(), 7); k++ ){
						const hhn_compart *cmp = ( Manager->Network.get_pop( i ))[j].get_compart( k );
						HTREEITEM VItem;
						HTREEITEM CaItem;
						HTREEITEM NaItem;
						HTREEITEM GItem;
						vector<HTREEITEM> ChanItem( (Manager->Network.get_pop(i))[j].get_compart(k)->size()+1 );
						code.PartIndex = k;
						code.ChanIndex = -1;
						code.Param = CViewParam::Potent;
						VItem = ViewTree.InsertItem("Membrane potential (V)",PartItem[k],PartItem[k]);
						ViewTree.SetItemData(VItem, code.encode());
						code.Param = CViewParam::Condc;
						GItem = ViewTree.InsertItem("Conductance",PartItem[k],VItem);
						ViewTree.SetItemData(GItem, code.encode());
						code.Param = CViewParam::NaConc;
						NaItem = ViewTree.InsertItem("Na concentration inside cell",PartItem[k],GItem);
						ViewTree.SetItemData(NaItem, code.encode());
						code.Param = CViewParam::CaConc;
						CaItem = ViewTree.InsertItem("Ca concentration inside cell",PartItem[k],NaItem);
						ViewTree.SetItemData(CaItem, code.encode());
						code.Param = -1;
						ChanItem[0] = ViewTree.InsertItem("Channels parameters",PartItem[k],CaItem);
						ViewTree.SetItemData(ChanItem[0], code.encode()); 	 
						for( size_t n = 1; n < min((Manager->Network.get_pop(i))[j].get_compart(k)->size(), 31)+1; n++ ){
							HTREEITEM gItem;
							HTREEITEM iItem;
							HTREEITEM mItem;
							HTREEITEM hItem;
							code.ChanIndex = n-1;
							ChanItem[n] = ViewTree.InsertItem(( *cmp )[n-1]->get_name(), ChanItem[0],ChanItem[n-1]);
							ViewTree.SetItemData(ChanItem[n], code.encode());
							code.Param = CViewParam::Condc;
							gItem = ViewTree.InsertItem("Channel conductance",ChanItem[n],ChanItem[n]);
							ViewTree.SetItemData(gItem, code.encode());
							code.Param = CViewParam::Curr;
							iItem = ViewTree.InsertItem("Channel current",ChanItem[n],gItem);
							ViewTree.SetItemData(iItem, code.encode());
							code.Param = CViewParam::Activation;
							mItem = ViewTree.InsertItem("Activation",ChanItem[n],iItem);
							ViewTree.SetItemData(mItem, code.encode());
							code.Param = CViewParam::Inactivation;
							hItem = ViewTree.InsertItem("Inactivation",ChanItem[n],mItem);
							ViewTree.SetItemData(hItem, code.encode());
							code.Param = -1;
						}
					}
				}
			}
		}
	}
#ifdef __MECHANICS__
	if( Manager->Network.size_fbk() ){
		code = unit_code();
		code.UnitId = _id_NNFeedback;
		code.NNIndex = 0;
		code.Param = CViewParam::Out;
		feedbackItems[0] = ViewTree.InsertItem(Manager->Network.get_fbk(0).get_name().c_str(),mainItems[FEEDBACK],mainItems[FEEDBACK]);
		ViewTree.SetItemData(feedbackItems[0], code.encode()); 
		for( i = 1; i < min(Manager->Network.size_fbk(), 255); i++ ){
			code.NNIndex = i;
			feedbackItems[i] = ViewTree.InsertItem(Manager->Network.get_fbk(i).get_name().c_str(),mainItems[FEEDBACK],feedbackItems[i-1]);
			ViewTree.SetItemData(feedbackItems[i], code.encode()); 
		}
	}
#endif // __MECHANICS__
	if( Manager->Network.size_drv() ){
		code = unit_code();
		code.UnitId = _id_NNDrive;
		code.NNIndex = 0;
		code.Param = CViewParam::Out;
		driveItems[0] = ViewTree.InsertItem(Manager->Network.get_drv(0).get_name().c_str(),mainItems[DRIVE],mainItems[DRIVE]);
		ViewTree.SetItemData(driveItems[0], code.encode());
		for( i = 1; i < min(Manager->Network.size_drv(), 255); i++ ){
			code.NNIndex = i;
			driveItems[i] = ViewTree.InsertItem(Manager->Network.get_drv(i).get_name().c_str(),mainItems[DRIVE],driveItems[i-1]);
			ViewTree.SetItemData(driveItems[i], code.encode());
		}
	}
	if( Manager->Network.size_out() ){
		code = unit_code();
		code.UnitId = _id_NNOutput;
		code.NNIndex = 0;
		code.Param = CViewParam::Out;
		outputItems[0] = ViewTree.InsertItem(Manager->Network.get_out(0).get_name().c_str(),mainItems[OUTPUT],mainItems[OUTPUT]);
		ViewTree.SetItemData(outputItems[0], code.encode());
		for( i = 1; i < min(Manager->Network.size_out(), 255); i++ ){
			code.NNIndex = i;
			outputItems[i] = ViewTree.InsertItem(Manager->Network.get_out(i).get_name().c_str(),mainItems[OUTPUT],outputItems[i-1]);
			ViewTree.SetItemData(outputItems[i], code.encode());
		}
	}
	if( Manager->Network.size_ctr() ){
		code = unit_code();
		code.UnitId = _id_NNControl;
		code.NNIndex = 0;
		code.Param = CViewParam::Out;
		controlItems[0] = ViewTree.InsertItem(Manager->Network.get_ctr(0).get_name().c_str(),mainItems[CONTROL],mainItems[CONTROL]);
		ViewTree.SetItemData(controlItems[0], code.encode());
		for( i = 1; i < min(Manager->Network.size_ctr(), 255); i++ ){
			code.NNIndex = i;
			controlItems[i] = ViewTree.InsertItem(Manager->Network.get_ctr(i).get_name().c_str(),mainItems[CONTROL],controlItems[i-1]);
			ViewTree.SetItemData(controlItems[i], code.encode());
		}
	}

#ifdef __MECHANICS__
	// Body
	vector<HTREEITEM> bodyItems( 5) ; 
	code = unit_code();
	bodyItems[0] = ViewTree.InsertItem("Vertices",mainItems[BODY],mainItems[BODY]);
	bodyItems[1] = ViewTree.InsertItem("Links",mainItems[BODY],bodyItems[0]);
	bodyItems[2] = ViewTree.InsertItem("Joints",mainItems[BODY],bodyItems[1]);
	bodyItems[3] = ViewTree.InsertItem("Muscles",mainItems[BODY],bodyItems[2]);
	bodyItems[4] = ViewTree.InsertItem("Ground",mainItems[BODY],bodyItems[3]);
	code.UnitId = _id_Vertex;
	ViewTree.SetItemData(bodyItems[0], code.encode());
	code.UnitId = _id_Link;
	ViewTree.SetItemData(bodyItems[1], code.encode());
	code.UnitId = _id_Joint;
	ViewTree.SetItemData(bodyItems[2], code.encode());
	code.UnitId = _id_Muscle;
	ViewTree.SetItemData(bodyItems[3], code.encode());
	code.UnitId = _id_Ground;
	ViewTree.SetItemData(bodyItems[4], code.encode());
	// vertex
	vector<HTREEITEM> typeItems( NUM_VERTEX ); 
	code = unit_code();
	code.UnitId = _id_Vertex;
	code.NNIndex = 0;
	typeItems[0] = ViewTree.InsertItem( Manager->Walker->get_name( code ).c_str(), bodyItems[0],bodyItems[0]);
	ViewTree.SetItemData(typeItems[0], code.encode()); 
	for( i = 1; i < min( NUM_VERTEX, 255 ); i++ ){
		code.NNIndex = i;
		typeItems[i] = ViewTree.InsertItem(Manager->Walker->get_name( code ).c_str(), bodyItems[0],typeItems[i-1]);
		ViewTree.SetItemData( typeItems[i], code.encode());
	}
	for(i = 0; i < min(NUM_VERTEX, 255); i++ ){
		vector<HTREEITEM> parameterItems( 4 );
		code.NNIndex = i;
		code.Param = _id_Coordinate_X;
		parameterItems[0] = ViewTree.InsertItem("Coordinate X",typeItems[i],typeItems[i]);
		ViewTree.SetItemData(parameterItems[0], code.encode());
		code.Param = _id_Coordinate_Y;
		parameterItems[1] = ViewTree.InsertItem("Coordinate Y",typeItems[i],parameterItems[0]);
		ViewTree.SetItemData(parameterItems[1], code.encode());
		code.Param = _id_Velocity_X;
		parameterItems[2] = ViewTree.InsertItem("Velocity X",typeItems[i],parameterItems[1]);
		ViewTree.SetItemData(parameterItems[2], code.encode());
		code.Param = _id_Velocity_Y;
		parameterItems[3] = ViewTree.InsertItem("Velocity Y",typeItems[i],parameterItems[2]);
		ViewTree.SetItemData(parameterItems[3], code.encode());
	}
	// Link
	typeItems.clear();
	code = unit_code();
	code.UnitId = _id_Link;
	code.NNIndex = 0;
	typeItems.push_back( ViewTree.InsertItem( Manager->Walker->get_name( code ).c_str(), bodyItems[1], bodyItems[1] ));
	ViewTree.SetItemData( typeItems[0], code.encode()); 
	for( i = 1; i < min( NUM_LINK, 255 ); i++ ){
		code.NNIndex = i;
		typeItems.push_back( ViewTree.InsertItem( Manager->Walker->get_name( code ).c_str(),bodyItems[1],typeItems[i-1]));
		ViewTree.SetItemData( typeItems[i], code.encode() );
	}
	for( i = 0; i < min( NUM_LINK, 255 ); i++ ){
		vector<HTREEITEM> parameterItems( 2 );
		code.NNIndex = i;
		code.Param = _id_Angle;
		parameterItems[0] = ViewTree.InsertItem( "Angle", typeItems[i], typeItems[i] );
		ViewTree.SetItemData( parameterItems[0], code.encode() );
		code.Param = _id_Angle_Velocity;
		parameterItems[1] = ViewTree.InsertItem("Angle velocity",typeItems[i],parameterItems[0]);
		ViewTree.SetItemData( parameterItems[1], code.encode() );
	}
	// Joint
	typeItems.clear();
	code = unit_code();
	code.UnitId = _id_Joint;
	code.NNIndex = 0;
	typeItems.push_back( ViewTree.InsertItem(Manager->Walker->get_name( code ).c_str(),bodyItems[2],bodyItems[2]));
	ViewTree.SetItemData( typeItems[0], code.encode()); 
	for(i = 1; i < min( NUM_JOINT, 255 ); i++ ){
		code.NNIndex = i;
		typeItems.push_back( ViewTree.InsertItem(Manager->Walker->get_name( code ).c_str(), bodyItems[2],typeItems[i-1]));
		ViewTree.SetItemData( typeItems[i], code.encode() );
	}
	for(i = 0; i < min( NUM_JOINT, 255); i++ ){
		vector<HTREEITEM> parameterItems( 2 );
		code.NNIndex = i;
		code.Param = _id_Angle;
		parameterItems[0] = ViewTree.InsertItem( "Angle", typeItems[i], typeItems[i] );
		ViewTree.SetItemData( parameterItems[0], code.encode());
		code.Param = _id_Angle_Velocity;
		parameterItems[1] = ViewTree.InsertItem("Angle velocity",typeItems[i],parameterItems[0]);
		ViewTree.SetItemData( parameterItems[1], code.encode() );
	}
	// Muscles
	typeItems.clear();
	if( !Manager->Walker->Muscles.empty() ){
		code = unit_code();
		code.UnitId = _id_Muscle;
		code.NNIndex = 0;
		typeItems.push_back( ViewTree.InsertItem( Manager->Walker->get_name( code ).c_str(), bodyItems[3],bodyItems[3]));
		ViewTree.SetItemData( typeItems[0], code.encode()); 
		for( i = 1; i < Manager->Walker->Muscles.size(); i++ ){
			code.NNIndex = i;
			typeItems.push_back( ViewTree.InsertItem(Manager->Walker->get_name( code ).c_str(), bodyItems[3],typeItems[i-1]));
			ViewTree.SetItemData( typeItems[i], code.encode() );
		}
		for( i = 0; i < Manager->Walker->Muscles.size(); i++ ){
			vector<HTREEITEM> parameterItems( 7 );
			code.NNIndex = i;
			code.Param = _id_MuscleForce;
			parameterItems[0] = ViewTree.InsertItem("Force",typeItems[i],typeItems[i]);
			ViewTree.SetItemData(parameterItems[0], code.encode());
			code.Param = _id_TotalLength;
			parameterItems[1] = ViewTree.InsertItem("Total length",typeItems[i],parameterItems[0]);
			ViewTree.SetItemData(parameterItems[1], code.encode());
			code.Param = _id_TotalVelocity;
			parameterItems[2] = ViewTree.InsertItem("Total velocity",typeItems[i],parameterItems[1]);
			ViewTree.SetItemData(parameterItems[2], code.encode());
			code.Param = _id_MuscleLength;
			parameterItems[3] = ViewTree.InsertItem("Muscle length",typeItems[i],parameterItems[2]);
			ViewTree.SetItemData(parameterItems[3], code.encode());
			code.Param = _id_MuscleVelocity;
			parameterItems[4] = ViewTree.InsertItem("Muscle velocity",typeItems[i],parameterItems[3]);
			ViewTree.SetItemData(parameterItems[4], code.encode());
			code.Param = _id_TendonLength;
			parameterItems[5] = ViewTree.InsertItem("Tendon length",typeItems[i],parameterItems[4]);
			ViewTree.SetItemData(parameterItems[5], code.encode());
		}
		typeItems.clear();
	}
	// Ground
	typeItems.clear();
	code = unit_code();
	code.UnitId = _id_Ground;
	code.NNIndex = 0;
	typeItems.push_back( ViewTree.InsertItem(Manager->Walker->get_name( code ).c_str(), bodyItems[4],bodyItems[4]) );
	ViewTree.SetItemData( typeItems[0], code.encode()) ; 
	for(i = 1; i < NUM_TOUCH; i++ ){
		code.NNIndex = i;
		typeItems.push_back( ViewTree.InsertItem(Manager->Walker->get_name( code ).c_str(), bodyItems[4],typeItems[i-1]));
		ViewTree.SetItemData( typeItems[i], code.encode() );
	}
	for(i = 0; i < NUM_TOUCH; i++ ){
		vector<HTREEITEM> parameterItems( 2 );
		code.NNIndex = i;
		code.Param = _id_GroundForce_X;
		parameterItems[0] = ViewTree.InsertItem("Force X",typeItems[i],typeItems[i]);
		ViewTree.SetItemData(parameterItems[0], code.encode());
		code.Param = _id_GroundForce_Y;
		parameterItems[1] = ViewTree.InsertItem("Force Y",typeItems[i],parameterItems[0]);
		ViewTree.SetItemData(parameterItems[1], code.encode());
	}
#endif //__MECHANICS__
	ListView.InsertColumn( 0, "");
	ShowView();
	Activate();
	return TRUE;
}

void CNetworkView::OnAddView() 
{
	HTREEITEM hItem = ViewTree.GetSelectedItem();
	if( hItem != NULL){
		unit_code code(ViewTree.GetItemData( hItem ));
		if( code.Param != CViewParam::None ){
			View.add_view(CViewParam(code));
			ShowView();
			Activate();
		}
	}
}

void CNetworkView::OnRemoveView() 
{
	int numSelected = ListView.GetSelectedCount();
	if( numSelected == 0 )
		return;
	int *delItems = new int [numSelected];
	POSITION pos = ListView.GetFirstSelectedItemPosition();
	if( pos != NULL ){
		for( unsigned int i = 0; pos; i++ )
			delItems[i] = ListView.GetNextSelectedItem( pos );
	}
	for( int i = numSelected-1; i >= 0; i-- ){
		unit_code code = unit_code(ListView.GetItemData( delItems[i] ));
		ListView.DeleteItem( delItems[i] );
		View.del_view( code );
		NumView--;
	}
	delete []delItems;
	Activate();
}

void CNetworkView::OnMoveUp() 
{
	size_t numSelected = ListView.GetSelectedCount();
	if( numSelected == 0 )
		return;
	int *selItems = new int [numSelected];
	POSITION pos = ListView.GetFirstSelectedItemPosition();
	if( pos != NULL ){
		for( size_t i = 0; pos; i++ )
			selItems[i] = ListView.GetNextSelectedItem( pos );
	}

	CString first_str;
	CString second_str;
	DWORD first_lparam;
	DWORD second_lparam;
	int top = 0;
	for( size_t i = 0; i < numSelected; i++ ){
		if( selItems[i] > top){
			first_str = ListView.GetItemText(selItems[i]-1, 0);
			first_lparam = ListView.GetItemData(selItems[i]-1);

			second_str = ListView.GetItemText(selItems[i], 0);
			second_lparam = ListView.GetItemData(selItems[i]);

			ListView.SetItemText(selItems[i]-1, 0, LPCTSTR(second_str));
			ListView.SetItemData(selItems[i]-1, second_lparam);

			ListView.SetItemText(selItems[i], 0, LPCTSTR(first_str));
			ListView.SetItemData(selItems[i], first_lparam);

			View.swap( selItems[i]-1, selItems[i]);
			selItems[i] = selItems[i]-1;
		}
		else{
			top++;
		}
	}
	for( int i = 0; i < ListView.GetItemCount(); i++ )
		ListView.SetItemState(i, 0, LVIS_SELECTED);
	for( size_t i = 0; i < numSelected; i++ )
		ListView.SetItemState(selItems[i], LVIS_SELECTED, LVIS_SELECTED);
	GotoDlgCtrl( &ListView );
	delete []selItems;
}

void CNetworkView::OnMoveDown() 
{
	size_t numSelected = ListView.GetSelectedCount();
	if( numSelected == 0 )
		return;
	int *selItems = new int [numSelected];
	POSITION pos = ListView.GetFirstSelectedItemPosition();
	if( pos != NULL ){
		for( size_t i = 0; pos; i++ )
		selItems[i] = ListView.GetNextSelectedItem( pos );
	}

	CString first_str;
	CString second_str;
	DWORD first_lparam;
	DWORD second_lparam;
	int bottom = ListView.GetItemCount()-1;
	int i = 0;
	for( i = numSelected-1; i >= 0; i-- ){
		if( selItems[i] < bottom){
			first_str = ListView.GetItemText(selItems[i]+1, 0);
			first_lparam = ListView.GetItemData(selItems[i]+1);

			second_str = ListView.GetItemText(selItems[i], 0);
			second_lparam = ListView.GetItemData(selItems[i]);

			ListView.SetItemText(selItems[i]+1, 0, LPCTSTR(second_str));
			ListView.SetItemData(selItems[i]+1, second_lparam);

			ListView.SetItemText(selItems[i], 0, LPCTSTR(first_str));
			ListView.SetItemData(selItems[i], first_lparam);

			View.swap( selItems[i]+1, selItems[i]);
			selItems[i] = selItems[i]+1;
		}
		else{
			bottom--;
		}
	}

	for( i = 0; i < ListView.GetItemCount(); i++ )
		ListView.SetItemState(i, 0, LVIS_SELECTED);
	for( i = 0; i < int( numSelected ); i++ )
		ListView.SetItemState(selItems[i], LVIS_SELECTED, LVIS_SELECTED);
	GotoDlgCtrl( &ListView );
	delete []selItems;
}

void CNetworkView::OnScaleView() 
{
    size_t numSelected = ListView.GetSelectedCount();
    if( numSelected == 0 )
        return;
    int *scaleItems = new int [numSelected];
    POSITION pos = ListView.GetFirstSelectedItemPosition();
    if( pos != NULL ){
        for( size_t i = 0; pos; i++ )
             scaleItems[i] = ListView.GetNextSelectedItem( pos );
        }
    CScaleView dlg;
    dlg.Ymax = View.ymax(scaleItems[0]);
    dlg.Ymin = View.ymin(scaleItems[0]);
    if( dlg.DoModal() == IDOK ){
        for( size_t i = 0; i < numSelected; i++ ){
             View.ymax(scaleItems[i]) = dlg.Ymax;
             View.ymin(scaleItems[i]) = dlg.Ymin;
             }
        }
    delete []scaleItems;
}

void CNetworkView::OnDblclkListView(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnScaleView();
	*pResult = 0;
}

void CNetworkView::OnGeometryView() 
{
	CGeomView dlg;
	dlg.NumCol = View.ncol();
	dlg.NumRow = View.nrow();
	if( dlg.DoModal() == IDOK ){
		View.set_geometry( dlg.NumRow, dlg.NumCol );
	}	
}

void CNetworkView::ShowView( void )
{
	ListView.DeleteAllItems();
	string str;
	LV_ITEM item;
	unit_code code;

	for( size_t i = 0; i < View.npars(); i++ ){
		CViewParam *par = View.get_par( i );
		if( par ){
			code = par->ParCode;
			if( !code.get_fullname( Manager, str ) ){
				View.del_view( code );
				NumView--;
				continue;
			}
			item.iItem = i;
			item.mask = LVIF_TEXT | LVIF_PARAM;
			item.lParam = code.encode(); 
			item.iSubItem = 0;
			item.pszText = ( LPTSTR )str.c_str();
			ListView.InsertItem(&item);
		}
	}
	ListView.SetColumnWidth(0, LVSCW_AUTOSIZE );
	ListView.EnsureVisible( View.npars()-1, 0 );
}

void CNetworkView::Activate( void )
{
	GeomView.EnableWindow( TRUE );
	ScaleView.EnableWindow( TRUE );
	RemoveView.EnableWindow( TRUE );
	AddView.EnableWindow( TRUE );
	if( View.npars() == 0 ){
		GeomView.EnableWindow( FALSE );
		ScaleView.EnableWindow( FALSE );
		RemoveView.EnableWindow( FALSE );
	}
	if( View.npars() >= 1000 )
		AddView.EnableWindow( FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// CImageSize dialog
CImageSize::CImageSize( CWnd* pParent /*=NULL*/)
	: CDialog(CImageSize::IDD, pParent)
{
	Height = 0;
	Width = 0;
}

void CImageSize::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_WIDTH, Spin_Width);
	DDX_Control(pDX, IDC_SPIN_HEIGHT, Spin_Height);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, Height);
	DDX_Text(pDX, IDC_EDIT_WIDTH, Width);
}

BEGIN_MESSAGE_MAP( CImageSize, CDialog )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageSize message handlers
BOOL CImageSize::OnInitDialog() 
{
	CDialog::OnInitDialog();
	Spin_Width.SetRange( 0, UD_MAXVAL );
	Spin_Height.SetRange( 0, UD_MAXVAL );
	Spin_Width.SetPos( Width );
	Spin_Height.SetPos( Height );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

#endif // __CONSOLE__
