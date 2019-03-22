#include "restraint_layout_params.h"


namespace ukive {

    RestraintLayoutParams::RestraintLayoutParams()
        : RestraintLayoutParams(FIT_CONTENT, FIT_CONTENT)
    {}

    RestraintLayoutParams::RestraintLayoutParams(int width, int height)
        : LayoutParams(width, height)
    {
        initLayoutParams();
    }

    RestraintLayoutParams::RestraintLayoutParams(const LayoutParams &lp)
        : LayoutParams(lp)
    {
        initLayoutParams();
    }

    RestraintLayoutParams::~RestraintLayoutParams() {
    }

    void RestraintLayoutParams::initLayoutParams() {
        is_width_measured = false;
        is_height_measured = false;

        is_vert_layouted = false;
        is_hori_layouted = false;

        start_handled_edge = NONE;
        top_handled_edge = NONE;
        end_handled_edge = NONE;
        bottom_handled_edge = NONE;

        vertical_weight = 0;
        horizontal_weight = 0;

        vertical_percent = .5f;
        horizontal_percent = .5f;
    }

    RestraintLayoutParams* RestraintLayoutParams::startHandle(int handle_id, Edge handle_edge, int margin) {
        start_handled_id = handle_id;
        start_handled_edge = handle_edge;
        left_margin = margin;
        return this;
    }

    RestraintLayoutParams* RestraintLayoutParams::topHandle(int handle_id, Edge handle_edge, int margin) {
        top_handled_id = handle_id;
        top_handled_edge = handle_edge;
        top_margin = margin;
        return this;
    }

    RestraintLayoutParams* RestraintLayoutParams::endHandle(int handle_id, Edge handle_edge, int margin) {
        end_handled_id = handle_id;
        end_handled_edge = handle_edge;
        right_margin = margin;
        return this;
    }

    RestraintLayoutParams* RestraintLayoutParams::bottomHandle(int handle_id, Edge handle_edge, int margin) {
        bottom_handled_id = handle_id;
        bottom_handled_edge = handle_edge;
        bottom_margin = margin;
        return this;
    }

    bool RestraintLayoutParams::hasStart() {
        return start_handled_edge != NONE;
    }

    bool RestraintLayoutParams::hasTop() {
        return top_handled_edge != NONE;
    }

    bool RestraintLayoutParams::hasEnd() {
        return end_handled_edge != NONE;
    }

    bool RestraintLayoutParams::hasBottom() {
        return bottom_handled_edge != NONE;
    }

    bool RestraintLayoutParams::hasVerticalWeight() {
        return vertical_weight != 0;
    }

    bool RestraintLayoutParams::hasHorizontalWeight() {
        return horizontal_weight != 0;
    }

    bool RestraintLayoutParams::hasVerticalCouple() {
        return (top_handled_edge != NONE
            && bottom_handled_edge != NONE);
    }

    bool RestraintLayoutParams::hasHorizontalCouple() {
        return (start_handled_edge != NONE
            && end_handled_edge != NONE);
    }

}