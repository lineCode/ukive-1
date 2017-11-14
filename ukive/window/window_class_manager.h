#ifndef UKIVE_WINDOW_WINDOW_CLASS_MANAGER_H_
#define UKIVE_WINDOW_WINDOW_CLASS_MANAGER_H_

#include <Windows.h>

#include <memory>
#include <vector>

#include "ukive/utils/string_utils.h"


namespace ukive {

    struct ClassInfo {
        int style;
        HICON icon;
        HICON icon_small;
        HCURSOR cursor;

        bool operator==(const ClassInfo &rhs) {
            return (style == rhs.style
                && icon == rhs.icon
                && icon_small == rhs.icon_small);
        }
    };

    class WindowClassManager {
    public:
        struct RegisteredClass {
            ClassInfo info;
            ATOM atom;
        };

        static WindowClassManager* getInstance();

        ATOM retrieveWindowClass(const ClassInfo &info);

    private:
        WindowClassManager()
            :class_counter_(0) {}

        ATOM createWindowClass(const string16 &className, const ClassInfo &info);

        static std::unique_ptr<WindowClassManager> instance_;
        static const string16 kWindowClassName;

        size_t class_counter_;
        std::vector<RegisteredClass> class_list_;
    };

}

#endif  // UKIVE_WINDOW_WINDOW_CLASS_MANAGER_H_