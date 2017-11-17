/////////////////////////////////////////////////////////////////////////////
// ConnectionSetup.h : header file
#ifndef __CONNECTIONSETUP_H
#define __CONNECTIONSETUP_H

#include "config.h"

#ifndef __CONSOLE__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "hhnunit.h"
#include "hhnconnect.h"
#include "resource.h"

class CHhnNetwork;

/////////////////////////////////////////////////////////////////////////////
// CConnectionSourceSetup dialog
class CConnectionSourceSetup : public CPropertyPage{
		enum{
			IDD = IDD_CONNECT_SOURCE_SETUP,
		};
		DECLARE_DYNCREATE( CConnectionSourceSetup )
	public: // Construction
		CConnectionSourceSetup();   // standard constructor
		~CConnectionSourceSetup();
	public:
virtual	BOOL OnSetActive();
	protected:
virtual	void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
virtual	BOOL OnInitDialog();
		afx_msg void OnAdd();
		afx_msg void OnDelete();
		afx_msg void OnWeights();
		afx_msg void OnSelchangeComboSources();
		afx_msg void OnDblclkListTargets(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDblclkListAllTargets(NMHDR* pNMHDR, LRESULT* pResult);
		DECLARE_MESSAGE_MAP()
	private:
		void ShowListUsedSources( void );
		void ShowListTargets( void );
	public:
		CConnectMatrix *pCM;
		CHhnNetwork *pNetwork;
		CButton Button_Add;
		CButton Button_Delete;
		CButton Button_Weights;
		CComboBox Combo_Source;
		CEdit Edit_Source;
		CListBox List_UsedSources;
		CListCtrl List_Targets;
		CListCtrl List_AllTargets;
};


/////////////////////////////////////////////////////////////////////////////
// CConnectionTargetSetup dialog
class CConnectionTargetSetup : public CPropertyPage{
	DECLARE_DYNCREATE(CConnectionTargetSetup)

// Construction
public:
	CConnectionTargetSetup();
	~CConnectionTargetSetup();

// Dialog Data
	//{{AFX_DATA(CConnectionTargetSetup)
	enum { IDD = IDD_CONNECT_TARGET_SETUP };
	CListBox	List_UsedTargets;
	CButton	Button_Weights;
	CListCtrl	List_Sources;
	CListCtrl	List_AllSources;
	CEdit	Edit_Target;
	CComboBox	Combo_Targets;
	CButton	Button_Delete;
	CButton	Button_Add;
	//}}AFX_DATA
private:
	void ShowListUsedTargets( void );
	void ShowListSources( void );


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CConnectionTargetSetup)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CConnectMatrix *pCM;
	CHhnNetwork *pNetwork;
protected:
	// Generated message map functions
	//{{AFX_MSG(CConnectionTargetSetup)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnSelchangeComboTargets();
	afx_msg void OnDelete();
	afx_msg void OnWeights();
	afx_msg void OnDblclkListSources(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListAllSources(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CConnectionSetup
class CConnectionSetup : public CPropertySheet
{
	DECLARE_DYNAMIC(CConnectionSetup)

// Construction
public:
	CConnectionSetup(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CConnectionSetup(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectionSetup)
	public:
	virtual int DoModal();
	//}}AFX_VIRTUAL

// Implementation
public:
	CConnectMatrix CM;
	CHhnNetwork *pNetwork;
public:
	virtual ~CConnectionSetup();

	// Generated message map functions
protected:
	//{{AFX_MSG(CConnectionSetup)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CConnectionSourceSetup ConnectionSourceSetup;
	CConnectionTargetSetup ConnectionTargetSetup;

};

/////////////////////////////////////////////////////////////////////////////

#endif // __CONSOLE__

#endif // __CONNECTIONSETUP_H
