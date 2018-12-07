#ifndef UKIVE_DRAWABLE_LAYER_DRAWABLE_H_
#define UKIVE_DRAWABLE_LAYER_DRAWABLE_H_

#include <memory>
#include <vector>

#include "ukive/drawable/drawable.h"


namespace ukive {

    class Canvas;

    class LayerDrawable : public Drawable {
    public:
        LayerDrawable();
        ~LayerDrawable();

        void addDrawable(Drawable* drawable);
        void removeDrawable(Drawable* drawable);

        virtual void draw(Canvas* canvas);

        virtual float getOpacity();

    protected:
        void onBoundChanged(const Rect& new_bound) override;
        bool onStateChanged(int new_state, int prev_state) override;

        std::vector<std::shared_ptr<Drawable>> drawable_list_;
    };

}

#endif  // UKIVE_DRAWABLE_LAYER_DRAWABLE_H_