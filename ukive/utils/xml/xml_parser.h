#ifndef UKIVE_UTILS_XML_PARSER_H_
#define UKIVE_UTILS_XML_PARSER_H_

#include <memory>

#include "ukive/utils/string_utils.h"

#include "ukive/utils/xml/xml_structs.h"


namespace ukive {

    class XMLParser {
    public:
        using Prolog = xml::Prolog;
        using Content = xml::Content;
        using Element = xml::Element;
        using crstring8 = const string8&;
        using index_t = string8::size_type;

        XMLParser();

        bool parse(crstring8 str, std::shared_ptr<Element>* out);
        const Prolog& getProlog() const;

    private:
        enum class DocStepper {
            None,
            Prolog,
            Elements,
            Misc,
        };

        enum class ElementStepper {
            TAG_NAME,
            ATTR_NAME,
        };

        enum class QuesTagType {
            Prolog,
            PIs,
        };

        enum class ExclTagType {
            CDATA,
            COND_SECT,
            DOCTYPE,
            ENTITY,
            ELEMENT,
            ATTLIST,
            COMMENT,
        };

        enum class NormTagType {
            EmptyTag,
            StartTag,
            EndTag,
        };

        bool parseQuesTag(crstring8 str, index_t idx, index_t* p_idx, QuesTagType* type);
        bool parseExclTag(crstring8 str, index_t idx, index_t* p_idx, ExclTagType* type);
        bool parseNormTag(crstring8 str, index_t idx, index_t* p_idx, NormTagType* type, Element* cur);

        bool parseEncodingDecl(crstring8 str, index_t idx, index_t* p_idx);
        bool parseSDDecl(crstring8 str, index_t idx, index_t* p_idx);
        bool parseEq(crstring8 str, index_t idx, index_t* p_idx);
        bool parseAttribute(crstring8 str, index_t idx, index_t* p_idx, Element* cur);

        bool isChar(uint32_t val) const;
        bool isDigit(uint32_t val) const;
        bool isAlphaBet(uint32_t val) const;
        bool isSpace(uint32_t val) const;
        bool isNameStartChar(uint32_t val) const;
        bool isNameChar(uint32_t val) const;
        bool isEncNameChar(uint32_t val) const;

        DocStepper doc_stepper_;

        Prolog prolog_;
    };

}

#endif  // UKIVE_UTILS_XML_PARSER_H_