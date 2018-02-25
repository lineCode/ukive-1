#include "drawable.h"


namespace ukive {

    Drawable::Drawable()
        :start_x_(0.f),
        start_y_(0.f),
        is_parent_has_focus_(false),
        state_(STATE_NONE),
        prev_state_(STATE_NONE) {}


    void Drawable::setBounds(const Rect &rect) {
        if (bounds_ == rect) {
            return;
        }

        bounds_ = rect;
        onBoundChanged(bounds_);
    }

    void Drawable::setBounds(int left, int top, int width, int height) {
        if (bounds_.left == left
            && bounds_.top == top
            && bounds_.right == left + width
            && bounds_.bottom == top + height) {
            return;
        }

        bounds_.left = left;
        bounds_.top = top;
        bounds_.right = left + width;
        bounds_.bottom = top + height;

        onBoundChanged(bounds_);
    }

    bool Drawable::setState(int state) {
        if (state == state_) {
            return false;
        }

        prev_state_ = state_;
        state_ = state;

        return onStateChanged(state, prev_state_);
    }

    void Drawable::setHotspot(int x, int y) {
        start_x_ = x;
        start_y_ = y;
    }

    bool Drawable::setParentFocus(bool focus) {
        if (focus == is_parent_has_focus_) {
            return false;
        }

        is_parent_has_focus_ = focus;
        return onFocusChanged(is_parent_has_focus_);
    }


    int Drawable::getState() const {
        return state_;
    }

    int Drawable::getPrevState() const {
        return prev_state_;
    }

    Rect Drawable::getBounds() const {
        return bounds_;
    }

    float Drawable::getOpacity() const {
        return 1.f;
    }

    bool Drawable::onFocusChanged(bool focus) {
        return false;
    }

    void Drawable::onBoundChanged(const Rect& new_bound) {}

    bool Drawable::onStateChanged(int new_state, int prev_state) {
        return false;
    }

    int Drawable::getIncWidth() const {
        return -1;
    }

    int Drawable::getIncHeight() const {
        return -1;
    }

}