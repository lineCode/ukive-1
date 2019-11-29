#ifndef CYRONENO_TEXT_OPENTYPE_TABLES_LOCA_H_
#define CYRONENO_TEXT_OPENTYPE_TABLES_LOCA_H_

#include <vector>

#include "head.h"
#include "maxp.h"


namespace cyro {
namespace otf {

    /****** loca
     * https://docs.microsoft.com/zh-cn/typography/opentype/spec/loca
     */
    class LOCA {
    public:
        static bool parseTable(
            std::ifstream& file,
            const HEAD::HeadTable& head, const MAXP::MaximumProfile& maxp,
            std::vector<uint32_t>* offsets);
    };

}
}

#endif  // CYRONENO_TEXT_OPENTYPE_TABLES_LOCA_H_