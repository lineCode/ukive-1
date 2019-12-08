#include "utils/xml/xml_structs.h"


namespace utl {
namespace xml {

    Content::Content()
        : type(Type::Element) {}

    Element::Element()
        : parent(nullptr) {}

}
}