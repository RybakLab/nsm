/////////////////////////////////////////////////////////////////////////////
// SimulateSetup.h : header file
#include "config.h"

#ifndef __SIMULATION_SETUP_H
#define __SIMULATION_SETUP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CONSOLE__

#include "simulate.h"
#include "afxcmn.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CSimParamSetup dialog
class CSimParamSetup : public CPropertyPage{
	DECLARE_DYNCREATE(CSimParamSetup)
	DECLARE_MESSAGE_MAP()
		enum { IDD = IDD_NSM_SIMULATE_PARAM };
	public:
		CSimParamSetup();
		~CSimParamSetup();
	public:
virtual		BOOL OnSetActive();
virtual		BOOL OnKillActive();
	protected:
virtual		void DoDataExchange(CDataExchange* pDX);
virtual		BOOL OnInitDialog();
	protected:
		afx_msg	void OnUpdateEditTimeFactor();
		afx_msg	void OnUpdateEditSimNnStep();
		afx_msg	void OnUpdateEditSimTime();
		afx_msg	void OnKillfocusEditSimNnStep();
		afx_msg	void OnKillfocusEditSimTime();
		afx_msg	void OnKillfocusEditTimeFactor();
		afx_msg	void OnDeltaposSpinSimTime(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg	void OnDeltaposSpinSimNnStep(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg	void OnDeltaposSpinTimeFactor(NMHDR* pNMHDR, LRESULT* pResult);
	public:
		CSimulateData *pData;
		double	SimTime;
		double	SimNNStep;
		double	TimeFactor;
	private:
		CEdit	Edit_TimeFactor;
		CEdit	Edit_SimTime;
		CEdit	Edit_SimNNStep;
		CSpinButtonCtrl	Spin_SimTime;
		CSpinButtonCtrl	Spin_TimeFactor;
		CSpinButtonCtrl	Spin_SimNNStep;
		CString NumThreads;
		CComboBox NumThreadsCombo;
};

/////////////////////////////////////////////////////////////////////////////
// CSimViewSetup dialog
class CSimViewSetup : public CPropertyPage{
	DECLARE_DYNCREATE(CSimViewSetup)
	DECLARE_MESSAGE_MAP()
	public: // Construction
		enum	{ IDD = IDD_SIMULATE_VIEW };
		CSimViewSetup();
		~CSimViewSetup();
	public:
virtual		BOOL OnSetActive();
virtual		BOOL OnKillActive();
	protected:
virtual		void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	public:
		CSpinButtonCtrl	Spin_HistNorm;
		CEdit	Edit_HistNorm;
		CSpinButtonCtrl	Spin_HistBin;
		CEdit	Edit_HistBin;
		CButton	Check_All_View;
		CSpinButtonCtrl	Spin_UpdateTime;
		CSpinButtonCtrl	Spin_EndView;
		CEdit	Edit_EndView;
		CSpinButtonCtrl	Spin_BeginView;
		CEdit	Edit_BeginView;
		double	BeginView;
		double	EndView;
		long	UpdateTime;
		BOOL	All_View;
		double	HistBin;
		double	HistNorm;
		CSimulateData *pData;
	protected: // Implementation
virtual		BOOL OnInitDialog();
		afx_msg void OnUpdateEditBeginView();
		afx_msg void OnUpdateEditEndView();
		afx_msg void OnDeltaposSpinBeginView(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinEndView(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnKillfocusEditBeginView();
		afx_msg void OnKillfocusEditEndView();
		afx_msg void OnKillfocusEditHistBin();
		afx_msg void OnUpdateEditHistBin();
		afx_msg void OnKillfocusEditHistNorm();
		afx_msg void OnUpdateEditHistNorm();
		afx_msg void OnDeltaposSpinHistBin(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinHistNorm(NMHDR* pNMHDR, LRESULT* pResult);
};

#if defined (__MECHANICS_2D__)
/////////////////////////////////////////////////////////////////////////////
// CSimLimbSetup dialog
class CSimLimbSetup : public CPropertyPage{
	DECLARE_DYNCREATE(CSimLimbSetup)
	DECLARE_MESSAGE_MAP()
	public: // Construction
		enum	{ IDD = IDD_SIMULATE_LIMB };
		CSimLimbSetup();
		~CSimLimbSetup();
	public:
virtual		BOOL OnSetActive();
virtual		BOOL OnKillActive();
	protected:
virtual		void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	public:
		CSpinButtonCtrl	Spin_LimbScale;
		CSpinButtonCtrl	Spin_LimbSkip;
		CSpinButtonCtrl Spin_LimbOriginX;
		CSpinButtonCtrl Spin_LimbOriginY;
		CEdit Edit_LimbScale;
		CEdit Edit_LimbOriginX;
		CEdit Edit_LimbOriginY;
		long	LimbSkip;
		long	LimbScale;
		double LimbOriginX;
		double LimbOriginY;
		CSimulateData *pData;
	protected:
virtual		BOOL OnInitDialog();
		afx_msg void OnUpdateEditLimbOriginX();
		afx_msg void OnDeltaposSpinLimbOriginX(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnUpdateEditLimbOriginY();
		afx_msg void OnDeltaposSpinLimbOriginY(NMHDR *pNMHDR, LRESULT *pResult);
};
#elif defined (__MECHANICS_3D__)
// TODO implementation 3d model
#endif // __MECHANICS_2D__

/////////////////////////////////////////////////////////////////////////////
// CSimulateSetup
class CSimulateSetup : public CPropertySheet{
	DECLARE_DYNAMIC(CSimulateSetup)
	public: // Construction
		CSimulateSetup( UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
		CSimulateSetup( LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
virtual		~CSimulateSetup( void );
	public:
		bool IsCheckAllView( void){return SimViewSetup.All_View != FALSE;};
	public:
		CSimulateData Data;
	private:
		CSimParamSetup SimParamSetup;
		CSimViewSetup SimViewSetup;
#if defined (__MECHANICS_2D__)
		CSimLimbSetup SimLimbSetup;
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
};

#endif // __CONSOLE__
#endif // __SIMULATION_SETUP_H
