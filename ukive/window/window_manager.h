#ifndef UKIVE_WINDOW_WINDOW_MANAGER_H_
#define UKIVE_WINDOW_WINDOW_MANAGER_H_

#include <memory>
#include <vector>

#include "window/window_impl.h"


namespace ukive {

    class WindowManager {
    public:
        static WindowManager* getInstance();

        void addWindow(WindowImpl *window);
        size_t getWindowCount();
        WindowImpl* getWindow(size_t index);

    private:
        static std::unique_ptr<WindowManager> instance_;

        std::vector<std::unique_ptr<WindowImpl>> window_list_;
    };

}

#endif  // UKIVE_WINDOW_WINDOW_MANAGER_H_
