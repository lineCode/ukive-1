#ifndef UKIVE_TEXT_SPAN_UNDERLINE_SPAN_H_
#define UKIVE_TEXT_SPAN_UNDERLINE_SPAN_H_

#include "ukive/text/span/text_span.h"


namespace ukive {

    class UnderlineSpan : public TextSpan
    {
    public:
        UnderlineSpan(unsigned int start, unsigned int end);
        ~UnderlineSpan();

        int getType() const override;
    };

}

#endif  // UKIVE_TEXT_SPAN_UNDERLINE_SPAN_H_