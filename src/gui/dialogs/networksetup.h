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
	private:
		using CDialog::DoModal;
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
// CNetworkParam property page
class CNetworkParam : public CPropertyPage{
		DECLARE_DYNCREATE(CNetworkParam)
		DECLARE_MESSAGE_MAP()
	public: // Construction
		CNetworkParam(void);               // standard constructor
		~CNetworkParam(void){};
	public:
		void SetNetwork( CHhnNetwork *network ){ Network = network; };
	public:	// Implementation
		BOOL OnSetActive();
		BOOL OnKillActive();
		BOOL OnInitDialog();
	protected:
virtual		void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		afx_msg void OnGridClick( NMHDR *pNotifyStruct, LRESULT* pResult );
		afx_msg void OnEndCellEdit( NMHDR *pNotifyStruct, LRESULT* pResult );
	private:
		void OnFlyUpdate( const string &path );
		bool DisplayUnit( uni_template *unit, const string &start_path, bool collapse = true );
		bool DisplayUnitPar( uni_template *unit, const string &start_path );
		bool DisplayUnitChildren( uni_template *unit, const string &start_path, bool collapse );
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
		CListBox ListDrive;
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
		afx_msg void OnUpdateEditThreshold();
		afx_msg void OnUpdateBias();
		afx_msg void OnUpdateSlpA();
		afx_msg void OnUpdatePowA();
		afx_msg void OnUpdateSlpT();
		afx_msg void OnUpdatePowT();
		afx_msg void OnUpdateCap();

		afx_msg void OnDeltaposSpinT1(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinT2(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinThreshold(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinBias(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinSlpA(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinPowA(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinSlpT(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinPowT(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinCap(NMHDR* pNMHDR, LRESULT* pResult);
	public:
		enum { IDD = IDD_OUTPUT_PROP };
		int Type;
		CString	Name;
		CEdit EditT2;
		CEdit EditT1;
		CEdit EditThreshold;
		CEdit EditBias;
		CEdit EditSlpA;
		CEdit EditPowA;
		CEdit EditSlpT;
		CEdit EditPowT;
		CEdit EditCap;
		CSpinButtonCtrl	SpinT2;
		CSpinButtonCtrl	SpinT1;
		CSpinButtonCtrl	SpinThreshold;
		CSpinButtonCtrl SpinBias;
		CSpinButtonCtrl SpinSlpA;
		CSpinButtonCtrl SpinPowA;
		CSpinButtonCtrl SpinSlpT;
		CSpinButtonCtrl SpinPowT;
		CSpinButtonCtrl SpinCap;
		double T1;
		double T2;
		double Threshold;
		double Bias;
		double SlpA;
		double PowA;
		double SlpT;
		double PowT;
		double Cap;
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
		enum { IDD = IDD_OUTPUT_SETUP };
		CButton PropOutput;
		CStatic StringSize;
		CListBox ListOutput;
		CButton DelOutput;
		CButton add_out;
		CHhnNetwork *Network;
	private:
		int NewOutput;
};

#if defined (__MECHANICS_2D__)
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
		int TypeFB;
		CString FBName;
		CString MechName;
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
#elif defined (__MECHANICS_3D__)
// TODO implementation 3d model
#endif // __MECHANICS_2D__

/////////////////////////////////////////////////////////////////////////////
// CNetworkSetup
class CNetworkSetup : public CPropertySheet{
		DECLARE_DYNAMIC( CNetworkSetup )
	public: // Construction
		CNetworkSetup( UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
		CNetworkSetup( LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
virtual		~CNetworkSetup( void );
	public:
		CHhnNetwork Network;
#if defined (__MECHANICS_2D__)
		t_biomech BiomT;
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
	private:
		CNetworkParam NetworkParam;
		CPopulatSetup PopulatSetup;
		CDriveSetup DriveSetup;
		COutputNetwork OutputSetup;
#if defined (__MECHANICS_2D__)
		CFeedbackSetup FeedbackSetup;
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
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
