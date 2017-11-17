#ifndef _USER_MESSAGE_H_
#define _USER_MESSAGE_H_

#include "../config.h"

#ifndef __CONSOLE__

#include <winuser.h>

#define WM_UPDATE_MAIN_WND     (WM_USER + 1)
#define WM_STOP_MAIN_WND_TIMER (WM_USER + 2)
#define GUI_DEL_UNITS			(WM_USER + 3)


#endif // __CONSOLE__

#endif  // _USER_MESSAGE_H_
