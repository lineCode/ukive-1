#include "window_manager.h"

#include "utils/log.h"
#include "utils/stl_utils.h"

#include "ukive/window/window.h"


namespace ukive {

    std::unique_ptr<WindowManager> WindowManager::instance_;

    WindowManager* WindowManager::getInstance() {
        if (instance_ == nullptr) {
            instance_.reset(new WindowManager());
        }

        return instance_.get();
    }

    void WindowManager::addWindow(Window *window) {
        if (window_list_.size() == 0
            && !window->isStartupWindow()) {
            window->setStartupWindow(true);
        }
        else if (window_list_.size() > 0
            && window->isStartupWindow()) {
            for (auto it = window_list_.begin();
                it != window_list_.end(); ++it) {
                if ((*it)->isStartupWindow()) {
                    (*it)->setStartupWindow(false);
                }
            }
        }

        window_list_.push_back(window);
    }

    int WindowManager::getWindowCount() {
        return utl::STLCInt(window_list_.size());
    }

    Window* WindowManager::getWindow(int index) {
        auto i = utl::STLCST(window_list_, index);
        if (i >= window_list_.size()) {
            LOG(Log::ERR) << "Out of bounds.";
            return nullptr;
        }
        return window_list_.at(i);
    }

    void WindowManager::removeWindow(Window *window) {
        for (auto it = window_list_.begin();
            it != window_list_.end(); ++it) {
            if (*it == window) {
                window_list_.erase(it);
                return;
            }
        }
    }

}
