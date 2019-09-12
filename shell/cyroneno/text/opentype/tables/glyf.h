#ifndef CYRONENO_TEXT_OPENTYPE_TABLES_GLYF_H_
#define CYRONENO_TEXT_OPENTYPE_TABLES_GLYF_H_

#include <cstdint>
#include <fstream>


namespace cyro {
namespace otf {

    /****** glyf
     * https://docs.microsoft.com/zh-cn/typography/opentype/spec/glyf
     * https://developer.apple.com/fonts/TrueType-Reference-Manual/RM01/Chap1.html#necessary
     */
    class GLYF {
    public:
        enum SimpleGlyphFlags {
            ON_CURVE_POINT = 0x01,
            X_SHORT_VECTOR = 0x02,
            Y_SHORT_VECTOR = 0x04,
            REPEAT_FLAG = 0x08,
            X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR = 0x10,
            Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR = 0x20,
            OVERLAP_SIMPLE = 0x40,
            SGF_Reserved = 0x80,
        };

        enum CompositeGlyphFlags {
            ARG_1_AND_2_ARE_WORDS = 0x0001,
            ARGS_ARE_XY_VALUES = 0x0002,
            ROUND_XY_TO_GRID = 0x0004,
            WE_HAVE_A_SCALE = 0x0008,
            MORE_COMPONENTS = 0x0020,
            WE_HAVE_AN_X_AND_Y_SCALE = 0x0040,
            WE_HAVE_A_TWO_BY_TWO = 0x0080,
            WE_HAVE_INSTRUCTIONS = 0x0100,
            USE_MY_METRICS = 0x0200,
            OVERLAP_COMPOUND = 0x0400,
            SCALED_COMPONENT_OFFSET = 0x0800,
            UNSCALED_COMPONENT_OFFSET = 0x1000,
            CGF_Reserved = 0xE010,
        };

        struct GlyphHeader {
            int16_t contour_num;
            int16_t min_x;
            int16_t min_y;
            int16_t max_x;
            int16_t max_y;
        };

        struct SimpleGlyphTable {
            std::shared_ptr<uint16_t[]> contour_end_pts;
            uint16_t inst_length;
            std::shared_ptr<uint8_t[]> insts;
            std::shared_ptr<uint8_t[]> flags;
            std::shared_ptr<int16_t[]> x_coord;
            std::shared_ptr<int16_t[]> y_coord;
        };

        struct CompositeGlyphTable {
            uint16_t flags;
            uint16_t glyph_idx;
            uint16_t arg1;
            uint16_t arg2;
        };

        static bool parseTable(
            std::ifstream& file, GlyphHeader* out_header, SimpleGlyphTable* out_glyph);
    };

}
}

#endif  // CYRONENO_TEXT_OPENTYPE_TABLES_GLYF_H_