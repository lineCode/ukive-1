#include "opentype_font.h"

#include "opentype_common.h"
#include "tables/loca.h"



namespace {

    // 'OTTO'
    const uint32_t kSfntVerCFFData = 0x4F54544F;

    // TrueType
    const uint32_t kSfntVerTrueType = 0x00010000;

    // Apple Spec TrueType 'true'
    const uint32_t kSfntVerAppleTT1 = 0x74727565;

    // Apple Spec TrueType 'typ1'
    const uint32_t kSfntVerAppleTT2 = 0x74797031;

    // 'ttcf'
    const uint32_t kTTCTag = 0x74746366;

}

namespace cyro {
namespace otf {

    OpenTypeFont::OpenTypeFont()
        : ot_(), head_(), profile_() {
    }

    bool OpenTypeFont::parse(const string16& file_name) {
        font_file_.open(file_name, std::ios::in | std::ios::binary);
        if (!font_file_) {
            return false;
        }

        uint32_t sfnt_ver;

        // 先看下是 TTC Header 还是 Offset Table
        READ_FONT_FILE_SWAP(sfnt_ver, 4);
        font_file_.seekg(0, std::ios::beg);

        if (sfnt_ver == kTTCTag) {
            // 解析 TTC
            return parseFontCollection();
        }
        if (sfnt_ver == kSfntVerCFFData ||
            sfnt_ver == kSfntVerTrueType ||
            sfnt_ver == kSfntVerAppleTT1 ||
            sfnt_ver == kSfntVerAppleTT2)
        {
            // 解析 Offset Table
            return parseOneFont();
        }

        return false;
    }

    bool OpenTypeFont::parseOneFont() {
        // Offset Table
        READ_FONT_FILE_SWAP(ot_.sfnt_ver, 4);
        READ_FONT_FILE_SWAP(ot_.table_num, 2);
        READ_FONT_FILE_SWAP(ot_.search_range, 2);
        READ_FONT_FILE_SWAP(ot_.entry_selector, 2);
        READ_FONT_FILE_SWAP(ot_.range_shift, 2);

        for (uint16_t i = 0; i < ot_.table_num; ++i) {
            TableRecordEntry tre;
            READ_FONT_FILE(tre.tag[0], 4);
            READ_FONT_FILE_SWAP(tre.check_sum, 4);
            READ_FONT_FILE_SWAP(tre.offset, 4);
            READ_FONT_FILE_SWAP(tre.length, 4);

            string8 tag_str(reinterpret_cast<char*>(tre.tag), 4);
            tr_[tag_str] = tre;
        }

        {
            auto it = tr_.find("head");
            if (it != tr_.end()) {
                font_file_.seekg(it->second.offset, std::ios::beg);
                if (calcTableChecksum(0, it->second.length) != it->second.check_sum) {
                    return false;
                }
                font_file_.seekg(it->second.offset, std::ios::beg);
                if (!HEAD::parseTable(font_file_, &head_)) {
                    return false;
                }
            } else {
                return false;
            }
        }

        {
            auto it = tr_.find("maxp");
            if (it != tr_.end()) {
                font_file_.seekg(it->second.offset, std::ios::beg);
                if (calcTableChecksum(0, it->second.length) != it->second.check_sum) {
                    return false;
                }
                font_file_.seekg(it->second.offset, std::ios::beg);
                if (!MAXP::parseTable(font_file_, &profile_)) {
                    return false;
                }
            } else {
                return false;
            }
        }

        {
            auto it = tr_.find("cvt ");
            if (it != tr_.end()) {
                font_file_.seekg(it->second.offset, std::ios::beg);
                if (calcTableChecksum(0, it->second.length) != it->second.check_sum) {
                    return false;
                }
                font_file_.seekg(it->second.offset, std::ios::beg);
                if (!CVT::parseTable(font_file_, it->second.length, &cvt_)) {
                    return false;
                }
            }
        }

        return true;
    }

    bool OpenTypeFont::parseGlyphIdForChar(uint16_t c, uint16_t* glyph_idx) {
        auto it = tr_.find("cmap");
        if (it != tr_.end()) {
            font_file_.seekg(it->second.offset, std::ios::beg);
            if (calcTableChecksum(0, it->second.length) != it->second.check_sum) {
                return false;
            }
            font_file_.seekg(it->second.offset, std::ios::beg);
            if (!CMAP::parseTable(font_file_, c, glyph_idx)) {
                return false;
            }
        } else {
            return false;
        }

        return true;
    }

    bool OpenTypeFont::parseGlyph(uint32_t index, GlyphInfo* glyph) {
        if (ot_.sfnt_ver != kSfntVerTrueType &&
            ot_.sfnt_ver != kSfntVerAppleTT1 &&
            ot_.sfnt_ver != kSfntVerAppleTT2)
        {
            return false;
        }

        if (!glyphs_.empty()) {
            if (glyphs_.size() > index) {
                *glyph = glyphs_[index];
                return true;
            }
            return false;
        }

        std::vector<uint32_t> loca;
        {
            auto it = tr_.find("loca");
            if (it != tr_.end()) {
                font_file_.seekg(it->second.offset, std::ios::beg);
                if (calcTableChecksum(0, it->second.length) != it->second.check_sum) {
                    return false;
                }
                font_file_.seekg(it->second.offset, std::ios::beg);
                if (!LOCA::parseTable(font_file_, head_, profile_, &loca)) {
                    return false;
                }
            } else {
                return false;
            }
        }

        if (loca.size() <= index + 1) {
            return false;
        }

        {
            auto it = tr_.find("glyf");
            if (it != tr_.end()) {
                /*font_file_.seekg(it->second.offset, std::ios::beg);
                if (calcTableChecksum(0, it->second.length) != it->second.check_sum) {
                    return false;
                }*/
                font_file_.seekg(it->second.offset + loca[index], std::ios::beg);
                uint32_t prev_offset = it->second.offset + loca[index];

                GlyphInfo glyph_info;
                {
                    auto cur_len = loca[index + 1] - loca[index];
                    if (cur_len == 0) {
                        glyph_info.empty = true;
                        *glyph = std::move(glyph_info);
                        return true;
                    }
                    if (!GLYF::parseTable(font_file_, &glyph_info.glyph_header, &glyph_info.glyph_table)) {
                        return false;
                    }
                    int64_t cur = font_file_.tellg();
                    if (cur - prev_offset > cur_len) {
                        return false;
                    }

                    glyph_info.empty = false;
                    *glyph = std::move(glyph_info);
                }
            }
        }
        return true;
    }

    bool OpenTypeFont::parseAllGlyph() {
        if (ot_.sfnt_ver == kSfntVerTrueType ||
            ot_.sfnt_ver == kSfntVerAppleTT1 ||
            ot_.sfnt_ver == kSfntVerAppleTT2)
        {
            std::vector<uint32_t> loca;
            {
                auto it = tr_.find("loca");
                if (it != tr_.end()) {
                    font_file_.seekg(it->second.offset, std::ios::beg);
                    if (calcTableChecksum(0, it->second.length) != it->second.check_sum) {
                        return false;
                    }
                    font_file_.seekg(it->second.offset, std::ios::beg);
                    if (!LOCA::parseTable(font_file_, head_, profile_, &loca)) {
                        return false;
                    }
                } else {
                    return false;
                }
            }

            glyphs_.clear();
            {
                auto it = tr_.find("glyf");
                if (it != tr_.end()) {
                    font_file_.seekg(it->second.offset, std::ios::beg);
                    if (calcTableChecksum(0, it->second.length) != it->second.check_sum) {
                        return false;
                    }
                    font_file_.seekg(it->second.offset, std::ios::beg);

                    GlyphInfo glyph_info;
                    uint32_t prev_offset = it->second.offset;
                    for (uint16_t i = 0; i < profile_.glyph_num; ++i) {
                        auto cur_len = loca[i + 1] - loca[i];
                        if (cur_len == 0) {
                            glyph_info.empty = true;
                            glyphs_.push_back(std::move(glyph_info));
                            continue;
                        }
                        if (!GLYF::parseTable(font_file_, &glyph_info.glyph_header, &glyph_info.glyph_table)) {
                            return false;
                        }
                        int64_t cur = font_file_.tellg();
                        if (cur - prev_offset < cur_len) {
                            font_file_.seekg(prev_offset + cur_len, std::ios::beg);
                            prev_offset += cur_len;
                        } else if (cur - prev_offset == cur_len) {
                            prev_offset = cur;
                        } else {
                            return false;
                        }

                        glyph_info.empty = false;
                        glyphs_.push_back(std::move(glyph_info));
                    }
                }
            }
        }
        return true;
    }

    bool OpenTypeFont::parseFontCollection() {
        TTCHeader header;

        READ_FONT_FILE(header.tag[0], 4);
        READ_FONT_FILE_SWAP(header.major_ver, 2);
        READ_FONT_FILE_SWAP(header.minor_ver, 2);
        READ_FONT_FILE_SWAP(header.font_num, 4);

        if (header.font_num > 0) {
            header.offset_tables.reset(new uint32_t[header.font_num], std::default_delete<uint32_t[]>());
            READ_FONT_FILE(header.offset_tables[0], header.font_num * 4);
            for (uint32_t i = 0; i < header.font_num; ++i) {
                auto tmp = header.offset_tables[i];
                header.offset_tables[i] = ukive::swapBits(tmp);
            }
        }

        if (header.major_ver == 2) {
            READ_FONT_FILE_SWAP(header.dsig_tag, 4);
            READ_FONT_FILE_SWAP(header.dsig_length, 4);
            READ_FONT_FILE_SWAP(header.dsig_offset, 4);
        }

        for (uint32_t i = 0; i < header.font_num; ++i) {
            auto offset = header.offset_tables[i];
            font_file_.seekg(offset, std::ios::beg);
            if (!parseOneFont()) {
                return false;
            }
            break;
        }

        return true;
    }

    uint32_t OpenTypeFont::calcTableChecksum(uint32_t table, uint32_t length) {
        uint32_t sum = 0;
        uint32_t end = (length + 3) / 4;
        while (end-- > 0) {
            uint32_t tmp = 0;
            READ_FONT_FILE_SWAP(tmp, 4);
            sum += tmp;
        }
        return sum;
    }

}
}