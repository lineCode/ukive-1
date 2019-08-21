#ifndef UKIVE_UTILS_DYNAMIC_WINDOWS_API_H_
#define UKIVE_UTILS_DYNAMIC_WINDOWS_API_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ShellScalingAPI.h>


namespace ukive {

    // Windows 8.1 or later
    HRESULT STDAPICALLTYPE UDGetDpiForMonitor(
        HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY);

    // Windows 10 1607 or later
    UINT WINAPI UDGetDpiForWindow(HWND hwnd);

    // Windows 8 or later
    BOOL WINAPI UDSetWindowFeedbackSetting(
        HWND hwnd, FEEDBACK_TYPE feedback, DWORD flags, UINT32 size, const VOID* configuration);

}

#endif  // UKIVE_UTILS_DYNAMIC_WINDOWS_API_H_