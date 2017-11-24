#include "interactable_span.h"


namespace ukive {

    InteractableSpan::InteractableSpan(
        unsigned int start, unsigned int end)
        :Span(start, end)
    {
    }

    InteractableSpan::~InteractableSpan()
    {
    }


    int InteractableSpan::getBaseType()
    {
        return Span::USPAN_BASE_TYPE_INTERACTABLE;
    }

}