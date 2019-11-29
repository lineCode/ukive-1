#include "glyf.h"

#include "../opentype_common.h"


namespace cyro {
namespace otf {

    bool GLYF::parseTable(std::ifstream& file, GlyphHeader* out_header, SimpleGlyphTable* out_glyph) {
        GlyphHeader header;

        READ_FONT_FILE_TABLE_SWAP(header.contour_num, 2);
        READ_FONT_FILE_TABLE_SWAP(header.min_x, 2);
        READ_FONT_FILE_TABLE_SWAP(header.min_y, 2);
        READ_FONT_FILE_TABLE_SWAP(header.max_x, 2);
        READ_FONT_FILE_TABLE_SWAP(header.max_y, 2);

        SimpleGlyphTable table;
        if (header.contour_num >= 0) {
            // Simple glyph
            if (header.contour_num > 0) {
                table.contour_end_pts.reset(new uint16_t[header.contour_num], std::default_delete<uint16_t[]>());
                READ_FONT_FILE_TABLE(table.contour_end_pts[0], header.contour_num * 2);
                for (int16_t i = 0; i < header.contour_num; ++i) {
                    auto tmp = table.contour_end_pts[i];
                    table.contour_end_pts[i] = utl::swapBits(tmp);
                }
            }

            READ_FONT_FILE_TABLE_SWAP(table.inst_length, 2);
            if (table.inst_length > 0) {
                table.insts.reset(new uint8_t[table.inst_length], std::default_delete<uint8_t[]>());
                READ_FONT_FILE_TABLE(table.insts[0], table.inst_length);
            }

            if (header.contour_num > 0) {
                uint16_t logic_flag_count = table.contour_end_pts[header.contour_num - 1] + 1;
                // flags
                table.flags.reset(new uint8_t[logic_flag_count], std::default_delete<uint8_t[]>());
                for (uint16_t i = 0; i < logic_flag_count; ++i) {
                    uint8_t flag;
                    READ_FONT_FILE_TABLE(flag, 1);
                    table.flags[i] = flag;
                    if (flag & REPEAT_FLAG) {
                        READ_FONT_FILE_TABLE(flag, 1);
                        std::memset(&table.flags[i + 1], table.flags[i], flag);
                        i += flag;
                    }
                }

                // x_coord
                table.x_coord.reset(new int16_t[logic_flag_count], std::default_delete<int16_t[]>());
                for (uint16_t i = 0; i < logic_flag_count; ++i) {
                    uint8_t flag = table.flags[i];
                    if (flag & X_SHORT_VECTOR) {
                        // 1 字节
                        uint8_t coord;
                        READ_FONT_FILE_TABLE(coord, 1);
                        bool positive = (flag & X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR);
                        table.x_coord[i] = positive ? coord : -coord;
                    } else {
                        // 2 字节
                        if (flag & X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR) {
                            table.x_coord[i] = 0;
                        } else {
                            READ_FONT_FILE_TABLE_SWAP(table.x_coord[i], 2);
                        }
                    }
                }

                // y_coord
                table.y_coord.reset(new int16_t[logic_flag_count], std::default_delete<int16_t[]>());
                for (uint16_t i = 0; i < logic_flag_count; ++i) {
                    uint8_t flag = table.flags[i];
                    if (flag & Y_SHORT_VECTOR) {
                        // 1 字节
                        uint8_t coord;
                        READ_FONT_FILE_TABLE(coord, 1);
                        bool positive = (flag & Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR);
                        table.y_coord[i] = positive ? coord : -coord;
                    } else {
                        // 2 字节
                        if (flag & Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR) {
                            table.y_coord[i] = 0;
                        } else {
                            READ_FONT_FILE_TABLE_SWAP(table.y_coord[i], 2);
                        }
                    }
                }
            }
        } else {
            // Composite glyph
            //return false;
        }

        *out_header = header;
        *out_glyph = std::move(table);
        return true;
    }

}
}