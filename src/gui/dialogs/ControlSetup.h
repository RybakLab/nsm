/////////////////////////////////////////////////////////////////////////////
// ControlSetup.h : header file
#ifndef __CONTROLSETUP_H
#define __CONTROLSETUP_H

#include "config.h"

#ifndef __CONSOLE__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "resource.h"
#include "hhnnetwork.h"

/////////////////////////////////////////////////////////////////////////////
// CControlledSetup dialog
class CControlledSetup : public CPropertyPage{
		DECLARE_DYNCREATE( CControlledSetup )
		DECLARE_MESSAGE_MAP()
	public: // Construction
		CControlledSetup();
		~CControlledSetup();
	protected:
virtual	void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
virtual	BOOL OnInitDialog();
		afx_msg void OnAddParam();
		afx_msg void OnRemoveParam();
	private:
		void ShowListBox( void );
	public:
		enum { IDD = IDD_CONTROLLED_SETUP };
		CListCtrl	List_Controlled;
		CTreeCtrl	Tree_Controlled;
		CButton	Button_RemoveParam;
		CButton	Button_AddParam;
		hhn_control *pControl;
		CHhnNetwork *pNetwork;
};

/////////////////////////////////////////////////////////////////////////////
// CControlFunction dialog
class CControlFunction : public CPropertyPage{
		DECLARE_DYNCREATE(CControlFunction)
		DECLARE_MESSAGE_MAP()
	public:	// Construction
		CControlFunction();
		~CControlFunction();
	public:	// Dialog Data
		enum { IDD = IDD_CONTROL_FUNCTION };
		CEdit	EditNameCtrl;
		CButton	SaveButton;
		CButton	PropPoint;
		CListBox	ListPoints;
		CButton	DelPoint;
		CButton	AddPoint;
		CString	NameCtrl;
		double NoiseLevel;
		BOOL IsNoiseAdded;
	public:
		hhn_control *pControl;
	public:	// Overrides
virtual	BOOL OnSetActive();
virtual	BOOL OnKillActive();
	protected:
virtual	void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	private:
		void Change( void );
	protected:	// Implementation
virtual	BOOL OnInitDialog();
		afx_msg void OnAddPoint();
		afx_msg void OnDelPoint();
		afx_msg void OnDblclkListPoints();
		afx_msg void OnLoad();
		afx_msg void OnPropPoint();
		afx_msg void OnSave();
public:
	int IsStep;
};

/////////////////////////////////////////////////////////////////////////////
// CPropPoint dialog
class CPropPoint : public CDialog{
		DECLARE_MESSAGE_MAP()
	public:	// Construction
		CPropPoint(CWnd* pParent = NULL);   // standard constructor
	public:	// Dialog Data
		enum { IDD = IDD_PROP_POINT };
		CSpinButtonCtrl	SpinMul;
		CSpinButtonCtrl	SpinTime;
		CEdit	EditMul;
		CEdit	EditTime;
		double	Time;
		double	Mul;
	protected:	// Overrides
virtual	void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	protected:	// Implementation
virtual	BOOL OnInitDialog();
		afx_msg void OnUpdateEditMul();
		afx_msg void OnDeltaposSpinMul(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnUpdateEditTime();
		afx_msg void OnDeltaposSpinTime(NMHDR* pNMHDR, LRESULT* pResult);
};

/////////////////////////////////////////////////////////////////////////////
// CControlProperties
class CControlProperties : public CPropertySheet{
		DECLARE_DYNAMIC(CControlProperties)
	public:	// Construction
		CControlProperties(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
		CControlProperties(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
virtual	~CControlProperties();
	public:	// Attributes
		hhn_control *Control;
	public:	// Overrides
virtual	int DoModal(CHhnNetwork * pnetwork, const hhn_control &control );
	private:	// Generated message map functions
		CControlledSetup ControlledSetup;
		CControlFunction ControlFunction;
};

/////////////////////////////////////////////////////////////////////////////
// CNetworkControl dialog
class CNetworkControl : public CDialog{
		DECLARE_MESSAGE_MAP()
	public:	// Construction
		CNetworkControl(CWnd* pParent = NULL);   // standard constructor
	public:	// Dialog Data
		enum { IDD = IDD_CONTROL_SETUP };
		CStatic	ControlSize;
		CButton	add_ctr;
		CButton	DelControl;
		CButton	PropControl;
		CListBox ListControl;
		CHhnNetwork Network;
	protected:	// Overrides
virtual	void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	protected:	// Implementation
virtual	BOOL OnInitDialog();
		afx_msg void OnAddControl();
		afx_msg void OnDelControl();
		afx_msg void OnPropControl();
		afx_msg void OnDblclkListControl();
	private:
		void Change( void );
	private:
		int NewControl;
};

#endif // __CONSOLE__
#endif // __CONTROLSETUP_H
