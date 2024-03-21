/////////////////////////////////////////////////////////////////////////////
// neuronsetup.h : header file
#include "../../config.h"

#ifndef __CONSOLE__

#ifndef __NEURONSETUP_H
#define __NEURONSETUP_H

#include "afxcoll.h"
#include "../core/nsm/hhnnetwork.h"
#include "genericdialogs.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/////////////////////////////////////////////////////////////////////////////
// CPopulatProperties dialog
class CPopulatProperties : public CDialog{
		DECLARE_MESSAGE_MAP()
	public: // Construction
		CPopulatProperties( CWnd* pParent =NULL );   // standard constructor
	public: // Dialog Data
		enum { IDD = IDD_POPULAT_PROPERTIES };
		t_populat PopulateTemplate;
	protected: // Overrides
virtual		void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	public:	// Implementation
		afx_msg void OnBnClickedOk();
		BOOL OnInitDialog();
	protected:	// Implementation
		afx_msg void OnGridClick( NMHDR *pNotifyStruct, LRESULT* pResult );
		afx_msg void OnGridDblClick( NMHDR *pNotifyStruct, LRESULT* pResult );
		afx_msg void OnEndCellEdit( NMHDR *pNotifyStruct, LRESULT* pResult );
		afx_msg LRESULT OnDelUnits( WPARAM /*x1*/, LPARAM /*x2*/ );
	private:
		void UpdateView( bool save );
		void OnFlyUpdate( const string &path );
		void add_unit( const CPoint &point );
		void del_unit( void );
		bool DisplayUnit( uni_template *unit, const string &start_path, bool collapse = true );
		bool DisplayUnitPar( uni_template *unit, const string &start_path );
		bool DisplayUnitChildren( uni_template *unit, const string &start_path, bool collapse );
	private:
		vector<string> DelPaths;
		string ParentPath;
	private:
		CGridSetup	NeuronView;
};

#endif // __NEURONSETUP_H
#endif // __CONSOLE__
