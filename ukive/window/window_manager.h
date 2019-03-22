#ifndef UKIVE_WINDOW_WINDOW_MANAGER_H_
#define UKIVE_WINDOW_WINDOW_MANAGER_H_

#include <memory>
#include <vector>


namespace ukive {

    class Window;

    class WindowManager {
    public:
        static WindowManager* getInstance();

        void addWindow(Window *window);
        int getWindowCount();
        Window* getWindow(int index);
        void removeWindow(Window *window);

    private:
        WindowManager() {}

        static std::unique_ptr<WindowManager> instance_;

        std::vector<Window*> window_list_;
    };

}

#endif  // UKIVE_WINDOW_WINDOW_MANAGER_H_
