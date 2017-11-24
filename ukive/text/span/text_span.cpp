#include "text_span.h"


namespace ukive {

    TextSpan::TextSpan(unsigned int start, unsigned int end)
        :Span(start, end)
    {
    }

    TextSpan::~TextSpan()
    {
    }


    int TextSpan::getBaseType()
    {
        return Span::USPAN_BASE_TYPE_TEXT;
    }

}