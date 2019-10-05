#include "opentype_rasterizer.h"

#include <assert.h>

#include "shell/cyroneno/vector.h"


namespace cyro {
namespace otf {

    OpenTypeRasterizer::OpenTypeRasterizer(int image_width, int image_height)
        : rasterizer_(image_width, image_height) {
    }

    void OpenTypeRasterizer::drawGlyph(
        const OpenTypeFont::GlyphInfo& glyph,
        int off_x, int off_y, float scale, const Color& c)
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
            Point2 bezier_start, bezier_mid;
            bool prev_has_beizer = false;
            uint16_t cur_end_pt = glyph.glyph_table.contour_end_pts[i];
            for (bool first = true; j <= cur_end_pt; ++j) {
                Point2 start(int(cur_x * scale), int(cur_y * scale));

                cur_x += glyph.glyph_table.x_coord[j];
                cur_y += glyph.glyph_table.y_coord[j];
                Point2 end(int(cur_x * scale), int(cur_y * scale));

                if (!first) {
                    if (glyph.glyph_table.flags[j] & GLYF::ON_CURVE_POINT) {
                        if (prev_has_beizer) {
                            rasterizer_.drawQuadBezier(bezier_start, bezier_mid, end, c);
                        } else {
                            rasterizer_.drawLine(start, end, c);
                        }

                        if (j == cur_end_pt) {
                            rasterizer_.drawLine(end, cur_start, c);
                        }
                        prev_has_beizer = false;
                    } else {
                        if (prev_has_beizer) {
                            auto tmp = bezier_mid + ((end - bezier_mid) / 2.0);
                            tmp.x_ = int(tmp.x_);
                            tmp.y_ = int(tmp.y_);
                            rasterizer_.drawQuadBezier(bezier_start, bezier_mid, tmp, c);
                            bezier_start = tmp;
                        } else {
                            prev_has_beizer = true;
                            bezier_start = start;
                        }
                        bezier_mid = end;

                        if (j == cur_end_pt) {
                            rasterizer_.drawQuadBezier(bezier_start, bezier_mid, cur_start, c);
                        }
                    }
                } else {
                    assert(glyph.glyph_table.flags[j] & GLYF::ON_CURVE_POINT);
                    cur_start = end;
                    first = false;
                }
            }
        }
    }

    const ImagePng* OpenTypeRasterizer::getOutput() const {
        return rasterizer_.getOutput();
    }

}
}
