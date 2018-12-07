#include "span.h"


namespace ukive {

    Span::Span(unsigned int start, unsigned int end)
        :start_(start),
        end_(end) {
    }

    Span::~Span() {
    }


    void Span::resize(unsigned int start, unsigned int end) {
        start_ = start;
        end_ = end;
    }

    unsigned int Span::getStart() const {
        return start_;
    }

    unsigned int Span::getEnd() const {
        return end_;
    }

}