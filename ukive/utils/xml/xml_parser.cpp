#include "ukive/utils/xml/xml_parser.h"

#define ADV_IDX(adv)  \
    idx += (adv);  \
    if (idx >= str.length()) return false;

#define LOOP_S()  \
    while(isSpace(str[idx])) { ADV_IDX(1); }

#define LOOP_DIGIT()  \
    while(isDigit(str[idx])) { ADV_IDX(1); }

#define ONE_LOOP_S()  \
    if (!isSpace(str[idx])) return false;  \
    ADV_IDX(1); LOOP_S();


namespace ukive {

    XMLParser::XMLParser()
        : doc_stepper_(DocStepper::None) {}

    bool XMLParser::parse(crstring8 str) {
        prolog_.version.clear();
        prolog_.charset.clear();
        root_element_.reset();
        doc_stepper_ = DocStepper::None;

        if (str.empty()) return false;
        // UTF-8 BOM
        index_t cur_index = 0;
        if (str[0] == 0xEFi8) {
            if (str.length() <= 3) return false;
            if (str[1] != 0xBBi8 || str[2] != 0xBFi8) return false;
            cur_index = 3;
        }

        string8 content;
        auto cur_element = root_element_.get();

        for (auto i = cur_index; i < str.length(); ++i) {
            if (str[i] == '<') {
                if (!content.empty()) {
                    cur_element->contents.push_back(content);
                    content.clear();
                }

                if (startWith(str, "<?", i)) {
                    QuesTagType type;
                    if (!parseQuesTag(str, i + 2, &i, &type)) return false;
                    if (type == QuesTagType::Prolog) {
                        doc_stepper_ = DocStepper::Prolog;
                    }
                } else if (startWith(str, "<!", i)) {
                    ExclTagType type;
                    if (!parseExclTag(str, i + 2, &i, &type)) return false;
                } else {
                    if (doc_stepper_ == DocStepper::Misc) return false;

                    NormTagType type;
                    auto tmp = std::make_shared<Element>();
                    if (!parseNormTag(str, i + 1, &i, &type, tmp.get())) return false;
                    doc_stepper_ = DocStepper::Elements;
                    switch (type) {
                    case NormTagType::StartTag:
                        if (cur_element) {
                            tmp->parent = cur_element;
                            cur_element->children.push_back(tmp);
                        } else {
                            root_element_ = tmp;
                        }
                        cur_element = tmp.get();
                        break;
                    case NormTagType::EndTag:
                        if (!cur_element) return false;
                        if (cur_element->tag_name != tmp->tag_name) return false;
                        cur_element = cur_element->parent;
                        if (!cur_element) {
                            doc_stepper_ = DocStepper::Misc;
                        }
                        break;
                    case NormTagType::EmptyTag:
                        if (cur_element) {
                            tmp->parent = cur_element;
                            cur_element->children.push_back(tmp);
                        } else {
                            root_element_ = tmp;
                            cur_element = tmp.get();
                            doc_stepper_ = DocStepper::Misc;
                        }
                        break;
                    }
                }
            } else {
                if (doc_stepper_ == DocStepper::None) {
                    return false;
                }
                if (doc_stepper_ == DocStepper::Prolog) {
                    if (!isSpace(str[i])) return false;
                } else if (doc_stepper_ == DocStepper::Elements) {
                    content.push_back(str[i]);
                } else if(doc_stepper_ == DocStepper::Misc){
                    if (!isSpace(str[i]) && str[i] != 0) return false;
                }
            }
        }

        if (cur_element) return false;

        return true;
    }

    const XMLParser::Prolog* XMLParser::getProlog() const {
        return &prolog_;
    }

    const XMLParser::Element* XMLParser::getRootElement() const {
        return root_element_.get();
    }

    bool XMLParser::parseQuesTag(crstring8 str, index_t idx, index_t* p_idx, QuesTagType* type) {
        if (idx >= str.length()) return false;

        if (startWith(str, "xml", idx)) {
            if (doc_stepper_ != DocStepper::None) return false;
            *type = QuesTagType::Prolog;
            // Prolog
            ADV_IDX(3);

            // VersionInfo
            ONE_LOOP_S();
            if (!startWith(str, "version", idx)) return false;
            ADV_IDX(7);
            if (!parseEq(str, idx, &idx)) return false;
            if (str[idx] != '\"' && str[idx] != '\'') return false;
            auto cur_sign = str[idx];
            ADV_IDX(1);
            if (!startWith(str, "1.", idx)) return false;
            ADV_IDX(2);
            auto ver_start_idx = idx;
            LOOP_DIGIT();
            prolog_.version = str.substr(ver_start_idx, idx - ver_start_idx);
            if (str[idx] != cur_sign) return false;
            ADV_IDX(1);

            parseEncodingDecl(str, idx, &idx);
            parseSDDecl(str, idx, &idx);

            LOOP_S();
            if (!startWith(str, "?>", idx)) return false;
            ADV_IDX(1);
        } else {
            *type = QuesTagType::PIs;
            // PIs
            while (!startWith(str, "?>", idx)) {
                if (isSpace(str[idx])) {
                    // TODO:
                }
                ADV_IDX(1);
            }
            ADV_IDX(1);
        }

        *p_idx = idx;
        return true;
    }

    bool XMLParser::parseExclTag(crstring8 str, index_t idx, index_t* p_idx, ExclTagType* type) {
        if (idx >= str.length()) return false;

        if (startWith(str, "[CDATA[", idx)) {
            if (doc_stepper_ == DocStepper::Misc) return false;
            *type = ExclTagType::CDATA;
            ADV_IDX(7);
            while (!startWith(str, "]]>", idx)) {
                // TODO:
                ADV_IDX(1);
            }
            ADV_IDX(2);
        } else if (str[idx] == '[') {
            if (doc_stepper_ == DocStepper::Misc) return false;
            *type = ExclTagType::COND_SECT;
            ADV_IDX(1);
            while (!startWith(str, "]]>", idx)) {
                // TODO:
                ADV_IDX(1);
            }
            ADV_IDX(2);
        } else if (startWith(str, "DOCTYPE", idx)) {
            if (doc_stepper_ == DocStepper::Misc) return false;
            *type = ExclTagType::DOCTYPE;
            ADV_IDX(7);
            while (!startWith(str, ">", idx)) {
                // TODO:
                ADV_IDX(1);
            }
        } else if (startWith(str, "ENTITY", idx)) {
            if (doc_stepper_ == DocStepper::Misc) return false;
            *type = ExclTagType::ENTITY;
            ADV_IDX(6);
            while (!startWith(str, ">", idx)) {
                // TODO:
                ADV_IDX(1);
            }
        } else if (startWith(str, "ELEMENT", idx)) {
            if (doc_stepper_ == DocStepper::Misc) return false;
            *type = ExclTagType::ELEMENT;
            ADV_IDX(7);
            while (!startWith(str, ">", idx)) {
                // TODO:
                ADV_IDX(1);
            }
        } else if (startWith(str, "ATTLIST", idx)) {
            if (doc_stepper_ == DocStepper::Misc) return false;
            *type = ExclTagType::ATTLIST;
            ADV_IDX(7);
            while (!startWith(str, ">", idx)) {
                // TODO:
                ADV_IDX(1);
            }
        } else if (startWith(str, "--", idx)) {
            *type = ExclTagType::COMMENT;
            ADV_IDX(2);
            bool illegal = false;
            while (!startWith(str, "-->", idx)) {
                illegal = (str[idx] == '-');
                // TODO:
                ADV_IDX(1);
            }
            if (illegal) return false;
            ADV_IDX(2);
        } else {
            return false;
        }

        *p_idx = idx;
        return true;
    }

    bool XMLParser::parseNormTag(crstring8 str, index_t idx, index_t* p_idx, NormTagType* type, Element* cur) {
        if (idx >= str.length()) return false;

        if (str[idx] == '/') {
            *type = NormTagType::EndTag;
            ADV_IDX(1);
            bool has_space = false;
            auto name_start_idx = idx;
            while (str[idx] != '>') {
                if (isSpace(str[idx])) {
                    has_space = true;
                } else {
                    if (has_space) return false;
                }
                ADV_IDX(1);
            }
            cur->tag_name = str.substr(name_start_idx, idx - name_start_idx);
        } else {
            bool first_char = true;
            auto stepper = ElementStepper::TAG_NAME;
            auto name_start_idx = idx;
            for (;;) {
                if (str[idx] == '>') {
                    *type = NormTagType::StartTag;
                    if (stepper == ElementStepper::TAG_NAME) {
                        if (idx - name_start_idx == 0) return false;
                        cur->tag_name = str.substr(name_start_idx, idx - name_start_idx);
                    }
                    break;
                }
                if (startWith(str, "/>", idx)) {
                    *type = NormTagType::EmptyTag;
                    if (stepper == ElementStepper::TAG_NAME) {
                        if (idx - name_start_idx == 0) return false;
                        cur->tag_name = str.substr(name_start_idx, idx - name_start_idx);
                    }
                    ADV_IDX(1);
                    break;
                }

                if (isSpace(str[idx])) {
                    if (stepper == ElementStepper::TAG_NAME) {
                        if (idx - name_start_idx == 0) return false;
                        cur->tag_name = str.substr(name_start_idx, idx - name_start_idx);
                        stepper = ElementStepper::ATTR_NAME;
                    } else if (stepper == ElementStepper::ATTR_NAME) {
                        // Do nothing
                    }
                    ADV_IDX(1);
                } else {
                    if (stepper == ElementStepper::TAG_NAME) {
                        if (first_char) {
                            first_char = false;
                            if (!isNameStartChar(str[idx])) return false;
                        } else {
                            if (!isNameChar(str[idx])) return false;
                        }
                        ADV_IDX(1);
                    } else if (stepper == ElementStepper::ATTR_NAME) {
                        if (!parseAttribute(str, idx, &idx, cur)) return false;
                    }
                }
            }
        }

        *p_idx = idx;
        return true;
    }

    bool XMLParser::parseEncodingDecl(crstring8 str, index_t idx, index_t* p_idx) {
        ONE_LOOP_S();
        if (!startWith(str, "encoding", idx)) return false;
        ADV_IDX(8);
        if (!parseEq(str, idx, &idx)) return false;
        if (str[idx] != '\"' && str[idx] != '\'') return false;
        auto cur_sign = str[idx];
        ADV_IDX(1);
        if (!isAlphaBet(str[idx])) return false;
        auto enc_start_idx = idx;
        ADV_IDX(1);
        while (isEncNameChar(str[idx])) {
            ADV_IDX(1);
        }
        if (str[idx] != cur_sign) return false;
        prolog_.charset = str.substr(enc_start_idx, idx - enc_start_idx);
        ADV_IDX(1);

        *p_idx = idx;
        return true;
    }

    bool XMLParser::parseSDDecl(crstring8 str, index_t idx, index_t* p_idx) {
        ONE_LOOP_S();
        if (!startWith(str, "standalone", idx)) return false;
        ADV_IDX(10);
        if (!parseEq(str, idx, &idx)) return false;
        if (str[idx] != '\"' && str[idx] != '\'') return false;
        auto cur_sign = str[idx];
        ADV_IDX(1);
        if (startWith(str, "yes", idx)) {
            ADV_IDX(3);
        } else if (startWith(str, "no", idx)) {
            ADV_IDX(2);
        } else {
            return false;
        }
        if (str[idx] != cur_sign) return false;
        ADV_IDX(1);

        *p_idx = idx;
        return true;
    }

    bool XMLParser::parseEq(crstring8 str, index_t idx, index_t* p_idx) {
        LOOP_S();
        if (str[idx] != '=') return false;
        ADV_IDX(1);
        LOOP_S();

        *p_idx = idx;
        return true;
    }

    bool XMLParser::parseAttribute(crstring8 str, index_t idx, index_t* p_idx, Element* cur) {
        bool name_first = true;
        auto attr_name_start_idx = idx;
        while (!isSpace(str[idx]) && str[idx] != '=') {
            if (name_first) {
                name_first = false;
                if (!isNameStartChar(str[idx])) return false;
            } else {
                if (!isNameChar(str[idx])) return false;
            }
            ADV_IDX(1);
        }
        auto attr_name = str.substr(attr_name_start_idx, idx - attr_name_start_idx);
        if (cur->attrs.find(attr_name) != cur->attrs.end()) return false;

        if (!parseEq(str, idx, &idx)) return false;
        if (str[idx] != '\"' && str[idx] != '\'') return false;
        auto cur_sign = str[idx];
        ADV_IDX(1);
        auto attr_val_start_idx = idx;
        while (str[idx] != '\"' && str[idx] != '\'') {
            ADV_IDX(1);
        }
        if (str[idx] != cur_sign) return false;
        auto attr_val = str.substr(attr_val_start_idx, idx - attr_val_start_idx);
        ADV_IDX(1);

        cur->attrs[attr_name] = attr_val;

        *p_idx = idx;
        return true;
    }

    bool XMLParser::isChar(uint32_t val) const {
        return (val == 0x9 ||
            val == 0xA ||
            val == 0xD ||
            (val >= 0x20 && val <= 0xD7FF) ||
            (val >= 0xE000 && val <= 0xFFFD) ||
            (val >= 0x10000 && val <= 0x10FFFF));
    }

    bool XMLParser::isDigit(uint32_t val) const {
        return (val >= '0' && val <= '9');
    }

    bool XMLParser::isAlphaBet(uint32_t val) const {
        return ((val >= 'a' && val <= 'z') ||
            (val >= 'A' && val <= 'Z'));
    }

    bool XMLParser::isSpace(uint32_t val) const {
        return (val == 0x20 || val == 0x9 || val == 0xD || val == 0xA);
    }

    bool XMLParser::isNameStartChar(uint32_t val) const {
        return (val == ':' ||
            (val >= 'A' && val <= 'Z') ||
            val == '_' ||
            (val >= 'a' && val <= 'z') ||
            (val >= 0xC0 && val <= 0xD6) ||
            (val >= 0xD8 && val <= 0xF6) ||
            (val >= 0xF8 && val <= 0x2FF) ||
            (val >= 0x370 && val <= 0x37D) ||
            (val >= 0x37F && val <= 0x1FFF) ||
            (val >= 0x200C && val <= 0x200D) ||
            (val >= 0x2070 && val <= 0x218F) ||
            (val >= 0x2C00 && val <= 0x2FEF) ||
            (val >= 0x3001 && val <= 0xD7FF) ||
            (val >= 0xF900 && val <= 0xFDCF) ||
            (val >= 0xFDF0 && val <= 0xFFFD) ||
            (val >= 0x10000 && val <= 0xEFFFF));
    }

    bool XMLParser::isNameChar(uint32_t val) const {
        return (isNameStartChar(val) ||
            val == '-' ||
            val == '.' ||
            (val >= 0 && val <= 9) ||
            val == 0xB7 ||
            (val >= 0x300 && val <= 0x36F) ||
            (val >= 0x203F && val <= 0x2040));
    }

    bool XMLParser::isEncNameChar(uint32_t val) const {
        return (isAlphaBet(val) ||
            isDigit(val) ||
            val == '.' ||
            val == '-' ||
            val == '_');
    }

}