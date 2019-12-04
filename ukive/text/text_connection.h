#ifndef UKIVE_TEXT_TEXT_CONNECTION_H_
#define UKIVE_TEXT_TEXT_CONNECTION_H_

#include <cstdint>

#include "utils/string_utils.h"


namespace ukive {

    class Rect;
    class Point;
    class Window;

    class TextConnection {
    public:
        enum TextLayoutChangeReason {
            TLC_CREATE,
            TLC_CHANGE,
            TLC_DESTROY,
        };

        struct SelectionInfo {
            int start;
            int end;
            int style;
        };

        struct TextChangeInfo {
            int start;
            int old_end;
            int new_end;
        };

        virtual ~TextConnection() = default;

        virtual bool initialization() = 0;

        virtual bool pushEditor() = 0;
        virtual bool popEditor() = 0;

        virtual bool requestFocus() = 0;
        virtual bool discardFocus() = 0;

        virtual bool terminateComposition() = 0;

        virtual void notifyStatusChanged(uint32_t flags) = 0;
        virtual void notifyTextChanged(bool correction, const TextChangeInfo& info) = 0;
        virtual void notifyTextLayoutChanged(TextLayoutChangeReason r) = 0;
        virtual void notifyTextSelectionChanged() = 0;

        virtual void onBeginProcess() = 0;
        virtual void onEndProcess() = 0;

        virtual bool isReadyOnly() const = 0;

        virtual void determineInsert(
            int start, int end, int rep_len, int* res_start, int* res_end) = 0;
        virtual bool getSelection(
            int start, int max_count, SelectionInfo* sels, int* fetched_count) = 0;
        virtual bool setSelection(int count, const SelectionInfo& sels) = 0;
        virtual string16 getText(int start, int end, int max_len) = 0;
        virtual void setText(int start, int end, const string16& text) = 0;
        virtual void insertTextAtPos(
            uint32_t flags, const string16& text,
            int* start, int* end, TextChangeInfo* info) = 0;

        virtual int getTextLength() const = 0;
        virtual bool getTextPosAtPoint(const Point& p, uint32_t flags, int* acp) const = 0;
        virtual bool getTextBounds(int start, int end, Rect* rect, bool* clipped) const = 0;
        virtual void getTextViewBounds(Rect* rect) const = 0;
        virtual Window* getWindow() const = 0;
    };

}

#endif  // UKIVE_TEXT_TEXT_CONNECTION_H_