/////////////////////////////////////////////////////////////////////////////
// MainFrm.h : interface of the CMainFrame class
#include "../config.h"

#ifndef __CONSOLE__

#ifndef __MAINFRAME_H
#define __MAINFRAME_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sptypes.h"
#include "usermessage.h"
#include "./gui/mditabs/mditabs.h"

class CFrameView;
class CMainFrame : public CMDIFrameWnd{
#ifdef _DEBUG
	public:
virtual	void AssertValid() const;
virtual	void Dump(CDumpContext& dc) const;
#endif // _DEBUG
		DECLARE_DYNAMIC(CMainFrame)
		DECLARE_MESSAGE_MAP()
	public:
		CMainFrame();
virtual	~CMainFrame();
	public:
		CDocument *GetActiveDoc();
		void ViewProperies( void );
		void StatusBarMessage( const char *info );
	public: // Overrides
		BOOL DestroyWindow();
		void OnUpdateFrameTitle(BOOL bAddToTitle = 1); 
	protected:
		void createFrameView( CFrameView *view );
		afx_msg	int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg	void OnClose();
		afx_msg	void OnNetworkSetup();
		afx_msg	void OnNetworkInit();
		afx_msg	void OnNetworkDiff();
		afx_msg	void OnModelParam();
		afx_msg	void OnModelStart();
		afx_msg	void OnModelStop();
		afx_msg	void OnModelPause();
		afx_msg	void OnNetworkView();
		afx_msg void OnNetworkControl();
		afx_msg	void OnWindowNew();
		afx_msg	void OnRedrawResults();
		afx_msg	void OnSaveResults();
		afx_msg	void OnConnectionSetup();
		afx_msg	void OnSaveView();
		afx_msg	void OnUpdateModelStop(CCmdUI* pCmdUI);
		afx_msg	void OnUpdateModelPause(CCmdUI* pCmdUI);
		afx_msg	void OnUpdateNetworkView(CCmdUI* pCmdUI);
		afx_msg	void OnTimer(UINT nIDEvent);
		afx_msg	void OnUpdateMenuItem(CCmdUI* pCmdUI);
		afx_msg	LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
		afx_msg	LRESULT OnUpdateResults( WPARAM hint, LPARAM counter );
		afx_msg	LRESULT OnStopTimer( WPARAM hint, LPARAM counter );
#if defined (__MECHANICS_2D__)
	public:
		bool OpenLimbWindow( void );
		bool CloseLimbWindow( void );
	private:
		bool IsLimbWnd( void );
		afx_msg	void OnNewlimbwindow();
		afx_msg	void OnUpdateNewlimbwindow(CCmdUI* pCmdUI);
		afx_msg	void OnBiomechanics();
		afx_msg	void OnIsStick();
		afx_msg	void OnUpdateIsStick(CCmdUI* pCmdUI);
#elif defined (__MECHANICS_3D__)
// TODO implementation 3d model
#endif // __MECHANICS_2D__
	private:
		unsigned long CurrStep;
		unsigned int TimerHandle;

		CMDITabs   m_mdiTabs;
		CToolBar defaultToolBar;
		CStatusBar m_wndStatusBar;
		CString m_sStatusBarString;
};

#endif // __MAINFRAME_H
#endif // __CONSOLE__
