#ifndef UKIVE_TEXT_SPAN_INLINE_OBJECT_SPAN_H_
#define UKIVE_TEXT_SPAN_INLINE_OBJECT_SPAN_H_

#include "ukive/text/span/span.h"


namespace ukive {

    class InlineObjectSpan : public Span {
    public:
        InlineObjectSpan(unsigned int start, unsigned int end);
        ~InlineObjectSpan();

        int getBaseType() const override;
    };

}

#endif  // UKIVE_TEXT_SPAN_INLINE_OBJECT_SPAN_H_