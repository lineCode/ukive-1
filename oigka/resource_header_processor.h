#ifndef OIGKA_RESOURCE_HEADER_PROCESSOR_H_
#define OIGKA_RESOURCE_HEADER_PROCESSOR_H_

#include <map>

#include "utils/string_utils.h"


namespace oigka {

    class ResourceHeaderProcessor {
    public:
        using IdMap = std::map<string8, int>;

        enum class Indent {
            Tab,
            Space,
        };

        enum class LineBreak {
            CR,
            LF,
            CRLF,
        };

        ResourceHeaderProcessor();

        void setIndent(Indent indent, uint32_t count);
        void setLineBreak(LineBreak lb);
        bool write(const string16& path, const IdMap& view_id_map, const IdMap& layout_id_map);

    private:
        void generateOutput(
            const string8& name_macro,
            const IdMap& view_id_map, const IdMap& layout_id_map, string8* out);
        string8 getIndent(uint32_t layer) const;
        string8 getLineBreak(uint32_t count = 1) const;

        Indent indent_;
        uint32_t indent_count_;
        LineBreak line_break_;
    };

}

#endif  // OIGKA_RESOURCE_HEADER_PROCESSOR_H_