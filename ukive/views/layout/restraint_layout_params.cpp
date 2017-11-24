#include "restraint_layout_params.h"


namespace ukive {

    RestraintLayoutParams::RestraintLayoutParams()
        : LayoutParams(FIT_CONTENT, FIT_CONTENT)
    {
        initLayoutParams();
    }

    RestraintLayoutParams::RestraintLayoutParams(int width, int height)
        : LayoutParams(width, height)
    {
        initLayoutParams();
    }

    RestraintLayoutParams::RestraintLayoutParams(LayoutParams *lp)
        : LayoutParams(lp)
    {
        initLayoutParams();
    }


    RestraintLayoutParams::~RestraintLayoutParams()
    {
    }


    void RestraintLayoutParams::initLayoutParams()
    {
        isWidthMeasured = false;
        isHeightMeasured = false;

        isVertLayouted = false;
        isHoriLayouted = false;

        startHandledEdge = NONE;
        topHandledEdge = NONE;
        endHandledEdge = NONE;
        bottomHandledEdge = NONE;

        verticalPercent = .5f;
        horizontalPercent = .5f;
    }


    RestraintLayoutParams* RestraintLayoutParams::startHandle(int handleId, int handleEdge, int margin)
    {
        startHandledId = handleId;
        startHandledEdge = handleEdge;
        leftMargin = margin;
        return this;
    }

    RestraintLayoutParams* RestraintLayoutParams::topHandle(int handleId, int handleEdge, int margin)
    {
        topHandledId = handleId;
        topHandledEdge = handleEdge;
        topMargin = margin;
        return this;
    }

    RestraintLayoutParams* RestraintLayoutParams::endHandle(int handleId, int handleEdge, int margin)
    {
        endHandledId = handleId;
        endHandledEdge = handleEdge;
        rightMargin = margin;
        return this;
    }

    RestraintLayoutParams* RestraintLayoutParams::bottomHandle(int handleId, int handleEdge, int margin)
    {
        bottomHandledId = handleId;
        bottomHandledEdge = handleEdge;
        bottomMargin = margin;
        return this;
    }


    bool RestraintLayoutParams::hasStart()
    {
        return startHandledEdge != NONE;
    }

    bool RestraintLayoutParams::hasTop()
    {
        return topHandledEdge != NONE;
    }

    bool RestraintLayoutParams::hasEnd()
    {
        return endHandledEdge != NONE;
    }

    bool RestraintLayoutParams::hasBottom()
    {
        return bottomHandledEdge != NONE;
    }

    bool RestraintLayoutParams::hasVerticalCouple()
    {
        return (topHandledEdge != NONE
            && bottomHandledEdge != NONE);
    }

    bool RestraintLayoutParams::hasHorizontalCouple()
    {
        return (startHandledEdge != NONE
            && endHandledEdge != NONE);
    }

}