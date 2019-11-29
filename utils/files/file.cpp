#include "utils/files/file.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "utils/log.h"


namespace {
    const wchar_t kDotMark[] = L".";
    const wchar_t kQuesMark[] = L"?";
    const wchar_t kUNCPrefix[] = L"UNC";
    const wchar_t kExtSeparator[] = L".";
    const wchar_t kPathSeparator[] = L"\\";
    const wchar_t kDiskDesignator[] = L":";
    const wchar_t kPathSeparatorFinder[] = L"/\\";

    bool isDirectory(const WIN32_FIND_DATAW& data) {
        return (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
    }
}

namespace utl {

    File::File()
        : has_device_prefix_(false),
          has_long_name_prefix_(false),
          cur_error_(None) {}

    File::File(const string16& path)
        : path_(path),
          has_device_prefix_(false),
          has_long_name_prefix_(false),
          cur_error_(None)
    {
        initFile();
    }

    File::File(const string16& dir, const string16& append)
        : has_device_prefix_(false),
          has_long_name_prefix_(false),
          cur_error_(None)
    {
        if (isSeparator(dir, dir.length() - 1)) {
            path_ = dir + append;
        } else {
            path_ = dir + kPathSeparator + append;
        }

        initFile();
    }

    void File::initFile() {
        if (path_.empty()) {
            return;
        }

        has_device_prefix_ = isDevicePath();
        has_long_name_prefix_ = hasLongNamePrefix();
        if (has_device_prefix_ || has_long_name_prefix_) {
            prefix_ = path_.substr(0, 4);
            path_.erase(0, 4);
        }

        if (isUNCPath()) {
            if (path_.length() == 2) {
                return;
            }
        } else if (isSlashPath()) {
            if (path_.length() == 1) {
                return;
            }
        } else if (isDiskRelativePath()) {
            if (path_.length() == 2) {
                return;
            }
        } else if (isDiskAbsolutePath()) {
            if (path_.length() == 3) {
                return;
            }
        }

        if (isSeparator(path_, path_.length() - 1)) {
            path_.pop_back();
        }

        string16::size_type adv = 0;
        if (isUNCPath()) {
            if (path_.length() == 2) {
                ++adv;
            }
        } else if (isDiskAbsolutePath()) {
            if (path_.length() == 3) {
                ++adv;
            }
        }

        auto i = path_.find_last_of(kPathSeparatorFinder);
        if (i != string16::npos) {
            parent_path_ = path_.substr(0, i + adv);
            name_ = path_.substr(i + 1, path_.length() - i - 1);
        } else {
            name_ = path_;
        }

        if (!name_.empty()) {
            i = name_.find_last_of(kExtSeparator);
            if (i != string16::npos) {
                ext_ = name_.substr(i + 1, name_.length() - i - 1);
            }
        }
    }

    std::vector<File> File::listFiles(const Filter& filter) {
        cur_error_ = None;

        if (!isDir()) {
            return {};
        }

        WIN32_FIND_DATAW data;
        string16 first_file = path_ + kPathSeparator + L"*";
        auto handle = ::FindFirstFileW(first_file.c_str(), &data);
        if (handle == INVALID_HANDLE_VALUE) {
            auto error_code = ::GetLastError();
            determineError(error_code);
            return {};
        }

        std::vector<File> files;
        if (!filter || filter(data.cFileName, isDirectory(data))) {
            files.push_back(File(path_ + kPathSeparator + data.cFileName));
        }

        for (;;) {
            BOOL result = ::FindNextFileW(handle, &data);
            if (result != FALSE) {
                if (!filter || filter(data.cFileName, isDirectory(data))) {
                    files.push_back(File(path_ + kPathSeparator + data.cFileName));
                }
            } else {
                auto error_code = ::GetLastError();
                if (error_code == ERROR_NO_MORE_FILES) {
                    break;
                }

                determineError(error_code);
                return {};
            }
        }

        return files;
    }

    File& File::append(const string16& append) {
        if (isSeparator(path_, path_.length() - 1)) {
            path_ += append;
        } else {
            path_ += kPathSeparator + append;
        }
        initFile();
        return *this;
    }

    File& File::appendExt(const string16& append) {
        path_ += kExtSeparator + append;
        initFile();
        return *this;
    }

    bool File::mkDir() {
        return mkDirByPath(path_);
    }

    bool File::mkDirs(bool only_parent) {
        cur_error_ = InvalidPath;

        string16 cur_path;
        auto dirs = splitString(path_, kPathSeparatorFinder);
        if (isUNCPath()) {
            if (has_long_name_prefix_) {
                if (dirs.size() <= 3) {
                    return false;
                }
                for (int i = 0; i < 3; ++i) {
                    cur_path.append(dirs[i]).append(kPathSeparator);
                }
                dirs.erase(dirs.begin(), dirs.begin() + 3);
            } else {
                if (dirs.size() <= 4) {
                    return false;
                }
                for (int i = 0; i < 4; ++i) {
                    cur_path.append(dirs[i]).append(kPathSeparator);
                }
                dirs.erase(dirs.begin(), dirs.begin() + 4);
            }
        } else if (isDiskAbsolutePath()) {
            if (dirs.size() <= 1) {
                return false;
            }
            cur_path.append(dirs[0]).append(kPathSeparator);
            dirs.erase(dirs.begin());
        } else if (isDiskRelativePath()) {
            if (dirs.size() <= 1) {
                return false;
            }
        } else if (isSlashPath()) {
            if (dirs.size() <= 1) {
                return false;
            }
            cur_path.append(kPathSeparator);
            dirs.erase(dirs.begin());
        }

        if (only_parent) {
            if (!dirs.empty()) {
                dirs.pop_back();
            } else {
                cur_error_ = InvalidPath;
                return false;
            }
        }

        for (const auto& dir_name : dirs) {
            if (dir_name.empty()) {
                return false;
            }
            cur_path.append(dir_name).append(kPathSeparator);
            if (!mkDirByPath(cur_path)) {
                return false;
            }
        }

        cur_error_ = None;
        return true;
    }

    bool File::mkDirByPath(const string16& path) {
        cur_error_ = None;
        if (::CreateDirectoryW((prefix_ + path).c_str(), nullptr) == 0) {
            auto error_code = ::GetLastError();
            if (error_code == ERROR_ALREADY_EXISTS) {
                return true;
            }
            determineError(error_code);
            return false;
        }

        return true;
    }

    bool File::isDot(const string16& path, string16::size_type off) const {
        return path.substr(off, 1).find_first_of(kDotMark) != string16::npos;
    }

    bool File::isQues(const string16& path, string16::size_type off) const {
        return path.substr(off, 1).find_first_of(kQuesMark) != string16::npos;
    }

    bool File::isSeparator(const string16& path, string16::size_type off) const {
        return path.substr(off, 1).find_first_of(kPathSeparatorFinder) != string16::npos;
    }

    bool File::isDiskDesignator(const string16& path, string16::size_type off) const {
        return path.substr(off, 1).find_first_of(kDiskDesignator) != string16::npos;
    }

    bool File::hasLongNamePrefix() const {
        if (path_.size() >= 4) {
            if (isSeparator(path_, 0) &&
                isSeparator(path_, 1) &&
                isQues(path_, 2) &&
                isSeparator(path_, 3))
            {
                return true;
            }
        }
        return false;
    }

    string16 File::getPath() const {
        return prefix_ + path_;
    }

    string16 File::getName() const {
        return name_;
    }

    string16 File::getExt() const {
        return ext_;
    }

    string16 File::getNameWithoutExt() const {
        if (!name_.empty()) {
            auto i = name_.find_last_of(kExtSeparator);
            if (i != string16::npos) {
                return name_.substr(0, i);
            }
        }
        return name_;
    }

    string16 File::getParentPath() const {
        return prefix_ + parent_path_;
    }

    File File::getParent() const {
        return File(prefix_ + parent_path_);
    }

    File::Error File::getError() const {
        return cur_error_;
    }

    bool File::isDir() {
        cur_error_ = None;

        auto attrs = ::GetFileAttributesW(getPath().c_str());
        if (attrs == INVALID_FILE_ATTRIBUTES) {
            auto error_code = ::GetLastError();
            determineError(error_code);
            return false;
        }
        return (attrs & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
    }

    bool File::isExist() {
        cur_error_ = None;

        WIN32_FIND_DATA data;
        auto handle = ::FindFirstFileW(getPath().c_str(), &data);
        if (handle == INVALID_HANDLE_VALUE) {
            auto error_code = GetLastError();
            if (error_code == ERROR_FILE_NOT_FOUND) {
                return false;
            }
            determineError(error_code);
            return false;
        }

        BOOL result = ::CloseHandle(handle);
        DCHECK(result == TRUE);
        return true;
    }

    bool File::isFailed() const {
        return cur_error_ != None;
    }

    bool File::isUNCPath() const {
        if (has_device_prefix_) {
            return false;
        }
        if (has_long_name_prefix_) {
            if (startWith(path_, kUNCPrefix, 0, false) && isSeparator(path_, 3)) {
                return true;
            }
        } else if (path_.size() >= 2) {
            if (isSeparator(path_, 0) && isSeparator(path_, 1)) {
                return true;
            }
        }
        return false;
    }

    bool File::isDiskAbsolutePath() const {
        return (path_.size() >= 3 &&
            !isSeparator(path_, 0) &&
            isDiskDesignator(path_, 1) &&
            isSeparator(path_, 2));
    }

    bool File::isDiskRelativePath() const {
        if (path_.size() == 2) {
            return (!isSeparator(path_, 0) && isDiskDesignator(path_, 1));
        }
        if (path_.size() > 2) {
            return (!isSeparator(path_, 0) &&
                isDiskDesignator(path_, 1) &&
                !isSeparator(path_, 2));
        }
        return false;
    }

    bool File::isSlashPath() const {
        if (path_.size() == 1) {
            if (isSeparator(path_, 0)) {
                return true;
            }
        } else if (path_.size() >= 2) {
            if (isSeparator(path_, 0) && !isSeparator(path_, 1)) {
                return true;
            }
        }
        return false;
    }

    bool File::isDevicePath() const {
        if (path_.size() >= 4) {
            if (isSeparator(path_, 0) &&
                isSeparator(path_, 1) &&
                isDot(path_, 2) &&
                isSeparator(path_, 3))
            {
                return true;
            }
        }
        return false;
    }

    bool File::isAbsolutePath() const {
        return isDiskAbsolutePath() || isUNCPath() || isSlashPath();
    }

    void File::determineError(uint32_t error_code) {
        switch (error_code) {
        case ERROR_FILE_NOT_FOUND:
            cur_error_ = FileNotFound;
            break;
        case ERROR_PATH_NOT_FOUND:
            cur_error_ = PathNotFound;
            break;
        case ERROR_ACCESS_DENIED:
            cur_error_ = AccessDenied;
            break;
        case ERROR_INVALID_NAME:
            cur_error_ = InvalidName;
            break;
        default:
            cur_error_ = Unknown;
            break;
        }
    }

}