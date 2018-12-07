﻿#include "underline_span.h"


namespace ukive {

    UnderlineSpan::UnderlineSpan(unsigned int start, unsigned int end)
        :TextSpan(start, end) {
    }

    UnderlineSpan::~UnderlineSpan() {
    }


    int UnderlineSpan::getType() const {
        return Span::TEXT_UNDERLINE;
    }

}