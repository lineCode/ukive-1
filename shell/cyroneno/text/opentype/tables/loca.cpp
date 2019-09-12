#include "loca.h"

#include "../opentype_common.h"


namespace cyro {
namespace otf {

    bool LOCA::parseTable(
        std::ifstream& file,
        const HEAD::HeadTable& head, const MAXP::MaximumProfile& maxp,
        std::vector<uint32_t>* offsets)
    {
        if (head.idx_to_loc_format == 0) {
            for (uint16_t i = 0; i <= maxp.glyph_num; ++i) {
                uint16_t tmp;
                READ_FONT_FILE_TABLE_SWAP(tmp, 2);
                offsets->push_back(uint32_t(tmp) * 2);
            }
        } else {
            for (uint16_t i = 0; i <= maxp.glyph_num; ++i) {
                uint32_t tmp;
                READ_FONT_FILE_TABLE_SWAP(tmp, 4);
                offsets->push_back(tmp);
            }
        }
        return true;
    }

}
}