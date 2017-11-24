#include "span.h"


namespace ukive {

    Span::Span(unsigned int start, unsigned int end)
    {
        mStart = start;
        mEnd = end;
    }

    Span::~Span()
    {
    }


    void Span::resize(unsigned int start, unsigned int end)
    {
        mStart = start;
        mEnd = end;
    }

    unsigned int Span::getStart()
    {
        return mStart;
    }

    unsigned int Span::getEnd()
    {
        return mEnd;
    }

}