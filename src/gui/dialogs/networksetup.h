/////////////////////////////////////////////////////////////////////////////
// networksetup.h : header file
#include "../../config.h"

#ifndef __NETWORK_SETUP_H
#define __NETWORK_SETUP_H

#ifndef __CONSOLE__

#include "afxcoll.h"
#include "walker.h"
#include "neuronsetup.h"
#include "hhnnetwork.h"
#include "syntemplate.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "biomtemplate.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// nn_unitSel dialog
class nn_unitSel : public CDialog{
	public: // Construction
		nn_unitSel(CWnd* pParent = NULL);   // standard constructor
	public:
		int DoModal(CStringArray &unitsList );
	protected:
		void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		BOOL OnInitDialog();
	public:
		enum{ IDD = IDD_NN_UNIT_SEL };
		int	NumUnit;
		CComboBox PopulatSel;
	private:
	    CStringArray UnitsList;
};

/////////////////////////////////////////////////////////////////////////////
// CWeightClone dialog
class CWeightClone : public CDialog{
		DECLARE_DYNAMIC( CWeightClone )
		DECLARE_MESSAGE_MAP()
	public:
		CWeightClone( CWnd* pParent = NULL );
virtual	~CWeightClone( void );

	public:
		enum { IDD = IDD_WEIGHT_DEP };
	protected:
virtual	void DoDataExchange( CDataExchange* pDX );
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
	protected:
		BOOL OnInitDialog();
		void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	protected: // Implementation
		afx_msg void OnWeightDep();
		afx_msg void OnUpdateEditExWeight();
		afx_msg void OnDeltaposSpinExWeight(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnUpdateEditExWeightVar();
		afx_msg void OnDeltaposSpinExWeightVar(NMHDR* pNMHDR, LRESULT* pResult);
#if defined( __RESPIRATION__ )
		afx_msg void OnUpdateEditExWeight2();
		afx_msg void OnDeltaposSpinExWeight2(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnUpdateEditExWeightVar2();
		afx_msg void OnDeltaposSpinExWeightVar2(NMHDR* pNMHDR, LRESULT* pResult);
#endif // defined( __RESPIRATION__ )
		afx_msg void OnUpdateEditInhAWeight();
		afx_msg void OnDeltaposSpinInhAWeight(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnUpdateEditInhAWeightVar();
		afx_msg void OnDeltaposSpinInhAWeightVar(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnUpdateEditInhBWeight();
		afx_msg void OnDeltaposSpinInhBWeight(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnUpdateEditInhBWeightVar();
		afx_msg void OnDeltaposSpinInhBWeightVar(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinProbability(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnUpdateEditProbability();
		afx_msg void OnSelchangeConnectType();
	private:
		void Change( void );
		void GetWeight( void );
		void SetWeight( void );
	public:
		CConnectMatrix *pCM;
		CString TrgName;
		CString SrcName;
#if defined( __RESPIRATION__ )
		enum { IDD = IDD_WEIGHTS_RESP };
#else 
		enum { IDD = IDD_WEIGHTS };
#endif // defined( __RESPIRATION__ )
		CComboBox	ConnectType;
		CEdit	EditExWeightVar;
		CSpinButtonCtrl	SpinExWeightVar;
		CSpinButtonCtrl	SpinExWeight;
		CEdit	EditExWeight;
		double	ExWeight;
		double	ExWeightVar;
		double	Probability;
#if defined( __RESPIRATION__ )
		CEdit	EditExWeightVar2;
		CSpinButtonCtrl	SpinExWeightVar2;
		CSpinButtonCtrl	SpinExWeight2;
		CEdit	EditExWeight2;
		double	ExWeight2;
		double	ExWeightVar2;
#endif // defined( __RESPIRATION__ )
		CEdit	EditInhAWeightVar;
		CSpinButtonCtrl	SpinInhAWeightVar;
		CSpinButtonCtrl	SpinInhAWeight;
		CEdit	EditInhAWeight;
		double	InhAWeight;
		double	InhAWeightVar;
		CEdit	EditInhBWeightVar;
		CSpinButtonCtrl	SpinInhBWeightVar;
		CSpinButtonCtrl	SpinInhBWeight;
		CEdit	EditInhBWeight;
		double	InhBWeight;
		double	InhBWeightVar;
		CEdit	EditProbability;
		CSpinButtonCtrl	SpinProbability;
		CNNConnect Connect;
		int ActiveSynapse;
	    int ExSpec;
#if defined( __RESPIRATION__ )
		int Ex2Spec;
#endif // defined( __RESPIRATION__ )
		int Inh1Spec;
		int Inh2Spec;
	private:
		CString Title;
		int OldIndex;
static	CString SelSrc;
static	CString SelTrg;
	public:
		BOOL IsChained;
};

/////////////////////////////////////////////////////////////////////////////
// CNetworkParam property page
class CNetworkParam : public CPropertyPage{
		DECLARE_DYNCREATE(CNetworkParam)
		DECLARE_MESSAGE_MAP()
	public: // Construction
		CNetworkParam(void);               // standard constructor
		~CNetworkParam(void){};
	public:
		void SetNetwork( CHhnNetwork *network ){ Network = network; };
virtual	BOOL OnSetActive();
virtual	BOOL OnKillActive();
	protected:
virtual	void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
virtual	BOOL OnInitDialog();
		afx_msg void OnGridClick( NMHDR *pNotifyStruct, LRESULT* pResult );
		afx_msg void OnEndCellEdit( NMHDR *pNotifyStruct, LRESULT* pResult );
	private:
		void OnFlyUpdate( const string &path );

		bool DisplayUnit( uni_template *unit, string &start_path, bool collapse = true );
		bool DisplayUnitPar( uni_template *unit, string &start_path );
		bool DisplayUnitChildren( uni_template *unit, string &start_path, bool collapse );
		void UpdateView( bool save );
	private:
		CGridSetup	NetParView;
	public:
		enum { IDD = IDD_NETWORK_PARAMETERS };
		CHhnNetwork *Network;
};

/////////////////////////////////////////////////////////////////////////////
// CPopulatSetup property page
class CPopulatSetup : public CPropertyPage{
		DECLARE_MESSAGE_MAP()
		DECLARE_DYNCREATE(CPopulatSetup)
	public:	// Construction
		CPopulatSetup();               // standard constructor
		~CPopulatSetup(); 
	public:
		BOOL OnSetActive();
	protected:
		void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		BOOL OnInitDialog();
	protected:
		afx_msg void OnAddPopulat();
		afx_msg void OnDelPopulat();
		afx_msg void OnPropPolulat();
		afx_msg void OnDblclkListPopulat();
	private:
		void Change( void );
	public:
		enum { IDD = IDD_POPULAT_SETUP };
		CStatic	StringSize;
		CButton	add_pop;
		CButton	DelPopulat;
		CButton	PropPopulat;
		CListBox ListPopulat;
		CHhnNetwork *Network;
	private:
		int NewPopulate;
};

/////////////////////////////////////////////////////////////////////////////
// CDriveName dialog
class CDriveName : public CDialog{
	public: // Construction
		CDriveName(CWnd* pParent = NULL);   // standard constructor
	protected:
		void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	public:
		enum { IDD = IDD_DRIVE_NAME };
		CString	DriveName;
};

/////////////////////////////////////////////////////////////////////////////
// CDriveSetup dialog
class CDriveSetup : public CPropertyPage{
		DECLARE_MESSAGE_MAP()
		DECLARE_DYNCREATE(CDriveSetup)
	public: // Construction
		CDriveSetup();
		~CDriveSetup(){};
	public:
		BOOL OnSetActive();
	protected:
		void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support
		BOOL OnInitDialog();
	protected: // Implementation
		afx_msg void OnAddDrive();
		afx_msg void OnDelDrive();
		afx_msg void OnPropDrive();
		afx_msg void OnDblclkListDrive();
	private:
	    void Change( void );
	public:
		enum { IDD = IDD_DRIVE_SETUP };
		CButton	PropDrive;
		CStatic	StringSize;
		CButton	add_drv;
		CButton	DelDrive;
		CListBox	ListDrive;
		CHhnNetwork *Network;
	private:
		int NewDrive;
};

/////////////////////////////////////////////////////////////////////////////
// COutputProp dialog
class COutputProp : public CDialog{
		DECLARE_MESSAGE_MAP()
	public: // Construction
		COutputProp(CWnd* pParent = NULL);   // standard constructor
	protected:
		void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		BOOL OnInitDialog();
	protected: // Implementation
		afx_msg void OnUpdateEditT1();
		afx_msg void OnUpdateEditT2();
		afx_msg void OnDeltaposSpinT1(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinT2(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnUpdateEditThreshold();
		afx_msg void OnDeltaposSpinThreshold(NMHDR* pNMHDR, LRESULT* pResult);
	public:
		enum { IDD = IDD_OUTPUT_PROP };
		CSpinButtonCtrl	Spin_Threshold;
		CEdit	Edit_Threshold;
		CSpinButtonCtrl	SpinT2;
		CSpinButtonCtrl	SpinT1;
		CEdit	EditT2;
		CEdit	EditT1;
		CString	Name;
		double	T1;
		double	T2;
		double	Threshold;
	    int Type;
};

/////////////////////////////////////////////////////////////////////////////
// COutputNetwork dialog
class COutputNetwork : public CPropertyPage{
		DECLARE_DYNCREATE(COutputNetwork)
		DECLARE_MESSAGE_MAP()
	public:	// Construction	
		COutputNetwork();
		~COutputNetwork(){};
	public: // Overrides
		BOOL OnSetActive();
	protected:
		void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		BOOL OnInitDialog();
	protected: // Implementation
		afx_msg void OnAddOutput();
		afx_msg void OnDelOutput();
		afx_msg void OnPropOutput();
		afx_msg void OnDblclkListOutput();
	private:
		void Change( void );
	public:	// Construction	
#if defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
		enum { IDD = IDD_OUTPUT_SETUP };
#else
		enum { IDD = IDD_OUTPUT_SETUP };
#endif //defined( __RESPIRATION__ ) && !defined( __LOCOMOTION__ )
		CButton	PropOutput;
		CStatic	StringSize;
		CListBox	ListOutput;
		CButton	DelOutput;
		CButton	add_out;
		CHhnNetwork *Network;
	private:
		int NewOutput;
};

#ifdef __MECHANICS__
class t_biomech;
/////////////////////////////////////////////////////////////////////////////
// CFBProp dialog
class CFBProp : public CDialog{
		DECLARE_MESSAGE_MAP()
	public: // Construction
		CFBProp( CWnd* pParent = NULL );   // standard constructor
	protected:
		void DoDataExchange( CDataExchange *pDX );    // DDX/DDV support
		BOOL OnInitDialog( void );
	protected:
		afx_msg void OnSelchangeComboType();
		afx_msg void OnSelchangeListFB();
		afx_msg void OnUpdateKV();
		afx_msg void OnUpdatePV();
		afx_msg void OnUpdateKL();
		afx_msg void OnUpdateKEMG();
		afx_msg void OnUpdateKF();
		afx_msg void OnUpdateKFTR();
		afx_msg void OnUpdateKC();
		afx_msg void OnUpdateKG();
		afx_msg void OnDeltaposSpinKV(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinPV(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinKL(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinKEMG(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinKF(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinKFTR(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinKC(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinKG(NMHDR* pNMHDR, LRESULT* pResult);
	private:
		void Change( void );
	public:
		enum { IDD = IDD_FEEDBACK_PROP };
		t_biomech *BiomT;
		int	TypeFB;
		CString	FBName;
		CString	MechName;
		CComboBox ComboType;
		CComboBox ListFB;
		CEdit EditKV;
		CEdit EditPV;
		CEdit EditKL;
		CEdit EditKEMG;
		CEdit EditKF;
		CEdit EditKFTR;
		CEdit EditKG;
		CEdit EditKC;
		CSpinButtonCtrl	SpinKV;
		CSpinButtonCtrl	SpinPV;
		CSpinButtonCtrl	SpinKL;
		CSpinButtonCtrl	SpinKEMG;
		CSpinButtonCtrl	SpinKF;
		CSpinButtonCtrl	SpinKFTR;
		CSpinButtonCtrl	SpinKC;
		CSpinButtonCtrl	SpinKG;

		double Kv;
		double Pv;
		double Kl;
		double Kemg;
		double Kf;
		double Ftr;
		double Kc;
		double Kg;
};

/////////////////////////////////////////////////////////////////////////////
// CFeedbackSetup dialog
class CFeedbackSetup : public CPropertyPage{
		DECLARE_DYNCREATE( CFeedbackSetup )
		DECLARE_MESSAGE_MAP()
	public: // Construction
		CFeedbackSetup();
		~CFeedbackSetup();
	public:
		BOOL OnSetActive( void );
	protected:
		void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	protected: // Implementation
		BOOL OnInitDialog();
		afx_msg void OnDelFeedback();
		afx_msg void OnAddFeedback();
		afx_msg void OnPropFeedback();
		afx_msg void OnDblclkListFeedback();
	private:
		void Change( void );
	public:
		enum { IDD = IDD_FEEDBACK_SETUP };
		CStatic	FeedbackSize;
		CButton	add_fbk;
		CButton	DelFeedback;
		CButton	PropFeedback;
		CListBox ListFeedback;
		CHhnNetwork *Network;
		t_biomech *BiomT;
	private:
		int NewFeedback;
};
#endif // __MECHANICS__

/////////////////////////////////////////////////////////////////////////////
// CNetworkSetup
class CNetworkSetup : public CPropertySheet{
		DECLARE_DYNAMIC( CNetworkSetup )
	public: // Construction
		CNetworkSetup( UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
		CNetworkSetup( LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
virtual	~CNetworkSetup( void );
	public:
	    CHhnNetwork Network;
#ifdef __MECHANICS__
		t_biomech BiomT;
#endif // __MECHANICS__
	private:
		CNetworkParam NetworkParam;
		CPopulatSetup PopulatSetup;
		CDriveSetup DriveSetup;
		COutputNetwork OutputSetup;
#ifdef __MECHANICS__
		CFeedbackSetup FeedbackSetup;
#endif // __MECHANICS__
};

/////////////////////////////////////////////////////////////////////////////
// CNetDiff dialog
class CNetDiff : public CDialog{
	public: // Construction
		CNetDiff(CWnd* pParent = NULL);   // standard constructor
	protected:
		void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	public:
		enum { IDD = IDD_NETWORK_DIFF };
		CString	Pattern;
};

#endif // __CONSOLE__
#endif // __NETWORK_SETUP_H
