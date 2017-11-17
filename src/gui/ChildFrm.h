/////////////////////////////////////////////////////////////////////////////
// ChildFrm.h : interface of the CChildFrame class
#include "../config.h"

#ifndef __CONSOLE__

#ifndef __CHILDFRAME_H
#define __CHILDFRAME_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CFrameView;

class CChildFrame : public CMDIChildWnd{
#ifdef _DEBUG
	public:
virtual	void AssertValid() const;
virtual	void Dump(CDumpContext& dc) const;
#endif // _DEBUG
		DECLARE_DYNCREATE(CChildFrame)
		DECLARE_MESSAGE_MAP()
	public:
		CChildFrame();
virtual	~CChildFrame();
	protected:
		void setWndPos( CFrameView *view );
	public:
virtual	BOOL PreCreateWindow( CREATESTRUCT &cs );
		void OnUpdateFrameTitle( BOOL bAddToTitle );
	protected:
		BOOL OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext );
afx_msg	void OnMove(int x, int y);
};

#endif // __CHILDFRAME_H
#endif // __CONSOLE__
