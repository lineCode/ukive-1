#include "cvt.h"

#include "../opentype_common.h"


namespace cyro {
namespace otf {

    bool CVT::parseTable(std::ifstream& file, uint32_t length, CVTTable* out_cvt) {
        auto n = length / 2;
        CVTTable table;
        for (uint32_t i = 0; i < n; ++i) {
            int16_t val;
            READ_FONT_FILE_TABLE_SWAP(val, 2);
            table.cvt.push_back(val);
        }
        *out_cvt = std::move(table);
        return true;
    }

}
}