#include "opentype_rasterizer.h"


namespace cyro {
namespace otf {

    OpenTypeRasterizer::OpenTypeRasterizer(int image_width, int image_height)
        : rasterizer_(image_width, image_height) {
    }

    void OpenTypeRasterizer::drawGlyph(
        const OpenTypeFont::GlyphInfo& glyph, int off_x, int off_y, float scale)
    {
        if (glyph.empty) {
            return;
        }

        auto contour_num = glyph.glyph_header.contour_num;
        if (contour_num <= 0) {
            return;
        }

        int cur_x = off_x;
        int cur_y = off_y;
        uint16_t j = 0;
        for (int16_t i = 0; i < contour_num; ++i) {
            // »æÖÆÃ¿¸öÂÖÀª
            Point2 cur_start;
            uint16_t cur_end_pt = glyph.glyph_table.contour_end_pts[i];
            for (bool first = true; j <= cur_end_pt; ++j) {
                Point2 start(cur_x * scale, cur_y * scale);

                cur_x += glyph.glyph_table.x_coord[j];
                cur_y += glyph.glyph_table.y_coord[j];
                Point2 end(cur_x * scale, cur_y * scale);

                if (!first) {
                    rasterizer_.drawLine(start, end, Color(0, 0, 0, 1));
                    if (j == cur_end_pt) {
                        rasterizer_.drawLine(end, cur_start, Color(0, 0, 0, 1));
                    }
                } else {
                    cur_start = end;
                    first = false;
                }
            }
        }
    }

    ImagePng OpenTypeRasterizer::getOutput() const {
        return rasterizer_.getOutput();
    }

}
}
