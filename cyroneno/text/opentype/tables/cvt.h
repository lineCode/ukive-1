#ifndef CYRONENO_TEXT_OPENTYPE_TABLES_CVT_H_
#define CYRONENO_TEXT_OPENTYPE_TABLES_CVT_H_

#include <cstdint>
#include <fstream>
#include <vector>


namespace cyro {
namespace otf {

    /****** cvt
     * https://docs.microsoft.com/zh-cn/typography/opentype/spec/cvt
     */
    class CVT {
    public:
        struct CVTTable {
            std::vector<int16_t> cvt;
        };

        static bool parseTable(std::ifstream& file, uint32_t length, CVTTable* out_cvt);
    };

}
}

#endif  // CYRONENO_TEXT_OPENTYPE_TABLES_CVT_H_