/////////////////////////////////////////////////////////////////////////////
// neuronsetup.cpp : implementation file
#include "precompile.h"

#include "neuronsetup.h"
#include "usermessage.h"

#ifndef __CONSOLE__
#include "../Spcord.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG


/////////////////////////////////////////////////////////////////////////////
// CPopulatProperties dialog
CPopulatProperties::CPopulatProperties( CWnd* pParent /*=NULL*/ )
	: CDialog( CPopulatProperties::IDD, pParent ), PopulateTemplate( NULL, GRID_NONE )
{
}

void CPopulatProperties::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_GRID, NeuronView );
}

BEGIN_MESSAGE_MAP(CPopulatProperties, CDialog)
	ON_NOTIFY( NM_CLICK, IDC_GRID, &CPopulatProperties::OnGridClick )
	ON_NOTIFY( NM_DBLCLK, IDC_GRID, &CPopulatProperties::OnGridDblClick )
	ON_NOTIFY( GVN_ENDLABELEDIT, IDC_GRID, &CPopulatProperties::OnEndCellEdit )
	ON_BN_CLICKED( IDOK, &CPopulatProperties::OnBnClickedOk )
	ON_MESSAGE( GUI_DEL_UNITS, &CPopulatProperties::OnDelUnits )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPopulatProperties message handlers
void CPopulatProperties::OnGridClick( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	NeuronView.OnGridClick( pNotifyStruct, pResult );
	if( *pResult & GRID_PROCESS ){ 
		string path;
		NeuronView.GetActiveBranch( path );
		OnFlyUpdate( path );
	}
	*pResult = 0;
}

void CPopulatProperties::OnGridDblClick( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	NeuronView.OnGridDblClk( pNotifyStruct, pResult );
	switch( *pResult & GRID_ALLATTR ){
		case GRID_DEL_ROW:
			del_unit();
		break;
		case GRID_ADD_ROW:{
			NM_GRIDVIEW* pItem = ( NM_GRIDVIEW *)pNotifyStruct;
			CRect rect; 
			if( NeuronView.GetCellRect( pItem->iRow, pItem->iColumn, &rect )){
				CPoint point;
				point.x = rect.left;
				point.y = rect.bottom;
				NeuronView.GetWindowRect( &rect );
				point.x += rect.left;
				point.y += rect.top;
				add_unit( point );
			}
		}
		break;
		default:;
	}
	*pResult = 0;
}

void CPopulatProperties::OnEndCellEdit( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	NeuronView.OnEndCellEdit( pNotifyStruct, pResult );
	string path;
	if( *pResult & GRID_PROCESS ){ 
		NeuronView.GetActiveBranch( path );
		OnFlyUpdate( path );
	} else if( *pResult & GRID_STRING ){
		NeuronView.GetActiveBranch( path );
		char delim = '\t';
		typedef string::size_type size_type;
		size_type pos = path.rfind( delim );
		path = path.substr( 0, pos );
		uni_template *unit = PopulateTemplate.get_unit( path.c_str());
		if( unit ){
			string old_name = unit->get_name();
			UpdateView( true );
			string new_name = unit->get_name();
			string new_path;
			if( pos != string::npos ){
				pos = path.rfind( delim );
			}
			new_path = path.substr( 0, pos );
			new_path += delim;
			new_path += new_name;
			unit->set_name( old_name.c_str());
			if( !PopulateTemplate.get_unit( new_path.c_str())){
				unit->set_name( new_name.c_str());
				NeuronView.DelElements( path );
				NeuronView.Update( false );
				PopulateTemplate.add_unit( new_path.c_str() );
				DisplayUnit( unit, new_path );
				NeuronView.Update( false );
			}
			UpdateView( false );
		}
	} else{ UpdateView( true ); }
	*pResult = 0;
}

void CPopulatProperties::OnFlyUpdate( const string &path )
{
	size_t pos = path.rfind( '\t' );
	if( pos != string::npos ){
		string unit_path = path.substr( 0, pos );
		string var_path = path.substr( pos+1 );
		uni_template *unit = PopulateTemplate.get_unit( unit_path.c_str() );
		if( unit ){
			UpdateView( true );
			if( unit->process( var_path.c_str() )){
				DisplayUnit( &PopulateTemplate, string( PopulateTemplate.get_name()), false );
				string dpath;
				string parent_path;
				vector<string> add_paths, del_paths;
				PopulateTemplate.reconfig( unit_path.c_str(), add_paths, del_paths, parent_path );
				if( !add_paths.empty()){
					DisplayUnit( &PopulateTemplate, string( PopulateTemplate.get_name()) );
					UpdateView( false );
				}
				if( !del_paths.empty()){
					ParentPath = parent_path;
					DelPaths = del_paths;
					PostMessage( GUI_DEL_UNITS, 0, 0 );
				}
				NeuronView.Update( false );
			}
		} else{ OnFlyUpdate( unit_path ); }
	}
}

BOOL CPopulatProperties::OnInitDialog() 
{
	CDialog::OnInitDialog();
	NeuronView.InitGridCtrl( "Name" );
	DisplayUnit( &PopulateTemplate, string( PopulateTemplate.get_name()));
	NeuronView.Update();
	UpdateView( false );
	return TRUE;
}

void CPopulatProperties::UpdateView( bool save )
{
	NeuronView.UpdateData( save );
	map<_Grid_Key,_Grid_Element> *all_cells = NeuronView.GetMap();
	typedef map<_Grid_Key,_Grid_Element>::iterator it;
	for( it pos = all_cells->begin(); pos != all_cells->end(); ++pos ){
		PopulateTemplate.update( pos->first.Path.c_str(), pos->first.Name.c_str(), pos->second.Data, save );
	}
	NeuronView.UpdateData( save );
}

void CPopulatProperties::add_unit( const CPoint &point )
{
	string path;
	if( NeuronView.GetActiveBranch( path )){
		std::vector<std::string> names;
		PopulateTemplate.get_unusednames( path.c_str(), names );
		HMENU hMenu = ::CreatePopupMenu();
		if( !names.empty() && NULL != hMenu ){
			for( size_t i = 0; i < names.size(); ++i )
				::AppendMenu( hMenu, MF_STRING, i+1, names[i].c_str() );
			CMenu menu;
			menu.Attach( hMenu );
			NeuronView.SetMenu( &menu );
			int sel = menu.TrackPopupMenuEx( TPM_LEFTALIGN | TPM_RETURNCMD, point.x, point.y, &NeuronView, NULL );
			if( sel <= 0 )
				return;
			menu.Detach();
			DrawMenuBar();
			string full_path = path;
			full_path += "\t";
			full_path += names[sel-1];
			if( PopulateTemplate.add_unit( full_path.c_str() )){
				string parent_path;
				vector<string> add_paths, del_paths;
				PopulateTemplate.reconfig( full_path.c_str(), add_paths, del_paths, parent_path );
				DisplayUnit( &PopulateTemplate, string( PopulateTemplate.get_name()));
			}
			NeuronView.Update( false );
		}
		UpdateView( false );
	}
}

void CPopulatProperties::del_unit( void ) 
{
	string path;
	if( NeuronView.GetActiveBranch( path )){
		if( NeuronView.DelElements( path ) ){
			string parent_path;
			vector<string> add_paths, del_paths;
			PopulateTemplate.del_unit( path.c_str(), del_paths, parent_path );
			for( size_t i = 0; i < del_paths.size(); ++i ){
				path = parent_path+del_paths[i];
				NeuronView.DelElements( path );
			}
			NeuronView.Update( false );
		}
	}
}

LRESULT CPopulatProperties::OnDelUnits( WPARAM /*var1*/, LPARAM /*var2*/ )
{
	string dpath;
	for( size_t i = 0; i < DelPaths.size(); ++i ){
		dpath = ParentPath+DelPaths[i];
		NeuronView.DelElements( dpath );
	}
	DelPaths.clear();
	ParentPath.clear();
	NeuronView.Update( false );
	return 0;
}

bool CPopulatProperties::DisplayUnit( uni_template *unit, const string &start_path, bool collapse )
{
	if( unit ){
		bool is_success = DisplayUnitPar( unit, start_path );
		return  DisplayUnitChildren( unit, start_path, collapse ) || is_success;
	}
	return false;
}

bool CPopulatProperties::DisplayUnitPar( uni_template *unit, const string &start_path )
{
	vector<pair<_Grid_Key,_Grid_Element> > par_list;
	unit->get_allpar( start_path.c_str(), par_list );
	size_t pos = 0;
	for( pos = 0; pos < par_list.size(); ++pos ){
		if( !NeuronView.InsertElement( par_list[pos].first.Path, par_list[pos].first.Name,
			par_list[pos].second.InitStr, par_list[pos].second.Type )){
			NeuronView.SetElement( par_list[pos].first.Path, par_list[pos].first.Name, par_list[pos].second );
		}
	}
	return ( pos > 0? true:false );
}

bool CPopulatProperties::DisplayUnitChildren( uni_template *unit, const string &start_path, bool collapse )
{
	vector<pair<string,int> > path;
	unit->get_childrenpath( start_path.c_str(), path );
	size_t pos = 0;
	for( pos = 0; pos < path.size(); ++pos ){
		if( !DisplayUnit( PopulateTemplate.get_unit( path[pos].first.c_str()), path[pos].first, collapse )){
			NeuronView.InsertElement( path[pos].first, "Data", "", path[pos].second );
			NeuronView.SetActiveElement( path[pos].first, path[pos].second );
		}
	}
	pair<string,int> unit_prop;
	unit->get_properties( start_path.c_str(), unit_prop );
	NeuronView.SetActiveElement( unit_prop.first, unit_prop.second );
	if( unit_prop.second != GRID_NONE  && collapse ){
		NeuronView.CollapseAllSubLevels( unit_prop.first );
	}
	return ( pos > 0? true:false );
}

void CPopulatProperties::OnBnClickedOk()
{
	UpdateView( true );
	OnOK();
}

#endif // __CONSOLE__
