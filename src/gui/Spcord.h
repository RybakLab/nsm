/////////////////////////////////////////////////////////////////////////////
// Spcord.h : main header file for the SPCORD application
#ifndef __SPCORD_H
#define __SPCORD_H

#include "config.h"

#ifndef __CONSOLE__

#include <afxmt.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include "../resource.h"	// main symbols
#include "func_lib.h"		// usefull functions

class neurosim_doc;

/////////////////////////////////////////////////////////////////////////////
// neurosim_app:
class neurosim_app : public CWinApp{
friend		neurosim_doc;
		DECLARE_MESSAGE_MAP()
	public:
		neurosim_app( void ){};
	public:
virtual		BOOL InitInstance();
virtual		CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	private:
		afx_msg	void OnAppAbout( void );
		afx_msg	void OnFileNew( void );
		void initViews( neurosim_doc *doc );
	public:
		CMultiDocTemplate* m_pView; 
#if defined (__MECHANICS_2D__)
		CMultiDocTemplate* m_pLimb;
#elif defined (__MECHANICS_3D__)
	// TODO implementation 3d model
#endif // __MECHANICS_2D__
};

#endif // __CONSOLE__
#endif // __SPCORD_H
