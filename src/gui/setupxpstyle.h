#ifndef __SETUP_XP_STYLE_H_
#define __SETUP_XP_STYLE_H_

#include "../config.h"

#ifndef __CONSOLE__

typedef struct{
	int Parts;
	int State;
	CString ClassList;
	} XPStyle;

extern BOOL decodeStyle( XPStyle *xpstyle, UINT DFC_type, UINT DFCS_state, UINT CBS_style );
extern BOOL xpDrawFrameControl( HDC hdc, LPRECT lprc, UINT DFC_type, UINT DFCS_state );

#endif // __CONSOLE__
#endif // __SETUP_XP_STYLE_H_