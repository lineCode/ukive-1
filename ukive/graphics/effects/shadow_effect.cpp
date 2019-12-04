#include "ukive/graphics/effects/shadow_effect.h"

#include "ukive/graphics/direct3d/effects/shadow_effect_dx.h"


namespace ukive {

    // static
    ShadowEffect* ShadowEffect::createShadowEffect() {
        return new ShadowEffectDX();
    }

}