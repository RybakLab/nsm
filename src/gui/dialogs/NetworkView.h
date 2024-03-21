/////////////////////////////////////////////////////////////////////////////
// NetworkView.h : header file
#include "config.h"

#ifndef __NETWORK_VIEW_H
#define __NETWORK_VIEW_H

#ifndef __CONSOLE__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "sptypes.h"
#include "frameview.h"
#include "walker.h"
#include "hhnnetwork.h"

/////////////////////////////////////////////////////////////////////////////
// CGeomView dialog
class CGeomView : public CDialog{
	DECLARE_MESSAGE_MAP()
	public: // constructions/destructor
		CGeomView( CWnd* pParent = NULL );
		~CGeomView( void ){};
	protected: // overrides
virtual		void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	protected: // message map
virtual		BOOL OnInitDialog();
		afx_msg void OnDeltaposSpinRow(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinCol(NMHDR* pNMHDR, LRESULT* pResult);
	public: // data
		enum { IDD = IDD_GEOMETRY_VIEW };
		CSpinButtonCtrl	SpinRow;
		CSpinButtonCtrl	SpinCol;
		int     NumParam;
		int		NumCol;
		int		NumRow;
};

/////////////////////////////////////////////////////////////////////////////
// CScaleView dialog
class CScaleView : public CDialog{
	DECLARE_MESSAGE_MAP()
	public: // constructions/destructor
		CScaleView( CWnd* pParent = NULL );
		~CScaleView( void ){};
	protected: // overrides
virtual		void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	protected: // message map
virtual		BOOL OnInitDialog();
		afx_msg void OnDeltaposSpinYmax(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSpinYmin(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnUpdateEditYmax();
		afx_msg void OnUpdateEditYmin();
	public: // data
		enum { IDD = IDD_SCALE_VIEW };
		CSpinButtonCtrl	SpinYmin;
		CSpinButtonCtrl	SpinYmax;
		CEdit	EditYmin;
		CEdit	EditYmax;
		double	Ymax;
		double	Ymin;
};

/////////////////////////////////////////////////////////////////////////////
// CNetworkView dialog
class CNetworkView : public CDialog{
	DECLARE_MESSAGE_MAP()
	public: // constructions/destructor
		CNetworkView( CWnd* pParent = NULL );
		~CNetworkView( void ){};
	public:// overrides
virtual		int DoModal();
	protected:
virtual		void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	protected: // message map
virtual		BOOL OnInitDialog();
		afx_msg void OnAddView();
		afx_msg void OnRemoveView();
		afx_msg void OnScaleView();
		afx_msg void OnGeometryView();
		afx_msg void OnDblclkListView(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnMoveUp();
		afx_msg void OnMoveDown();
	private:
		void ShowView( void );
		void Activate( void );
	public: // data
		enum { IDD = IDD_NETWORK_VIEW };
		CButton	GeomView;
		CButton	ScaleView;
		CButton	RemoveView;
		CButton	AddView;
		CListCtrl	ListView;
		CTreeCtrl	ViewTree;
		CEdit	EditNameView;
		CString	NameView;
		CChartFrameView View;
		CSimulate *Manager;
	private:
		int NumView;
};


/////////////////////////////////////////////////////////////////////////////
// CImageSize dialog
class CImageSize : public CDialog{
	DECLARE_MESSAGE_MAP()
	public: // constructions/destructor
		CImageSize( CWnd* pParent = NULL );   // standard constructor
		~CImageSize( void ){};
	public:// overrides
virtual		void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	protected: // message map
virtual		BOOL OnInitDialog();
	public: // data
		enum { IDD = IDD_IMAGE_SIZE };
		CSpinButtonCtrl	Spin_Width;
		CSpinButtonCtrl	Spin_Height;
		int		Height;
		int		Width;
};

#endif // __CONSOLE__
#endif // __NETWORK_VIEW_H
