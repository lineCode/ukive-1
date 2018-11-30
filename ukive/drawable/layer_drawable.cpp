#include "layer_drawable.h"


namespace ukive {

    LayerDrawable::LayerDrawable()
        :Drawable() {}

    LayerDrawable::~LayerDrawable() {}


    void LayerDrawable::addDrawable(Drawable* drawable) {
        drawable_list_.push_back(std::shared_ptr<Drawable>(drawable));
    }

    void LayerDrawable::removeDrawable(Drawable* drawable) {
        for (auto it = drawable_list_.begin();
            it != drawable_list_.end();)
        {
            if ((*it).get() == drawable) {
                it = drawable_list_.erase(it);
            } else {
                ++it;
            }
        }
    }

    void LayerDrawable::onBoundChanged(const Rect& new_bound) {
        for (auto& drawable : drawable_list_) {
            drawable->setBounds(new_bound);
        }
    }

    bool LayerDrawable::onStateChanged(int new_state, int prev_state) {
        bool need_redraw = false;
        for (auto& drawable : drawable_list_) {
            need_redraw |= drawable->setState(new_state);
        }

        return need_redraw;
    }

    void LayerDrawable::draw(Canvas* canvas) {
        for (auto& drawable : drawable_list_) {
            drawable->draw(canvas);
        }
    }

    float LayerDrawable::getOpacity() {
        if (drawable_list_.empty()) {
            return 0.f;
        } else {
            return 1.f;
        }
    }

}