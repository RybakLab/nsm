// func_lib.cpp : Defines the class behaviors for the application.
#define _CRT_SECURE_NO_WARNINGS

#include "precompile.h"

#ifndef __CONSOLE__

#include "func_lib.h"


#ifdef _DEBUG
////#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*===================================================================*/
BOOL RdWr_INI(int &value,const char *lpszSection,const char *lpszEntry,BOOL ifread, const char * File_name_ini /*=0*/)
{
	int i_val;
	char buff[256];
	if(ifread) {
		CString strBuffer, lpDefault;
		DWORD nSize = 255;
		if( File_name_ini ) {
			if( ! GetPrivateProfileString(lpszSection, lpszEntry, lpDefault, buff, nSize, File_name_ini) ) return FALSE;
			strBuffer = buff;
		} else 
			strBuffer = AfxGetApp()->GetProfileString(lpszSection, lpszEntry);

		if (strBuffer.IsEmpty()) return FALSE;
		sscanf_s(strBuffer,"%d",&i_val);
		value=i_val;
	} else {
		i_val=value;
		sprintf_s(buff, 256, "%d", i_val );

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
		buff = new char[strBuffer.GetLength()+1];
		strcpy_s( buff, strBuffer.GetLength()+1, ( LPCTSTR ) strBuffer );

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
		buff = new char[strBuffer.GetLength()+1];
		strcpy_s( buff, strBuffer.GetLength()+1, ( LPCTSTR ) strBuffer );

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
		sscanf_s(strBuffer,"%d",&value);
	} else {
		sprintf_s( buff, 256, "%d", value );

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
		sscanf_s(strBuffer,"%g",&value);
	} else {
		sprintf_s(buff, 256, "%g", value );

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
		sscanf_s(strBuffer,"%lg",&value);
	} else {
		sprintf_s(buff, 256, "%lg", value );

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
		sscanf_s(strBuffer,"%d,%d,%d",&R,&G,&B);
		RGB_COLORREF(R,G,B,&value);
	} else {
		COLORREF_RGB(value,&R,&G,&B);
		sprintf_s(buff, 256, "%d,%d,%d", R, G, B );

		if( File_name_ini ) {
			if( ! WritePrivateProfileString(lpszSection,lpszEntry,buff, File_name_ini)) return FALSE;
		} else 
			if(!AfxGetApp()->WriteProfileString(lpszSection,lpszEntry,buff)) return FALSE;
	}
	return TRUE;
}
/*-------------------------------------------------*/ 
BOOL RdWr_INI(char *str, const char *lpszSection, const char *lpszEntry,BOOL ifread, const char * File_name_ini)
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
		sprintf_s(str, 256, "%s", (LPCTSTR)strBuffer );
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
	char buff[256];
	strcpy_s( buff, 256, str);
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
#endif //__CONSOLE__
