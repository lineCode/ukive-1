#ifndef CYRONENO_TEXT_OPENTYPE_TABLES_HEAD_H_
#define CYRONENO_TEXT_OPENTYPE_TABLES_HEAD_H_

#include <cstdint>
#include <fstream>


namespace cyro {
namespace otf {

    // ***** head
    // https://docs.microsoft.com/zh-cn/typography/opentype/spec/head
    class HEAD {
    public:
        struct HeadTable {
            uint16_t major_ver;
            uint16_t minor_ver;
            uint32_t font_rev;
            uint32_t checksum_adj;
            uint32_t magic;
            uint16_t flags;
            uint16_t unit_per_em;
            int64_t created;
            int64_t modified;
            int16_t min_x;
            int16_t min_y;
            int16_t max_x;
            int16_t max_y;
            uint16_t mac_style;
            uint16_t lowest_rec_PPEM;
            int16_t font_dir_hint;
            int16_t idx_to_loc_format;
            int16_t glyph_data_format;
        };

        static bool parseTable(std::ifstream& file, HeadTable* out);

    private:
        static const uint32_t kMagicNumber = 0x5F0F3CF5;
    };

}
}

#endif  // CYRONENO_TEXT_OPENTYPE_TABLES_HEAD_H_