/////////////////////////////////////////////////////////////////////////////
// setupXPstyle.cpp : implementation file
#include "precompile.h"

#ifndef __CONSOLE__

#include "setupXPstyle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif // _DEBUG

BOOL xpDrawFrameControl( HDC hdc, LPRECT lprc, UINT DFC_type, UINT DFCS_state )
{
    XPStyle style;
    if( decodeStyle( &style, DFC_type, DFCS_state, 0 )){
		USES_CONVERSION;
		HTHEME hTheme = ::OpenThemeData(NULL, A2CW(style.ClassList));
		if (hTheme != NULL){
			::DrawThemeBackground(hTheme, hdc, style.Parts, style.State, lprc, NULL);
			::CloseThemeData(hTheme);
			return TRUE;
			}
		else{
			return ::DrawFrameControl( hdc, lprc, DFC_type, DFCS_state );
			}
		}
	return ::DrawFrameControl( hdc, lprc, DFC_type, DFCS_state );
}

BOOL decodeStyle( XPStyle *xpstyle, UINT DFC_type, UINT DFCS_state, UINT CBS_style  )
{
    if( DFC_type == DFC_BUTTON ){
		xpstyle->ClassList = "BUTTON";
		if( DFCS_state & DFCS_BUTTONCHECK ){
			xpstyle->Parts = BP_CHECKBOX;
			xpstyle->State = CBS_UNCHECKEDNORMAL;
			if( DFCS_state & DFCS_HOT )
				xpstyle->State = CBS_UNCHECKEDHOT;
			else if( DFCS_state & DFCS_PUSHED )
				xpstyle->State = CBS_UNCHECKEDPRESSED;
			else if( DFCS_state & DFCS_INACTIVE )
				xpstyle->State = CBS_UNCHECKEDDISABLED;
			if( DFCS_state & DFCS_CHECKED ){
				xpstyle->State = CBS_CHECKEDNORMAL;
				if( DFCS_state & DFCS_HOT )
					xpstyle->State = CBS_CHECKEDHOT;
				else if( DFCS_state & DFCS_PUSHED )
					xpstyle->State = CBS_CHECKEDPRESSED;
				else if( DFCS_state & DFCS_INACTIVE )
					xpstyle->State = CBS_CHECKEDDISABLED;
				}
			else if( DFCS_state & DFCS_TRANSPARENT ){
				xpstyle->State = CBS_MIXEDNORMAL;
				if( DFCS_state & DFCS_HOT )
					xpstyle->State = CBS_MIXEDHOT;
				else if( DFCS_state & DFCS_PUSHED )
					xpstyle->State = CBS_MIXEDPRESSED;
				else if( DFCS_state & DFCS_INACTIVE )
					xpstyle->State = CBS_MIXEDDISABLED;
				}
			return TRUE;
			}
		else if( DFCS_state & DFCS_BUTTONPUSH ){
			xpstyle->Parts = BP_PUSHBUTTON;
			xpstyle->State = PBS_NORMAL;
			if( DFCS_state & DFCS_ADJUSTRECT )
				xpstyle->State = PBS_DEFAULTED;
			else if( DFCS_state & DFCS_HOT )
				xpstyle->State = PBS_HOT;
			else if( DFCS_state & DFCS_PUSHED )
				xpstyle->State = PBS_PRESSED;
			else if( DFCS_state & DFCS_INACTIVE )
				xpstyle->State = PBS_DISABLED;
			return TRUE;
			}
		else if( DFCS_state & DFCS_BUTTONRADIO ){
			xpstyle->Parts = BP_RADIOBUTTON;
			xpstyle->State = RBS_UNCHECKEDNORMAL;
			if( DFCS_state & DFCS_HOT )
				xpstyle->State = RBS_UNCHECKEDHOT;
			else if( DFCS_state & DFCS_PUSHED )
				xpstyle->State = RBS_UNCHECKEDPRESSED;
			else if( DFCS_state & DFCS_INACTIVE )
				xpstyle->State = RBS_UNCHECKEDDISABLED;
			if( DFCS_state & DFCS_CHECKED ){
				xpstyle->State = RBS_CHECKEDNORMAL;
				if( DFCS_state & DFCS_HOT )
					xpstyle->State = RBS_CHECKEDHOT;
				else if( DFCS_state & DFCS_PUSHED )
					xpstyle->State = RBS_CHECKEDPRESSED;
				else if( DFCS_state & DFCS_INACTIVE )
					xpstyle->State = RBS_CHECKEDDISABLED;
				}
			return TRUE;
			}
		}
    else if( DFC_type == DFC_CAPTION ){
		xpstyle->ClassList = "WINDOW";
        xpstyle->Parts = WP_CAPTION;
		xpstyle->State = CS_ACTIVE;
        if( DFCS_state & DFCS_TRANSPARENT ){
			xpstyle->State = CS_DISABLED;
			}
		else if( DFCS_state & DFCS_INACTIVE ){
			xpstyle->State = CS_INACTIVE;
			}
        if( DFCS_state & DFCS_CAPTIONCLOSE ){
            xpstyle->Parts = WP_CLOSEBUTTON;
			xpstyle->State = CBS_NORMAL;
			if( DFCS_state & DFCS_HOT )
				xpstyle->State = CBS_HOT;
			else if( DFCS_state & DFCS_PUSHED )
				xpstyle->State = CBS_PUSHED;
			else if( DFCS_state & DFCS_INACTIVE )
				xpstyle->State = CBS_DISABLED;
            }
        else if ( DFCS_state & DFCS_CAPTIONHELP ){
            xpstyle->Parts = WP_HELPBUTTON;
			xpstyle->State = HBS_NORMAL;
			if( DFCS_state & DFCS_HOT )
				xpstyle->State = HBS_HOT;
			else if( DFCS_state & DFCS_PUSHED )
				xpstyle->State = HBS_PUSHED;
			else if( DFCS_state & DFCS_INACTIVE )
				xpstyle->State = HBS_DISABLED;
            }
        else if ( DFCS_state & DFCS_CAPTIONMAX ){
            xpstyle->Parts = WP_MAXBUTTON;
			xpstyle->State = MAXBS_NORMAL;
			if( DFCS_state & DFCS_HOT )
				xpstyle->State = MAXBS_HOT;
			else if( DFCS_state & DFCS_PUSHED )
				xpstyle->State = MAXBS_PUSHED;
			else if( DFCS_state & DFCS_INACTIVE )
				xpstyle->State = MAXBS_DISABLED;
            }
        else if ( DFCS_state & DFCS_CAPTIONMIN ){
            xpstyle->Parts = WP_MINBUTTON;
			xpstyle->State = MINBS_NORMAL;
			if( DFCS_state & DFCS_HOT )
				xpstyle->State = MINBS_HOT;
			else if( DFCS_state & DFCS_PUSHED )
				xpstyle->State = MINBS_PUSHED;
			else if( DFCS_state & DFCS_INACTIVE )
				xpstyle->State = MINBS_DISABLED;
            }
        else if( DFCS_state & DFCS_CAPTIONRESTORE ){
            xpstyle->Parts = WP_RESTOREBUTTON;
			xpstyle->State = RBS_NORMAL;
			if( DFCS_state & DFCS_HOT )
				xpstyle->State = RBS_HOT;
			else if( DFCS_state & DFCS_PUSHED )
				xpstyle->State = RBS_PUSHED;
			else if( DFCS_state & DFCS_INACTIVE )
				xpstyle->State = RBS_DISABLED;
            }
			return TRUE;
		}
    else if( DFC_type == DFC_SCROLL ){
		xpstyle->ClassList = "SCROLLBAR";
        xpstyle->Parts = SBP_ARROWBTN;
		xpstyle->State = ABS_UPNORMAL;
		if( DFCS_state & DFCS_HOT )
			xpstyle->State = ABS_UPHOT;
		else if( DFCS_state & DFCS_PUSHED )
			xpstyle->State = ABS_UPPRESSED;
		else if( DFCS_state & DFCS_INACTIVE )
			xpstyle->State = ABS_UPDISABLED;
        if( DFCS_state & DFCS_SCROLLDOWN ){
            xpstyle->Parts = SBP_ARROWBTN;
			xpstyle->State = ABS_DOWNNORMAL;
			if( DFCS_state & DFCS_HOT )
				xpstyle->State = ABS_DOWNHOT;
			else if( DFCS_state & DFCS_PUSHED )
				xpstyle->State = ABS_DOWNPRESSED;
			else if( DFCS_state & DFCS_INACTIVE )
				xpstyle->State = ABS_DOWNDISABLED;
			return TRUE;
			}
		else if( DFCS_state & DFCS_SCROLLLEFT ){
            xpstyle->Parts = SBP_ARROWBTN;
			xpstyle->State = ABS_LEFTNORMAL;
			if( DFCS_state & DFCS_HOT )
				xpstyle->State = ABS_LEFTHOT;
			else if( DFCS_state & DFCS_PUSHED )
				xpstyle->State = ABS_LEFTPRESSED;
			else if( DFCS_state & DFCS_INACTIVE )
				xpstyle->State = ABS_LEFTDISABLED;
			return TRUE;
			}
		else if( DFCS_state & DFCS_SCROLLRIGHT ){
            xpstyle->Parts = SBP_ARROWBTN;
			xpstyle->State = ABS_RIGHTNORMAL;
			if( DFCS_state & DFCS_HOT )
				xpstyle->State = ABS_RIGHTHOT;
			else if( DFCS_state & DFCS_PUSHED )
				xpstyle->State = ABS_RIGHTPRESSED;
			else if( DFCS_state & DFCS_INACTIVE )
				xpstyle->State = ABS_RIGHTDISABLED;
			return TRUE;
			}
		return TRUE;
		}
	xpstyle->ClassList = "";
    xpstyle->Parts = 0;
	xpstyle->State = 0;
	return FALSE;
}

#endif // __CONSOLE__