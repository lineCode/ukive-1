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

    TextView::TextView(Window *wnd)
        :View(wnd),
        mTextColor(Color::Black),
        mSelectionBackgroundColor(Color::Blue200) {
        initTextView();
    }

    TextView::~TextView()
    {
        delete mBaseText;
        delete mTextBlink;
        delete mTextKeyListener;
    }


    void TextView::initTextView()
    {
        mTextSize = static_cast<int>(std::round(getWindow()->dpToPx(15.f)));
        mIsAutoWrap = true;
        mIsEditable = false;
        mIsSelectable = false;
        mIsMouseLeftKeyDown = false;
        mIsMouseRightKeyDown = false;
        mIsMouseLeftKeyDownOnText = false;

        mLineSpacingMultiple = 1.f;
        mLineSpacingMethod = DWRITE_LINE_SPACING_METHOD_DEFAULT;

        mTextOffsetAtViewTop = 0;

        mProcessRef = 0;
        mFontFamilyName = L"微软雅黑";
        mTextActionMode = nullptr;
        mTextAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
        mParagraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
        mTextWeight = DWRITE_FONT_WEIGHT_NORMAL;
        mTextStyle = DWRITE_FONT_STYLE_NORMAL;

        mTextBlink = new TextBlink(this);
        mTextBlink->setColor(Color::Blue500);

        mInputConnection = new InputConnection(this);
        mTextKeyListener = new TextKeyListener();

        mBaseText = new Editable(L"");
        mBaseText->addEditWatcher(this);

        makeNewTextFormat();
        makeNewTextLayout(0.f, 0.f, false);

        mBaseText->setSelection(0);
        locateTextBlink(0);

        setFocusable(mIsEditable | mIsSelectable);
    }


    int TextView::computeVerticalScrollRange()
    {
        int textHeight = (int)std::ceil(getTextHeight());
        int contentHeight = getHeight() - getPaddingTop() - getPaddingBottom();

        return std::max(0, textHeight - contentHeight);
    }

    int TextView::computeHorizontalScrollRange()
    {
        int textWidth = (int)std::ceil(getTextWidth());
        int contentWidth = getWidth() - getPaddingLeft() - getPaddingRight();

        return std::max(0, textWidth - contentWidth);
    }


    void TextView::computeTextOffsetAtViewTop()
    {
        BOOL isInside;
        BOOL isTrailingHit;
        DWRITE_HIT_TEST_METRICS metrics;

        mTextLayout->HitTestPoint(
            0.f, (float)getScrollY(), &isTrailingHit, &isInside, &metrics);

        mTextOffsetAtViewTop = metrics.textPosition + (isTrailingHit == TRUE ? 1 : 0);
        mVerticalOffset = (float)getScrollY() - metrics.top;
    }

    int TextView::computeVerticalScrollOffsetFromTextOffset(uint32_t tOff)
    {
        uint32_t actual;
        DWRITE_HIT_TEST_METRICS hitMetrics;

        mTextLayout->HitTestTextRange(
            tOff, 0, 0.f, 0.f, &hitMetrics, 1, &actual);

        return (int)hitMetrics.top - getScrollY();
    }

    int TextView::determineVerticalScroll(int dy)
    {
        int finalY = getScrollY();
        int range = computeVerticalScrollRange();
        if (range > 0)
        {
            if (dy < 0)
                finalY = std::max(0, getScrollY() + dy);
            else if (dy > 0)
                finalY = std::min(range, getScrollY() + dy);
        }

        return finalY - getScrollY();
    }

    int TextView::determineHorizontalScroll(int dx)
    {
        int finalX = getScrollX();
        int range = computeHorizontalScrollRange();
        if (range > 0)
        {
            if (dx < 0)
                finalX = std::max(0, getScrollX() + dx);
            else if (dx > 0)
                finalX = std::min(range, getScrollX() + dx);
        }

        return finalX - getScrollX();
    }


    void TextView::scrollToFit(bool considerSelection)
    {
        bool scrolled = false;

        if (computeVerticalScrollRange() == 0)
        {
            if (getScrollY() != 0)
            {
                scrollTo(0, 0);
                scrolled = true;
            }
        }
        else
        {
            int scrollOffset = 0;

            if (considerSelection)
            {
                uint32_t selection;
                if (hasSelection())
                    selection = mLastSelection;
                else
                    selection = getSelectionStart();

                uint32_t actual;
                DWRITE_HIT_TEST_METRICS metrics;
                mTextLayout->HitTestTextRange(
                    selection, 0, 0.f, 0.f, &metrics, 1, &actual);

                int lineTop = (int)metrics.top;
                int lineBottom = (int)(metrics.top + metrics.height);
                int contentHeight = getHeight() - getPaddingTop() - getPaddingBottom();

                if (lineTop < getScrollY())
                    scrollOffset = lineTop - getScrollY();
                if (lineBottom > getScrollY() + contentHeight)
                    scrollOffset = lineBottom - (getScrollY() + contentHeight);
            }
            else
            {
                //置于大小变化前的位置。
                uint32_t prevTextOffset = mTextOffsetAtViewTop;
                float prevVerticalOffset = mVerticalOffset;

                BOOL isInside;
                BOOL isTrailingHit;
                DWRITE_HIT_TEST_METRICS metrics;

                mTextLayout->HitTestPoint(
                    0.f, (float)getScrollY(), &isTrailingHit, &isInside, &metrics);
                uint32_t curTextOffset = metrics.textPosition + (isTrailingHit == TRUE ? 1 : 0);

                if (curTextOffset != prevTextOffset)
                    scrollOffset = computeVerticalScrollOffsetFromTextOffset(prevTextOffset);
            }

            //检查是否超出屏幕。
            int maxScrollOffset = computeVerticalScrollRange() - getScrollY();
            int offsetY = std::min(maxScrollOffset, scrollOffset);
            if (offsetY != 0)
            {
                scrollBy(0, offsetY);
                scrolled = true;
            }
        }
    }


    float TextView::getTextWidth()
    {
        DWRITE_TEXT_METRICS metrics;
        mTextLayout->GetMetrics(&metrics);
        return metrics.width;
    }

    float TextView::getTextHeight()
    {
        DWRITE_TEXT_METRICS metrics;
        mTextLayout->GetMetrics(&metrics);
        return metrics.height;
    }

    bool TextView::getLineInfo(
        uint32_t position, uint32_t *line, float *height, uint32_t *count)
    {
        uint32_t lineCount;
        HRESULT hr = mTextLayout->GetLineMetrics(nullptr, 0, &lineCount);
        if (hr == E_NOT_SUFFICIENT_BUFFER && lineCount > 0)
        {
            DWRITE_LINE_METRICS *lineMetrics = new DWRITE_LINE_METRICS[lineCount];
            hr = mTextLayout->GetLineMetrics(lineMetrics, lineCount, &lineCount);
            if (SUCCEEDED(hr))
            {
                uint32_t lineTextPosStart = 0;
                for (uint32_t i = 0; i < lineCount; ++i)
                {
                    uint32_t lineTextPosEnd
                        = lineTextPosStart + lineMetrics[i].length;
                    if (position >= lineTextPosStart && position <= lineTextPosEnd)
                    {
                        *line = i;
                        *height = lineMetrics[i].height;
                        if (count != nullptr)
                            *count = lineCount;
                        return true;
                    }

                    lineTextPosStart = lineTextPosEnd;
                }
            }
        }

        return false;
    }

    bool TextView::getLineHeight(
        uint32_t line, float *height)
    {
        uint32_t lineCount;
        HRESULT hr = mTextLayout->GetLineMetrics(nullptr, 0, &lineCount);
        if (hr == E_NOT_SUFFICIENT_BUFFER && lineCount > 0)
        {
            DWRITE_LINE_METRICS *lineMetrics = new DWRITE_LINE_METRICS[lineCount];
            hr = mTextLayout->GetLineMetrics(lineMetrics, lineCount, &lineCount);
            if (SUCCEEDED(hr))
            {
                if (line >= 0 && line < lineCount)
                {
                    *height = lineMetrics[line].height;
                    return true;
                }
            }
        }

        return false;
    }

    void TextView::blinkNavigator(int keyCode)
    {
        if (keyCode == VK_LEFT)
        {
            uint32_t start = mBaseText->getSelectionStart();

            if (mBaseText->hasSelection())
                mBaseText->setSelection(start);
            else
            {
                if (start > 0)
                {
                    size_t len = 1;
                    size_t index = start - 1;
                    wchar_t prev = mBaseText->at(index);
                    if (prev == L'\n' && index > 0)
                    {
                        prev = mBaseText->at(index - 1);
                        if (prev == L'\r')
                            len = 2;
                    }

                    mBaseText->setSelection(start - len);
                }
            }
        }
        else if (keyCode == VK_RIGHT)
        {
            uint32_t end = mBaseText->getSelectionEnd();

            if (mBaseText->hasSelection())
                mBaseText->setSelection(end);
            else
            {
                if (end < mBaseText->length())
                {
                    size_t len = 1;
                    size_t index = end;
                    wchar_t next = mBaseText->at(index);
                    if (next == L'\r' && (++index) < mBaseText->length())
                    {
                        next = mBaseText->at(index);
                        if (next == L'\n')
                            len = 2;
                    }

                    mBaseText->setSelection(end + len);
                }
            }
        }
        else if (keyCode == VK_UP)
        {
            uint32_t start;
            if (hasSelection() && (mLastSelection == mBaseText->getSelectionStart()
                || mLastSelection == mBaseText->getSelectionEnd()))
                start = mLastSelection;
            else
                start = mBaseText->getSelectionStart();

            uint32_t actual;
            DWRITE_HIT_TEST_METRICS rangeMetrics;
            mTextLayout->HitTestTextRange(
                start, 0, 0.f, 0.f, &rangeMetrics, 1, &actual);

            BOOL isTrailing, isInside;
            DWRITE_HIT_TEST_METRICS pointMetrics;
            mTextLayout->HitTestPoint(
                rangeMetrics.left, rangeMetrics.top - 1.f,
                &isTrailing, &isInside, &pointMetrics);

            mBaseText->setSelection(
                pointMetrics.textPosition + (isTrailing == TRUE ? 1 : 0));
        }
        else if (keyCode == VK_DOWN)
        {
            uint32_t start;
            if (hasSelection() && (mLastSelection == mBaseText->getSelectionStart()
                || mLastSelection == mBaseText->getSelectionEnd()))
                start = mLastSelection;
            else
                start = mBaseText->getSelectionStart();

            uint32_t actual;
            DWRITE_HIT_TEST_METRICS rangeMetrics;
            mTextLayout->HitTestTextRange(
                start, 0, 0.f, 0.f, &rangeMetrics, 1, &actual);

            BOOL isTrailing, isInside;
            DWRITE_HIT_TEST_METRICS pointMetrics;
            mTextLayout->HitTestPoint(
                rangeMetrics.left, rangeMetrics.top + rangeMetrics.height,
                &isTrailing, &isInside, &pointMetrics);

            mBaseText->setSelection(
                pointMetrics.textPosition + (isTrailing == TRUE ? 1 : 0));
        }
    }


    void TextView::makeNewTextFormat()
    {
        mTextFormat.reset();
        getWindow()->getRenderer()->createTextFormat(
            mFontFamilyName,
            mTextSize,
            L"en-US",
            &mTextFormat);
    }

    void TextView::makeNewTextLayout(float maxWidth, float maxHeight, bool autoWrap)
    {
        mTextLayout.reset();

        DWRITE_TEXT_RANGE range;
        range.startPosition = 0;
        range.length = mBaseText->length();

        getWindow()->getRenderer()->createTextLayout(
            mBaseText->toString(),
            mTextFormat.get(),
            maxWidth,
            maxHeight,
            &mTextLayout);

        mTextLayout->SetLineSpacing(
            mLineSpacingMethod,
            mTextSize*mLineSpacingMultiple,
            mTextSize*mLineSpacingMultiple*0.8f);

        mTextLayout->SetWordWrapping(
            autoWrap ? DWRITE_WORD_WRAPPING_EMERGENCY_BREAK : DWRITE_WORD_WRAPPING_NO_WRAP);
        mTextLayout->SetTextAlignment(mTextAlignment);
        mTextLayout->SetParagraphAlignment(mParagraphAlignment);

        mTextLayout->SetFontStyle(mTextStyle, range);
        mTextLayout->SetFontWeight(mTextWeight, range);
    }


    void TextView::locateTextBlink(int position)
    {
        DWRITE_HIT_TEST_METRICS hitMetrics;

        if (mLineSpacingMethod == DWRITE_LINE_SPACING_METHOD_UNIFORM)
        {
            UINT32 actual;
            mTextLayout->HitTestTextRange(position, 0, 0.f, 0.f, &hitMetrics, 1, &actual);
        }
        else
        {
            float pointX, pointY;
            mTextLayout->HitTestTextPosition(position, FALSE, &pointX, &pointY, &hitMetrics);
        }

        mTextBlink->locate(hitMetrics.left, hitMetrics.top, hitMetrics.top + hitMetrics.height);
    }

    void TextView::locateTextBlink(float textX, float textY)
    {
        int hitPosition = this->getHitTextPosition(textX, textY);
        locateTextBlink(hitPosition);
    }


    void TextView::onBeginProcess()
    {
        ++mProcessRef;
    }

    void TextView::onEndProcess()
    {
        --mProcessRef;
    }


    void TextView::onAttachedToWindow()
    {
        View::onAttachedToWindow();
    }

    void TextView::onDetachedFromWindow()
    {
        View::onDetachedFromWindow();
    }

    void TextView::onFocusChanged(bool getFocus)
    {
        View::onFocusChanged(getFocus);

        if (getFocus) {
            if (mIsEditable) {
                mTextBlink->show();
            }
        } else {
            if (mTextActionMode != nullptr) {
                mTextActionMode->close();
            }

            mTextBlink->hide();
            mBaseText->setSelection(mBaseText->getSelectionStart());
            invalidate();
        }
    }

    void TextView::onWindowFocusChanged(bool windowFocus)
    {
        View::onWindowFocusChanged(windowFocus);

        if (hasFocus()) {
            if (windowFocus) {
                if (mIsEditable && !hasSelection()) {
                    mTextBlink->show();
                }
            } else {
                mTextBlink->hide();
                if (mTextActionMode != nullptr) {
                    mTextActionMode->close();
                }
            }
        }
    }

    void TextView::onScrollChanged(int scrollX, int scrollY, int oldScrollX, int oldScrollY)
    {
        View::onScrollChanged(scrollX, scrollY, oldScrollX, oldScrollY);

        computeTextOffsetAtViewTop();
    }


    void TextView::onDraw(Canvas *canvas)
    {
        if (mIsSelectable)
        {
            for (auto it = mSelectionList.begin();
                it != mSelectionList.end(); ++it)
            {
                canvas->fillRect((*it)->rect, mSelectionBackgroundColor);
            }
        }

        mTextBlink->draw(canvas);

        canvas->drawTextLayoutWithEffect(
            this, 0.f, 0.f, mTextLayout.get(), mTextColor);
    }

    void TextView::onMeasure(int width, int height, int widthSpec, int heightSpec)
    {
        int finalWidth = 0;
        int finalHeight = 0;

        int horizontalPadding = getPaddingLeft() + getPaddingRight();
        int verticalPadding = getPaddingTop() + getPaddingBottom();

        switch (widthSpec)
        {
        case FIT:
        {
            mTextLayout->SetMaxWidth(
                static_cast<float>(width - horizontalPadding));

            finalWidth = std::min(
                static_cast<int>(
                    std::ceil(getTextWidth() + mTextBlink->getThickness()))
                + horizontalPadding, width);

            finalWidth = std::max(finalWidth, getMinimumWidth());

            if (width != finalWidth)
            {
                mTextLayout->SetMaxWidth(
                    static_cast<float>(finalWidth - horizontalPadding));
            }
            break;
        }

        case EXACTLY:
            mTextLayout->SetMaxWidth(
                static_cast<float>(width - horizontalPadding));
            finalWidth = width;
            break;

        case UNKNOWN:
            mTextLayout->SetMaxWidth(0.f);
            mTextLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

            finalWidth = static_cast<int>(
                std::ceil(getTextWidth()
                    + mTextBlink->getThickness()))
                + horizontalPadding;
            finalWidth = std::max(finalWidth, getMinimumWidth());

            mTextLayout->SetMaxWidth(
                static_cast<float>(finalWidth - horizontalPadding));
            break;
        }

        switch (heightSpec)
        {
        case FIT:
        {
            mTextLayout->SetMaxHeight(
                static_cast<float>(height - verticalPadding));

            finalHeight = std::min(
                static_cast<int>(std::ceil(getTextHeight()))
                + verticalPadding, height);

            finalHeight = std::max(finalHeight, getMinimumHeight());

            if (finalHeight != height)
            {
                mTextLayout->SetMaxHeight(
                    static_cast<float>(finalHeight - verticalPadding));
            }
            break;
        }

        case EXACTLY:
            mTextLayout->SetMaxHeight(
                static_cast<float>(height - verticalPadding));
            finalHeight = height;
            break;

        case UNKNOWN:
            mTextLayout->SetMaxHeight(0.f);

            finalHeight = static_cast<int>(std::ceil(getTextHeight())) + verticalPadding;
            finalHeight = std::max(finalHeight, getMinimumHeight());

            mTextLayout->SetMaxHeight(
                static_cast<float>(finalHeight - verticalPadding));
            break;
        }

        setMeasuredDimension(finalWidth, finalHeight);
    }

    void TextView::onLayout(
        bool changed, bool sizeChanged,
        int left, int top, int right, int bottom)
    {
        if (mTextActionMode && changed) {
            mTextActionMode->invalidatePosition();
        }
    }

    void TextView::onSizeChanged(int width, int height, int oldWidth, int oldHeight)
    {
        int selStart = mBaseText->getSelectionStart();
        int selEnd = mBaseText->getSelectionEnd();

        if (selStart == selEnd && mIsEditable && mIsAutoWrap)
            locateTextBlink(selStart);
        else if (selStart != selEnd && mIsSelectable && mIsAutoWrap)
            drawSelection(selStart, selEnd);

        if (hasFocus() && mIsEditable)
            mInputConnection->notifyTextLayoutChanged(TS_LC_CHANGE);

        scrollToFit(false);
    }

    bool TextView::onInputEvent(InputEvent *e)
    {
        View::onInputEvent(e);

        switch (e->getEvent())
        {
        case InputEvent::EVM_LEAVE_OBJ:
        {
            setCurrentCursor(Cursor::ARROW);
            return true;
        }

        case InputEvent::EVM_SCROLL_ENTER:
        {
            if (mIsSelectable)
                setCurrentCursor(Cursor::IBEAM);
            return true;
        }

        case InputEvent::EVM_DOWN:
        {
            bool isMouseKeyDown = false;
            bool isMouseKeyDownOnText = false;

            if (e->getMouseKey() == InputEvent::MK_LEFT
                || e->getMouseKey() == InputEvent::MK_RIGHT)
            {
                if (mTextActionMode != nullptr)
                    mTextActionMode->close();

                isMouseKeyDown = true;
                isMouseKeyDownOnText = isHitText(
                    e->getMouseX() - getPaddingLeft() + getScrollX(),
                    e->getMouseY() - getPaddingTop() + getScrollY());

                if (mIsSelectable && isMouseKeyDownOnText)
                    setCurrentCursor(Cursor::IBEAM);

                if (e->getMouseKey() == InputEvent::MK_LEFT)
                {
                    mIsMouseLeftKeyDown = isMouseKeyDown;
                    mIsMouseLeftKeyDownOnText = isMouseKeyDownOnText;
                }
                else if (e->getMouseKey() == InputEvent::MK_RIGHT)
                {
                    mIsMouseRightKeyDown = isMouseKeyDown;
                    //mIsMouseRightKeyDownOnText = isMouseKeyDownOnText;
                }

                if (e->getMouseKey() != InputEvent::MK_RIGHT || !hasSelection())
                {
                    mFirstSelection = getHitTextPosition(
                        e->getMouseX() - getPaddingLeft() + getScrollX(),
                        e->getMouseY() - getPaddingTop() + getScrollY());

                    mBaseText->setSelection(mFirstSelection);
                }
            }
            return true;
        }

        case InputEvent::EVM_UP:
        {
            if (e->getMouseKey() == InputEvent::MK_LEFT)
            {
                mIsMouseLeftKeyDown = false;
                mIsMouseLeftKeyDownOnText = false;

                if (mIsSelectable
                    && (isHitText(
                        e->getMouseX() - getPaddingLeft() + getScrollX(),
                        e->getMouseY() - getPaddingTop() + getScrollY()) || mIsEditable))
                    setCurrentCursor(Cursor::IBEAM);
                else
                    setCurrentCursor(Cursor::ARROW);
            }
            else if (e->getMouseKey() == InputEvent::MK_RIGHT)
            {
                mPrevX = e->getMouseX();
                mPrevY = e->getMouseY();
                mIsMouseRightKeyDown = false;

                if (mIsEditable || mIsSelectable)
                    mTextActionMode = getWindow()->startTextActionMode(this);
            }
            return true;
        }

        case InputEvent::EVM_MOVE:
        {
            if (mIsMouseLeftKeyDown)
            {
                unsigned int start = mFirstSelection;
                unsigned int end = getHitTextPosition(
                    e->getMouseX() - getPaddingLeft() + getScrollX(),
                    e->getMouseY() - getPaddingTop() + getScrollY());

                mLastSelection = end;

                if (start > end)
                {
                    unsigned int tmp = start;
                    start = end;
                    end = tmp;
                }

                mBaseText->setSelection(start, end);
            }
            else
            {
                if (mIsSelectable
                    && (isHitText(
                        e->getMouseX() - getPaddingLeft() + getScrollX(),
                        e->getMouseY() - getPaddingTop() + getScrollY()) || mIsEditable))
                {
                    setCurrentCursor(Cursor::IBEAM);
                }
                else
                    setCurrentCursor(Cursor::ARROW);
            }
            return true;
        }

        case InputEvent::EVM_WHEEL:
        {
            int originDy = -(int)mTextSize * e->getMouseWheel();

            if (mLineSpacingMethod
                == DWRITE_LINE_SPACING_METHOD_DEFAULT)
            {
                originDy = -(int)mTextSize * e->getMouseWheel();
            }
            else if (mLineSpacingMethod
                == DWRITE_LINE_SPACING_METHOD_UNIFORM)
            {
                originDy = -(int)(mTextSize * mLineSpacingMultiple) * e->getMouseWheel();
            }

            scrollBy(0, determineVerticalScroll(originDy * 3));
            break;
        }

        case InputEvent::EVK_DOWN:
        {
            mTextKeyListener->onKeyDown(
                mBaseText, mIsEditable, mIsSelectable, e->getKeyboardKey());
            if (mIsEditable && mIsSelectable)
                blinkNavigator(e->getKeyboardKey());
            break;
        }

        case InputEvent::EVK_UP:
        {
            mTextKeyListener->onKeyUp(
                mBaseText, mIsEditable, mIsSelectable, e->getKeyboardKey());
            break;
        }

        default:;
        }

        return false;
    }

    bool TextView::onCheckIsTextEditor()
    {
        return mIsEditable;
    }

    InputConnection *TextView::onCreateInputConnection()
    {
        return mInputConnection;
    }


    void TextView::autoWrap(bool enable)
    {
        if (mIsAutoWrap == enable)
            return;

        mIsAutoWrap = enable;
        mTextLayout->SetWordWrapping(
            enable ? DWRITE_WORD_WRAPPING_EMERGENCY_BREAK : DWRITE_WORD_WRAPPING_NO_WRAP);

        requestLayout();
        invalidate();
    }

    void TextView::setIsEditable(bool editable)
    {
        if (editable == mIsEditable)
            return;

        mIsEditable = editable;

        if (editable)
        {
            setFocusable(true);
            if (hasFocus())
                mTextBlink->show();
        }
        else
        {
            setFocusable(mIsSelectable);
            mTextBlink->hide();
        }
    }

    void TextView::setIsSelectable(bool selectable)
    {
        if (selectable == mIsSelectable)
            return;

        mIsSelectable = selectable;

        if (selectable)
        {
            setFocusable(true);
        }
        else
        {
            setFocusable(mIsEditable);

            if (!mSelectionList.empty())
            {
                mSelectionList.clear();
                invalidate();
            }
        }
    }


    bool TextView::isAutoWrap()
    {
        return mIsAutoWrap;
    }

    bool TextView::isEditable()
    {
        return mIsEditable;
    }

    bool TextView::isSelectable()
    {
        return mIsSelectable;
    }


    void TextView::setText(std::wstring text)
    {
        mBaseText->replace(text, 0, mBaseText->length());
        mBaseText->setSelection(0);
    }

    void TextView::setTextSize(int size)
    {
        size = static_cast<int>(std::round(getWindow()->dpToPx(size)));
        if (size == mTextSize)
            return;

        mTextSize = size;

        makeNewTextFormat();
        makeNewTextLayout(
            mTextLayout->GetMaxWidth(),
            mTextLayout->GetMaxHeight(), mIsAutoWrap);

        requestLayout();
        invalidate();
    }

    void TextView::setTextColor(Color color)
    {
        mTextColor = color;
        invalidate();
    }

    void TextView::setTextAlignment(DWRITE_TEXT_ALIGNMENT alignment)
    {
        if (mTextAlignment == alignment) return;

        mTextAlignment = alignment;
        mTextLayout->SetTextAlignment(alignment);
    }

    void TextView::setParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT alignment)
    {
        if (mParagraphAlignment == alignment) return;

        mParagraphAlignment = alignment;
        mTextLayout->SetParagraphAlignment(alignment);
    }

    void TextView::setTextStyle(DWRITE_FONT_STYLE style)
    {
        if (mTextStyle == style)
            return;

        mTextStyle = style;

        DWRITE_TEXT_RANGE range;
        range.startPosition = 0;
        range.length = mBaseText->length();

        mTextLayout->SetFontStyle(mTextStyle, range);

        requestLayout();
        invalidate();
    }

    void TextView::setTextWeight(DWRITE_FONT_WEIGHT weight)
    {
        if (mTextWeight == weight)
            return;

        mTextWeight = weight;

        DWRITE_TEXT_RANGE range;
        range.startPosition = 0;
        range.length = mBaseText->length();

        mTextLayout->SetFontWeight(mTextWeight, range);

        requestLayout();
        invalidate();
    }

    void TextView::setFontFamilyName(std::wstring font)
    {
        if (mFontFamilyName == font)
            return;

        mFontFamilyName = font;

        makeNewTextFormat();
        makeNewTextLayout(
            mTextLayout->GetMaxWidth(),
            mTextLayout->GetMaxHeight(), mIsAutoWrap);

        requestLayout();
        invalidate();
    }

    void TextView::setLineSpacing(bool uniform, float spacingMultiple)
    {
        if (uniform)
            mLineSpacingMethod = DWRITE_LINE_SPACING_METHOD_UNIFORM;
        else
            mLineSpacingMethod = DWRITE_LINE_SPACING_METHOD_DEFAULT;

        mLineSpacingMultiple = spacingMultiple;

        mTextLayout->SetLineSpacing(
            mLineSpacingMethod,
            mTextSize*mLineSpacingMultiple,
            mTextSize*mLineSpacingMultiple*0.8f);

        requestLayout();
        invalidate();
    }


    std::wstring TextView::getText()
    {
        return mBaseText->toString();
    }

    Editable *TextView::getEditable()
    {
        return mBaseText;
    }

    float TextView::getTextSize()
    {
        return mTextSize;
    }


    void TextView::setSelection(unsigned int position)
    {
        mBaseText->setSelection(position);
    }

    void TextView::setSelection(unsigned int start, unsigned int end)
    {
        mBaseText->setSelection(start, end);
    }

    void TextView::drawSelection(unsigned int start, unsigned int end)
    {
        UINT hitTextMetricsCount = 0;
        DWRITE_HIT_TEST_METRICS *hitTextMetrics;

        if (end - start <= 0)
            return;

        mTextLayout->HitTestTextRange(
            start, end - start,
            0.f, 0.f, 0, 0,
            &hitTextMetricsCount);

        if (hitTextMetricsCount < 1)
            return;

        hitTextMetrics = new DWRITE_HIT_TEST_METRICS[hitTextMetricsCount];
        mTextLayout->HitTestTextRange(
            start, end - start,
            0.f, 0.f,
            hitTextMetrics, hitTextMetricsCount,
            &hitTextMetricsCount);

        mSelectionList.clear();

        for (UINT i = 0; i < hitTextMetricsCount; i++)
        {
            SelectionBlock *block = new SelectionBlock();

            float extraWidth = 0.f;
            uint32_t tPos = hitTextMetrics[i].textPosition;
            uint32_t tLength = hitTextMetrics[i].length;

            //一行中只有\n或\r\n时，添加一个一定宽度的Selection。
            if ((tLength == 1 && mBaseText->at(tPos) == L'\n')
                || (tLength == 2 && mBaseText->at(tPos) == L'\r'
                    && mBaseText->at(tPos + 1) == L'\n'))
            {
                extraWidth = mTextSize;
            }
            //一行中句尾有\n或\r\n时，句尾添加一个一定宽度的Selection。
            else if ((tLength > 0 && mBaseText->at(tPos + tLength - 1) == L'\n')
                || (tLength > 1 && mBaseText->at(tPos + tLength - 2) == L'\r'
                    && mBaseText->at(tPos + tLength - 1) == L'\n'))
            {
                extraWidth = mTextSize;
            }

            block->rect.left = std::floor(hitTextMetrics[i].left);
            block->rect.top = std::floor(hitTextMetrics[i].top);
            block->rect.right = std::ceil(hitTextMetrics[i].left + hitTextMetrics[i].width + extraWidth);
            block->rect.bottom = std::ceil(hitTextMetrics[i].top + hitTextMetrics[i].height);
            block->start = hitTextMetrics[i].textPosition;
            block->length = hitTextMetrics[i].length;
            mSelectionList.push_back(std::shared_ptr<SelectionBlock>(block));
        }

        delete hitTextMetrics;

        invalidate();
    }

    std::wstring TextView::getSelection()
    {
        return mBaseText->getSelection();
    }

    int TextView::getSelectionStart()
    {
        return mBaseText->getSelectionStart();
    }

    int TextView::getSelectionEnd()
    {
        return mBaseText->getSelectionEnd();
    }

    bool TextView::hasSelection()
    {
        return mBaseText->hasSelection();
    }


    uint32_t TextView::getHitTextPosition(float textX, float textY)
    {
        BOOL isInside;
        BOOL isTrailingHit;
        DWRITE_HIT_TEST_METRICS metrics;

        mTextLayout->HitTestPoint(
            textX, textY, &isTrailingHit, &isInside, &metrics);

        return metrics.textPosition + (isTrailingHit == TRUE ? 1 : 0);
    }

    bool TextView::isHitText(float textX, float textY, uint32_t *hitPos)
    {
        BOOL isInside;
        BOOL isTrailingHit;
        DWRITE_HIT_TEST_METRICS metrics;

        mTextLayout->HitTestPoint(
            textX, textY, &isTrailingHit, &isInside, &metrics);

        if (hitPos) {
            *hitPos = metrics.textPosition + (isTrailingHit == TRUE ? 1 : 0);
        }

        return isInside == TRUE ? true : false;
    }

    bool TextView::isHitText(
        float textX, float textY,
        uint32_t position, uint32_t length, uint32_t *hitPos)
    {
        BOOL isInside;
        BOOL isTrailingHit;
        DWRITE_HIT_TEST_METRICS metrics;

        mTextLayout->HitTestPoint(
            textX, textY, &isTrailingHit, &isInside, &metrics);

        if (hitPos) {
            *hitPos = metrics.textPosition + (isTrailingHit == TRUE ? 1 : 0);
        }

        if (isInside == TRUE)
        {
            if (metrics.textPosition >= position
                && metrics.textPosition <= position + length - 1) {
                return true;
            }
        }

        return false;
    }

    RectF TextView::getSelectionBound(unsigned int start, unsigned int end)
    {
        if (end < start) {
            return RectF();
        }

        RectF bound;

        if (end == start)
        {
            float pointX;
            float pointY;
            DWRITE_HIT_TEST_METRICS metrics;

            mTextLayout->HitTestTextPosition(start, FALSE, &pointX, &pointY, &metrics);

            bound.left = pointX;
            bound.top = pointY;
            bound.right = pointX;
            bound.bottom = pointY + metrics.height;
        }
        else
        {
            UINT hitTextMetricsCount = 0;
            DWRITE_HIT_TEST_METRICS *hitTextMetrics;

            mTextLayout->HitTestTextRange(
                start, end - start,
                0.f, 0.f,
                0, 0,
                &hitTextMetricsCount);

            if (hitTextMetricsCount < 1) {
                return RectF();
            }

            hitTextMetrics = new DWRITE_HIT_TEST_METRICS[hitTextMetricsCount];
            mTextLayout->HitTestTextRange(
                start, end - start,
                0.f, 0.f,
                hitTextMetrics, hitTextMetricsCount,
                &hitTextMetricsCount);

            for (UINT i = 0; i < hitTextMetricsCount; i++)
            {
                if (i == 0) {
                    bound.left = hitTextMetrics[i].left;
                    bound.top = hitTextMetrics[i].top;
                    bound.right = hitTextMetrics[i].left + hitTextMetrics[i].width;
                    bound.bottom = hitTextMetrics[i].top + hitTextMetrics[i].height;
                    continue;
                }

                if (bound.left > hitTextMetrics[i].left)
                    bound.left = hitTextMetrics[i].left;

                if (bound.top > hitTextMetrics[i].top)
                    bound.top = hitTextMetrics[i].top;

                if (bound.right < hitTextMetrics[i].left + hitTextMetrics[i].width)
                    bound.right = hitTextMetrics[i].left + hitTextMetrics[i].width;

                if (bound.bottom < hitTextMetrics[i].top + hitTextMetrics[i].height)
                    bound.bottom = hitTextMetrics[i].top + hitTextMetrics[i].height;
            }
        }

        return bound;
    }


    void TextView::computeVisibleRegion(RectF *visibleRegon)
    {
        visibleRegon->left = 0.f + getScrollX();
        visibleRegon->top = 0.f + getScrollY();
        visibleRegon->right = visibleRegon->left + mTextLayout->GetMaxWidth();
        visibleRegon->bottom = visibleRegon->top + mTextLayout->GetMaxHeight();
    }


    void TextView::onTextChanged(
        Editable *editable,
        int start, int oldEnd, int newEnd)
    {
        float maxWidth = mTextLayout->GetMaxWidth();
        float maxHeight = mTextLayout->GetMaxHeight();

        makeNewTextLayout(maxWidth, maxHeight, isAutoWrap());
        requestLayout();
        invalidate();

        if (mProcessRef == 0 && hasFocus() && mIsEditable)
            mInputConnection->notifyTextChanged(false, start, oldEnd, newEnd);

        scrollToFit(false);
    }

    void TextView::onSelectionChanged(
        unsigned int ns, unsigned int ne,
        unsigned int os, unsigned int oe)
    {
        if (ns == ne)
        {
            if (os != oe)
            {
                if (mTextActionMode != nullptr)
                    mTextActionMode->close();

                mSelectionList.clear();
                invalidate();
            }

            if (mIsEditable && hasFocus())
            {
                locateTextBlink(ns);
                mTextBlink->show();
            }
        }
        else
        {
            setSelection(ns, ne);
            mTextBlink->hide();
            drawSelection(ns, ne);
        }

        if (mProcessRef == 0 && hasFocus() && mIsEditable)
            mInputConnection->notifyTextSelectionChanged();

        scrollToFit(true);
    }

    void TextView::onSpanChanged(
        Span *span, Editable::EditWatcher::SpanChange action)
    {
        DWRITE_TEXT_RANGE range;
        range.startPosition = span->getStart();
        range.length = span->getEnd() - span->getStart();

        switch (span->getBaseType())
        {
        case Span::USPAN_BASE_TYPE_TEXT:
        {
            switch (span->getType())
            {
            case Span::TEXT_UNDERLINE:
                if (action == SpanChange::ADD)
                    mTextLayout->SetUnderline(TRUE, range);
                else if (action == SpanChange::REMOVE)
                    mTextLayout->SetUnderline(FALSE, range);
                break;
            }
            break;
        }

        case Span::USPAN_BASE_TYPE_EFFECT:
            if (action == SpanChange::ADD)
            {
                TextDrawingEffect *tdEffect = new TextDrawingEffect();
                tdEffect->mEffectSpan = (EffectSpan*)span;

                mTextLayout->SetDrawingEffect(tdEffect, range);
            }
            else if (action == SpanChange::REMOVE)
            {
                mTextLayout->SetDrawingEffect(0, range);
            }
            break;

        case Span::USPAN_BASE_TYPE_INTERACTABLE:
            break;

        case Span::USPAN_BASE_TYPE_INLINEOBJECT:
            break;
        }
    }


    bool TextView::canCut()
    {
        return mIsEditable && mBaseText->hasSelection();
    }

    bool TextView::canCopy()
    {
        return mBaseText->hasSelection();
    }

    bool TextView::canPaste()
    {
        if (mIsEditable)
        {
            std::wstring content = ClipboardManager::getFromClipboard();
            return !content.empty();
        }
        return false;
    }

    bool TextView::canSelectAll()
    {
        if (mBaseText->length() == 0)
            return false;

        if (mBaseText->getSelectionStart() == 0
            && mBaseText->getSelectionEnd() == mBaseText->length())
            return false;

        return true;
    }

    void TextView::performCut()
    {
        ClipboardManager::saveToClipboard(getSelection());
        mBaseText->replace(L"");

        if (mTextActionMode != nullptr)
            mTextActionMode->close();
    }

    void TextView::performCopy()
    {
        ClipboardManager::saveToClipboard(getSelection());

        if (mTextActionMode != nullptr)
            mTextActionMode->close();
    }

    void TextView::performPaste()
    {
        std::wstring content = ClipboardManager::getFromClipboard();
        if (hasSelection())
            mBaseText->replace(content);
        else
            mBaseText->insert(content);

        if (mTextActionMode != nullptr)
            mTextActionMode->close();
    }

    void TextView::performSelectAll()
    {
        setSelection(0, mBaseText->length());
        mTextActionMode->invalidateMenu();
    }


    bool TextView::onCreateActionMode(TextActionMode *mode, Menu *menu)
    {
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

    bool TextView::onPrepareActionMode(TextActionMode *mode, Menu *menu)
    {
        bool canCopy = this->canCopy();
        MenuItem *copyItem = menu->findItem(MENU_ID_COPY);
        if (copyItem != nullptr)
            copyItem->setItemVisible(canCopy);
        else if (canCopy)
            menu->addItem(MENU_ID_COPY, MENU_ORDER_COPY, L"复制");

        bool canCut = this->canCut();
        MenuItem *cutItem = menu->findItem(MENU_ID_CUT);
        if (cutItem != nullptr)
            cutItem->setItemVisible(canCut);
        else if (canCut)
            menu->addItem(MENU_ID_CUT, MENU_ORDER_CUT, L"剪切");

        bool canPaste = this->canPaste();
        MenuItem *pasteItem = menu->findItem(MENU_ID_PASTE);
        if (pasteItem != nullptr)
            pasteItem->setItemVisible(canPaste);
        else if (canPaste)
            menu->addItem(MENU_ID_PASTE, MENU_ORDER_PASTE, L"粘贴");

        bool canSelectAll = this->canSelectAll();
        MenuItem *selectAllItem = menu->findItem(MENU_ID_SELECTALL);
        if (selectAllItem != nullptr)
            selectAllItem->setItemVisible(canSelectAll);
        else if (canSelectAll)
            menu->addItem(MENU_ID_SELECTALL, MENU_ORDER_SELECTALL, L"全选");

        return true;
    }

    bool TextView::onActionItemClicked(TextActionMode *mode, MenuItem *item)
    {
        switch (item->getItemId())
        {
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
        }

        return true;
    }

    void TextView::onDestroyActionMode(TextActionMode *mode)
    {
        mTextActionMode = nullptr;
    }

    void TextView::onGetContentPosition(int *x, int *y)
    {
        int left = 0;
        int top = 0;
        View *parent = this;
        while (parent) {
            left += parent->getLeft() - parent->getScrollX();
            top += parent->getTop() - parent->getScrollY();
            parent = parent->getParent();
        }

        *x = left + mPrevX + 1;
        *y = top + mPrevY + 1;
    }

}