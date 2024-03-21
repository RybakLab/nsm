/////////////////////////////////////////////////////////////////////////////
// MuscleSetup.cpp : implementation file
#include "precompile.h"

#ifndef __CONSOLE__

#include "biomechsetup.h"

#if defined (__MECHANICS_2D__)

CBiomechSetup::CBiomechSetup( CWnd* pParent ) :
		CDialog( CBiomechSetup::IDD, pParent ), BiomechT( NULL, GRID_NONE )
{
}

void CBiomechSetup::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_GRID, BiomechView );
}

BEGIN_MESSAGE_MAP(CBiomechSetup, CDialog)
	ON_NOTIFY( NM_CLICK, IDC_GRID, &CBiomechSetup::OnGridClick )
	ON_NOTIFY( GVN_ENDLABELEDIT, IDC_GRID, &CBiomechSetup::OnEndCellEdit )
//	ON_NOTIFY( NM_DBLCLK, IDC_GRID, OnGridDblClick )
//	ON_NOTIFY( NM_CLICK, IDC_BIOMECH_PROP, OnGridClick )
//	ON_NOTIFY( GVN_ENDLABELEDIT, IDC_BIOMECH_PROP, OnEndCellEdit )
END_MESSAGE_MAP()

void CBiomechSetup::OnGridClick( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	BiomechView.OnGridClick( pNotifyStruct, pResult );
	if( *pResult & GRID_PROCESS ){ 
		string path;
		BiomechView.GetActiveBranch( path );
		OnFlyUpdate( path );
	}
	*pResult = 0;
}

void CBiomechSetup::OnEndCellEdit( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	BiomechView.OnEndCellEdit( pNotifyStruct, pResult );
	string path;
	if( *pResult & GRID_PROCESS ){ 
		BiomechView.GetActiveBranch( path );
		OnFlyUpdate( path );
	} else{	UpdateView( true ); }
	*pResult = 0;
}

void CBiomechSetup::OnFlyUpdate( const string &path )
{
	unsigned int pos = path.rfind( '\t' );
	if( pos != string::npos ){
		string parent_path = path.substr( 0, pos );
		string var_path = path.substr( pos+1 );
		uni_template *unit = BiomechT.get_unit( parent_path.c_str() );
		if( unit ){
			UpdateView( true );
			if( unit->process( var_path.c_str() )){
				DisplayUnit( &BiomechT, string( BiomechT.get_name()), false );
				BiomechView.Update( false );
			}
		} else{ OnFlyUpdate( parent_path ); }
	}
}

BOOL CBiomechSetup::OnInitDialog() 
{
	CDialog::OnInitDialog();
	BiomechView.InitGridCtrl( "Name" );
	DisplayUnit( &BiomechT, string( BiomechT.get_type()));
	BiomechView.Update();
	UpdateView( false );
	return TRUE;
}

void CBiomechSetup::UpdateView( bool save )
{
	BiomechView.UpdateData( save );
	map<_Grid_Key,_Grid_Element> *all_cells = BiomechView.GetMap();
	typedef map<_Grid_Key,_Grid_Element>::iterator it;
	for( it pos = all_cells->begin(); pos != all_cells->end(); ++pos ){
		BiomechT.update( pos->first.Path.c_str(), pos->first.Name.c_str(), pos->second.Data, save );
	}
	BiomechView.UpdateData( save );
}

void CBiomechSetup::OnOK( void )
{
	UpdateView( true );
	CDialog::OnOK();
}

bool CBiomechSetup::DisplayUnit( uni_template *unit, const string &start_path, bool collapse )
{
	if( unit ){
		bool is_success = DisplayUnitPar( unit, start_path );
		return  DisplayUnitChildren( unit, start_path, collapse ) || is_success;
	}
	return false;
}

bool CBiomechSetup::DisplayUnitPar( uni_template *unit, const string &start_path )
{
	vector<pair<_Grid_Key,_Grid_Element> > par_list;
	unit->get_allpar( start_path.c_str(), par_list );
	unsigned int pos = 0;
	for( pos = 0; pos < par_list.size(); ++pos ){
		if( !BiomechView.InsertElement( par_list[pos].first.Path, par_list[pos].first.Name,
			par_list[pos].second.InitStr, par_list[pos].second.Type )){
			BiomechView.SetElement( par_list[pos].first.Path, par_list[pos].first.Name, par_list[pos].second );
		}
	}
	return ( pos > 0? true:false );
}

bool CBiomechSetup::DisplayUnitChildren( uni_template *unit, const string &start_path, bool collapse )
{
	vector<pair<string,int> > path;
	unit->get_childrenpath( start_path.c_str(), path );
	unsigned int pos = 0;
	for( pos = 0; pos < path.size(); ++pos ){
		if( !DisplayUnit( BiomechT.get_unit( path[pos].first.c_str()), path[pos].first, collapse )){
			BiomechView.InsertElement( path[pos].first, "Data", "", path[pos].second );
			BiomechView.SetActiveElement( path[pos].first, path[pos].second );
		}
	}
	pair<string,int> unit_prop;
	unit->get_properties( start_path.c_str(), unit_prop );
	BiomechView.SetActiveElement( unit_prop.first, unit_prop.second );
	if( collapse ){
		BiomechView.CollapseAllSubLevels( unit_prop.first );
	}
	return ( pos > 0? true:false );
}
#elif defined (__MECHANICS_3D__)
// TODO implementation 3d model
#endif // __MECHANICS_2D__
#endif // __CONSOLE__
