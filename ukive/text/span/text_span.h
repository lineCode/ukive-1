#ifndef UKIVE_TEXT_SPAN_TEXT_SPAN_H_
#define UKIVE_TEXT_SPAN_TEXT_SPAN_H_

#include "ukive/text/span/span.h"


namespace ukive {

    class TextSpan : public Span
    {
    public:
        TextSpan(unsigned int start, unsigned int end);
        virtual ~TextSpan();

        int getBaseType();
    };

}

#endif  // UKIVE_TEXT_SPAN_TEXT_SPAN_H_