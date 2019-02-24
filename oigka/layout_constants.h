#ifndef OIGKA_LAYOUT_CONSTANTS_H_
#define OIGKA_LAYOUT_CONSTANTS_H_


namespace oigka {

    const wchar_t kLayoutIdFileName[] = L"oigka_layout_id";

    // View
    const wchar_t kAttrViewId[] = L"id";
    const wchar_t kAttrViewPadding[] = L"padding";
    const wchar_t kAttrViewPaddingStart[] = L"padding_start";
    const wchar_t kAttrViewPaddingEnd[] = L"padding_end";
    const wchar_t kAttrViewPaddingTop[] = L"padding_top";
    const wchar_t kAttrViewPaddingBottom[] = L"padding_bottom";
    const wchar_t kAttrViewElevation[] = L"elevation";

    // TextView
    const wchar_t kAttrTextViewText[] = L"text";
    const wchar_t kAttrTextViewIsSelectable[] = L"is_selectable";
    const wchar_t kAttrTextViewIsEditable[] = L"is_editable";

    // LinearLayout (View)
    const wchar_t kAttrLinearLayoutViewOri[] = L"orientation";
    const wchar_t kAttrValLinearLayoutViewOriVert[] = L"vertical";
    const wchar_t kAttrValLinearLayoutViewOriHori[] = L"horizontal";

    // ViewGroup
    const wchar_t kAttrLayoutWidth[] = L"layout_width";
    const wchar_t kAttrLayoutHeight[] = L"layout_height";
    const wchar_t kAttrLayoutMargin[] = L"layout_margin";
    const wchar_t kAttrLayoutMarginStart[] = L"layout_margin_start";
    const wchar_t kAttrLayoutMarginEnd[] = L"layout_margin_end";
    const wchar_t kAttrLayoutMarginTop[] = L"layout_margin_top";
    const wchar_t kAttrLayoutMarginBottom[] = L"layout_margin_bottom";

    const wchar_t kAttrValLayoutFit[] = L"fit_content";
    const wchar_t kAttrValLayoutMatch[] = L"match_parent";

    // LinearLayout
    const wchar_t kAttrLinearLayoutWeight[] = L"layout_weight";

    // RestraintLayout
    const wchar_t kAttrRestraintLayoutSHId[] = L"layout_start_handle_id";
    const wchar_t kAttrRestraintLayoutSHEdge[] = L"layout_start_handle_edge";
    const wchar_t kAttrRestraintLayoutSHMargin[] = L"layout_start_handle_margin";
    const wchar_t kAttrRestraintLayoutEHId[] = L"layout_end_handle_id";
    const wchar_t kAttrRestraintLayoutEHEdge[] = L"layout_end_handle_edge";
    const wchar_t kAttrRestraintLayoutEHMargin[] = L"layout_end_handle_margin";
    const wchar_t kAttrRestraintLayoutTHId[] = L"layout_top_handle_id";
    const wchar_t kAttrRestraintLayoutTHEdge[] = L"layout_top_handle_edge";
    const wchar_t kAttrRestraintLayoutTHMargin[] = L"layout_top_handle_margin";
    const wchar_t kAttrRestraintLayoutBHId[] = L"layout_bottom_handle_id";
    const wchar_t kAttrRestraintLayoutBHEdge[] = L"layout_bottom_handle_edge";
    const wchar_t kAttrRestraintLayoutBHMargin[] = L"layout_bottom_handle_margin";

    const wchar_t kAttrValRestraintLayoutHEStart[] = L"start";
    const wchar_t kAttrValRestraintLayoutHEEnd[] = L"end";
    const wchar_t kAttrValRestraintLayoutHETop[] = L"top";
    const wchar_t kAttrValRestraintLayoutHEBottom[] = L"bottom";


}

#endif  // OIGKA_LAYOUT_CONSTANTS_H_