#include "window_manager.h"

#include "ukive/log.h"


namespace ukive {

    std::unique_ptr<WindowManager> WindowManager::instance_;

    WindowManager* WindowManager::getInstance() {
        if (instance_ == nullptr) {
            instance_.reset(new WindowManager());
        }

        return instance_.get();
    }

    void WindowManager::addWindow(WindowImpl *window) {
        if (window_list_.size() == 0
            && !window->isStartupWindow()) {
            window->setStartupWindow(true);
        }
        else if (window_list_.size() > 0
            && window->isStartupWindow()) {
            for (auto it = window_list_.begin();
                it != window_list_.end(); ++it) {
                if (it->get()->isStartupWindow()) {
                    it->get()->setStartupWindow(false);
                }
            }
        }

        std::shared_ptr<WindowImpl> window_ptr(window);
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

    void WindowManager::removeWindow(WindowImpl *window) {
        for (auto it = window_list_.begin();
            it != window_list_.end(); ++it) {
            if (it->get() == window) {
                window_list_.erase(it);
                return;
            }
        }
    }

}