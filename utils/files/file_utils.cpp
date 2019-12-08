#include "utils/files/file_utils.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "file.h"


namespace utl {

    string16 getExecFileName(bool dir) {
        WCHAR buffer[MAX_PATH];
        DWORD result = ::GetModuleFileNameW(nullptr, buffer, MAX_PATH);
        if (result == 0) {
            return {};
        }

        DWORD size = MAX_PATH;
        std::unique_ptr<WCHAR[]> heap_buff;
        while (::GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            size *= 2;
            heap_buff.reset(new WCHAR[size]);
            result = ::GetModuleFileNameW(nullptr, heap_buff.get(), size);
            if (result == 0) {
                return {};
            }
        }

        string16 file_name;
        if (heap_buff) {
            file_name = heap_buff.get();
        } else {
            file_name = buffer;
        }

        if (dir) {
            file_name = File(file_name).getParentPath();
        }

        return file_name;
    }

}
