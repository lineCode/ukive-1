#include "layer_drawable.h"


namespace ukive {

    LayerDrawable::LayerDrawable()
        :Drawable() {}

    LayerDrawable::~LayerDrawable() {}


    void LayerDrawable::addDrawable(Drawable* drawable) {
        mDrawableList.push_back(std::shared_ptr<Drawable>(drawable));
    }

    void LayerDrawable::removeDrawable(Drawable* drawable) {
        for (auto it = mDrawableList.begin();
            it != mDrawableList.end(); ++it) {
            if ((*it).get() == drawable) {
                mDrawableList.erase(it);
                return;
            }
        }
    }


    void LayerDrawable::onBoundChanged(const Rect& new_bound) {
        for (auto drawable : mDrawableList) {
            drawable->setBounds(new_bound);
        }
    }


    void LayerDrawable::draw(Canvas* canvas) {
        for (auto drawable : mDrawableList) {
            drawable->draw(canvas);
        }
    }

    float LayerDrawable::getOpacity() {
        if (mDrawableList.empty()) {
            return 0.f;
        } else {
            return 1.f;
        }
    }

}