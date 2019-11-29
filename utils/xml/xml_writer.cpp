#include "utils/xml/xml_writer.h"


namespace utl {

    XMLWriter::XMLWriter()
        : indent_(Indent::Space),
          indent_count_(4),
          line_break_(LineBreak::CRLF) {
    }

    void XMLWriter::setIndent(Indent indent, uint32_t count) {
        indent_ = indent;
        indent_count_ = count;
    }

    void XMLWriter::setLineBreak(LineBreak lb) {
        line_break_ = lb;
    }

    bool XMLWriter::write(const Element& root, string8* out) {
        if (!out || !out->empty()) return false;
        writeProlog(out);
        out->append(getLineBreak());
        if (!writeElements(root, 1, out)) return false;
        return true;
    }

    void XMLWriter::writeProlog(string8* out) {
        out->append("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    }

    bool XMLWriter::writeElements(const Element& root, uint32_t layer, string8* out) {
        out->append("<").append(root.tag_name);
        for (const auto& attr : root.attrs) {
            out->append(getLineBreak());
            out->append(getIndent(layer));
            out->append(attr.first).append("=").append("\"").append(attr.second).append("\"");
        }
        if (root.contents.empty()) {
            out->append("/>");
        } else {
            out->append(">");
            for (const auto& content : root.contents) {
                switch (content.type) {
                case Content::Type::CharData:
                    out->append(content.char_data);
                    break;
                case Content::Type::Element:
                    if (!writeElements(*(content.element.get()), layer + 1, out)) return false;
                    break;
                default:
                    return false;
                }
            }
            out->append("</").append(root.tag_name).append(">");
        }

        return true;
    }

    string8 XMLWriter::getIndent(uint32_t layer) const {
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

    string8 XMLWriter::getLineBreak() const {
        switch (line_break_) {
        case LineBreak::CR: return "\r";
        case LineBreak::LF: return "\n";
        case LineBreak::CRLF: return "\r\n";
        default: return "\r\n";
        }
    }

}