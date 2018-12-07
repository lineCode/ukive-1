#include "clipboard_manager.h"

#include <Windows.h>


namespace ukive {

    ClipboardManager::ClipboardManager() {
    }

    ClipboardManager::~ClipboardManager() {
    }


    void ClipboardManager::saveToClipboard(string16 text) {
        if (::OpenClipboard(nullptr)) {
            ::EmptyClipboard();

            std::size_t space = (text.length() + 1) * sizeof(wchar_t);

            HANDLE hHandle = ::GlobalAlloc(GMEM_FIXED, space);
            wchar_t* pData = reinterpret_cast<wchar_t*>(::GlobalLock(hHandle));

            text._Copy_s(pData, text.length(), text.length());
            pData[text.length()] = L'\0';

            ::SetClipboardData(CF_UNICODETEXT, hHandle);
            ::GlobalUnlock(hHandle);
            ::CloseClipboard();
        }
    }

    string16 ClipboardManager::getFromClipboard() {
        string16 content = L"";

        if (::OpenClipboard(nullptr)) {
            HGLOBAL hMem = ::GetClipboardData(CF_UNICODETEXT);
            if (hMem != nullptr) {
                wchar_t* lpStr = reinterpret_cast<wchar_t*>(::GlobalLock(hMem));
                if (lpStr != nullptr) {
                    content = string16(lpStr);
                    ::GlobalUnlock(hMem);
                }
            }

            ::CloseClipboard();
        }

        return content;
    }

}