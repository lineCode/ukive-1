#ifndef UKIVE_UTILS_XML_XML_STRUCTS_H_
#define UKIVE_UTILS_XML_XML_STRUCTS_H_

#include <map>

#include "ukive/utils/string_utils.h"


namespace ukive {
namespace xml {

    struct Prolog {
        string8 version;
        string8 charset;
    };

    struct Content {
        enum class Type {
            CharData,
            Element,
        };

        Type type;
        string8 char_data;
        std::shared_ptr<struct Element> element;

        Content();
    };

    struct Element {
        string8 tag_name;
        std::map<string8, string8> attrs;
        std::vector<Content> contents;
        Element* parent;

        Element();
    };

}
}

#endif  // UKIVE_UTILS_XML_XML_STRUCTS_H_