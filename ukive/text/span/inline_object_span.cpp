#include "inline_object_span.h"


namespace ukive {

    InlineObjectSpan::InlineObjectSpan(unsigned int start, unsigned int end)
        :Span(start, end)
    {
    }

    InlineObjectSpan::~InlineObjectSpan()
    {
    }


    int InlineObjectSpan::getBaseType()
    {
        return Span::USPAN_BASE_TYPE_INLINEOBJECT;
    }

}