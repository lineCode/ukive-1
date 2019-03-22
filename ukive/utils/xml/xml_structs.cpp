#include "ukive/utils/xml/xml_structs.h"


namespace ukive {
namespace xml {

    Content::Content()
        : type(Type::Element) {}

    Element::Element()
        : parent(nullptr) {}

}
}