#ifndef UKIVE_WINDOW_WINDOW_LISTENER_H_
#define UKIVE_WINDOW_WINDOW_LISTENER_H_

#include "ukive/utils/string_utils.h"


namespace ukive {

    class OnWindowStatusChangedListener {
    public:
        virtual ~OnWindowStatusChangedListener() = default;

        virtual void onWindowTextChanged(const string16& text) {}
        virtual void onWindowIconChanged() {}
        virtual void onWindowStatusChanged() {}
    };

}

#endif  // UKIVE_WINDOW_WINDOW_LISTENER_H_