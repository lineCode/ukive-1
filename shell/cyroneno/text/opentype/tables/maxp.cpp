#include "maxp.h"

#include "../opentype_common.h"


namespace cyro {
namespace otf {

    bool MAXP::parseTable(std::ifstream& file, MaximumProfile* out) {
        MaximumProfile profile;

        READ_FONT_FILE_TABLE_SWAP(profile.version, 4);
        READ_FONT_FILE_TABLE_SWAP(profile.glyph_num, 2);

        uint16_t major_ver = profile.version >> 16;
        uint16_t minor_ver = profile.version & 0xFFFF;

        if (major_ver == 1) {
            READ_FONT_FILE_TABLE_SWAP(profile.max_pts, 2);
            READ_FONT_FILE_TABLE_SWAP(profile.max_contours, 2);
            READ_FONT_FILE_TABLE_SWAP(profile.max_composite_pts, 2);
            READ_FONT_FILE_TABLE_SWAP(profile.max_composite_contours, 2);
            READ_FONT_FILE_TABLE_SWAP(profile.max_zones, 2);
            READ_FONT_FILE_TABLE_SWAP(profile.max_twilight_pts, 2);
            READ_FONT_FILE_TABLE_SWAP(profile.max_storage, 2);
            READ_FONT_FILE_TABLE_SWAP(profile.max_func_defs, 2);
            READ_FONT_FILE_TABLE_SWAP(profile.max_inst_defs, 2);
            READ_FONT_FILE_TABLE_SWAP(profile.max_stack_elements, 2);
            READ_FONT_FILE_TABLE_SWAP(profile.max_inst_size, 2);
            READ_FONT_FILE_TABLE_SWAP(profile.max_component_elements, 2);
            READ_FONT_FILE_TABLE_SWAP(profile.max_component_depth, 2);
        } else if (major_ver > 1) {
            return false;
        }

        *out = profile;
        return true;
    }

}
}