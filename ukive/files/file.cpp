#include "ukive/files/file.h"

#include <Windows.h>

#include "ukive/log.h"


namespace {
    const wchar_t kExtSeparator[] = L".";
    const wchar_t kPathSeparator[] = L"\\";

    bool isDirectory(const WIN32_FIND_DATAW& data) {
        return (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
    }
}

namespace ukive {

    File::File() {
    }

    File::File(const string16& path) {
        path_ = path;
        initFile();
    }

    File::File(const string16& dir, const string16& append) {
        path_ = dir + kPathSeparator + append;
        initFile();
    }


    void File::initFile() {
        if (path_.empty()) {
            return;
        }

        auto i = path_.find_last_of(kPathSeparator);
        if (i != string16::npos) {
            parent_path_ = path_.substr(0, i);
            name_ = path_.substr(i + 1, path_.length() - i - 1);
        }

        if (!name_.empty()) {
            i = name_.find_last_of(kExtSeparator);
            if (i != string16::npos) {
                ext_ = name_.substr(i + 1, name_.length() - i - 1);
            }
        }
    }

    std::vector<File> File::listFiles(const Filter& filter) {
        if (!isDir()) {
            return {};
        }

        WIN32_FIND_DATAW data;
        string16 first_file = path_ + kPathSeparator + L"*";
        auto handle = ::FindFirstFileW(first_file.c_str(), &data);
        if (handle == INVALID_HANDLE_VALUE) {
            return {};
        }

        std::vector<File> files;
        if (filter(data.cFileName, isDirectory(data))) {
            files.push_back(File(path_ + kPathSeparator + data.cFileName));
        }

        for (;;) {
            BOOL result = ::FindNextFileW(handle, &data);
            if (result != FALSE) {
                if (filter(data.cFileName, isDirectory(data))) {
                    files.push_back(File(path_ + kPathSeparator + data.cFileName));
                }
            } else {
                auto last_error = ::GetLastError();
                if (last_error == ERROR_NO_MORE_FILES) {
                    break;
                } else {
                    // TODO:
                    break;
                }
            }
        }

        return files;
    }

    string16 File::getPath() const {
        return path_;
    }

    string16 File::getName() const {
        return name_;
    }

    string16 File::getExt() const {
        return ext_;
    }

    string16 File::getParentPath() const {
        return parent_path_;
    }

    File File::getParent() const {
        return File(parent_path_);
    }

    bool File::isDir() const {
        auto attrs = ::GetFileAttributesW(path_.c_str());
        if (attrs == INVALID_FILE_ATTRIBUTES) {
            // TODO:
            return false;
        }

        return (attrs & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
    }

    bool File::isExist() const {
        WIN32_FIND_DATA data;
        auto handle = ::FindFirstFileW(path_.c_str(), &data);
        if (handle == INVALID_HANDLE_VALUE) {
            if (GetLastError() == ERROR_FILE_NOT_FOUND) {
                return false;
            }
            // TODO:
        } else {
            BOOL result = ::CloseHandle(handle);
            DCHECK(result == TRUE);
            return true;
        }

        return false;
    }

}