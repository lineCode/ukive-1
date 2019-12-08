#ifndef UKIVE_GRAPHICS_EFFECTS_SHADOW_EFFECT_H_
#define UKIVE_GRAPHICS_EFFECTS_SHADOW_EFFECT_H_

#include "ukive/graphics/effects/cyro_effect.h"


namespace ukive {

    class ShadowEffect : public CyroEffect {
    public:
        virtual void setRadius(int radius) = 0;
        virtual int getRadius() const = 0;

        static ShadowEffect* createShadowEffect();
    };

}

#endif  // UKIVE_GRAPHICS_EFFECTS_SHADOW_EFFECT_H_