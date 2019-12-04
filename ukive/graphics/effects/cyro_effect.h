#ifndef UKIVE_GRAPHICS_EFFECTS_CYRO_EFFECT_H_
#define UKIVE_GRAPHICS_EFFECTS_CYRO_EFFECT_H_

#include <memory>


namespace ukive {

    class Bitmap;
    class Canvas;
    class OffscreenBuffer;

    class CyroEffect {
    public:
        virtual ~CyroEffect() = default;

        virtual void draw() = 0;
        virtual void draw(Canvas* c) = 0;
        virtual void setContent(OffscreenBuffer* content) = 0;

        virtual std::shared_ptr<Bitmap> getOutput(Canvas* c) = 0;
    };

}

#endif  // UKIVE_GRAPHICS_EFFECTS_CYRO_EFFECT_H_