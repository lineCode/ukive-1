#include "window/window_manager.h"

#include "log.h"


namespace ukive {

    std::unique_ptr<WindowManager> WindowManager::instance_;

    WindowManager* WindowManager::getInstance() {
        if (instance_ == nullptr) {
            instance_.reset(new WindowManager());
        }

        return instance_.get();
    }

    void WindowManager::addWindow(WindowImpl *window) {
        std::unique_ptr<WindowImpl> window_ptr;
        window_ptr.reset(window);

        window_list_.push_back(window_ptr);
    }

    size_t WindowManager::getWindowCount() {
        return window_list_.size();
    }

    WindowImpl* WindowManager::getWindow(size_t index) {
        if (index >= window_list_.size()) {
            Log::e(L"out of bound.");
            return nullptr;
        }
        return window_list_.at(index).get();
    }
}