#include "ukive/window/window_class_manager.h"

#include "ukive/application.h"
#include "ukive/log.h"
#include "ukive/window/window_impl.h"


namespace ukive {

    std::unique_ptr<WindowClassManager> WindowClassManager::instance_;
    const string16 WindowClassManager::kWindowClassName = L"Ukive_WindowClass_";

    WindowClassManager* WindowClassManager::getInstance() {
        if (instance_ == nullptr) {
            instance_.reset(new WindowClassManager());
        }

        return instance_.get();
    }

    ATOM WindowClassManager::retrieveWindowClass(const ClassInfo &info) {
        for (auto it = class_list_.begin(); it != class_list_.end(); ++it) {
            if (it->info == info) {
                return it->atom;
            }
        }

        string16 class_name = kWindowClassName
            + std::to_wstring(class_counter_);
        ATOM atom = createWindowClass(class_name, info);
        if (atom != 0) {
            RegisteredClass reg;
            reg.info = info;
            reg.atom = atom;
            class_list_.push_back(reg);

            ++class_counter_;
        }
        else {
            Log::e(L"Cannot create new window class: " + class_name);
        }

        return atom;
    }

    ATOM WindowClassManager::createWindowClass(const string16 &className, const ClassInfo &info) {
        WNDCLASSEX wcex;
        wcex.style = info.style;
        wcex.lpfnWndProc = WindowImpl::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = Application::getModuleHandle();
        wcex.hIcon = info.icon;
        wcex.hIconSm = info.icon_small;
        wcex.hCursor = info.cursor;
        wcex.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = className.c_str();
        wcex.cbSize = sizeof(WNDCLASSEXW);

        return ::RegisterClassEx(&wcex);
    }

}