#ifndef UKIVE_FILES_FILE_H_
#define UKIVE_FILES_FILE_H_

#include <functional>
#include <vector>

#include "ukive/utils/string_utils.h"


namespace ukive {

    class File {
    public:
        using Filter = std::function<
            bool(const string16& name, bool is_dir)>;

        File();
        explicit File(const string16& path);
        File(const string16& dir, const string16& append);

        std::vector<File> listFiles(const Filter& filter);

        string16 getPath() const;
        string16 getName() const;
        string16 getExt() const;
        string16 getParentPath() const;

        File getParent() const;

        bool isDir() const;
        bool isExist() const;

    private:
        void initFile();

        string16 path_;
        string16 name_;
        string16 ext_;
        string16 parent_path_;
    };

}

#endif  // UKIVE_FILES_FILE_H_