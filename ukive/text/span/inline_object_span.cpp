#include "inline_object_span.h"


namespace ukive {

    InlineObjectSpan::InlineObjectSpan(unsigned int start, unsigned int end)
        :Span(start, end) {
    }

    InlineObjectSpan::~InlineObjectSpan() {
    }


    int InlineObjectSpan::getBaseType() const {
        return Span::INLINEOBJECT;
    }

}