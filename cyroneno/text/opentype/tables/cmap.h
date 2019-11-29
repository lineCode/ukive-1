#ifndef CYRONENO_TEXT_OPENTYPE_TABLES_CMAP_H_
#define CYRONENO_TEXT_OPENTYPE_TABLES_CMAP_H_

#include <cstdint>
#include <fstream>
#include <vector>


namespace cyro {
namespace otf {

    /****** cmap
     * https://docs.microsoft.com/zh-cn/typography/opentype/spec/cmap
     */
    class CMAP {
    public:
        enum PlatformId {
            PID_Unicode = 0,
            PID_Macintosh = 1,
            PID_Windows = 3,
            PID_Custom = 4,
        };

        enum WindowsEncodingId {
            WEID_Symbol = 0,
            WEID_UnicodeBMP = 1,
            WEID_ShiftJIS = 2,
            WEID_PRC = 3,
            WEID_Big5 = 4,
            WEID_Wansung = 5,
            WEID_Johab = 6,
            WEID_Reserved0 = 7,
            WEID_Reserved1 = 8,
            WEID_Reserved2 = 9,
            WEID_UnicodeFull = 10,
        };

        struct EncodingRecord {
            uint16_t platform_id;
            uint16_t encoding_id;
            uint32_t offset;
        };

        struct CMAPHeader {
            uint16_t version;
            uint16_t table_num;
            std::vector<EncodingRecord> records;
        };

        struct Format0 {
            uint16_t format;
            uint16_t length;
            uint16_t lang;
            uint8_t glyph_ids[256];
        };

        struct F2SubHeader {
            uint16_t first_code;
            uint16_t entry_count;
            int16_t id_delta;
            uint16_t id_range_offset;
        };

        struct Format2 {
            uint16_t format;
            uint16_t length;
            uint16_t lang;
            uint16_t sub_header_keys[256];
            std::shared_ptr<F2SubHeader[]> sub_headers;
            std::shared_ptr<uint16_t[]> glyph_indices;
        };

        struct Format4 {
            uint16_t format;
            uint16_t length;
            uint16_t lang;
            uint16_t seg_count_x2;
            uint16_t search_range;
            uint16_t entry_selector;
            uint16_t range_shift;
            std::shared_ptr<uint16_t[]> end_code;
            uint16_t reserved_pad;
            std::shared_ptr<uint16_t[]> start_code;
            std::shared_ptr<int16_t[]> id_delta;
            std::shared_ptr<uint16_t[]> id_range_offset;
            std::shared_ptr<uint16_t[]> glyph_ids;
        };

        struct Format6 {
            uint16_t format;
            uint16_t length;
            uint16_t lang;
            uint16_t first_code;
            uint16_t entry_count;
            std::shared_ptr<uint16_t[]> glyph_ids;
        };

        struct SeqMapGroup {
            uint32_t start_char_code;
            uint32_t end_char_code;
            uint32_t start_glyph_id;
        };

        struct Format8 {
            uint16_t format;
            uint16_t reserved;
            uint32_t length;
            uint32_t lang;
            uint8_t is32[8192];
            uint32_t group_num;
            std::shared_ptr<SeqMapGroup[]> groups;
        };

        struct Format10 {
            uint16_t format;
            uint16_t reserved;
            uint32_t length;
            uint32_t lang;
            uint32_t start_char_code;
            uint32_t char_num;
            std::shared_ptr<uint16_t[]> glyph_ids;
        };

        struct Format12 {
            uint16_t format;
            uint16_t reserved;
            uint32_t length;
            uint32_t lang;
            uint32_t group_num;
            std::vector<SeqMapGroup> groups;
        };

        struct ConstMapGroup {
            uint32_t start_char_code;
            uint32_t end_char_code;
            uint32_t glyph_id;
        };

        struct Format13 {
            uint16_t format;
            uint16_t reserved;
            uint32_t length;
            uint32_t lang;
            uint32_t group_num;
            std::shared_ptr<ConstMapGroup[]> groups;
        };

        struct VarSelector {
            // 24 bit
            uint32_t var_selector;
            uint32_t def_UVS_offset;
            uint32_t nondef_UVS_offset;
        };

        struct Format14 {
            uint16_t format;
            uint32_t length;
            uint32_t var_sel_record_num;
            std::shared_ptr<VarSelector[]> var_sels;
        };

        struct UnicodeRange {
            // 24 bit
            uint32_t start_unicode_val;
            uint8_t additional_count;
        };

        struct DefaultUVS {
            uint32_t unicode_val_range_num;
            std::shared_ptr<UnicodeRange[]> ranges;
        };

        struct UVSMapping {
            // 24 bit
            uint32_t unicode_val;
            uint16_t glyph_id;
        };

        struct NonDefaultUVS {
            uint32_t UVS_mapping_num;
            std::shared_ptr<UVSMapping[]> mappings;
        };

        static bool parseTable(std::ifstream& file, uint32_t c, uint16_t* glyph_idx);

    private:
        static bool parseRecord(
            std::ifstream& file, const EncodingRecord& record, uint32_t tab_start,
            uint32_t c, uint16_t* glyph_idx);
        static bool parseFormat4(std::ifstream& file, uint16_t c, uint16_t* glyph_idx);
        static bool parseFormat12(std::ifstream& file, uint32_t c, uint16_t* glyph_idx);
    };

}
}

#endif  // CYRONENO_TEXT_OPENTYPE_TABLES_CMAP_H_