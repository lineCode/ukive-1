#include "resource_header_processor.h"

#include <fstream>

#include "utils/log.h"
#include "utils/files/file.h"


namespace oigka {

    ResourceHeaderProcessor::ResourceHeaderProcessor()
        : indent_(Indent::Space),
          indent_count_(4),
          line_break_(LineBreak::CRLF) {}

    void ResourceHeaderProcessor::setIndent(Indent indent, uint32_t count) {
        indent_ = indent;
        indent_count_ = count;
    }

    void ResourceHeaderProcessor::setLineBreak(LineBreak lb) {
        line_break_ = lb;
    }

    bool ResourceHeaderProcessor::write(const string16& path, const IdMap& view_id_map, const IdMap& layout_id_map) {
        std::ofstream writer(path, std::ios::binary | std::ios::ate);
        if (writer.fail()) {
            LOG(Log::ERR) << "Cannot open file: " << path;
            return false;
        }

        utl::File out_file(path);
        if (!out_file.mkDirs(true)) {
            LOG(Log::ERR) << "Failed to make dir: " << out_file.getParentPath();
            return false;
        }
        auto name_macro = utl::toUpperCase(out_file.getName());
        if (name_macro.empty()) {
            LOG(Log::ERR) << "Invalid out file name: " << out_file.getPath();
            return false;
        }

        name_macro = utl::replaceAll(name_macro, L".", L"_").append(L"_");

        string8 out_str;
        generateOutput(utl::UTF16ToUTF8(name_macro), view_id_map, layout_id_map, &out_str);

        writer.write(out_str.data(), out_str.length());

        return true;
    }

    void ResourceHeaderProcessor::generateOutput(
        const string8& name_macro,
        const IdMap& view_id_map, const IdMap& layout_id_map, string8* out)
    {
        DCHECK(out != nullptr && out->empty());

        out->append("#ifndef RESOURCES_OIGKA_").append(name_macro);
        out->append(getLineBreak());
        out->append("#define RESOURCES_OIGKA_").append(name_macro);
        out->append(getLineBreak(2));
        out->append("#include <cstdint>");
        out->append(getLineBreak(3));
        out->append("namespace Res {").append(getLineBreak());
        out->append(getLineBreak());

        // view id
        out->append(getIndent(1)).append("namespace Id {").append(getLineBreak());
        out->append(getLineBreak());

        for (const auto& pair : view_id_map) {
            out->append(getIndent(2)).append("const int ");
            out->append(pair.first);
            out->append(" = ");
            out->append(std::to_string(pair.second));
            out->append(";");
            out->append(getLineBreak());
        }

        out->append(getLineBreak());
        out->append(getIndent(1)).append("}  // namespace Id").append(getLineBreak(2));

        // layout id
        out->append(getIndent(1)).append("namespace Layout {").append(getLineBreak());
        out->append(getLineBreak());

        for (const auto& pair : layout_id_map) {
            out->append(getIndent(2)).append("const int ");
            out->append(utl::replaceAll(pair.first, ".", "_"));
            out->append(" = ");
            out->append(std::to_string(pair.second));
            out->append(";");
            out->append(getLineBreak());
        }

        out->append(getLineBreak());
        out->append(getIndent(1)).append("}  // namespace Layout").append(getLineBreak(2));

        out->append("}  // namespace Res").append(getLineBreak(2));
        out->append("#endif  // RESOURCES_OIGKA_").append(name_macro);
    }

    string8 ResourceHeaderProcessor::getIndent(uint32_t layer) const {
        char indent_ch;
        string8 indent_str;
        switch (indent_) {
        case Indent::Space: indent_ch = ' '; break;
        case Indent::Tab: indent_ch = '\t'; break;
        default: indent_ch = ' '; break;
        }
        for (uint32_t i = 0; i < layer * indent_count_; ++i) {
            indent_str.push_back(indent_ch);
        }
        return indent_str;
    }

    string8 ResourceHeaderProcessor::getLineBreak(uint32_t count) const {
        if (count == 0) {
            return string8();
        }

        string8 break_unit;
        string8 break_str;
        switch (line_break_) {
        case LineBreak::CR: break_unit = "\r"; break;
        case LineBreak::LF: break_unit = "\n"; break;
        case LineBreak::CRLF: break_unit = "\r\n"; break;
        default: break_unit = "\r\n"; break;
        }

        for (uint32_t i = 0; i < count; ++i) {
            break_str.append(break_unit);
        }
        return break_str;
    }

}
