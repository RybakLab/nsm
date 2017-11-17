/////////////////////////////////////////////////////////////////////////////
// SpcordDoc.h : interface of the neurosim_doc class
#include "../config.h"

#ifndef __CONSOLE__
#ifndef __NEUROSIM_DOC_H
#define __NEUROSIM_DOC_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../core/simulate.h"

class neurosim_view;

/////////////////////////////////////////////////////////////////////////////
// neurosim_doc
class neurosim_doc : public CDocument{
#ifdef _DEBUG
	public:
virtual	void AssertValid() const;
virtual	void Dump( CDumpContext &dc ) const;
#endif // _DEBUG
		DECLARE_DYNCREATE( neurosim_doc )
		DECLARE_MESSAGE_MAP()
	protected:
		neurosim_doc();
	public:
virtual	~neurosim_doc();
	public: // Operations
		CFrameView *get_view( void );
		void pushView( CFrameView *view );
		void popView( void );

		bool start( void );
		bool stop( void );
		void pause( bool pause );

		void DrawTo( bool complete, double time );
		bool InitViews( void );
		void ShowResults( void );
		void ShowResults( neurosim_view *view );
		void UpdateViews( bool complete, unsigned long counter );
		void ClearAllViews( void );
	public:
virtual	BOOL OnNewDocument( void );
virtual	BOOL OnOpenDocument( LPCTSTR lpszPathName );
virtual	BOOL OnSaveDocument( LPCTSTR lpszPathName );
	private:
afx_msg	void OnRefresh( void );
afx_msg	void OnUpdateFilePrint( CCmdUI *pCmdUI );
afx_msg	void OnUpdateFilePrintPreview( CCmdUI *pCmdUI );
	public:
		queue<CFrameView *> ViewFIFO;
		CSimulateData *SimData;
		CSimulate *Model;
	private:
		HANDLE IsPlotting;
};

#endif // __NEUROSIM_DOC_H
#endif // __CONSOLE__

