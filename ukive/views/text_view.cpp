#include "text_view.h"

#include <algorithm>

#include "ukive/text/input_connection.h"
#include "ukive/event/input_event.h"
#include "ukive/text/text_blink.h"
#include "ukive/system/clipboard_manager.h"
#include "ukive/text/text_action_mode.h"
#include "ukive/text/text_key_listener.h"
#include "ukive/window/window.h"
#include "ukive/text/span/span.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/renderer.h"
#include "ukive/text/text_drawing_effect.h"
#include "ukive/menu/menu.h"
#include "ukive/menu/menu_item.h"


namespace ukive {

    namespace {
        enum {
            MENU_ID_COPY = 1,
            MENU_ID_CUT,
            MENU_ID_PASTE,
            MENU_ID_SELECTALL,
        };

        enum {
            MENU_ORDER_COPY = 1,
            MENU_ORDER_CUT,
            MENU_ORDER_PASTE,
            MENU_ORDER_SELECTALL,
        };
    }

    TextView::TextView(Window* wnd)
        :View(wnd),
        text_color_(Color::Black),
        sel_bg_color_(Color::Blue200)
    {
        initTextView();
    }

    TextView::~TextView() {
        delete base_text_;
        delete text_blink_;
        delete text_key_listener_;
    }


    void TextView::initTextView() {
        text_size_ = static_cast<int>(std::round(getWindow()->dpToPx(15.f)));
        is_auto_wrap_ = true;
        is_editable_ = false;
        is_selectable_ = false;
        is_plkey_down_ = false;
        is_prkey_down_ = false;
        is_plkey_down_on_text_ = false;

        line_spacing_multiple_ = 1.f;
        line_spacing_method_ = DWRITE_LINE_SPACING_METHOD_DEFAULT;

        text_offset_at_view_top_ = 0;

        process_ref_ = 0;
        font_family_name_ = L"微软雅黑";
        text_action_mode_ = nullptr;
        text_alignment_ = DWRITE_TEXT_ALIGNMENT_LEADING;
        paragraph_alignment_ = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
        text_weight_ = DWRITE_FONT_WEIGHT_NORMAL;
        text_style_ = DWRITE_FONT_STYLE_NORMAL;

        text_blink_ = new TextBlink(this);
        text_blink_->setColor(Color::Blue500);

        input_connection_ = new InputConnection(this);
        text_key_listener_ = new TextKeyListener();

        base_text_ = new Editable(L"");
        base_text_->addEditWatcher(this);

        makeNewTextFormat();
        makeNewTextLayout(0.f, 0.f, false);

        base_text_->setSelection(0);
        locateTextBlink(0);

        setFocusable(is_editable_ | is_selectable_);
    }


    int TextView::computeVerticalScrollRange() {
        int textHeight = std::ceil(getTextHeight());
        int contentHeight = getHeight() - getPaddingTop() - getPaddingBottom();

        return std::max(0, textHeight - contentHeight);
    }

    int TextView::computeHorizontalScrollRange() {
        int textWidth = std::ceil(getTextWidth());
        int contentWidth = getWidth() - getPaddingLeft() - getPaddingRight();

        return std::max(0, textWidth - contentWidth);
    }


    void TextView::computeTextOffsetAtViewTop() {
        BOOL isInside;
        BOOL isTrailingHit;
        DWRITE_HIT_TEST_METRICS metrics;

        text_layout_->HitTestPoint(
            0.f, getScrollY(), &isTrailingHit, &isInside, &metrics);

        text_offset_at_view_top_ = metrics.textPosition + (isTrailingHit == TRUE ? 1 : 0);
        vertical_offset_ = getScrollY() - metrics.top;
    }

    int TextView::computeVerticalScrollOffsetFromTextOffset(uint32_t tOff) {
        uint32_t actual;
        DWRITE_HIT_TEST_METRICS hitMetrics;

        text_layout_->HitTestTextRange(
            tOff, 0, 0.f, 0.f, &hitMetrics, 1, &actual);

        return hitMetrics.top - getScrollY();
    }

    int TextView::determineVerticalScroll(int dy) {
        int final_y = getScrollY();
        int range = computeVerticalScrollRange();
        if (range > 0) {
            if (dy < 0) {
                final_y = std::max(0, getScrollY() + dy);
            } else if (dy > 0) {
                final_y = std::min(range, getScrollY() + dy);
            }
        }

        return final_y - getScrollY();
    }

    int TextView::determineHorizontalScroll(int dx) {
        int final_x = getScrollX();
        int range = computeHorizontalScrollRange();
        if (range > 0) {
            if (dx < 0) {
                final_x = std::max(0, getScrollX() + dx);
            } else if (dx > 0) {
                final_x = std::min(range, getScrollX() + dx);
            }
        }

        return final_x - getScrollX();
    }


    void TextView::scrollToFit(bool considerSelection) {
        bool scrolled = false;

        if (computeVerticalScrollRange() == 0) {
            if (getScrollY() != 0) {
                scrollTo(0, 0);
                scrolled = true;
            }
        } else {
            int scrollOffset = 0;

            if (considerSelection) {
                uint32_t selection;
                if (hasSelection()) {
                    selection = last_sel_;
                } else {
                    selection = getSelectionStart();
                }

                uint32_t actual;
                DWRITE_HIT_TEST_METRICS metrics;
                text_layout_->HitTestTextRange(
                    selection, 0, 0.f, 0.f, &metrics, 1, &actual);

                int lineTop = metrics.top;
                int lineBottom = metrics.top + metrics.height;
                int contentHeight = getHeight() - getPaddingTop() - getPaddingBottom();

                if (lineTop < getScrollY())
                    scrollOffset = lineTop - getScrollY();
                if (lineBottom > getScrollY() + contentHeight)
                    scrollOffset = lineBottom - (getScrollY() + contentHeight);
            } else {
                //置于大小变化前的位置。
                uint32_t prevTextOffset = text_offset_at_view_top_;
                float prevVerticalOffset = vertical_offset_;

                BOOL isInside;
                BOOL isTrailingHit;
                DWRITE_HIT_TEST_METRICS metrics;

                text_layout_->HitTestPoint(
                    0.f, getScrollY(), &isTrailingHit, &isInside, &metrics);
                uint32_t curTextOffset = metrics.textPosition + (isTrailingHit == TRUE ? 1 : 0);

                if (curTextOffset != prevTextOffset) {
                    scrollOffset = computeVerticalScrollOffsetFromTextOffset(prevTextOffset);
                }
            }

            //检查是否超出屏幕。
            int maxScrollOffset = computeVerticalScrollRange() - getScrollY();
            int offsetY = std::min(maxScrollOffset, scrollOffset);
            if (offsetY != 0) {
                scrollBy(0, offsetY);
                scrolled = true;
            }
        }
    }


    float TextView::getTextWidth() const {
        DWRITE_TEXT_METRICS metrics;
        text_layout_->GetMetrics(&metrics);
        return metrics.width;
    }

    float TextView::getTextHeight() const {
        DWRITE_TEXT_METRICS metrics;
        text_layout_->GetMetrics(&metrics);
        return metrics.height;
    }

    bool TextView::getLineInfo(
        uint32_t position, uint32_t* line, float* height, uint32_t* count) {

        uint32_t lineCount;
        HRESULT hr = text_layout_->GetLineMetrics(nullptr, 0, &lineCount);
        if (hr == E_NOT_SUFFICIENT_BUFFER && lineCount > 0) {
            std::unique_ptr<DWRITE_LINE_METRICS[]> lineMetrics(new DWRITE_LINE_METRICS[lineCount]);
            hr = text_layout_->GetLineMetrics(lineMetrics.get(), lineCount, &lineCount);
            if (SUCCEEDED(hr)) {
                uint32_t lineTextPosStart = 0;
                for (uint32_t i = 0; i < lineCount; ++i) {
                    uint32_t lineTextPosEnd
                        = lineTextPosStart + lineMetrics[i].length;
                    if (position >= lineTextPosStart && position <= lineTextPosEnd) {
                        *line = i;
                        *height = lineMetrics[i].height;
                        if (!count) {
                            *count = lineCount;
                        }
                        return true;
                    }

                    lineTextPosStart = lineTextPosEnd;
                }
            }
        }

        return false;
    }

    bool TextView::getLineHeight(uint32_t line, float* height) {
        uint32_t lineCount;
        HRESULT hr = text_layout_->GetLineMetrics(nullptr, 0, &lineCount);
        if (hr == E_NOT_SUFFICIENT_BUFFER && lineCount > 0) {
            std::unique_ptr<DWRITE_LINE_METRICS[]> lineMetrics(new DWRITE_LINE_METRICS[lineCount]);
            hr = text_layout_->GetLineMetrics(lineMetrics.get(), lineCount, &lineCount);
            if (SUCCEEDED(hr)) {
                if (line >= 0 && line < lineCount) {
                    *height = lineMetrics[line].height;
                    return true;
                }
            }
        }

        return false;
    }

    void TextView::blinkNavigator(int keyCode) {
        if (keyCode == VK_LEFT) {
            uint32_t start = base_text_->getSelectionStart();

            if (base_text_->hasSelection()) {
                base_text_->setSelection(start);
            } else {
                if (start > 0) {
                    size_t len = 1;
                    size_t index = start - 1;
                    wchar_t prev = base_text_->at(index);
                    if (prev == L'\n' && index > 0) {
                        prev = base_text_->at(index - 1);
                        if (prev == L'\r') {
                            len = 2;
                        }
                    }

                    base_text_->setSelection(start - len);
                }
            }
        } else if (keyCode == VK_RIGHT) {
            uint32_t end = base_text_->getSelectionEnd();

            if (base_text_->hasSelection()) {
                base_text_->setSelection(end);
            } else {
                if (end < base_text_->length()) {
                    size_t len = 1;
                    size_t index = end;
                    wchar_t next = base_text_->at(index);
                    if (next == L'\r' && (++index) < base_text_->length()) {
                        next = base_text_->at(index);
                        if (next == L'\n') {
                            len = 2;
                        }
                    }

                    base_text_->setSelection(end + len);
                }
            }
        } else if (keyCode == VK_UP) {
            uint32_t start;
            if (hasSelection() && (last_sel_ == base_text_->getSelectionStart()
                || last_sel_ == base_text_->getSelectionEnd())) {
                start = last_sel_;
            } else {
                start = base_text_->getSelectionStart();
            }

            uint32_t actual;
            DWRITE_HIT_TEST_METRICS rangeMetrics;
            text_layout_->HitTestTextRange(
                start, 0, 0.f, 0.f, &rangeMetrics, 1, &actual);

            BOOL isTrailing, isInside;
            DWRITE_HIT_TEST_METRICS pointMetrics;
            text_layout_->HitTestPoint(
                rangeMetrics.left, rangeMetrics.top - 1.f,
                &isTrailing, &isInside, &pointMetrics);

            base_text_->setSelection(
                pointMetrics.textPosition + (isTrailing == TRUE ? 1 : 0));
        } else if (keyCode == VK_DOWN) {
            uint32_t start;
            if (hasSelection() && (last_sel_ == base_text_->getSelectionStart()
                || last_sel_ == base_text_->getSelectionEnd())) {
                start = last_sel_;
            } else {
                start = base_text_->getSelectionStart();
            }

            uint32_t actual;
            DWRITE_HIT_TEST_METRICS rangeMetrics;
            text_layout_->HitTestTextRange(
                start, 0, 0.f, 0.f, &rangeMetrics, 1, &actual);

            BOOL isTrailing, isInside;
            DWRITE_HIT_TEST_METRICS pointMetrics;
            text_layout_->HitTestPoint(
                rangeMetrics.left, rangeMetrics.top + rangeMetrics.height,
                &isTrailing, &isInside, &pointMetrics);

            base_text_->setSelection(
                pointMetrics.textPosition + (isTrailing == TRUE ? 1 : 0));
        }
    }


    void TextView::makeNewTextFormat() {
        text_format_.reset();
        text_format_ = ukive::Renderer::createTextFormat(
            font_family_name_,
            text_size_,
            L"en-US");
    }

    void TextView::makeNewTextLayout(float maxWidth, float maxHeight, bool autoWrap) {
        text_layout_.reset();

        DWRITE_TEXT_RANGE range;
        range.startPosition = 0;
        range.length = base_text_->length();

        text_layout_ = ukive::Renderer::createTextLayout(
            base_text_->toString(),
            text_format_.get(),
            maxWidth,
            maxHeight);

        text_layout_->SetLineSpacing(
            line_spacing_method_,
            text_size_*line_spacing_multiple_,
            text_size_*line_spacing_multiple_*0.8f);

        text_layout_->SetWordWrapping(
            autoWrap ? DWRITE_WORD_WRAPPING_EMERGENCY_BREAK : DWRITE_WORD_WRAPPING_NO_WRAP);
        text_layout_->SetTextAlignment(text_alignment_);
        text_layout_->SetParagraphAlignment(paragraph_alignment_);

        text_layout_->SetFontStyle(text_style_, range);
        text_layout_->SetFontWeight(text_weight_, range);
    }


    void TextView::locateTextBlink(int position) {
        DWRITE_HIT_TEST_METRICS hitMetrics;

        if (line_spacing_method_ == DWRITE_LINE_SPACING_METHOD_UNIFORM) {
            UINT32 actual;
            text_layout_->HitTestTextRange(position, 0, 0.f, 0.f, &hitMetrics, 1, &actual);
        } else {
            float pointX, pointY;
            text_layout_->HitTestTextPosition(position, FALSE, &pointX, &pointY, &hitMetrics);
        }

        text_blink_->locate(hitMetrics.left, hitMetrics.top, hitMetrics.top + hitMetrics.height);
    }

    void TextView::locateTextBlink(float textX, float textY) {
        int hitPosition = this->getHitTextPosition(textX, textY);
        locateTextBlink(hitPosition);
    }


    void TextView::onBeginProcess() {
        ++process_ref_;
    }

    void TextView::onEndProcess() {
        --process_ref_;
    }


    void TextView::onAttachedToWindow() {
        View::onAttachedToWindow();
    }

    void TextView::onDetachedFromWindow() {
        View::onDetachedFromWindow();
    }

    void TextView::onFocusChanged(bool getFocus) {
        View::onFocusChanged(getFocus);

        if (getFocus) {
            if (is_editable_) {
                text_blink_->show();
            }
        } else {
            if (text_action_mode_ != nullptr) {
                text_action_mode_->close();
            }

            text_blink_->hide();
            base_text_->setSelection(base_text_->getSelectionStart());
            invalidate();
        }
    }

    void TextView::onWindowFocusChanged(bool windowFocus) {
        View::onWindowFocusChanged(windowFocus);

        if (hasFocus()) {
            if (windowFocus) {
                if (is_editable_ && !hasSelection()) {
                    text_blink_->show();
                }
            } else {
                text_blink_->hide();
                if (text_action_mode_ != nullptr) {
                    text_action_mode_->close();
                }
            }
        }
    }

    void TextView::onScrollChanged(int scrollX, int scrollY, int oldScrollX, int oldScrollY) {
        View::onScrollChanged(scrollX, scrollY, oldScrollX, oldScrollY);

        computeTextOffsetAtViewTop();
    }


    void TextView::onDraw(Canvas* canvas) {
        if (is_selectable_) {
            for (auto sel : sel_list_) {
                canvas->fillRect(sel->rect, sel_bg_color_);
            }
        }

        text_blink_->draw(canvas);

        Color text_color = text_color_;
        if (!isEnabled()) {
            text_color.a *= 0.5f;
        }

        canvas->drawTextLayoutWithEffect(
            this, 0.f, 0.f, text_layout_.get(), text_color);
    }

    void TextView::onMeasure(int width, int height, int width_mode, int height_mode) {
        int final_width = 0;
        int final_height = 0;

        int hori_padding = getPaddingLeft() + getPaddingRight();
        int vert_padding = getPaddingTop() + getPaddingBottom();

        switch (width_mode) {
        case FIT: {
            text_layout_->SetMaxWidth(
                static_cast<float>(width - hori_padding));

            final_width = std::min(
                static_cast<int>(
                    std::ceil(getTextWidth() + text_blink_->getThickness()))
                + hori_padding, width);

            final_width = std::max(final_width, getMinimumWidth());

            if (width != final_width) {
                text_layout_->SetMaxWidth(
                    static_cast<float>(final_width - hori_padding));
            }
            break;
        }

        case UNKNOWN:
            text_layout_->SetMaxWidth(0.f);
            text_layout_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

            final_width = static_cast<int>(
                std::ceil(getTextWidth()
                    + text_blink_->getThickness()))
                + hori_padding;
            final_width = std::max(final_width, getMinimumWidth());

            text_layout_->SetMaxWidth(
                static_cast<float>(final_width - hori_padding));
            break;

        case EXACTLY:
        default:
            text_layout_->SetMaxWidth(
                static_cast<float>(width - hori_padding));
            final_width = width;
            break;
        }

        switch (height_mode) {
        case FIT:
        {
            text_layout_->SetMaxHeight(
                static_cast<float>(height - vert_padding));

            final_height = std::min(
                static_cast<int>(std::ceil(getTextHeight()))
                + vert_padding, height);

            final_height = std::max(final_height, getMinimumHeight());

            if (final_height != height) {
                text_layout_->SetMaxHeight(
                    static_cast<float>(final_height - vert_padding));
            }
            break;
        }

        case UNKNOWN:
            text_layout_->SetMaxHeight(0.f);

            final_height = static_cast<int>(std::ceil(getTextHeight())) + vert_padding;
            final_height = std::max(final_height, getMinimumHeight());

            text_layout_->SetMaxHeight(
                static_cast<float>(final_height - vert_padding));
            break;

        case EXACTLY:
        default:
            text_layout_->SetMaxHeight(
                static_cast<float>(height - vert_padding));
            final_height = height;
            break;
        }

        setMeasuredSize(final_width, final_height);
    }

    void TextView::onLayout(
        bool changed, bool sizeChanged,
        int left, int top, int right, int bottom) {
        if (text_action_mode_ && changed) {
            text_action_mode_->invalidatePosition();
        }
    }

    void TextView::onSizeChanged(int width, int height, int oldWidth, int oldHeight) {
        int selStart = base_text_->getSelectionStart();
        int selEnd = base_text_->getSelectionEnd();

        if (selStart == selEnd && is_editable_ && is_auto_wrap_) {
            locateTextBlink(selStart);
        } else if (selStart != selEnd && is_selectable_ && is_auto_wrap_) {
            drawSelection(selStart, selEnd);
        }

        if (hasFocus() && is_editable_) {
            input_connection_->notifyTextLayoutChanged(TS_LC_CHANGE);
        }

        scrollToFit(false);
    }

    bool TextView::onInputEvent(InputEvent* e) {
        View::onInputEvent(e);

        switch (e->getEvent()) {
        case InputEvent::EV_CANCEL:
        case InputEvent::EV_LEAVE_VIEW:
        {
            is_plkey_down_ = false;
            is_plkey_down_on_text_ = false;
            is_prkey_down_ = false;
            if (e->isMouseEvent()) {
                setCurrentCursor(Cursor::ARROW);
            }
            return true;
        }

        case InputEvent::EVM_SCROLL_ENTER:
        {
            if (is_selectable_) {
                setCurrentCursor(Cursor::IBEAM);
            }
            return true;
        }

        case InputEvent::EVM_DOWN:
        {
            if (e->getMouseKey() == InputEvent::MK_LEFT ||
                e->getMouseKey() == InputEvent::MK_RIGHT)
            {
                if (text_action_mode_ != nullptr) {
                    text_action_mode_->close();
                }

                bool is_down = true;
                bool is_down_on_text = isHitText(
                    e->getX() - getPaddingLeft() + getScrollX(),
                    e->getY() - getPaddingTop() + getScrollY());

                if (is_selectable_ && (is_down_on_text || is_editable_)) {
                    setCurrentCursor(Cursor::IBEAM);
                }

                if (e->getMouseKey() == InputEvent::MK_LEFT) {
                    is_plkey_down_ = is_down;
                    is_plkey_down_on_text_ = is_down_on_text;
                } else if (e->getMouseKey() == InputEvent::MK_RIGHT) {
                    is_prkey_down_ = is_down;
                    //mIsMouseRightKeyDownOnText = isMouseKeyDownOnText;
                }

                if (e->getMouseKey() != InputEvent::MK_RIGHT || !hasSelection()) {
                    first_sel_ = getHitTextPosition(
                        e->getX() - getPaddingLeft() + getScrollX(),
                        e->getY() - getPaddingTop() + getScrollY());

                    base_text_->setSelection(first_sel_);
                }
            }
            return true;
        }

        case InputEvent::EVT_DOWN:
        {
            return true;
        }

        case InputEvent::EVM_UP:
        {
            if (e->getMouseKey() == InputEvent::MK_LEFT) {
                is_plkey_down_ = false;
                is_plkey_down_on_text_ = false;

                if (is_selectable_
                    && (isHitText(
                        e->getX() - getPaddingLeft() + getScrollX(),
                        e->getY() - getPaddingTop() + getScrollY()) || is_editable_))
                {
                    setCurrentCursor(Cursor::IBEAM);
                } else {
                    setCurrentCursor(Cursor::ARROW);
                }
            } else if (e->getMouseKey() == InputEvent::MK_RIGHT) {
                prev_x_ = e->getX();
                prev_y_ = e->getY();
                is_prkey_down_ = false;

                if (is_editable_ || is_selectable_) {
                    text_action_mode_ = getWindow()->startTextActionMode(this);
                }
            }
            return true;
        }

        case InputEvent::EVT_UP:
        {
            if (text_action_mode_ != nullptr) {
                text_action_mode_->close();
            }

            bool is_down_on_text = isHitText(
                e->getX() - getPaddingLeft() + getScrollX(),
                e->getY() - getPaddingTop() + getScrollY());

            first_sel_ = getHitTextPosition(
                e->getX() - getPaddingLeft() + getScrollX(),
                e->getY() - getPaddingTop() + getScrollY());
            base_text_->setSelection(first_sel_);
            return true;
        }

        case InputEvent::EVM_MOVE:
        {
            if (is_plkey_down_) {
                unsigned int start = first_sel_;
                unsigned int end = getHitTextPosition(
                    e->getX() - getPaddingLeft() + getScrollX(),
                    e->getY() - getPaddingTop() + getScrollY());

                last_sel_ = end;

                if (start > end) {
                    unsigned int tmp = start;
                    start = end;
                    end = tmp;
                }

                base_text_->setSelection(start, end);
            } else {
                if (is_selectable_
                    && (isHitText(
                        e->getX() - getPaddingLeft() + getScrollX(),
                        e->getY() - getPaddingTop() + getScrollY()) || is_editable_))
                {
                    setCurrentCursor(Cursor::IBEAM);
                } else {
                    setCurrentCursor(Cursor::ARROW);
                }
            }
            return true;
        }

        case InputEvent::EVT_MOVE:
        {
            return true;
        }

        case InputEvent::EVM_WHEEL:
        {
            int dy = -text_size_ * e->getMouseWheel();
            if (line_spacing_method_ == DWRITE_LINE_SPACING_METHOD_DEFAULT) {
                dy = -text_size_ * e->getMouseWheel();
            } else if (line_spacing_method_ == DWRITE_LINE_SPACING_METHOD_UNIFORM) {
                dy = -text_size_ * line_spacing_multiple_ * e->getMouseWheel();
            }

            scrollBy(0, determineVerticalScroll(dy * 3));
            break;
        }

        case InputEvent::EVK_DOWN:
        {
            text_key_listener_->onKeyDown(
                base_text_, is_editable_, is_selectable_, e->getKeyboardVirtualKey());
            if (is_editable_ && is_selectable_) {
                blinkNavigator(e->getKeyboardVirtualKey());
            }
            break;
        }

        case InputEvent::EVK_UP:
        {
            text_key_listener_->onKeyUp(
                base_text_, is_editable_, is_selectable_, e->getKeyboardVirtualKey());
            break;
        }

        default:;
        }

        return false;
    }

    bool TextView::onCheckIsTextEditor() {
        return is_editable_;
    }

    InputConnection* TextView::onCreateInputConnection() {
        return input_connection_;
    }


    void TextView::autoWrap(bool enable) {
        if (is_auto_wrap_ == enable) {
            return;
        }

        is_auto_wrap_ = enable;
        text_layout_->SetWordWrapping(
            enable ? DWRITE_WORD_WRAPPING_EMERGENCY_BREAK : DWRITE_WORD_WRAPPING_NO_WRAP);

        requestLayout();
        invalidate();
    }

    void TextView::setIsEditable(bool editable) {
        if (editable == is_editable_) {
            return;
        }

        is_editable_ = editable;

        if (editable) {
            setFocusable(true);
            if (hasFocus()) {
                text_blink_->show();
            }
        } else {
            setFocusable(is_selectable_);
            text_blink_->hide();
        }
    }

    void TextView::setIsSelectable(bool selectable) {
        if (selectable == is_selectable_) {
            return;
        }

        is_selectable_ = selectable;

        if (selectable) {
            setFocusable(true);
        } else {
            setFocusable(is_editable_);

            if (!sel_list_.empty()) {
                sel_list_.clear();
                invalidate();
            }
        }
    }


    bool TextView::isAutoWrap() const {
        return is_auto_wrap_;
    }

    bool TextView::isEditable() const {
        return is_editable_;
    }

    bool TextView::isSelectable() const {
        return is_selectable_;
    }


    void TextView::setText(const string16& text) {
        base_text_->replace(text, 0, base_text_->length());
        base_text_->setSelection(0);
    }

    void TextView::setTextSize(int size) {
        size = int(std::round(getWindow()->dpToPx(size)));
        if (size == text_size_) {
            return;
        }

        text_size_ = size;

        makeNewTextFormat();
        makeNewTextLayout(
            text_layout_->GetMaxWidth(),
            text_layout_->GetMaxHeight(), is_auto_wrap_);

        requestLayout();
        invalidate();
    }

    void TextView::setTextColor(Color color) {
        text_color_ = color;
        invalidate();
    }

    void TextView::setTextAlignment(DWRITE_TEXT_ALIGNMENT alignment) {
        if (text_alignment_ == alignment) {
            return;
        }

        text_alignment_ = alignment;
        text_layout_->SetTextAlignment(alignment);
    }

    void TextView::setParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT alignment) {
        if (paragraph_alignment_ == alignment) {
            return;
        }

        paragraph_alignment_ = alignment;
        text_layout_->SetParagraphAlignment(alignment);
    }

    void TextView::setTextStyle(DWRITE_FONT_STYLE style) {
        if (text_style_ == style) {
            return;
        }

        text_style_ = style;

        DWRITE_TEXT_RANGE range;
        range.startPosition = 0;
        range.length = base_text_->length();

        text_layout_->SetFontStyle(text_style_, range);

        requestLayout();
        invalidate();
    }

    void TextView::setTextWeight(DWRITE_FONT_WEIGHT weight) {
        if (text_weight_ == weight) {
            return;
        }

        text_weight_ = weight;

        DWRITE_TEXT_RANGE range;
        range.startPosition = 0;
        range.length = base_text_->length();

        text_layout_->SetFontWeight(text_weight_, range);

        requestLayout();
        invalidate();
    }

    void TextView::setFontFamilyName(const string16& font) {
        if (font_family_name_ == font) {
            return;
        }

        font_family_name_ = font;

        makeNewTextFormat();
        makeNewTextLayout(
            text_layout_->GetMaxWidth(),
            text_layout_->GetMaxHeight(), is_auto_wrap_);

        requestLayout();
        invalidate();
    }

    void TextView::setLineSpacing(bool uniform, float spacingMultiple) {
        if (uniform) {
            line_spacing_method_ = DWRITE_LINE_SPACING_METHOD_UNIFORM;
        } else {
            line_spacing_method_ = DWRITE_LINE_SPACING_METHOD_DEFAULT;
        }

        line_spacing_multiple_ = spacingMultiple;

        text_layout_->SetLineSpacing(
            line_spacing_method_,
            text_size_*line_spacing_multiple_,
            text_size_*line_spacing_multiple_*0.8f);

        requestLayout();
        invalidate();
    }


    string16 TextView::getText() const {
        return base_text_->toString();
    }

    Editable* TextView::getEditable() const {
        return base_text_;
    }

    float TextView::getTextSize() const {
        return text_size_;
    }


    void TextView::setSelection(unsigned int position) {
        base_text_->setSelection(position);
    }

    void TextView::setSelection(unsigned int start, unsigned int end) {
        base_text_->setSelection(start, end);
    }

    void TextView::drawSelection(unsigned int start, unsigned int end) {
        UINT hitTextMetricsCount = 0;
        if (end - start <= 0) {
            return;
        }

        text_layout_->HitTestTextRange(
            start, end - start,
            0.f, 0.f, nullptr, 0,
            &hitTextMetricsCount);

        if (hitTextMetricsCount < 1) {
            return;
        }

        auto hitTextMetrics = new DWRITE_HIT_TEST_METRICS[hitTextMetricsCount];
        text_layout_->HitTestTextRange(
            start, end - start,
            0.f, 0.f,
            hitTextMetrics, hitTextMetricsCount,
            &hitTextMetricsCount);

        sel_list_.clear();

        for (UINT i = 0; i < hitTextMetricsCount; i++) {
            SelectionBlock* block = new SelectionBlock();

            float extraWidth = 0.f;
            uint32_t tPos = hitTextMetrics[i].textPosition;
            uint32_t tLength = hitTextMetrics[i].length;

            //一行中只有\n或\r\n时，添加一个一定宽度的Selection。
            if ((tLength == 1 && base_text_->at(tPos) == L'\n')
                || (tLength == 2 && base_text_->at(tPos) == L'\r'
                    && base_text_->at(tPos + 1) == L'\n')) {
                extraWidth = text_size_;
            }
            //一行中句尾有\n或\r\n时，句尾添加一个一定宽度的Selection。
            else if ((tLength > 0 && base_text_->at(tPos + tLength - 1) == L'\n')
                || (tLength > 1 && base_text_->at(tPos + tLength - 2) == L'\r'
                    && base_text_->at(tPos + tLength - 1) == L'\n')) {
                extraWidth = text_size_;
            }

            block->rect.left = std::floor(hitTextMetrics[i].left);
            block->rect.top = std::floor(hitTextMetrics[i].top);
            block->rect.right = std::ceil(hitTextMetrics[i].left + hitTextMetrics[i].width + extraWidth);
            block->rect.bottom = std::ceil(hitTextMetrics[i].top + hitTextMetrics[i].height);
            block->start = hitTextMetrics[i].textPosition;
            block->length = hitTextMetrics[i].length;
            sel_list_.push_back(std::shared_ptr<SelectionBlock>(block));
        }

        delete[] hitTextMetrics;

        invalidate();
    }

    std::wstring TextView::getSelection() const {
        return base_text_->getSelection();
    }

    int TextView::getSelectionStart() const {
        return base_text_->getSelectionStart();
    }

    int TextView::getSelectionEnd() const {
        return base_text_->getSelectionEnd();
    }

    bool TextView::hasSelection() const {
        return base_text_->hasSelection();
    }


    uint32_t TextView::getHitTextPosition(float textX, float textY) const {
        BOOL isInside;
        BOOL isTrailingHit;
        DWRITE_HIT_TEST_METRICS metrics;

        text_layout_->HitTestPoint(
            textX, textY, &isTrailingHit, &isInside, &metrics);

        return metrics.textPosition + (isTrailingHit == TRUE ? 1 : 0);
    }

    bool TextView::isHitText(float textX, float textY, uint32_t* hitPos) const {
        BOOL isInside;
        BOOL isTrailingHit;
        DWRITE_HIT_TEST_METRICS metrics;

        text_layout_->HitTestPoint(
            textX, textY, &isTrailingHit, &isInside, &metrics);

        if (hitPos) {
            *hitPos = metrics.textPosition + (isTrailingHit == TRUE ? 1 : 0);
        }

        return isInside == TRUE ? true : false;
    }

    bool TextView::isHitText(
        float textX, float textY,
        uint32_t position, uint32_t length, uint32_t* hitPos) const {
        BOOL isInside;
        BOOL isTrailingHit;
        DWRITE_HIT_TEST_METRICS metrics;

        text_layout_->HitTestPoint(
            textX, textY, &isTrailingHit, &isInside, &metrics);

        if (hitPos) {
            *hitPos = metrics.textPosition + (isTrailingHit == TRUE ? 1 : 0);
        }

        if (isInside == TRUE) {
            if (metrics.textPosition >= position
                && metrics.textPosition <= position + length - 1) {
                return true;
            }
        }

        return false;
    }

    RectF TextView::getSelectionBound(unsigned int start, unsigned int end) const {
        if (end < start) {
            return RectF();
        }

        RectF bound;

        if (end == start) {
            float pointX;
            float pointY;
            DWRITE_HIT_TEST_METRICS metrics;

            text_layout_->HitTestTextPosition(start, FALSE, &pointX, &pointY, &metrics);

            bound.left = pointX;
            bound.top = pointY;
            bound.right = pointX;
            bound.bottom = pointY + metrics.height;
        } else {
            UINT hitTextMetricsCount = 0;

            text_layout_->HitTestTextRange(
                start, end - start,
                0.f, 0.f,
                nullptr, 0,
                &hitTextMetricsCount);

            if (hitTextMetricsCount < 1) {
                return RectF();
            }

            auto hitTextMetrics = new DWRITE_HIT_TEST_METRICS[hitTextMetricsCount];
            text_layout_->HitTestTextRange(
                start, end - start,
                0.f, 0.f,
                hitTextMetrics, hitTextMetricsCount,
                &hitTextMetricsCount);

            for (UINT i = 0; i < hitTextMetricsCount; i++) {
                if (i == 0) {
                    bound.left = hitTextMetrics[i].left;
                    bound.top = hitTextMetrics[i].top;
                    bound.right = hitTextMetrics[i].left + hitTextMetrics[i].width;
                    bound.bottom = hitTextMetrics[i].top + hitTextMetrics[i].height;
                    continue;
                }

                if (bound.left > hitTextMetrics[i].left) {
                    bound.left = hitTextMetrics[i].left;
                }

                if (bound.top > hitTextMetrics[i].top) {
                    bound.top = hitTextMetrics[i].top;
                }

                if (bound.right < hitTextMetrics[i].left + hitTextMetrics[i].width) {
                    bound.right = hitTextMetrics[i].left + hitTextMetrics[i].width;
                }

                if (bound.bottom < hitTextMetrics[i].top + hitTextMetrics[i].height) {
                    bound.bottom = hitTextMetrics[i].top + hitTextMetrics[i].height;
                }
            }

            delete[] hitTextMetrics;
        }

        return bound;
    }


    void TextView::computeVisibleRegion(RectF* visibleRegon) {
        visibleRegon->left = 0.f + getScrollX();
        visibleRegon->top = 0.f + getScrollY();
        visibleRegon->right = visibleRegon->left + text_layout_->GetMaxWidth();
        visibleRegon->bottom = visibleRegon->top + text_layout_->GetMaxHeight();
    }


    void TextView::onTextChanged(
        Editable* editable,
        int start, int oldEnd, int newEnd) {

        float maxWidth = text_layout_->GetMaxWidth();
        float maxHeight = text_layout_->GetMaxHeight();

        makeNewTextLayout(maxWidth, maxHeight, isAutoWrap());
        requestLayout();
        invalidate();

        if (process_ref_ == 0 && hasFocus() && is_editable_) {
            input_connection_->notifyTextChanged(false, start, oldEnd, newEnd);
        }

        scrollToFit(false);
    }

    void TextView::onSelectionChanged(
        unsigned int ns, unsigned int ne,
        unsigned int os, unsigned int oe) {

        if (ns == ne) {
            if (os != oe) {
                if (text_action_mode_) {
                    text_action_mode_->close();
                }

                sel_list_.clear();
                invalidate();
            }

            if (is_editable_) {
                locateTextBlink(ns);
                if (hasFocus()) {
                    text_blink_->show();
                }
            }
        } else {
            setSelection(ns, ne);
            text_blink_->hide();
            drawSelection(ns, ne);
        }

        if (process_ref_ == 0 && hasFocus() && is_editable_) {
            input_connection_->notifyTextSelectionChanged();
        }

        scrollToFit(true);
    }

    void TextView::onSpanChanged(Span* span, SpanChange action) {
        DWRITE_TEXT_RANGE range;
        range.startPosition = span->getStart();
        range.length = span->getEnd() - span->getStart();

        switch (span->getBaseType()) {
        case Span::TEXT: {
            switch (span->getType()) {
            case Span::TEXT_UNDERLINE:
                if (action == SpanChange::ADD) {
                    text_layout_->SetUnderline(TRUE, range);
                } else if (action == SpanChange::REMOVE) {
                    text_layout_->SetUnderline(FALSE, range);
                }
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }

        case Span::EFFECT:
            if (action == SpanChange::ADD) {
                TextDrawingEffect* tdEffect = new TextDrawingEffect();
                tdEffect->effect_span_ = reinterpret_cast<EffectSpan*>(span);

                text_layout_->SetDrawingEffect(tdEffect, range);
            } else if (action == SpanChange::REMOVE) {
                text_layout_->SetDrawingEffect(nullptr, range);
            }
            break;

        case Span::INTERACTABLE:
            break;

        case Span::INLINEOBJECT:
            break;
        }
    }


    bool TextView::canCut() const {
        return is_editable_ && base_text_->hasSelection();
    }

    bool TextView::canCopy() const {
        return base_text_->hasSelection();
    }

    bool TextView::canPaste() const {
        if (is_editable_) {
            std::wstring content = ClipboardManager::getFromClipboard();
            return !content.empty();
        }
        return false;
    }

    bool TextView::canSelectAll() const {
        if (base_text_->length() == 0) {
            return false;
        }

        if (base_text_->getSelectionStart() == 0
            && base_text_->getSelectionEnd() == base_text_->length()) {
            return false;
        }

        return true;
    }

    void TextView::performCut() {
        ClipboardManager::saveToClipboard(getSelection());
        base_text_->replace(L"");

        if (text_action_mode_) {
            text_action_mode_->close();
        }
    }

    void TextView::performCopy() {
        ClipboardManager::saveToClipboard(getSelection());

        if (text_action_mode_) {
            text_action_mode_->close();
        }
    }

    void TextView::performPaste() {
        std::wstring content = ClipboardManager::getFromClipboard();
        if (hasSelection()) {
            base_text_->replace(content);
        } else {
            base_text_->insert(content);
        }

        if (text_action_mode_) {
            text_action_mode_->close();
        }
    }

    void TextView::performSelectAll() {
        setSelection(0, base_text_->length());
        text_action_mode_->invalidateMenu();
    }


    bool TextView::onCreateActionMode(TextActionMode* mode, Menu* menu) {
        if (this->canCopy())
            menu->addItem(MENU_ID_COPY, MENU_ORDER_COPY, L"复制");

        if (this->canCut())
            menu->addItem(MENU_ID_CUT, MENU_ORDER_CUT, L"剪切");

        if (this->canPaste())
            menu->addItem(MENU_ID_PASTE, MENU_ORDER_PASTE, L"粘贴");

        if (this->canSelectAll())
            menu->addItem(MENU_ID_SELECTALL, MENU_ORDER_SELECTALL, L"全选");

        return true;
    }

    bool TextView::onPrepareActionMode(TextActionMode* mode, Menu* menu) {
        bool canCopy = this->canCopy();
        MenuItem* copyItem = menu->findItem(MENU_ID_COPY);
        if (copyItem != nullptr) {
            copyItem->setItemVisible(canCopy);
        } else if (canCopy) {
            menu->addItem(MENU_ID_COPY, MENU_ORDER_COPY, L"复制");
        }

        bool canCut = this->canCut();
        MenuItem* cutItem = menu->findItem(MENU_ID_CUT);
        if (cutItem != nullptr) {
            cutItem->setItemVisible(canCut);
        } else if (canCut) {
            menu->addItem(MENU_ID_CUT, MENU_ORDER_CUT, L"剪切");
        }

        bool canPaste = this->canPaste();
        MenuItem* pasteItem = menu->findItem(MENU_ID_PASTE);
        if (pasteItem != nullptr) {
            pasteItem->setItemVisible(canPaste);
        } else if (canPaste) {
            menu->addItem(MENU_ID_PASTE, MENU_ORDER_PASTE, L"粘贴");
        }

        bool canSelectAll = this->canSelectAll();
        MenuItem* selectAllItem = menu->findItem(MENU_ID_SELECTALL);
        if (selectAllItem != nullptr) {
            selectAllItem->setItemVisible(canSelectAll);
        } else if (canSelectAll) {
            menu->addItem(MENU_ID_SELECTALL, MENU_ORDER_SELECTALL, L"全选");
        }

        return true;
    }

    bool TextView::onActionItemClicked(TextActionMode* mode, MenuItem* item) {
        switch (item->getItemId()) {
        case MENU_ID_COPY:
            performCopy();
            break;
        case MENU_ID_CUT:
            performCut();
            break;
        case MENU_ID_PASTE:
            performPaste();
            break;
        case MENU_ID_SELECTALL:
            performSelectAll();
            break;
        default:
            break;
        }

        return true;
    }

    void TextView::onDestroyActionMode(TextActionMode* mode) {
        text_action_mode_ = nullptr;
    }

    void TextView::onGetContentPosition(int* x, int* y) {
        int left = 0;
        int top = 0;
        View* parent = this;
        while (parent) {
            left += parent->getLeft() - parent->getScrollX();
            top += parent->getTop() - parent->getScrollY();
            parent = parent->getParent();
        }

        *x = left + prev_x_ + 1;
        *y = top + prev_y_ + 1;
    }

}