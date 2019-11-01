#include "head.h"

#include "../opentype_common.h"


namespace cyro {
namespace otf {

    bool HEAD::parseTable(std::ifstream& file, HeadTable* out) {
        HeadTable head;

        READ_FONT_FILE_TABLE_SWAP(head.major_ver, 2);
        READ_FONT_FILE_TABLE_SWAP(head.minor_ver, 2);
        READ_FONT_FILE_TABLE_SWAP(head.font_rev, 4);
        READ_FONT_FILE_TABLE_SWAP(head.checksum_adj, 4);
        READ_FONT_FILE_TABLE_SWAP(head.magic, 4);

        if (head.magic != kMagicNumber) {
            return false;
        }

        READ_FONT_FILE_TABLE_SWAP(head.flags, 2);
        READ_FONT_FILE_TABLE_SWAP(head.unit_per_em, 2);
        READ_FONT_FILE_TABLE_SWAP(head.created, 8);
        READ_FONT_FILE_TABLE_SWAP(head.modified, 8);
        READ_FONT_FILE_TABLE_SWAP(head.min_x, 2);
        READ_FONT_FILE_TABLE_SWAP(head.min_y, 2);
        READ_FONT_FILE_TABLE_SWAP(head.max_x, 2);
        READ_FONT_FILE_TABLE_SWAP(head.max_y, 2);
        READ_FONT_FILE_TABLE_SWAP(head.mac_style, 2);
        READ_FONT_FILE_TABLE_SWAP(head.lowest_rec_PPEM, 2);
        READ_FONT_FILE_TABLE_SWAP(head.font_dir_hint, 2);
        READ_FONT_FILE_TABLE_SWAP(head.idx_to_loc_format, 2);
        READ_FONT_FILE_TABLE_SWAP(head.glyph_data_format, 2);

        *out = head;
        return true;
    }

}
}