#ifndef CYRONENO_TEXT_OPENTYPE_TABLES_MAXP_H_
#define CYRONENO_TEXT_OPENTYPE_TABLES_MAXP_H_

#include <cstdint>
#include <fstream>


namespace cyro {
namespace otf {

    // ***** maxp
    // https://docs.microsoft.com/zh-cn/typography/opentype/spec/maxp
    class MAXP {
    public:
        struct MaximumProfile {
            // Version 0.5
            uint32_t version;
            uint16_t glyph_num;

            // Version 1.0
            uint16_t max_pts;
            uint16_t max_contours;
            uint16_t max_composite_pts;
            uint16_t max_composite_contours;
            uint16_t max_zones;
            uint16_t max_twilight_pts;
            uint16_t max_storage;
            uint16_t max_func_defs;
            uint16_t max_inst_defs;
            uint16_t max_stack_elements;
            uint16_t max_inst_size;
            uint16_t max_component_elements;
            uint16_t max_component_depth;
        };

        static bool parseTable(std::ifstream& file, MaximumProfile* out);
    };

}
}

#endif  // CYRONENO_TEXT_OPENTYPE_TABLES_MAXP_H_