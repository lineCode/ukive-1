#include "ukive/utils/dynamic_windows_api.h"


namespace ukive {

    HRESULT STDAPICALLTYPE UDGetDpiForMonitor(
        HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY)
    {
        using GetDpiForMonitorPtr = HRESULT(STDAPICALLTYPE*)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);
        auto func = reinterpret_cast<GetDpiForMonitorPtr>(
            ::GetProcAddress(::LoadLibraryW(L"Shcore.dll"), "GetDpiForMonitor"));
        if (func) {
            return func(hmonitor, dpiType, dpiX, dpiY);
        }
        return ERROR_PROC_NOT_FOUND;
    }

    UINT WINAPI UDGetDpiForWindow(HWND hwnd) {
        using GetDpiForWindowPtr = UINT(WINAPI*)(HWND);
        auto func = reinterpret_cast<GetDpiForWindowPtr>(
            ::GetProcAddress(::LoadLibraryW(L"User32.dll"), "GetDpiForWindow"));
        if (func) {
            return func(hwnd);
        }
        return 0;
    }

    BOOL WINAPI UDSetWindowFeedbackSetting(
        HWND hwnd, FEEDBACK_TYPE feedback, DWORD flags, UINT32 size, const VOID* configuration)
    {
        using SetWindowFeedbackSettingPtr = BOOL(WINAPI*)(HWND, FEEDBACK_TYPE, DWORD, UINT32, const VOID*);
        auto func = reinterpret_cast<SetWindowFeedbackSettingPtr>(
            ::GetProcAddress(::LoadLibraryW(L"User32.dll"), "SetWindowFeedbackSetting"));
        if (func) {
            return func(hwnd, feedback, flags, size, configuration);
        }
        return 0;
    }

}