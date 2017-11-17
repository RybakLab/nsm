// genericdialogs.cpp : implementation file
#include "precompile.h"

#ifndef __CONSOLE__

#include "resource.h"
#include "genericdialogs.h"
#include "GridBtnCell.h"
#include "GridBtnCellCombo.h"
#include "GridTreeCell.h"

#define UTIL_COLUMN 0
#define TREE_COLUMN ( UTIL_COLUMN+1 )
#define FIXED_COLUMN_COUNT ( TREE_COLUMN+1 )
#define CTL_COLUMN_BEGIN ( TREE_COLUMN+1 )
#define FIXED_ROW_COUNT 1
#define TOP_LEVEL 1

/////////////////////////////////////////////////////////////////////////////
// CGridSetup
CGridSetup::CGridSetup( int IDD_IMAGE_RES, char delim )
{
	Delim = delim;
	m_ImageList.Create( MAKEINTRESOURCE( IDD_IMAGE_RES ), 14, 1, RGB( 255,255,255 ));
	LastRow = LastCol = 0;
}

CGridSetup::~CGridSetup( void )
{
}

/////////////////////////////////////////////////////////////////////////////
// public methods
bool CGridSetup::InitGridCtrl( const string &name )
{
	SetImageList( &m_ImageList );
	unsigned char ucTreeLevelAry[] = { 0 };
	TRY{
		SetColumnCount( CTL_COLUMN_BEGIN );
		SetFixedColumnCount( FIXED_COLUMN_COUNT );

		m_TreeColumn.TreeSetup( this, TREE_COLUMN, NULL, FIXED_ROW_COUNT, ucTreeLevelAry, TRUE, FALSE );
		m_TreeColumn.SetTreeLineColor( RGB( 0x20, 0x20, 0x20) );
	}
	CATCH( CMemoryException, e ){
		e->ReportError();
		e->Delete();
		return false;
	}
	END_CATCH
	m_TreeColumn.TreeDisplayOutline( UCHAR_MAX );
	EnableBtnDataBase.SetGrid( this );
	DisableBtnDataBase.SetGrid( this );
	GridCells.clear();
	Name = name;
	SetRowResize( FALSE );
	EnableSelection( FALSE );
	SetItemState( 0, 0, GVIS_DROPHILITED );
	SetItemText( 0, TREE_COLUMN, name.c_str() );
	SetItemData( 0, TREE_COLUMN, GRID_NONE );
	SetItemFormat( 0, TREE_COLUMN, DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS );
	LastRow = LastCol = 0;
	return true;
}

bool CGridSetup::GetElement( const string &path, const string &name, string &data ) const
{
	_Grid_Key key( path, name );
	typedef map<_Grid_Key,_Grid_Element>::const_iterator it;
	it pos = GridCells.find( key );
	if( pos != GridCells.end() ){
		data = ( pos->second ).Data;
		return true;
	}
	return false;  
}

bool CGridSetup::GetActiveElement( string &name, string &data ) const
{
	CCellID cell_id = GetFocusCell();
	typedef map<_Grid_Key,_Grid_Element>::const_iterator it;
	for( it pos = GridCells.begin(); pos != GridCells.end(); ++pos ){
		if( pos->second.Position.first == cell_id.row && pos->second.Position.second == cell_id.col ){
			name = pos->first.Name;
			data = pos->second.Data;
			return true;
		}
	}
	return false; 
}

bool CGridSetup::GetBranch( string &branch_path, int row )
{
	_Grid_Element cell;
	CGridTreeCellBase *pGridCell = ( CGridTreeCellBase *)GetCell( row, TREE_COLUMN );
	if( pGridCell ){
		for( int level = 255; row >= GetFixedRowCount(); --row ){
			pGridCell = ( CGridTreeCellBase *)GetCell( row, TREE_COLUMN );
			if( pGridCell ){
				if( level > pGridCell->GetLevel() ){
					level = pGridCell->GetLevel();
					branch_path.insert( 0, pGridCell->GetText());
					branch_path.insert( 0, 1, Delim );
				}
			}
		}
	}
	if( branch_path.size() > 0 ){
		branch_path.erase( branch_path.begin() );
		return true;
	}
	branch_path.clear();
	return false;
}

bool CGridSetup::GetActiveBranch( string &branch_path )
{
	CCellID cell_id = GetFocusCell();
	return GetBranch( branch_path, cell_id.row );
}

bool CGridSetup::GetString( const string &path, vector<string> &elements ) const
{
	elements.clear();
	typedef map<_Grid_Key,_Grid_Element>::const_iterator it;
	for( it pos = GridCells.begin(); pos != GridCells.end(); ++pos ){
		if(( pos->first ).Path == path )
			elements.push_back(( pos->second ).Data );
	}
	return !elements.empty();
}

bool CGridSetup::SetElement( const string &path, const string &name, const _Grid_Element &element  )
{
	_Grid_Key key( path, name );
	typedef map<_Grid_Key,_Grid_Element>::iterator it;
	it pos = GridCells.find( key );
	if( pos != GridCells.end() ){
		( pos->second ).Data = element.Data;
		( pos->second ).Type = element.Type;
		return true;
	}
	return false;  
}

bool CGridSetup::SetActiveElement( const string &path, int action )
{
	unsigned char levels = TOP_LEVEL;
	pair<int, int> row_range = make_pair( m_TreeColumn.GetFixedRowCount(), m_TreeColumn.GetRowCount());  
	typedef string::size_type size_type;
	size_type pos = 0, next_pos = 0;
	for( pos = 0, next_pos = 0; next_pos < string::npos; pos = next_pos+1 ){
		next_pos = path.find( Delim, pos );
		string sub_path = path.substr( pos, next_pos-pos );
		if( !FindBranch( sub_path, levels, row_range ))
			break;
	}
	if( next_pos == string::npos ){
		if( action >= GRID_DEL_ROW ){
			SetItemData( row_range.first, UTIL_COLUMN, action );
			SetItemFormat( row_range.first, UTIL_COLUMN, DT_LEFT|DT_VCENTER );
			SetItemImage( row_range.first, UTIL_COLUMN, action>>BEG_IMAGE_INDEX );
		}
		return true;
	}
	return false;
}

bool CGridSetup::InsertElement( const string &path, const string &name, const string &data, int type )
{
	_Grid_Key key( path, name );
	if( GridCells.find( key ) == GridCells.end() ){
		pair<int,int> pos = InsertBranch( path, name, type );
		if( pos.first != -1 && pos.second != -1 ){
			_Grid_Element element( data, type, pos );
			GridCells[key] = element;
			LastRow = LastCol = 0;
			return true;
		}
	}
	return false;
}

bool CGridSetup::DelElements( const string &path )
{
	unsigned char levels = TOP_LEVEL;
	pair<int, int> row_range = make_pair( m_TreeColumn.GetFixedRowCount(), m_TreeColumn.GetRowCount());  
	typedef string::size_type size_type;
	size_type pos = 0, next_pos = 0;
	for( pos = 0, next_pos = 0; next_pos < string::npos; pos = next_pos+1 ){
		next_pos = path.find( Delim, pos );
		string sub_path = path.substr( pos, next_pos-pos );
		if( !FindBranch( sub_path, levels, row_range ))
			break;
	}
	if( next_pos == string::npos ){
		return DelBranch( path, row_range );
	}
	return false;
}

void CGridSetup::ExpandOneLevel( const string &path )
{
	unsigned char levels = TOP_LEVEL;
	pair<int, int> row_range = make_pair( m_TreeColumn.GetFixedRowCount(), m_TreeColumn.GetRowCount());  
	typedef string::size_type size_type;
	for( size_type pos = 0, next_pos = 0; next_pos < string::npos; pos = next_pos+1 ){
		next_pos = path.find( Delim, pos );
		string sub_path = path.substr( pos, next_pos-pos );
		if( !FindBranch( sub_path, levels, row_range ))
			break;
	}
	m_TreeColumn.TreeDataExpandOneLevel( row_range.first );
	m_TreeColumn.TreeRefreshRows();
}

void CGridSetup::CollapseAllSubLevels( const string &path )
{
	unsigned char levels = TOP_LEVEL;
	pair<int, int> row_range = make_pair( m_TreeColumn.GetFixedRowCount(), m_TreeColumn.GetRowCount());  
	typedef string::size_type size_type;
	for( size_type pos = 0, next_pos = 0; next_pos < string::npos; pos = next_pos+1 ){
		next_pos = path.find( Delim, pos );
		string sub_path = path.substr( pos, next_pos-pos );
		if( !FindBranch( sub_path, levels, row_range ))
			break;
	}
	m_TreeColumn.TreeDataCollapseAllSubLevels( row_range.first );
	m_TreeColumn.TreeRefreshRows();
}

void CGridSetup::Update( bool auto_size )
{
	for( int col = CTL_COLUMN_BEGIN; col < GetColumnCount(); ++col ){ 
		for( int row = FIXED_ROW_COUNT; row < GetRowCount(); ++row ){
			SetItemState( row, col, GVIS_READONLY );
		}
	}
	UpdateData( false );
	Invalidate();
	UpdateWindow();
	SetupBntStyle();
	if( auto_size ){
		for( int col = UTIL_COLUMN; col < GetColumnCount(); ++col )
			AutoSizeColumn( col, GVS_BOTH );
	}
}

void CGridSetup::UpdateData( bool upload )
{
	typedef map<_Grid_Key,_Grid_Element>::iterator it;
	for( it pos = GridCells.begin(); pos != GridCells.end(); ++pos ){
		int row = pos->second.Position.first;
		int col = pos->second.Position.second;
		CGridCellBase *pGridCell = ( CGridCellBase * )GetCell( row, col );
		if( pGridCell ){
			if( pos->second.Type & GRID_READONLY ){
				SetItemState( row, col, GVIS_READONLY );
			}
			else{
				SetItemState( row, col, GVIS_MODIFIED );
			}
			string data = LPCTSTR( pGridCell->GetText() );
			switch( pos->second.Type & GRID_ALLTYPES ){
				case GRID_BOOL:
					if( upload ){
						if( data == "true" ){
						pos->second.Data = "true";
						}
						else{
							pos->second.Data = "false";
						}
					}
					else{
						if( pos->second.Data == "true" ){
							pGridCell->SetText( "true" );
						}
						else{
							pGridCell->SetText( "false" );
						}
					}
				break;
				case GRID_COMBO:
				case GRID_STRING:
					if( upload && !data.empty() ){
						pos->second.Data = data;
					}
					else{
						pGridCell->SetText( pos->second.Data.c_str() );
					}
				break;
				case GRID_INT:
					int idata;
					data >> idata;
					if(( pos->second.Type & GRID_POSITIVE ) && idata <= 0 ){
						if( pos->second.Type & GRID_EXCL_ZERO )
							idata = 1;
						else
							idata = 0;
					}
					if(( pos->second.Type & GRID_NEGATIVE ) && idata >= 0 ){
						if( pos->second.Type & GRID_EXCL_ZERO )
							idata = -1;
						else
							idata = 0;
					}
					data << idata;
					if( upload ){
						pos->second.Data = data;
					}
					else{
						pGridCell->SetText( pos->second.Data.c_str() );
					}
				break;
				case GRID_DOUBLE:
					double fdata;
					data >> fdata;
					if(( pos->second.Type & GRID_POSITIVE ) && fdata <= 0 ){
						if( pos->second.Type & GRID_EXCL_ZERO )
							fdata = 0.1;
						else
							fdata = 0;
					}
					if(( pos->second.Type & GRID_NEGATIVE ) && fdata >= 0 ){
						if( pos->second.Type & GRID_EXCL_ZERO )
							fdata = -0.1;
						else
							fdata = 0;
					}
					data << fdata;
					if( upload ){
						pos->second.Data = data;
					}
					else{
						pGridCell->SetText( pos->second.Data.c_str() );
					}
				break;
				default:
					SetItemState( row, col, GVIS_READONLY );
			}
		}
	}
}

void CGridSetup::OnEndCellEdit( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	NM_GRIDVIEW *pItem = ( NM_GRIDVIEW *)pNotifyStruct;
	int row = pItem->iRow;
	int col = pItem->iColumn;
	pair<_Grid_Key,_Grid_Element> cell;
	if( GetCell( row, col )){
		GetElement( row, col, cell  );
	}
	*pResult = cell.second.Type;
}

void CGridSetup::OnGridClick( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	NM_GRIDVIEW* pItem = ( NM_GRIDVIEW *)pNotifyStruct;
	UpdateCell( LastRow, LastCol );
	int row = pItem->iRow;
	int col = pItem->iColumn;
	CString strMsg;
	pair<_Grid_Key,_Grid_Element> cell;

	CGridCellBase *pGridCell = GetCell( row, col );
	if( pGridCell ){
		GetElement( row, col, cell  );
		if( pGridCell->IsKindOf( RUNTIME_CLASS( CGridBtnCell )) && !( cell.second.Type & GRID_READONLY )){
			switch( cell.second.Type & GRID_ALLTYPES ){
				case GRID_BOOL:
				case GRID_COMBO:
				case GRID_STRING:
				break;
				case GRID_INT:
				{
					int data, offset;
					data = offset = 0;
					if((( CGridBtnCell *)pGridCell )->GetLastCtlClicked() == 0)
						offset = 1;
					else if((( CGridBtnCell *)pGridCell )->GetLastCtlClicked() > 0 )
						offset = -1;
					strMsg = pGridCell->GetText();
					data = atoi( LPCTSTR( strMsg ));
					data += offset;
					UpdateCell( row, col, &data );
				}
				break;
				case GRID_DOUBLE:
				{
					double data, offset;
					data = offset = 0.0;
					if((( CGridBtnCell *)pGridCell )->GetLastCtlClicked() == 0 )
						offset = 0.1;
					else if((( CGridBtnCell *)pGridCell )->GetLastCtlClicked() > 0 )
						offset = -0.1;
					strMsg = pGridCell->GetText();
					data = atof( LPCTSTR( strMsg ));
					data += offset;
					UpdateCell( row, col, &data );
				}
				break;
				default:;
			} 
		}
	}
	LastRow = pItem->iRow; LastCol = pItem->iColumn;
	*pResult = cell.second.Type;
}

void CGridSetup::OnGridDblClk( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	NM_GRIDVIEW* pItem = ( NM_GRIDVIEW *)pNotifyStruct;
	int row = pItem->iRow;
	int col = pItem->iColumn;
	if( row < GetFixedRowCount() || col != UTIL_COLUMN )
		return;
	*pResult = GetItemData( row, col );
}

void CGridSetup::SetupBntStyle( void )
{
	static CGridBtnCellBase::STRUCT_DRAWCTL scroll_up = { 0, DFCS_SCROLLUP, FALSE, CGridBtnCellBase::CTL_ALIGN_RIGHT, DFC_SCROLL };
	static CGridBtnCellBase::STRUCT_DRAWCTL scroll_down = { 0, DFCS_SCROLLDOWN, FALSE, CGridBtnCellBase::CTL_ALIGN_RIGHT, DFC_SCROLL };
	static CGridBtnCellBase::STRUCT_DRAWCTL combobox = { 0, DFCS_SCROLLDOWN, FALSE, CGridBtnCellBase::CTL_ALIGN_RIGHT, DFC_SCROLL };
	static bool IsComboStyle = false;
	static bool IsSpinStyle = false;

	int width = GetSystemMetrics( SM_CXVSCROLL )+2;
	typedef map<_Grid_Key,_Grid_Element>::iterator it;
	for( it pos = GridCells.begin(); pos != GridCells.end(); ++pos ){
		int row = pos->second.Position.first;
		int col = pos->second.Position.second;
		CGridBtnCell *pGridBtnCell = NULL;
		switch( pos->second.Type & GRID_ALLTYPES ){
			case GRID_BOOL:
			{
				if( !pos->second.IsStyle ){
					pos->second.IsStyle = true;
					SetCellType( row, col, RUNTIME_CLASS( CGridBtnCellCombo ));
					CStringArray strAryCombo;
					strAryCombo.Add( "true" );
					strAryCombo.Add( "false" );
					pGridBtnCell = ( CGridBtnCell* )GetCell( row, col);
					(( CGridBtnCellCombo *)pGridBtnCell )->SetComboStyle( CBS_DROPDOWNLIST );
					(( CGridBtnCellCombo *)pGridBtnCell )->SetComboString( strAryCombo );
				}
				else{
					pGridBtnCell = ( CGridBtnCell* )GetCell( row, col);
				}
				if( pos->second.Type & GRID_READONLY ){
					pGridBtnCell->SetBtnDataBase( &DisableBtnDataBase );
					pGridBtnCell->SetupBtns( 0, combobox.ucType, combobox.sState, ( CGridBtnCellBase::CTL_ALIGN )combobox.ucAlign, width+2, combobox.ucIsMbrRadioGrp, NULL );
					(( CGridBtnCellCombo *)pGridBtnCell )->SetComboStyle( CBS_DROPDOWNLIST );
					pGridBtnCell->SetBackClr( GetFixedBkColor() );
				}
				else{
					pGridBtnCell->SetBtnDataBase( &EnableBtnDataBase );
					pGridBtnCell->SetupBtns( 0, combobox.ucType, combobox.sState, ( CGridBtnCellBase::CTL_ALIGN )combobox.ucAlign, width+2, combobox.ucIsMbrRadioGrp, NULL );
					(( CGridBtnCellCombo *)pGridBtnCell )->SetComboStyle( CBS_DROPDOWNLIST );
				}
			}
			break;
			case GRID_COMBO:
			{
				if( !pos->second.IsStyle ){
					pos->second.IsStyle = true;
					SetCellType( row, col, RUNTIME_CLASS( CGridBtnCellCombo ));
					string str_combo = pos->second.InitStr;
					CStringArray strAryCombo;
					typedef string::size_type size_type;
					size_type pos = 0, next_pos = 0;
					for( pos = 0, next_pos = 0; next_pos < string::npos; pos = next_pos+1 ){
						next_pos = str_combo.find( Delim, pos );
						string combo = str_combo.substr( pos, next_pos-pos );
						strAryCombo.Add( combo.c_str() );
					}
					pGridBtnCell = ( CGridBtnCell* )GetCell( row, col);
					(( CGridBtnCellCombo *)pGridBtnCell )->SetComboStyle( CBS_DROPDOWN );
					(( CGridBtnCellCombo *)pGridBtnCell )->SetComboString( strAryCombo );
				}
				else{
					pGridBtnCell = ( CGridBtnCell* )GetCell( row, col);
				}
				if( pos->second.Type & GRID_READONLY ){
					pGridBtnCell->SetBtnDataBase( &DisableBtnDataBase );
					pGridBtnCell->SetupBtns( 0, combobox.ucType, combobox.sState, ( CGridBtnCellBase::CTL_ALIGN )combobox.ucAlign, width+2, combobox.ucIsMbrRadioGrp, NULL );
					(( CGridBtnCellCombo *)pGridBtnCell )->SetComboStyle( CBS_DROPDOWN );
					pGridBtnCell->SetBackClr( GetFixedBkColor() );
				}
				else{
					pGridBtnCell->SetBtnDataBase( &EnableBtnDataBase );
					pGridBtnCell->SetupBtns( 0, combobox.ucType, combobox.sState, ( CGridBtnCellBase::CTL_ALIGN )combobox.ucAlign, width+2, combobox.ucIsMbrRadioGrp, NULL );
					(( CGridBtnCellCombo *)pGridBtnCell )->SetComboStyle( CBS_DROPDOWN );
				}
			}
			break;
			case GRID_INT:
			case GRID_DOUBLE:
			{
				if( !pos->second.IsStyle ){
					pos->second.IsStyle = true;
					SetCellType( row, col, RUNTIME_CLASS( CGridBtnCell ));
					pGridBtnCell = ( CGridBtnCell* )GetCell( row, col);
				}
				else{
					pGridBtnCell = ( CGridBtnCell* )GetCell( row, col);
				}
				if( pos->second.Type & GRID_READONLY ){
					pGridBtnCell->SetBtnDataBase( &DisableBtnDataBase );
					pGridBtnCell->SetupBtns( 0, scroll_up.ucType, scroll_up.sState, ( CGridBtnCellBase::CTL_ALIGN )scroll_up.ucAlign, width, scroll_up.ucIsMbrRadioGrp, NULL );
					pGridBtnCell->SetupBtns( 1, scroll_down.ucType, scroll_down.sState, ( CGridBtnCellBase::CTL_ALIGN )scroll_down.ucAlign, width, scroll_down.ucIsMbrRadioGrp, NULL );
					pGridBtnCell->SetBackClr( GetFixedBkColor() );
				}
				else{
					pGridBtnCell->SetBtnDataBase( &EnableBtnDataBase );
					pGridBtnCell->SetupBtns( 0, scroll_up.ucType, scroll_up.sState, ( CGridBtnCellBase::CTL_ALIGN )scroll_up.ucAlign, width, scroll_up.ucIsMbrRadioGrp, NULL );
					pGridBtnCell->SetupBtns( 1, scroll_down.ucType, scroll_down.sState, ( CGridBtnCellBase::CTL_ALIGN )scroll_down.ucAlign, width, scroll_down.ucIsMbrRadioGrp, NULL );
				}
			}
			break;
			default:;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// private methods
int CGridSetup::AddNewName( const string &name, int type )
{
	int col = GetNamePos( name );
	if( col == -1 ){
		int row = 0;
		col = GetColumnCount();
		SetColumnCount( col+1 );
		SetItemText( row, col, name.c_str() );
		LastRow = LastCol = 0;
	}
	return col;
}

bool CGridSetup::GetElement( int row, int col, pair<_Grid_Key,_Grid_Element> &x ) const
{
	typedef map<_Grid_Key,_Grid_Element>::const_iterator it;
	for( it pos = GridCells.begin(); pos != GridCells.end(); ++pos ){
		if( pos->second.Position.first == row && pos->second.Position.second == col ){
			x = *pos;
			return true;
		}
	}
	return false; 
}

int CGridSetup::GetNamePos( const string &name )
{
	for( int row = 0; row < m_TreeColumn.GetFixedRowCount(); ++row ){
		for( int col = CTL_COLUMN_BEGIN; col < GetColumnCount(); ++col ){
			CGridTreeCellBase *cell = ( CGridTreeCellBase *)GetCell( row, col );
			if( name == cell->GetText() )
				return col;
		}
	}
	return -1;
}

pair<int,int> CGridSetup::InsertBranch( const string &path, const string &name, int type )
{
	vector<unsigned char> tree_levels;
	int ins_row = -1; // if ins_row == -1 the new branch should be attached
	if( FindPath( path, tree_levels, ins_row )){
		return make_pair( ins_row, AddNewName( name, type ));
	}
	else{
		unsigned char *tree = new unsigned char[tree_levels.size()];
		for( unsigned int i = 0; i < tree_levels.size(); ++i )
			tree[i] = tree_levels[i];
		if( m_TreeColumn.InsertTreeBranch( tree, tree_levels.size(), ins_row, FALSE ) != -1 &&  AddNewName( name, type ) != -1 ){
			LastRow = LastCol = 0;
			if( ins_row != -1 ){
				typedef map<_Grid_Key,_Grid_Element>::iterator it;
				for( it pos = GridCells.begin(); pos != GridCells.end(); ++pos ){
					if(( pos->second ).Position.first >= ins_row )
						( pos->second ).Position.first += tree_levels.size();
				}
			}
			else{
				ins_row = m_TreeColumn.GetRowCount()-tree_levels.size();
			}
			PrintTree( path, ins_row, tree_levels );
			delete tree;
			return make_pair( ins_row+tree_levels.size()-1, AddNewName( name, type ));
		}
		delete tree;
		return make_pair( -1, -1 );
	} 
}

bool CGridSetup::DelBranch( const string &path, const pair<int,int> &row_range )
{
	typedef map<_Grid_Key,_Grid_Element>::iterator it;
	int test = 1;
	pair<it,it> branch = make_pair( GridCells.end(), GridCells.end());
	for( it pos = GridCells.begin(); pos != GridCells.end(); ){
		if(( pos->first ).Path.find( path ) == 0 ){
			GridCells.erase( pos );
			pos = GridCells.begin();
		}
		else{
			++pos;
		}
	}
	m_TreeColumn.DeleteTreeBranch( row_range.first, TRUE );
	int diff = row_range.second-row_range.first;
	for( it pos = GridCells.begin(); pos != GridCells.end(); ++pos ){
		if( pos->second.Position.first >= row_range.second ){
			pos->second.Position.first -= diff;
		}
	}
	if( GridCells.empty()){	// doesn't work properly
		for( int col = GetColumnCount()-1; col >= FIXED_COLUMN_COUNT; --col ) 
			DeleteColumn( col );
	}
	LastRow = LastCol = 0;
	return true; 
}

bool CGridSetup::FindPath( const string &path, vector<unsigned char> &tree_levels, int &ins_row )
{
	bool path_exist = true;
	unsigned char level = TOP_LEVEL;
	pair<int, int> row_range = make_pair( m_TreeColumn.GetFixedRowCount(),m_TreeColumn.GetRowCount());  

	typedef string::size_type size_type;
	size_type pos = 0, next_pos = 0;
	for( ; next_pos < string::npos; ++pos ){
		next_pos = path.find( Delim, pos );
		string sub_path = path.substr( pos, next_pos-pos );
		pos = next_pos;
		if( !FindBranch( sub_path, level, row_range )){
			path_exist = false;
			break;
		}
	}
	if( path_exist ){
		ins_row = row_range.first;
	}
	else{
		tree_levels.clear();
		tree_levels.push_back( level );
		for( pos = next_pos+1; next_pos < string::npos; ++pos ){
			next_pos = path.find( Delim, pos );
			pos = next_pos;
			tree_levels.push_back( ++level );
		}
		ins_row = row_range.second >= m_TreeColumn.GetRowCount()? -1:row_range.second;
	}
	return path_exist;
}

bool CGridSetup::FindBranch( const string &sub_path, unsigned char &level, pair<int,int> &row_range )
{
	int col = m_TreeColumn.GetColumnWithTree();
	for( int row = row_range.first; row < row_range.second; ++row ){
		CGridTreeCell *tree_cell = ( CGridTreeCell *)GetCell( row, col );
		if( tree_cell && level == tree_cell->GetLevel() && sub_path == tree_cell->GetText()){
			row_range.first = row;
			for( ++row; row < row_range.second; ++row ){
				tree_cell = ( CGridTreeCell *)GetCell( row, col );
				if( level == tree_cell->GetLevel())
					break;
			}
			row_range.second = row;
			++level;
			return true;
		}
	}
	return false;
}

void CGridSetup::PrintTree( const string &path, int ins_row, const vector<unsigned char> &tree_levels )
{
	int index = 0;
	unsigned char curr_level = TOP_LEVEL;
	typedef string::size_type size_type;
	for( size_type pos = 0, next_pos = 0; next_pos < string::npos; pos = next_pos+1, curr_level++ ){
		next_pos = path.find( Delim, pos );
		string sub_path = path.substr( pos, next_pos-pos );
		if( curr_level == tree_levels[index] ){
			index++;
			SetItemText( ins_row++, TREE_COLUMN, sub_path.c_str() );
		}
	}
}

void CGridSetup::UpdateCell( int row, int col, void *data )
{
	CString strMsg;
	if( row < GetFixedRowCount() || col < GetFixedColumnCount())
		return;
	CGridCellBase *pGridCell = GetCell( row, col );
	if( pGridCell ){
		pair<_Grid_Key,_Grid_Element> cell;
		GetElement( row, col, cell  );
		if( pGridCell->IsKindOf( RUNTIME_CLASS( CGridBtnCell )) && !( cell.second.Type & GRID_READONLY )){
			switch( cell.second.Type & GRID_ALLTYPES ){
				case GRID_BOOL:
				case GRID_COMBO:
				case GRID_STRING:
				break;
				case GRID_INT:
				{
					int idata = 0;
					int *p_idata = &idata;
					if( data == NULL ){
						strMsg = pGridCell->GetText();
						idata = atoi( LPCTSTR( strMsg ));
					}
					else{
						p_idata = ( int *)data;
					}
					if(( cell.second.Type & GRID_POSITIVE ) && *p_idata <= 0 ){
						if( cell.second.Type & GRID_EXCL_ZERO )
							*p_idata = 1;
						else
							*p_idata = 0;
					}
					if(( cell.second.Type & GRID_NEGATIVE ) && *p_idata >= 0 ){
						if( cell.second.Type & GRID_EXCL_ZERO )
							*p_idata = -1;
						else
							*p_idata = 0;
					}
					strMsg.Format( "%i", *p_idata );
					pGridCell->SetText( strMsg );
				}
				break;
				case GRID_DOUBLE:
				{
					double fdata = 0;
					double *p_fdata = &fdata;
					if( data == NULL ){
						strMsg = pGridCell->GetText();
						fdata = atof( LPCTSTR( strMsg ));
					}
					else{
						p_fdata = ( double *)data;
					}
					if(( cell.second.Type & GRID_POSITIVE ) && *p_fdata <= 0 ){
						if( cell.second.Type & GRID_EXCL_ZERO )
							*p_fdata = 0.1;
						else
							*p_fdata = 0;
					}
					if(( cell.second.Type & GRID_NEGATIVE ) && *p_fdata >= 0 ){
						if( cell.second.Type & GRID_EXCL_ZERO )
							*p_fdata = -0.1;
						else
							*p_fdata = 0;
					}
					strMsg.Format( "%g", *p_fdata );
					pGridCell->SetText( strMsg );
				}
				break;
				default:;
			} 
		}
		RedrawCell( row, col );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCheckListCtrl

CCheckListCtrl::CCheckListCtrl()
{
}

CCheckListCtrl::~CCheckListCtrl()
{
}


BEGIN_MESSAGE_MAP(CCheckListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CCheckListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCheckListCtrl public functions


//////////////////////////////////////////////////////////////////////////
// Function:    InitList
//

void CCheckListCtrl::InitList( vector<string> &list )
{
    DWORD dwRemoveStyles = LVS_OWNERDRAWFIXED | LVS_SHOWSELALWAYS;
    DWORD dwNewStyles    = LVS_REPORT | LVS_NOCOLUMNHEADER /*| LVS_SHAREIMAGELISTS */|
                           LVS_SINGLESEL/* | LVS_SORTASCENDING*/;

    DWORD dwRemoveExStyles = 0;
    DWORD dwNewExStyles = LVS_EX_CHECKBOXES;

    ASSERT ( 0 == ( GetStyle() & LVS_OWNERDATA ));
    ModifyStyle ( dwRemoveStyles, dwNewStyles );
    ListView_SetExtendedListViewStyleEx ( GetSafeHwnd(),
                                          dwRemoveExStyles | dwNewExStyles,
                                          dwNewExStyles );
    // Empty the control, and if there are no columns, insert a column.

    DeleteAllItems();
    LVCOLUMN rCol = { LVCF_WIDTH };
    if( !GetColumn( 0, &rCol ) )
        InsertColumn( 0, _T("") );
    
    BOOL exit_res = FALSE;
    for( unsigned int i = 0; i < list.size(); i++ ){
         InsertItem ( i, list[i].c_str() );
         SetItemData ( i, i );
         }
    SetColumnWidth( 0, LVSCW_AUTOSIZE_USEHEADER );
}


void CCheckListCtrl::Select( vector<int> &itemindex )
{
    for( int i = 0; i < GetItemCount(); i++ ){
		if( std::find( itemindex.begin(), itemindex.end(), i ) != itemindex.end() )
            SetCheck ( i, TRUE );
         }
}

void CCheckListCtrl::SelectAll(void)
{
    for( int i = 0; i < GetItemCount(); i++ ){
         SetCheck ( i, TRUE );
         }
}

void CCheckListCtrl::UnselectAll(void)
{
    for( int i = 0; i < GetItemCount(); i++ ){
         SetCheck ( i, FALSE );
         }
}

int CCheckListCtrl::GetNumSelectedItems( void )
{
	int i = 0, num_selected_items = 0;
    for( i = 0, num_selected_items = 0; i < GetItemCount(); i++){
         if( GetCheck( i ))
             ++num_selected_items;
         }
    return num_selected_items;

}

void CCheckListCtrl::GetSelectedItems( vector<int> &itemindex )
{
    itemindex.clear();
    for( int i = 0, num_selected_items = 0; i < GetItemCount(); i++){
         CString name = GetItemText( i, 0 );
         int n = GetItemData( i );
         if( GetCheck( i ))
             itemindex.push_back( GetItemData( i ));
         }
}

// CFormatDialog dialog
IMPLEMENT_DYNAMIC(CFormatDialog, CDialog)

CFormatDialog::CFormatDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFormatDialog::IDD, pParent)
    , NumSelItems(0)
    , FormatType(0)
    , Wnd1(-1)
    , Wnd2(-1)
{
}

CFormatDialog::~CFormatDialog()
{
}

void CFormatDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATA_LIST, DataList);
	DDX_Text(pDX, IDC_EDIT_NUMDATA, NumSelItems);
	DDX_CBIndex(pDX, IDC_COMBO_FORMAT, FormatType);
	DDX_Control(pDX, IDC_COMBO_FORMAT, ComboFormats);
	DDX_Text(pDX, IDC_EDIT_DATA_WND1, Wnd1);
	DDX_Text(pDX, IDC_EDIT_DATA_WND2, Wnd2);
}


BEGIN_MESSAGE_MAP(CFormatDialog, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_SELECTALL, OnBnClickedButtonSelectall)
    ON_BN_CLICKED(IDC_BUTTON_UNSELECTALL, OnBnClickedButtonUnselectall)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_DATA_LIST, OnLvnItemchangedDataList)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
    ON_CBN_SELCHANGE(IDC_COMBO_FORMAT, OnCbnSelchangeComboFormat)
END_MESSAGE_MAP()


// CFormatDialog message handlers
BOOL CFormatDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
    ComboFormats.ResetContent();
    for( int i = 0; i < _id_MAX_FORMAT; i++ )
         ComboFormats.AddString( _FormatNames[i] );
    DataList.InitList( AllNames );
    DataList.Select( SelectedItems );
    NumSelItems = DataList.GetNumSelectedItems();
    UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFormatDialog::OnBnClickedButtonSelectall()
{
    DataList.SelectAll();
    NumSelItems = DataList.GetNumSelectedItems();
    UpdateData( FALSE );
}

void CFormatDialog::OnBnClickedButtonUnselectall()
{
    DataList.UnselectAll();
    NumSelItems = DataList.GetNumSelectedItems();
    UpdateData( FALSE );
}

void CFormatDialog::OnLvnItemchangedDataList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    NumSelItems = DataList.GetNumSelectedItems();
    UpdateData( FALSE );
    *pResult = 0;
}

void CFormatDialog::OnBnClickedOk()
{
    DataList.GetSelectedItems( SelectedItems );
    UpdateData( TRUE );
    OnOK();
}

void CFormatDialog::OnCbnSelchangeComboFormat()
{
    UpdateData( TRUE );
}

#endif // __CONSOLE__
