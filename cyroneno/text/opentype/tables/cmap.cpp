#include "cmap.h"

#include "utils/stl_utils.h"

#include "../opentype_common.h"


namespace cyro {
namespace otf {

    bool CMAP::parseTable(std::ifstream& file, uint32_t c, uint16_t* glyph_idx) {
        *glyph_idx = 0;
        auto start_offset = utl::STLCU32(file.tellg());

        CMAPHeader header;

        READ_FONT_FILE_TABLE_SWAP(header.version, 2);
        READ_FONT_FILE_TABLE_SWAP(header.table_num, 2);

        for (uint16_t i = 0; i < header.table_num; ++i) {
            EncodingRecord record;
            READ_FONT_FILE_TABLE_SWAP(record.platform_id, 2);
            READ_FONT_FILE_TABLE_SWAP(record.encoding_id, 2);
            READ_FONT_FILE_TABLE_SWAP(record.offset, 4);
            header.records.push_back(record);
        }

        for (uint16_t i = 0; i < header.table_num; ++i) {
            if (header.records[i].platform_id == PID_Windows &&
                header.records[i].encoding_id == WEID_UnicodeFull)
            {
                if (!parseRecord(file, header.records[i], start_offset, c, glyph_idx)) {
                    return false;
                }
                return true;
            }
        }

        return true;
    }

    bool CMAP::parseRecord(
        std::ifstream& file, const EncodingRecord& record, uint32_t tab_start,
        uint32_t c, uint16_t* glyph_idx)
    {
        int64_t cur_offset = file.tellg();
        file.seekg(tab_start + record.offset, std::ios::beg);

        uint16_t format = 0;
        READ_FONT_FILE_TABLE_SWAP(format, 2);
        file.seekg(tab_start + record.offset, std::ios::beg);

        switch (format) {
        case 4:
            if (c > 0xFFFF) {
                *glyph_idx = 0;
                break;
            }
            if (!parseFormat4(file, c, glyph_idx)) {
                return false;
            }
            break;

        case 12:
            if (!parseFormat12(file, c, glyph_idx)) {
                return false;
            }
            break;

        default:
            break;
        }

        file.seekg(cur_offset, std::ios::beg);
        return true;
    }

    bool CMAP::parseFormat4(std::ifstream& file, uint16_t c, uint16_t* glyph_idx) {
        Format4 sub_tab;

        READ_FONT_FILE_TABLE_SWAP(sub_tab.format, 2);
        READ_FONT_FILE_TABLE_SWAP(sub_tab.length, 2);
        READ_FONT_FILE_TABLE_SWAP(sub_tab.lang, 2);
        READ_FONT_FILE_TABLE_SWAP(sub_tab.seg_count_x2, 2);
        READ_FONT_FILE_TABLE_SWAP(sub_tab.search_range, 2);
        READ_FONT_FILE_TABLE_SWAP(sub_tab.entry_selector, 2);
        READ_FONT_FILE_TABLE_SWAP(sub_tab.range_shift, 2);

        auto seg_count = sub_tab.seg_count_x2 / 2;
        if (seg_count > 0) {
            sub_tab.end_code.reset(new uint16_t[seg_count], std::default_delete<uint16_t[]>());
            READ_FONT_FILE_TABLE(sub_tab.end_code[0], seg_count * 2);
            for (uint16_t i = 0; i < seg_count; ++i) {
                auto tmp = utl::swapBits(sub_tab.end_code[i]);
                sub_tab.end_code[i] = tmp;
            }
        }

        READ_FONT_FILE_TABLE(sub_tab.reserved_pad, 2);

        if (seg_count > 0) {
            sub_tab.start_code.reset(new uint16_t[seg_count], std::default_delete<uint16_t[]>());
            READ_FONT_FILE_TABLE(sub_tab.start_code[0], seg_count * 2);
            for (uint16_t i = 0; i < seg_count; ++i) {
                auto tmp = utl::swapBits(sub_tab.start_code[i]);
                sub_tab.start_code[i] = tmp;
            }
        }

        if (seg_count > 0) {
            sub_tab.id_delta.reset(new int16_t[seg_count], std::default_delete<int16_t[]>());
            READ_FONT_FILE_TABLE(sub_tab.id_delta[0], seg_count * 2);
            for (uint16_t i = 0; i < seg_count; ++i) {
                auto tmp = utl::swapBits(sub_tab.id_delta[i]);
                sub_tab.id_delta[i] = tmp;
            }
        }

        uint64_t iro_off = file.tellg();
        if (seg_count > 0) {
            sub_tab.id_range_offset.reset(new uint16_t[seg_count], std::default_delete<uint16_t[]>());
            READ_FONT_FILE_TABLE(sub_tab.id_range_offset[0], seg_count * 2);
            for (uint16_t i = 0; i < seg_count; ++i) {
                auto tmp = utl::swapBits(sub_tab.id_range_offset[i]);
                sub_tab.id_range_offset[i] = tmp;
            }
        }

        // ËÑË÷ endCode
        uint16_t i;
        bool hit = false;
        for (i = 0; i < seg_count; ++i) {
            if (sub_tab.end_code[i] == 0xFFFF) {
                break;
            }
            if (sub_tab.end_code[i] >= c && sub_tab.start_code[i] <= c) {
                hit = true;
                break;
            }
        }

        if (!hit) {
            *glyph_idx = 0;
            return true;
        }

        auto cur_iro = sub_tab.id_range_offset[i];
        if (cur_iro == 0) {
            *glyph_idx = c + sub_tab.id_delta[i];
        } else {
            uint16_t glyph_id = 0;
            auto idx = (cur_iro + 2 * (c - sub_tab.start_code[i]) + (iro_off + i * 2));
            file.seekg(idx, std::ios::beg);
            READ_FONT_FILE_TABLE_SWAP(glyph_id, 2);
            if (glyph_id == 0) {
                *glyph_idx = 0;
            } else {
                *glyph_idx = glyph_id + sub_tab.id_delta[i];
            }
        }

        return true;
    }

    bool CMAP::parseFormat12(std::ifstream& file, uint32_t c, uint16_t* glyph_idx) {
        *glyph_idx = 0;

        Format12 sub_tab;

        READ_FONT_FILE_TABLE_SWAP(sub_tab.format, 2);
        READ_FONT_FILE_TABLE(sub_tab.reserved, 2);
        READ_FONT_FILE_TABLE_SWAP(sub_tab.length, 4);
        READ_FONT_FILE_TABLE_SWAP(sub_tab.lang, 4);
        READ_FONT_FILE_TABLE_SWAP(sub_tab.group_num, 4);

        for (uint32_t i = 0; i < sub_tab.group_num; ++i) {
            SeqMapGroup group;
            READ_FONT_FILE_TABLE_SWAP(group.start_char_code, 4);
            READ_FONT_FILE_TABLE_SWAP(group.end_char_code, 4);
            READ_FONT_FILE_TABLE_SWAP(group.start_glyph_id, 4);
            sub_tab.groups.push_back(group);
        }

        for (uint32_t i = 0; i < sub_tab.group_num; ++i) {
            auto& group = sub_tab.groups[i];
            if (group.start_char_code <= c && group.end_char_code >= c) {
                *glyph_idx = group.start_glyph_id + (c - group.start_char_code);
                break;
            }
        }

        return true;
    }

}
}
