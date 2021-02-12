#include "Windows.h"

#ifndef WIN_UTIL_
#define WIN_UTIL_
    void ClipOrCenterRectToMonitor(LPRECT prc, UINT flags);

    void ClipOrCenterWindowToMonitor(HWND hwnd, UINT flags);
#endif