// SpcordView.h : header file
#ifndef __SPCORDVIEW_H
#define __SPCORDVIEW_H

#include "config.h"

#ifndef __CONSOLE__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "cmatrix.h"
#include "dataview.h"

class Grafor;
class CFrameView;
class neurosim_doc;

/////////////////////////////////////////////////////////////////////////////
// neurosim_view view
class neurosim_view : public CView{
#ifdef _DEBUG
	public:
virtual	void AssertValid() const;
virtual	void Dump( CDumpContext& dc ) const;
#endif // _DEBUG
		DECLARE_DYNCREATE( neurosim_view )
		DECLARE_MESSAGE_MAP()
	protected:
		neurosim_view( void );           // protected constructor used by dynamic creation
	public:
virtual	~neurosim_view( void );
	public: // overridden function
		void OnInitialUpdate();
	protected: // overridden function
		void OnDraw( CDC* pDC );      
		BOOL OnPreparePrinting( CPrintInfo *pInfo );
		BOOL PreCreateWindow( CREATESTRUCT &cs );
	public:
virtual	void InitDraw( void );
	protected:
virtual	void PrepareDC( CDC* pDC, CRect &rect ){};
virtual	void DrawPicture( CDC* pDC, CRect &rect ){};
virtual	unsigned long GetMaxCounter( void ){ return 0; };
	public:
		neurosim_doc *GetDocument( void );
		bool IsInitUpdated( void ){ return InitUpdate; };
		void DrawTo( bool complete, unsigned long counter = -1 );
		void SaveImage( const char *filename, CSize size );
	public:
		const char *getViewName( void ) const;
		void attachView( CFrameView *view ){ View = view; };
		void changeViewSize( void );
	protected:
afx_msg	int OnCreate(LPCREATESTRUCT lpCreateStruct);
afx_msg	void OnSize(UINT nType, int cx, int cy);
afx_msg	void OnDestroy();
afx_msg	void OnLButtonDblClk(UINT nFlags, CPoint point);
		void EraseBackground( void );
	public:
		CFrameView *View;
		bool InitUpdate;
		bool IsUpdated;
		unsigned long LastCounter;
		unsigned long NewCounter;
		CDC *dcMem;
		CBitmap *Bitmap;
};

#ifndef _DEBUG  // debug version in SpcordView.cpp
inline neurosim_doc* neurosim_view::GetDocument()
   { return (neurosim_doc*)m_pDocument; }
#endif // _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChartView view
class CChartView : public neurosim_view{
#ifdef _DEBUG
	public:
virtual void AssertValid() const;
virtual void Dump(CDumpContext& dc) const;
#endif // _DEBUG
		DECLARE_DYNCREATE( CChartView )
	protected:
		CChartView();           // protected constructor used by dynamic creation
	public:
virtual	~CChartView();
	public:
	    void SetView( void );
	public:
		void OnInitialUpdate();
	private:
		void DrawPicture( CDC* pDC, CRect &rect );
		void SetRegionRect( const CRect &rect, int charsize );
		unsigned long GetMaxCounter( void );
		hhn_pair<int> GetViewLayout( void );
	private:
		Grafor *gr;
		CMatrix<CRect> RegionRect;	// rect to draw data ( in your case has to be 30 such rectangles)
};

#ifdef __MECHANICS__
/////////////////////////////////////////////////////////////////////////////
// CLimbView view
class CLimbView : public neurosim_view{
#ifdef _DEBUG
	public:
virtual	void AssertValid() const;
virtual	void Dump(CDumpContext& dc) const;
#endif // _DEBUG
		DECLARE_DYNCREATE(CLimbView)
	protected:
		CLimbView();           // protected constructor used by dynamic creation
virtual	~CLimbView();
	public:
		void InitDraw( void );
	private:
		void PrepareDC( CDC* pDC, CRect &rect );
		void DrawPicture( CDC* pDC, CRect &rect );
		void DrawGround( CDC* pDC, CRect &rect );
		void DrawWalker( CDC* pDC, const void *buf, bool isstick, unsigned long counter );
		void DrawLimb( CDC* pDC, const void *limb, COLORREF color );
		void ChangeViewSize( void );
		unsigned long GetMaxCounter( void );
	private:
		bool IsSliceEmpty;
		CWalkerVertex LastSlice;
		COLORREF LeftLegColor;
		COLORREF RightLegColor;
		COLORREF WalkerColorL;
		COLORREF WalkerColorR;
		COLORREF EraseColor;
		COLORREF GroundColor;
};
#endif // __MECHANICS__
#endif // __CONSOLE__
#endif // __SPCORDVIEW_H
