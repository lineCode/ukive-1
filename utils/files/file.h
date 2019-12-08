#ifndef UTILS_FILES_FILE_H_
#define UTILS_FILES_FILE_H_

#include <functional>
#include <vector>

#include "utils/string_utils.h"


namespace utl {

    class File {
    public:
        enum Error {
            None,
            PathNotFound,
            FileNotFound,
            InvalidName,
            InvalidPath,
            AccessDenied,
            Unknown,
        };

        using Filter = std::function<
            bool(const string16& name, bool is_dir)>;

        File();
        explicit File(const string16& path);
        File(const string16& dir, const string16& append);

        std::vector<File> listFiles(const Filter& filter);

        File& append(const string16& append);
        File& appendExt(const string16& append);
        bool mkDir();
        bool mkDirs(bool only_parent = false);

        string16 getPath() const;
        string16 getName() const;
        string16 getExt() const;
        string16 getNameWithoutExt() const;
        string16 getParentPath() const;

        File getParent() const;

        Error getError() const;

        bool isDir();
        bool isExist();
        bool isFailed() const;

        bool isUNCPath() const;
        bool isDiskAbsolutePath() const;
        bool isDiskRelativePath() const;
        bool isSlashPath() const;
        bool isDevicePath() const;

        bool isAbsolutePath() const;

    private:
        void initFile();
        void determineError(uint32_t error_code);
        bool mkDirByPath(const string16& path);
        bool isDot(const string16& path, string16::size_type off) const;
        bool isQues(const string16& path, string16::size_type off) const;
        bool isSeparator(const string16& path, string16::size_type off) const;
        bool isDiskDesignator(const string16& path, string16::size_type off) const;
        bool hasLongNamePrefix() const;

        string16 prefix_;
        string16 path_;
        string16 name_;
        string16 ext_;
        string16 parent_path_;
        bool has_device_prefix_;
        bool has_long_name_prefix_;
        Error cur_error_;
    };

}

#endif  // UTILS_FILES_FILE_H_