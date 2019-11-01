#ifndef CYRONENO_TEXT_OPENTYPE_OPENTYPE_RASTERIZER_H_
#define CYRONENO_TEXT_OPENTYPE_OPENTYPE_RASTERIZER_H_

#include "../../pipeline/rasterizer.h"
#include "opentype_font.h"

/**
 * ²Î¿¼: https://developer.apple.com/fonts/TrueType-Reference-Manual/RM02/Chap2.html#distinguishing
 */

namespace cyro {
namespace otf {

    class OpenTypeRasterizer {
    public:
        OpenTypeRasterizer(int image_width, int image_height);

        void drawGlyph(const OpenTypeFont::GlyphInfo& glyph, int off_x, int off_y, float scale);

        ImagePng getOutput() const;

    private:
        Rasterizer rasterizer_;
    };

}
}

#endif  // CYRONENO_TEXT_OPENTYPE_OPENTYPE_RASTERIZER_H_