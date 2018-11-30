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


    class TextView : public View,
        public Editable::EditWatcher,
        public TextActionModeCallback {
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

        bool isAutoWrap() const;
        bool isEditable() const;
        bool isSelectable() const;

        void setText(const string16& text);
        void setTextSize(int size);
        void setTextColor(Color color);
        void setTextAlignment(DWRITE_TEXT_ALIGNMENT alignment);
        void setParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT alignment);
        void setTextStyle(DWRITE_FONT_STYLE style);
        void setTextWeight(DWRITE_FONT_WEIGHT weight);
        void setFontFamilyName(const string16& font);
        void setLineSpacing(bool uniform, float spacingMultiple);

        string16 getText() const;
        Editable* getEditable() const;
        float getTextSize() const;

        void setSelection(unsigned int position);
        void setSelection(unsigned int start, unsigned int end);
        void drawSelection(unsigned int start, unsigned int end);
        string16 getSelection() const;
        int getSelectionStart() const;
        int getSelectionEnd() const;
        bool hasSelection() const;

        uint32_t getHitTextPosition(float textX, float textY) const;
        bool isHitText(float textX, float textY, uint32_t* hitPos = nullptr) const;
        bool isHitText(float textX, float textY, uint32_t position, uint32_t length, uint32_t* hitPos = nullptr) const;
        RectF getSelectionBound(unsigned int start, unsigned int end) const;

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

        float getTextWidth() const;
        float getTextHeight() const;

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

        bool canCut() const;
        bool canCopy() const;
        bool canPaste() const;
        bool canSelectAll() const;

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
        Editable* base_text_;
        ComPtr<IDWriteTextFormat> text_format_;
        ComPtr<IDWriteTextLayout> text_layout_;

        TextBlink* text_blink_;
        TextActionMode* text_action_mode_;
        InputConnection* input_connection_;
        TextKeyListener* text_key_listener_;

        ULONG64 process_ref_;

        int text_size_;
        string16 font_family_name_;
        Color text_color_;
        Color sel_bg_color_;
        DWRITE_TEXT_ALIGNMENT text_alignment_;
        DWRITE_PARAGRAPH_ALIGNMENT paragraph_alignment_;
        DWRITE_FONT_WEIGHT text_weight_;
        DWRITE_FONT_STYLE text_style_;

        float line_spacing_multiple_;
        DWRITE_LINE_SPACING_METHOD line_spacing_method_;

        bool is_auto_wrap_;
        bool is_editable_;
        bool is_selectable_;

        int prev_x_, prev_y_;
        bool is_mouse_left_key_down_;
        bool is_mouse_right_key_down_;
        bool is_mouse_left_key_down_on_text_;

        int last_sel_;
        int first_sel_;
        float last_sel_left_;

        float vertical_offset_;
        uint32_t text_offset_at_view_top_;

        std::vector<ComPtr<TextDrawingEffect>> tde_list_;
        std::vector<std::shared_ptr<SelectionBlock>> sel_list_;
    };

}

#endif  // UKIVE_VIEWS_TEXT_VIEW_H_