#include "../config.h"

#ifndef __CONSOLE__

#if !defined(func_lib_h)
#define func_lib_h

#include <math.h>
/*-------------------------------------------------*/
struct Byte4{
	unsigned char byte1,byte2,byte3,byte4;
};
union ByteCOLORREF{
	struct Byte4 bytes;
	COLORREF c;
};

/*===================================================================*/
 VOID	ErrorMessage( LPCTSTR lpOrigin );
 inline VOID	ErrorMessage( LPCTSTR lpOrigin, int i ) { ErrorMessage(lpOrigin); }

 BOOL	Minmax(const short *x, long n, short *min, short *max);
 double Time_calc_from_box(double t1, double t2, RECT *r, CPoint point);
 double drand( void );	// return pseudorandom-number in the interval 0 to 1
 char*	Remove_tab( char *str );	// replace tab by up to 8 space bars 

#ifndef _DEBUG  // debug version in Runing_ScrolView.cpp
 BOOL	RdWr_INI(bool *array, int n_array, const char *lpszSection, const char *lpszEntry,BOOL ifread, const char * File_name_ini = 0);
#endif

 BOOL	RdWr_INI(int *array, int n_array, const char *lpszSection, const char *lpszEntry,BOOL ifread, const char * File_name_ini = 0);
 BOOL	RdWr_INI(int **array, int n_row, int n_col, const char *lpszSection, const char *lpszEntry,BOOL ifread, const char * File_name_ini = 0);
 BOOL	RdWr_INI(int &value,const char *lpszSection,const char *lpszEntry,BOOL ifread, const char * File_name_ini = 0);
 BOOL	RdWr_INI(BYTE &value_byte,const char *lpszSection,const char *lpszEntry,BOOL ifread, const char * File_name_ini = 0);
 BOOL	RdWr_INI(short &value_shrt,const char *lpszSection,const char *lpszEntry,BOOL ifread, const char * File_name_ini = 0);
 BOOL	RdWr_INI(long &value,const char *lpszSection,const char *lpszEntry, BOOL ifread, const char * File_name_ini = 0);
 BOOL	RdWr_INI(float &value,const char *lpszSection,const char *lpszEntry,BOOL ifread, const char * File_name_ini = 0);
 BOOL	RdWr_INI(double &value,const char *lpszSection,const char *lpszEntry,BOOL ifread, const char * File_name_ini = 0);
 BOOL	RdWr_INI(char *str,const char *lpszSection,const char *lpszEntry,BOOL ifread, const char * File_name_ini = 0);
 BOOL	RdWr_INI(CString &str,const char *lpszSection,const char *lpszEntry,BOOL ifread, const char * File_name_ini = 0);
 BOOL	RdWr_INI(COLORREF &value,const char *lpszSection,const char *lpszEntry,BOOL ifread, const char * File_name_ini = 0);

 void RGB_COLORREF(int R,int G,int B,COLORREF *color);
 void COLORREF_RGB(COLORREF c,int *R,int *G,int *B);
 BOOL Line_R1_R2(int x1,int y1,int x2,int y2,COLORREF color,CDC* pDC,int width=1);
 void CRect_boundary_draw(CDC* pDC, CRect &rec, COLORREF color, int width);
 void EllipticRect_boundary_draw(CDC* pDC, CRect &rec, COLORREF color, int width);

 BOOL Paint_rect(CDC* pDC,CRect *Rec,COLORREF color);
 BOOL Paint_rect(CDC* pDC,RECT *Rec,COLORREF color);

/*===================================================================*/
inline bool save_wndpl( CWnd * pWnd, CString section )
{
//	if( pWnd->IsIconic() ) return false;

	WINDOWPLACEMENT wp;
	wp.length = sizeof wp;
	if( !pWnd->GetWindowPlacement( &wp ) ) return false;
	CWinApp * pApp = AfxGetApp();
	pApp->WriteProfileBinary( section, "wplacement", (LPBYTE) &wp, wp.length );
	return true;
}

inline bool load_wndpl( CWnd * pWnd, CString section )
{
	unsigned length;
	WINDOWPLACEMENT * wp;
	CWinApp * pApp = AfxGetApp();
	if( !pApp->GetProfileBinary( section, "wplacement", (LPBYTE *) &wp, &length ) )
		return false;
	pWnd->SetWindowPlacement( wp );
	free( wp );
	return true;
}

inline bool load_wndpl( CWnd * pWnd, CString section, WINDOWPLACEMENT *wp )
{
	unsigned length;
	CWinApp * pApp = AfxGetApp();
	if( !pApp->GetProfileBinary( section, "wplacement", (LPBYTE *) &wp, &length ) )
		return false;
	pWnd->SetWindowPlacement( wp );
	return true;
}

/*===================================================================*/
#endif // !defined()

#endif //__CONSOLE__
