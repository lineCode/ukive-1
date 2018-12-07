#ifndef UKIVE_TEXT_SPAN_INTERACTABLE_SPAN_H_
#define UKIVE_TEXT_SPAN_INTERACTABLE_SPAN_H_

#include "ukive/text/span/span.h"


namespace ukive {

    class InteractableSpan : public Span {
    public:
        InteractableSpan(unsigned int start, unsigned int end);
        ~InteractableSpan();

        int getBaseType() const override;
    };

}

#endif  // UKIVE_TEXT_SPAN_INTERACTABLE_SPAN_H_