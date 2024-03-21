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
// CWeightClone dialog
class CWeightClone : public CDialog{
		DECLARE_DYNAMIC( CWeightClone )
		DECLARE_MESSAGE_MAP()
	public:
		CWeightClone( CWnd* pParent = NULL );
virtual		~CWeightClone( void );

	public:
		enum { IDD = IDD_WEIGHT_DEP };
	protected:
virtual		void DoDataExchange( CDataExchange* pDX );
		BOOL OnInitDialog( void );
		afx_msg void OnClone( void );
	private:
		void ShowContent( void );
	public:
		vector<string> TrgNames;
		vector<string> SrcNames;
	public:
		BOOL IsClone;
		CString SelSrc;
		CString SelTrg;
	private:
		CComboBox ListAllSrc;
		CComboBox ListAllTrg;
};

/////////////////////////////////////////////////////////////////////////////
// CWeightSetup dialog
class CWeightSetup : public CDialog{
		DECLARE_MESSAGE_MAP()
	public:
		CWeightSetup(CWnd* pParent = NULL);   // standard constructor
	public: // Overrides
		int DoModal( const char *title );
		BOOL DestroyWindow();
	private:
		using CDialog::DoModal;
	protected:
		BOOL OnInitDialog();
		void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	protected: // Implementation
		afx_msg void OnWeightDep();

		afx_msg void OnUpdateEditExWeight();
		afx_msg void OnDeltaposSpinExWeight(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnUpdateEditExWeightVar();
		afx_msg void OnDeltaposSpinExWeightVar(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnUpdateEditExWeight2();
		afx_msg void OnDeltaposSpinExWeight2(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnUpdateEditExWeightVar2();
		afx_msg void OnDeltaposSpinExWeightVar2(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnUpdateEditInhAWeight();
		afx_msg void OnDeltaposSpinInhAWeight(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnUpdateEditInhAWeightVar();
		afx_msg void OnDeltaposSpinInhAWeightVar(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnUpdateEditInhBWeight();
		afx_msg void OnDeltaposSpinInhBWeight(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnUpdateEditInhBWeightVar();
		afx_msg void OnDeltaposSpinInhBWeightVar(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnUpdateEditSyn1Weight();
		afx_msg void OnDeltaposSpinSyn1Weight(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnUpdateEditSyn1WeightVar();
		afx_msg void OnDeltaposSpinSyn1WeightVar(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnUpdateEditSyn2Weight();
		afx_msg void OnDeltaposSpinSyn2Weight(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnUpdateEditSyn2WeightVar();
		afx_msg void OnDeltaposSpinSyn2WeightVar(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnUpdateEditSyn3Weight();
		afx_msg void OnDeltaposSpinSyn3Weight(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnUpdateEditSyn3WeightVar();
		afx_msg void OnDeltaposSpinSyn3WeightVar(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnUpdateEditSyn4Weight();
		afx_msg void OnDeltaposSpinSyn4Weight(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnUpdateEditSyn4WeightVar();
		afx_msg void OnDeltaposSpinSyn4WeightVar(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnDeltaposSpinProbability(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnUpdateEditProbability();
		afx_msg void OnSelchangeConnectType();
		afx_msg void OnBnClickedExtend();
	private:
		void Change( void );
		void GetWeight( void );
		void SetWeight( void );
	public:
		CConnectMatrix *pCM;
		CString TrgName;
		CString SrcName;
		enum { IDD = IDD_WEIGHTS };
		CComboBox ConnectType;
		CComboBox PresynMod;
		CNNConnect Connect;
		int SynSpec[_id_MAX_SYN];
		double Weights[_id_MAX_SYN];
		double WeightVars[_id_MAX_SYN];
		double Probability;

		CButton ExtendWeight;
		CEdit EditWeights[_id_MAX_SYN];
		CEdit EditWeightVars[_id_MAX_SYN];
		CSpinButtonCtrl SpinWeights[_id_MAX_SYN];
		CSpinButtonCtrl SpinWeightVars[_id_MAX_SYN];

		CEdit EditProbability;
		CSpinButtonCtrl SpinProbability;
		BOOL IsChained;
	private:
		CString Title;
		int OldIndex;
static		CString SelSrc;
static		CString SelTrg;
};

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
virtual		BOOL OnSetActive();
	protected:
virtual		void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
virtual		BOOL OnInitDialog();
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
	public: // Construction
		CConnectionTargetSetup();
		~CConnectionTargetSetup();
		enum { IDD = IDD_CONNECT_TARGET_SETUP };
		CListBox	List_UsedTargets;
		CButton	Button_Weights;
		CListCtrl	List_Sources;
		CListCtrl	List_AllSources;
		CEdit	Edit_Target;
		CComboBox	Combo_Targets;
		CButton	Button_Delete;
		CButton	Button_Add;
	private:
		void ShowListUsedTargets( void );
		void ShowListSources( void );
	public: // Overrides
virtual		BOOL OnSetActive();
	protected:
virtual		void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	public: // Implementation
		CConnectMatrix *pCM;
		CHhnNetwork *pNetwork;
	protected:
virtual		BOOL OnInitDialog();
		afx_msg void OnAdd();
		afx_msg void OnSelchangeComboTargets();
		afx_msg void OnDelete();
		afx_msg void OnWeights();
		afx_msg void OnDblclkListSources(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDblclkListAllSources(NMHDR* pNMHDR, LRESULT* pResult);
		DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CConnectionSetup
class CConnectionSetup : public CPropertySheet
{
	DECLARE_DYNAMIC(CConnectionSetup)
	public: // Construction
		CConnectionSetup(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
		CConnectionSetup(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	public: // Overrides
virtual		int DoModal();
	public: // Implementation
		CConnectMatrix CM;
		CHhnNetwork *pNetwork;
	public:
		virtual ~CConnectionSetup();
	private:
		CConnectionSourceSetup ConnectionSourceSetup;
		CConnectionTargetSetup ConnectionTargetSetup;
};

/////////////////////////////////////////////////////////////////////////////

#endif // __CONSOLE__

#endif // __CONNECTIONSETUP_H
