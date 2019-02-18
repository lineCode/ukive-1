#ifndef UKIVE_UTILS_XML_WRITER_H_
#define UKIVE_UTILS_XML_WRITER_H_

#include "ukive/utils/xml/xml_parser.h"


namespace ukive {

    class XMLWriter {
    public:
        using Prolog = xml::Prolog;
        using Content = xml::Content;
        using Element = xml::Element;

        enum class Indent {
            Tab,
            Space,
        };

        enum class LineBreak {
            CR,
            LF,
            CRLF,
        };

        XMLWriter();

        void setIndent(Indent indent, uint32_t count);
        void setLineBreak(LineBreak lb);
        bool write(const Element& root, string8* out);

    private:
        void writeProlog(string8* out);
        bool writeElements(const Element& root, uint32_t layer, string8* out);
        string8 getIndent(uint32_t layer) const;
        string8 getLineBreak() const;

        Indent indent_;
        uint32_t indent_count_;
        LineBreak line_break_;
    };

}

#endif  // UKIVE_UTILS_XML_WRITER_H_