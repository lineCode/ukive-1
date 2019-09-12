#ifndef CYRONENO_TEXT_OPENTYPE_OPENTYPE_FONT_H_
#define CYRONENO_TEXT_OPENTYPE_OPENTYPE_FONT_H_

#include <fstream>
#include <map>

#include "ukive/utils/string_utils.h"

#include "tables/cmap.h"
#include "tables/cvt.h"
#include "tables/glyf.h"
#include "tables/head.h"
#include "tables/maxp.h"


// 根据 OpenType 文档写成的 OpenType 字体解析器
// https://docs.microsoft.com/zh-cn/typography/opentype/spec/

namespace cyro {
namespace otf {

    class OpenTypeFont {
    public:
        struct OffsetTable {
            uint32_t sfnt_ver;
            uint16_t table_num;
            uint16_t search_range;
            uint16_t entry_selector;
            uint16_t range_shift;
        };

        struct TableRecordEntry {
            uint8_t tag[4];
            uint32_t check_sum;
            uint32_t offset;
            uint32_t length;
        };

        struct TTCHeader {
            // Version 1.0
            uint8_t tag[4];
            uint16_t major_ver;
            uint16_t minor_ver;
            uint32_t font_num;
            std::shared_ptr<uint32_t[]> offset_tables;

            // Version 2.0
            uint32_t dsig_tag;
            uint32_t dsig_length;
            uint32_t dsig_offset;
        };

        struct GlyphInfo {
            bool empty = true;
            GLYF::GlyphHeader glyph_header;
            GLYF::SimpleGlyphTable glyph_table;
        };

        OpenTypeFont();

        bool parse(const string16& file_name);
        bool parseGlyph(uint32_t index, GlyphInfo* glyph);
        bool parseAllGlyph();
        bool parseGlyphIdForChar(uint16_t c, uint16_t* glyph_idx);

        const HEAD::HeadTable& GetHead() const { return head_; }
        const MAXP::MaximumProfile& GetProfile() const { return profile_; }
        const std::vector<GlyphInfo>& GetGlyphs() const { return glyphs_; }

    private:
        bool parseOneFont();
        bool parseFontCollection();

        uint32_t calcTableChecksum(uint32_t table, uint32_t length);

        std::ifstream font_file_;
        OffsetTable ot_;
        std::map<string8, TableRecordEntry> tr_;
        HEAD::HeadTable head_;
        MAXP::MaximumProfile profile_;
        CVT::CVTTable cvt_;
        std::vector<GlyphInfo> glyphs_;
    };

}
}

#endif  // CYRONENO_TEXT_OPENTYPE_OPENTYPE_FONT_H_