#include "../../config.h"

#ifndef __CONSOLE__

#ifndef _GENERIC_DIALOGS_H_
#define _GENERIC_DIALOGS_H_

#include "sptypes.h"

#include <afxwin.h>
#include <afxcmn.h>
#include "GridCtrl.h"
#include "TreeColumn.h"
#include "BtnDataBase.h"

#include "../../resource.h"

#pragma once
class CGridSetup : public CGridCtrl{
      protected:
      public:
            CGridSetup( int IDD_IMAGE_RES = IDB_TREE_IMAGES, char delim = '\t' );
virtual    ~CGridSetup( void );
      public:
            bool InitGridCtrl( const string &name );

            bool GetElement( const string &path, const string &name, string &x ) const;
            bool GetActiveElement( string &name, string &data ) const;
            bool GetString( const string &path, vector<string> &elements ) const;
            bool GetBranch( string &branch_path, int row );
            bool GetActiveBranch( string &branch_path );
            map<_Grid_Key,_Grid_Element> *GetMap( void )
            {
             return &GridCells;
            };

            bool SetElement( const string &path, const string &name, const _Grid_Element &element );
            bool SetActiveElement( const string &path, int action = GRID_NONE );
            void SetupBntStyle( void );

            bool InsertElement( const string &path, const string &name, const string &data, int type = GRID_DOUBLE );
            bool DelElements( const string &path );

            void ExpandOneLevel( const string &path );
            void CollapseAllSubLevels( const string &path );
            void Update( bool auto_size = true );
            void UpdateData( bool upload = true );
            
            void OnGridClick( NMHDR *pNotifyStruct, LRESULT* pResult );
            void OnGridDblClk( NMHDR *pNotifyStruct, LRESULT* pResult );
            void OnEndCellEdit( NMHDR *pNotifyStruct, LRESULT* pResult );
      private:
            int  AddNewName( const string &name, int type );
            bool GetElement( int row, int col, pair<_Grid_Key,_Grid_Element> &x ) const;
            pair<int,int> InsertBranch( const string &path, const string &name, int type );
            bool DelBranch( const string &path, const pair<int,int> &row_range );
            bool FindPath( const string &path, vector<unsigned char> &tree, int &ins_row );
            bool FindBranch( const string &sub_path, unsigned char &level, pair<int,int> &row_range );
            void PrintTree( const string &path, int ins_row, const vector<unsigned char> &tree_levels );
            int  GetNamePos( const string &name );
            void UpdateCell( int row, int col, void *data = NULL );
      private:
            char Delim;
            string Name;
            map<_Grid_Key,_Grid_Element> GridCells;
            int LastRow;
            int LastCol;
      private:
            CImageList m_ImageList;
            CTreeColumn m_TreeColumn;   // grid with tree column
            CBtnDataBase EnableBtnDataBase; // grid with some cells with buttons/controls
            CBtnDataBase DisableBtnDataBase; // grid with some cells with buttons/controls
};

class CCheckListCtrl : public CListCtrl{
	DECLARE_MESSAGE_MAP()

        public:
	        CCheckListCtrl();
virtual         ~CCheckListCtrl();

        public:
            void InitList( vector<string> &list );

            void Select( vector<int> &itemindex );
            void SelectAll( void );
            void UnselectAll( void );
            int GetNumSelectedItems( void );
            void GetSelectedItems( vector<int> &itemindex );
};

// CFormatDialog dialog

class CFormatDialog : public CDialog{
        DECLARE_DYNAMIC(CFormatDialog)
        public:
                CFormatDialog(CWnd* pParent = NULL);   // standard constructor
virtual          ~CFormatDialog();

        // Dialog Data
        enum { IDD = IDD_SAVE_RESULTS };
        protected:
                virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
                virtual BOOL OnInitDialog();

                DECLARE_MESSAGE_MAP()
        public:
                CCheckListCtrl DataList;
                CComboBox ComboFormats;
                int NumSelItems;
                int FormatType;
                vector<string> AllNames;
                vector<int> SelectedItems;
                float Wnd1;
                float Wnd2;
                int Prec_A;
                double Prec_T;
   
        public:
                afx_msg void OnBnClickedButtonSelectall();
                afx_msg void OnBnClickedButtonUnselectall();
                afx_msg void OnLvnItemchangedDataList(NMHDR *pNMHDR, LRESULT *pResult);
                afx_msg void OnBnClickedOk();
                afx_msg void OnCbnSelchangeComboFormat();
};

#endif // _GENERIC_DIALOGS_H_

#endif // __CONSOLE__
