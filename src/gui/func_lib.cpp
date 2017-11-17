// func_lib.cpp : Defines the class behaviors for the application.
#include "precompile.h"

#ifndef __CONSOLE__

#include "func_lib.h"

//#include "Runing_ScrolDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*===================================================================*/
char* Remove_tab( char *str )
{
 char buff[1024];
 int i, j;
	for( i = 0, j = 0; str[i]; i++ ) {
		if( str[i] == '\t' ) {
			buff[j++] = ' ';
			while( j%8 ) buff[j++] = ' ';
		}
		else
		buff[j++] = str[i];
	}
	buff[j] = '\0';
	strcpy(str, buff);

	return str;
}
/*===================================================================*/
double drand( void )
{
	double d = rand();
	return d/RAND_MAX;
}
/*===================================================================*/
BOOL Minmax(const short *x, long n, short *min, short *max)
{
	*min = *max	= 0;
	if( n <= 0 ) return FALSE;
	for(long i=0; i<n; i++) {
		short a = x[i];
		if ( *min > a || i==0 ) *min = a;
		if ( *max < a || i==0 ) *max = a;
	}
	return TRUE;
}
/*===================================================================*/
double Time_calc_from_box(double t1, double t2, RECT *r, CPoint point)
{
 double	t,d;
    d = r->right - r->left;
    if(d==0) d = 1;
    t=t1+(point.x-r->left)*(t2-t1)/d;

    return t;
}
/*===================================================================*/
VOID
ErrorMessage( 
	LPCTSTR lpOrigin	// Indicates error location
	)
{
	LPTSTR msgBuffer;		// string returned from system
	DWORD cBytes;			// length of returned string
	DWORD dwMessageId;	// ERROR_XXX code value
	char	msgBuffer_out[256];

	dwMessageId = GetLastError();
	cBytes = FormatMessage(
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_ALLOCATE_BUFFER,
				NULL,
				dwMessageId,
				MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
				(TCHAR *)&msgBuffer,
				500,
				NULL );
	if( msgBuffer ) {
		msgBuffer[ cBytes ] = TEXT('\0');
		sprintf( msgBuffer_out, "Error: %s -- %s MessageId %d", lpOrigin, msgBuffer, dwMessageId );
		LocalFree( msgBuffer );
	}
	else {
		sprintf( msgBuffer_out, "Error: %s FormatMessage error: MessageId %d\n", lpOrigin, GetLastError());
	}

	AfxMessageBox( msgBuffer_out );
}
/*-------------------------------------------------*/ 
BOOL RdWr_INI(int &value,const char *lpszSection,const char *lpszEntry,BOOL ifread, const char * File_name_ini /*=0*/)
{
 int	i_val;
 char	buff[256];
	if(ifread) {
		CString strBuffer, lpDefault;
		DWORD nSize = 255;
		if( File_name_ini ) {
			if( ! GetPrivateProfileString(lpszSection, lpszEntry, lpDefault, buff, nSize, File_name_ini) ) return FALSE;
			strBuffer = buff;
		} else 
			strBuffer = AfxGetApp()->GetProfileString(lpszSection, lpszEntry);

		if (strBuffer.IsEmpty()) return FALSE;
		sscanf(strBuffer,"%d",&i_val);
		value=i_val;
	} else {
		i_val=value;
		sprintf(buff,"%d",i_val);

		if( File_name_ini ) {
			if( ! WritePrivateProfileString(lpszSection,lpszEntry,buff, File_name_ini)) return FALSE;
		} else 
			if(!AfxGetApp()->WriteProfileString(lpszSection,lpszEntry,buff)) return FALSE;
	}
	return TRUE;
}
/*-------------------------------------------------*/ 
BOOL RdWr_INI(BYTE &value_byte,const char *lpszSection,const char *lpszEntry,BOOL ifread, const char * File_name_ini /*=0*/)
{
 int	value=value_byte;
	BOOL	ret=RdWr_INI(value,lpszSection,lpszEntry,ifread, File_name_ini);
	value_byte = ( BYTE ) value;
	return ret;
}
/*-------------------------------------------------*/ 
BOOL RdWr_INI(short &value_short,const char *lpszSection,const char *lpszEntry,BOOL ifread, const char * File_name_ini /*=0*/)
{
 int	value = value_short;
	BOOL	ret = RdWr_INI(value,lpszSection,lpszEntry,ifread, File_name_ini);
	value_short = ( short ) value;
	return ret;
}
/*-------------------------------------------------*/ 
#ifndef _DEBUG  // debug version in Runing_ScrolView.cpp
BOOL RdWr_INI(bool *array, int n_array, const char *lpszSection, const char *lpszEntry,BOOL ifread, const char * File_name_ini /*=0*/)
{
	int *array_int = new int [n_array];
	int i=0;
	for(i=0; i<n_array; i++) array_int[i] = (int) array[i];

	BOOL ret = RdWr_INI(array_int, n_array, lpszSection, lpszEntry, ifread,  File_name_ini /*=0*/);

	for( i=0; i<n_array; i++) array[i] = array_int[i] ? true : false;

	delete[] array_int;

	return ret;
}
#endif
/*-------------------------------------------------*/ 
BOOL RdWr_INI(int **array, int n_row, int n_col, const char *lpszSection, const char *lpszEntry,BOOL ifread, const char * File_name_ini )
{
 int	i;
 int    j;
 char	buff[256];
	if(ifread) {	
		CString strBuffer, lpDefault;
		DWORD nSize = 255;
		if( File_name_ini ) {
			if( ! GetPrivateProfileString(lpszSection, lpszEntry, lpDefault, buff, nSize, File_name_ini) ) return FALSE;
			strBuffer = buff;
		} else 
			strBuffer = AfxGetApp()->GetProfileString(lpszSection, lpszEntry);

		if (strBuffer.IsEmpty()) return FALSE;

		char	*buff, *end_buff;
		buff = new char[ strBuffer.GetLength() +1 ];
		strcpy( buff, ( LPCTSTR ) strBuffer );

		end_buff = buff;
		for( i=0; i<n_row; i++)
             for( j=0; j<n_col; j++)
			      if( end_buff ) array[i][j] = (int) strtol( end_buff, &end_buff, 10 );

		delete [] buff;
	} else {
		CString tmp, tmp1;
		tmp ="";
		for( i=0; i<n_row; i++)
            for( j=0; j <n_col; j++){
			     tmp1.Format("%d ", array[i][j] );
			     tmp += tmp1;
		         }
		if( File_name_ini ) {
			if( ! WritePrivateProfileString(lpszSection,lpszEntry,tmp, File_name_ini)) return FALSE;
		} else 
			if(!AfxGetApp()->WriteProfileString(lpszSection,lpszEntry,tmp)) return FALSE;
	}
	return TRUE;
}

BOOL RdWr_INI(int *array, int n_array, const char *lpszSection, const char *lpszEntry, BOOL ifread, const char * File_name_ini /*=0*/)
{
 int	i;
 char	buff[256];
	if(ifread) {	
		CString strBuffer, lpDefault;
		DWORD nSize = 255;
		if( File_name_ini ) {
			if( ! GetPrivateProfileString(lpszSection, lpszEntry, lpDefault, buff, nSize, File_name_ini) ) return FALSE;
			strBuffer = buff;
		} else 
			strBuffer = AfxGetApp()->GetProfileString(lpszSection, lpszEntry);

		if (strBuffer.IsEmpty()) return FALSE;

		char	*buff, *end_buff;
		buff = new char[ strBuffer.GetLength() +1 ];
		strcpy( buff, ( LPCTSTR ) strBuffer );

		end_buff = buff;
		for( i=0; i<n_array; i++)
			if( end_buff ) array[i] = (int) strtol( end_buff, &end_buff, 10 );

		delete [] buff;
	} else {
		CString tmp, tmp1;
		tmp ="";
		for( i=0; i<n_array; i++) {
			tmp1.Format("%d ", array[i] );
			tmp += tmp1;
		}
		if( File_name_ini ) {
			if( ! WritePrivateProfileString(lpszSection,lpszEntry,tmp, File_name_ini)) return FALSE;
		} else 
			if(!AfxGetApp()->WriteProfileString(lpszSection,lpszEntry,tmp)) return FALSE;
	}
	return TRUE;
}
/*-------------------------------------------------*/ 
BOOL RdWr_INI(long &value,const char *lpszSection,const char *lpszEntry, BOOL ifread, const char * File_name_ini /*=0*/)
{
 char	buff[256];
	if(ifread) {	
		CString strBuffer, lpDefault;
		DWORD nSize = 255;
		if( File_name_ini ) {
			if( ! GetPrivateProfileString(lpszSection, lpszEntry, lpDefault, buff, nSize, File_name_ini) ) return FALSE;
			strBuffer = buff;
		} else 
			strBuffer = AfxGetApp()->GetProfileString(lpszSection, lpszEntry);

		if (strBuffer.IsEmpty()) return FALSE;
		sscanf(strBuffer,"%d",&value);
	} else {
		sprintf(buff,"%d",value);

		if( File_name_ini ) {
			if( ! WritePrivateProfileString(lpszSection,lpszEntry,buff, File_name_ini)) return FALSE;
		} else 
			if(!AfxGetApp()->WriteProfileString(lpszSection,lpszEntry,buff)) return FALSE;
	}
	return TRUE;
}
/*-------------------------------------------------*/ 
BOOL RdWr_INI(float &value,const char *lpszSection,const char *lpszEntry,BOOL ifread, const char * File_name_ini)
{
 char	buff[256];
	if(ifread) {	
		CString strBuffer, lpDefault;
		DWORD nSize = 255;
		if( File_name_ini ) {
			if( ! GetPrivateProfileString(lpszSection, lpszEntry, lpDefault, buff, nSize, File_name_ini) ) return FALSE;
			strBuffer = buff;
		} else 
			strBuffer = AfxGetApp()->GetProfileString(lpszSection, lpszEntry);

		if (strBuffer.IsEmpty()) return FALSE;
		sscanf(strBuffer,"%g",&value);
	} else {
		sprintf(buff,"%g",value);

		if( File_name_ini ) {
			if( ! WritePrivateProfileString(lpszSection,lpszEntry,buff, File_name_ini)) return FALSE;
		} else 
			if(!AfxGetApp()->WriteProfileString(lpszSection,lpszEntry,buff)) return FALSE;
	}
	return TRUE;
}
/*-------------------------------------------------*/ 
BOOL RdWr_INI(double &value,const char *lpszSection,const char *lpszEntry,BOOL ifread, const char * File_name_ini /*=0*/)
{
 char	buff[256];
	if(ifread) {	
		CString strBuffer, lpDefault;
		DWORD nSize = 255;
		if( File_name_ini ) {
			if( ! GetPrivateProfileString(lpszSection, lpszEntry, lpDefault, buff, nSize, File_name_ini) ) return FALSE;
			strBuffer = buff;
		} else 
			strBuffer = AfxGetApp()->GetProfileString(lpszSection, lpszEntry);

		if (strBuffer.IsEmpty()) return FALSE;
		sscanf(strBuffer,"%lg",&value);
	} else {
		sprintf(buff,"%lg",value);

		if( File_name_ini ) {
			if( ! WritePrivateProfileString(lpszSection,lpszEntry,buff, File_name_ini)) return FALSE;
		} else 
			if(!AfxGetApp()->WriteProfileString(lpszSection,lpszEntry,buff)) return FALSE;
	}
	return TRUE;
}
/*-------------------------------------------------*/ 
BOOL RdWr_INI(COLORREF &value,const char *lpszSection,const char *lpszEntry,BOOL ifread, const char * File_name_ini)
{
 int	R,G,B;
 char	buff[256];
	if(ifread) {	
		CString strBuffer, lpDefault;
		DWORD nSize = 255;
		if( File_name_ini ) {
			if( ! GetPrivateProfileString(lpszSection, lpszEntry, lpDefault, buff, nSize, File_name_ini) ) return FALSE;
			strBuffer = buff;
		} else 
			strBuffer = AfxGetApp()->GetProfileString(lpszSection, lpszEntry);

		if (strBuffer.IsEmpty()) return FALSE;
		sscanf(strBuffer,"%d,%d,%d",&R,&G,&B);
		RGB_COLORREF(R,G,B,&value);
	} else {
		COLORREF_RGB(value,&R,&G,&B);
		sprintf(buff,"%d,%d,%d",R,G,B);

		if( File_name_ini ) {
			if( ! WritePrivateProfileString(lpszSection,lpszEntry,buff, File_name_ini)) return FALSE;
		} else 
			if(!AfxGetApp()->WriteProfileString(lpszSection,lpszEntry,buff)) return FALSE;
	}
	return TRUE;
}
/*-------------------------------------------------*/ 
BOOL RdWr_INI(char *str,const char *lpszSection,const char *lpszEntry,BOOL ifread, const char * File_name_ini)
{
	if(ifread) {	
		CString strBuffer, lpDefault;
		DWORD nSize = 255;
		if( File_name_ini ) {
			if( ! GetPrivateProfileString(lpszSection, lpszEntry, lpDefault, str, nSize, File_name_ini) ) return FALSE;
			strBuffer = str;
		} else 
			strBuffer = AfxGetApp()->GetProfileString(lpszSection, lpszEntry);

		if (strBuffer.IsEmpty()) return FALSE;
		sprintf(str,"%s",strBuffer);
	} else {
		if( File_name_ini ) {
			if( ! WritePrivateProfileString(lpszSection,lpszEntry,str, File_name_ini)) return FALSE;
		} else 
			if(!AfxGetApp()->WriteProfileString(lpszSection,lpszEntry,str)) return FALSE;
	}
	return TRUE;
}
/*-------------------------------------------------*/ 
BOOL RdWr_INI(CString &str,const char *lpszSection,const char *lpszEntry,BOOL ifread, const char * File_name_ini /*=0*/)
{
 char	buff[256];
	strcpy(buff,str);
	if(!RdWr_INI(buff,lpszSection,lpszEntry,ifread, File_name_ini)) return FALSE;
	str=buff;
	return TRUE;
}
/*-------------------------------------------------*/
void RGB_COLORREF(int R,int G,int B,COLORREF *color)
{
	union ByteCOLORREF col;
	col.bytes.byte1=R;
	col.bytes.byte2=G;
	col.bytes.byte3=B;
	*color	=col.c;
}
/*-------------------------------------------------*/
void COLORREF_RGB(COLORREF color,int *R,int *G,int *B)
{
	union ByteCOLORREF col;
	col.c = color;
	*R=col.bytes.byte1;
	*G=col.bytes.byte2;
	*B=col.bytes.byte3;
//	*b4=col.bytes.byte4;
}
/*-------------------------------------------------------------------------------*/
BOOL Paint_rect(CDC* pDC,CRect *Rec,COLORREF color)
{  
	CBrush brush_cur;
	if (!brush_cur.CreateSolidBrush(color)) return FALSE;
	CBrush* pOldbrush = pDC->SelectObject(&brush_cur);
	pDC->FillRect(Rec,&brush_cur);
	pDC->SelectObject(pOldbrush);
	return TRUE;
}
/*-------------------------------------------------------------------------------*/
BOOL Paint_rect(CDC* pDC,RECT *Rec,COLORREF color)
{  
	CBrush brush_cur;
	if (!brush_cur.CreateSolidBrush(color)) return FALSE;
	CBrush* pOldbrush = pDC->SelectObject(&brush_cur);
	pDC->FillRect(Rec,&brush_cur);
	pDC->SelectObject(pOldbrush);
	return TRUE;
}
/*-------------------------------------------------------------------------------*/
BOOL Line_R1_R2(int x1,int y1,int x2,int y2,COLORREF color,CDC* pDC,int width)
{  
	CPen pen_cur;

	if (!pen_cur.CreatePen(PS_SOLID,width,color)) return FALSE;
	CPen* pOldPen = pDC->SelectObject(&pen_cur);

  	pDC->MoveTo(x1,y1);
  	pDC->LineTo(x2,y2);

	pDC->SelectObject(pOldPen);
	return TRUE;
}
/*-------------------------------------------------------------------------------*/
void CRect_boundary_draw(CDC* pDC, CRect &rec, COLORREF color, int width)
{  
	CRgn Rgn;
	if( ! Rgn.CreateRectRgnIndirect( ( LPCRECT ) rec ) ) return;

	CBrush brush_cur;
	if (!brush_cur.CreateSolidBrush(color)) return;
	CBrush* pOldbrush = pDC->SelectObject(&brush_cur);
	pDC->FrameRgn( &Rgn, &brush_cur, width, width );

	pDC->SelectObject(pOldbrush);

/*
	CBrush brush_cur;
	if (!brush_cur.CreateSolidBrush(color)) return;
	CBrush* pOldbrush = pDC->SelectObject(&brush_cur);
	pDC->FrameRect( &rec, &brush_cur );

	pDC->SelectObject(pOldbrush);
*/
/*
	Line_R1_R2( rec.left+2,		rec.top+2,		rec.right-2,	rec.top+2,		color, pDC, width);
	Line_R1_R2( rec.right-2,	rec.top+2,		rec.right-2,	rec.bottom-2,	color, pDC, width);
	Line_R1_R2( rec.right-2,	rec.bottom-2,	rec.left+2,		rec.bottom-2,	color, pDC, width);
	Line_R1_R2( rec.left+2,		rec.bottom-2,	rec.left+2,		rec.top+2,		color, pDC, width);
*/
}
/*-------------------------------------------------------------------------------*/
void EllipticRect_boundary_draw(CDC* pDC, CRect &rec, COLORREF color, int width)
{  
/*
	CRgn Rgn;
	if( ! Rgn.CreateEllipticRgnIndirect( ( LPCRECT ) rec ) ) return;

BOOL FrameRgn( CRgn* pRgn, CBrush* pBrush, int nWidth, int nHeight );
	pDC->SelectObject(pOldbrush);
*/
    pDC->SelectStockObject( NULL_BRUSH ); // deselect hatchBrush
	CPen pen_cur;
	if (!pen_cur.CreatePen( PS_SOLID, 1, color )) return;
	CPen* pOldPen = pDC->SelectObject(&pen_cur);

	pDC->Ellipse( rec );

	int	x = (rec.left + rec.right)/2;
	int	y = (rec.bottom + rec.top)/2;

	pDC->SelectObject(pOldPen);

	Line_R1_R2(x, y, x, y +width, color, pDC, width);
}

#endif //__CONSOLE__
