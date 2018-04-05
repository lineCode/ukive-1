#include "text_span.h"


namespace ukive {

    TextSpan::TextSpan(unsigned int start, unsigned int end)
        :Span(start, end) {
    }

    TextSpan::~TextSpan() {
    }


    int TextSpan::getBaseType() const {
        return Span::TEXT;
    }

}