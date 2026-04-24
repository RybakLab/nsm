/////////////////////////////////////////////////////////////////////////////
// biomechsetup.h : header file
#include "config.h"

#ifndef __CONSOLE__
#include "../Spcord.h"

#include "genericdialogs.h"
#include "../core/nsm/hhnnetwork.h"
#include "biomtemplate.h"

#ifndef __BIOMECH_SETUP_H
#define __BIOMECH_SETUP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (__MECHANICS__)

class CBiomechSetup : public CDialog{
		DECLARE_MESSAGE_MAP()
	public: // Construction
		CBiomechSetup( CWnd* pParent = NULL );   // standard constructor
	public: // Dialog Data
enum		{ IDD = IDD_BIOMECH_PARAMETERS };
		t_biomech BiomechT;
	public:	// Implementation
		BOOL OnInitDialog();
		void OnOK( void );
	protected:
		void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support
		afx_msg void OnGridClick( NMHDR *pNotifyStruct, LRESULT* pResult );
		afx_msg void OnEndCellEdit( NMHDR *pNotifyStruct, LRESULT* pResult );
	private:
		void OnFlyUpdate( const string &path );
		bool DisplayUnit( uni_template *unit, const string &start_path, bool collapse = true );
		bool DisplayUnitPar( uni_template *unit, const string &start_path );
		bool DisplayUnitChildren( uni_template *unit, const string &start_path, bool collapse );
		void UpdateView( bool save );
	private:
		CGridSetup BiomechView;
};

#endif // __MECHANICS__
#endif // __BIOMECH_SETUP_H
#endif // __CONSOLE__
