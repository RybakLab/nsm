/////////////////////////////////////////////////////////////////////////////
// biomechsetup.h : header file
#include "config.h"

#ifndef __CONSOLE__
#include "../Spcord.h"

#include "genericdialogs.h"
#include "biomtemplate.h"

#ifndef __BIOMECH_SETUP_H
#define __BIOMECH_SETUP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __MECHANICS__

class CBiomechSetup : public CDialog{
	DECLARE_MESSAGE_MAP()
	public: // Construction
enum		{ IDD = IDD_BIOMECH_PARAMETERS };
		CBiomechSetup( CWnd* pParent = NULL );   // standard constructor
	protected:
		void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support
		BOOL OnInitDialog();
		void OnOK( void );
		afx_msg void OnGridClick( NMHDR *pNotifyStruct, LRESULT* pResult );
		afx_msg void OnEndCellEdit( NMHDR *pNotifyStruct, LRESULT* pResult );
	private:
		void OnFlyUpdate( const string &path );
		bool DisplayUnit( uni_template *unit, string &start_path, bool collapse = true );
		bool DisplayUnitPar( uni_template *unit, string &start_path );
		bool DisplayUnitChildren( uni_template *unit, string &start_path, bool collapse );
		void UpdateView( bool save );
	private:
		CGridSetup	BiomechView;
	public: // test purpose only
		t_biomech BiomechT;
};

#endif // __MECHANICS__
#endif // __BIOMECH_SETUP_H
#endif // __CONSOLE__
