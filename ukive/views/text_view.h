#ifndef UKIVE_VIEWS_TEXT_VIEW_H_
#define UKIVE_VIEWS_TEXT_VIEW_H_

#include "ukive/views/view.h"
#include "ukive/text/editable.h"
#include "ukive/graphics/color.h"
#include "ukive/text/text_action_mode_callback.h"
#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    class TextBlink;
    class TextActionMode;
    class TextKeyListener;
    class InputConnection;
    class TextDrawingEffect;


    ///<summary>
    /// 文本编辑器，使用 TSF。
    ///</summary>
    class TextView : public View,
        public Editable::EditWatcher, public TextActionModeCallback {
    public:
        TextView(Window* w);
        ~TextView();

        void onBeginProcess();
        void onEndProcess();

        void onAttachedToWindow() override;
        void onDetachedFromWindow() override;

        void autoWrap(bool enable);
        void setIsEditable(bool editable);
        void setIsSelectable(bool selectable);

        bool isAutoWrap();
        bool isEditable();
        bool isSelectable();

        void setText(const string16& text);
        void setTextSize(int size);
        void setTextColor(Color color);
        void setTextAlignment(DWRITE_TEXT_ALIGNMENT alignment);
        void setParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT alignment);
        void setTextStyle(DWRITE_FONT_STYLE style);
        void setTextWeight(DWRITE_FONT_WEIGHT weight);
        void setFontFamilyName(const string16& font);
        void setLineSpacing(bool uniform, float spacingMultiple);

        string16 getText();
        Editable* getEditable();
        float getTextSize();

        void setSelection(unsigned int position);
        void setSelection(unsigned int start, unsigned int end);
        void drawSelection(unsigned int start, unsigned int end);
        string16 getSelection();
        int getSelectionStart();
        int getSelectionEnd();
        bool hasSelection();

        uint32_t getHitTextPosition(float textX, float textY);
        bool isHitText(float textX, float textY, uint32_t* hitPos = nullptr);
        bool isHitText(float textX, float textY, uint32_t position, uint32_t length, uint32_t* hitPos = nullptr);
        RectF getSelectionBound(unsigned int start, unsigned int end);

        void computeVisibleRegion(RectF* region);

        void onTextChanged(
            Editable* editable,
            int start, int oldEnd, int newEnd) override;
        void onSelectionChanged(
            unsigned int ns, unsigned int ne,
            unsigned int os, unsigned int oe) override;
        void onSpanChanged(
            Span* span, SpanChange action) override;

        bool onCreateActionMode(TextActionMode* mode, Menu* menu) override;
        bool onPrepareActionMode(TextActionMode* mode, Menu* menu) override;
        bool onActionItemClicked(TextActionMode* mode, MenuItem* item) override;
        void onDestroyActionMode(TextActionMode* mode) override;
        void onGetContentPosition(int* x, int* y) override;

    protected:
        void onDraw(Canvas* canvas) override;
        void onMeasure(int width, int height, int widthSpec, int heightSpec) override;
        bool onInputEvent(InputEvent* e) override;

        void onLayout(
            bool changed, bool sizeChanged,
            int left, int top, int right, int bottom) override;
        void onSizeChanged(int width, int height, int oldWidth, int oldHeight) override;
        void onFocusChanged(bool getFocus) override;
        void onWindowFocusChanged(bool windowFocus) override;

        void onScrollChanged(int scrollX, int scrollY, int oldScrollX, int oldScrollY) override;

        bool onCheckIsTextEditor() override;
        InputConnection* onCreateInputConnection() override;

    private:
        void initTextView();

        int computeVerticalScrollRange();
        int computeHorizontalScrollRange();

        void computeTextOffsetAtViewTop();
        int computeVerticalScrollOffsetFromTextOffset(uint32_t tOff);

        int determineVerticalScroll(int dy);
        int determineHorizontalScroll(int dx);

        void scrollToFit(bool considerSelection);

        float getTextWidth();
        float getTextHeight();

        bool getLineInfo(
            uint32_t position,
            uint32_t* line, float* height, uint32_t* count = nullptr);
        bool getLineHeight(
            uint32_t line, float* height);

        void blinkNavigator(int keyCode);

        void locateTextBlink(int position);
        void locateTextBlink(float textX, float textY);
        void makeNewTextFormat();
        void makeNewTextLayout(float maxWidth, float maxHeight, bool autoWrap);

        bool canCut();
        bool canCopy();
        bool canPaste();
        bool canSelectAll();

        void performCut();
        void performCopy();
        void performPaste();
        void performSelectAll();

    private:
        // 为支持撤销而创建的结构体（未完成）。
        struct UndoBlock {
            std::wstring text;
            uint32_t start;
            uint32_t oldEnd;
            uint32_t newEnd;
        };

        struct SelectionBlock {
            unsigned int start;
            unsigned int length;
            RectF rect;
        };

    private:
        Editable* mBaseText;
        ComPtr<IDWriteTextFormat> mTextFormat;
        ComPtr<IDWriteTextLayout> mTextLayout;

        TextBlink* mTextBlink;
        TextActionMode* mTextActionMode;
        InputConnection* mInputConnection;
        TextKeyListener* mTextKeyListener;

        ULONG64 mProcessRef;

        int mTextSize;
        string16 mFontFamilyName;
        Color mTextColor;
        Color mSelectionBackgroundColor;
        DWRITE_TEXT_ALIGNMENT mTextAlignment;
        DWRITE_PARAGRAPH_ALIGNMENT mParagraphAlignment;
        DWRITE_FONT_WEIGHT mTextWeight;
        DWRITE_FONT_STYLE mTextStyle;

        float mLineSpacingMultiple;
        DWRITE_LINE_SPACING_METHOD mLineSpacingMethod;

        bool mIsAutoWrap;
        bool mIsEditable;
        bool mIsSelectable;

        int mPrevX, mPrevY;
        bool mIsMouseLeftKeyDown;
        bool mIsMouseRightKeyDown;
        bool mIsMouseLeftKeyDownOnText;

        int mLastSelection;
        int mFirstSelection;
        float mLastSelectionLeft;

        float mVerticalOffset;
        uint32_t mTextOffsetAtViewTop;

        std::vector<ComPtr<TextDrawingEffect>> mTDEffectList;
        std::vector<std::shared_ptr<SelectionBlock>> mSelectionList;
    };

}

#endif  // UKIVE_VIEWS_TEXT_VIEW_H_