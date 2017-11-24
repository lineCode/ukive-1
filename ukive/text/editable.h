#ifndef UKIVE_TEXT_EDITABLE_H_
#define UKIVE_TEXT_EDITABLE_H_

#include <list>
#include <memory>
#include <vector>

#include "ukive/utils/string_utils.h"


namespace ukive {

    class Span;

    class Editable
    {
    public:
        struct EditWatcher
        {
            enum SpanChange
            {
                ADD,
                REMOVE,
                EDIT,
            };

            virtual void onTextChanged(
                Editable *editable,
                int start, int oldEnd, int newEnd) = 0;

            virtual void onSelectionChanged(
                uint32_t ns, uint32_t ne,
                uint32_t os, uint32_t oe) = 0;

            virtual void onSpanChanged(
                Span *span, SpanChange action) = 0;
        };

    private:
        std::wstring mText;
        std::list<EditWatcher*> mWatcherList;
        std::vector<std::shared_ptr<Span>> mSpanList;

        uint32_t mSelectionStart;
        uint32_t mSelectionEnd;

        void notifyTextChanged(
            uint32_t start, uint32_t oldEnd, uint32_t newEnd);
        void notifySelectionChanged(
            uint32_t ns, uint32_t ne, uint32_t os, uint32_t oe);
        void notifyEditWatcher(
            int start, int oldEnd, int newEnd,
            uint32_t ns, uint32_t ne, uint32_t os, uint32_t oe);
        void notifySpanChanged(
            Span *span, EditWatcher::SpanChange action);

    public:
        Editable(std::wstring text);
        ~Editable();

        uint32_t length();

        void append(std::wstring text);
        void insert(std::wstring text, uint32_t position);
        void remove(uint32_t start, uint32_t length);
        void replace(std::wstring text, uint32_t start, uint32_t length);
        void clear();

        void replace(std::wstring find, std::wstring replacement);
        void replaceAll(std::wstring find, std::wstring replacement);

        void insert(std::wstring text);
        void remove();
        void replace(std::wstring text);

        void setSelection(uint32_t selection);
        void setSelection(uint32_t start, uint32_t end);

        void setSelectionForceNotify(uint32_t selection);
        void setSelectionForceNotify(uint32_t start, uint32_t end);

        uint32_t getSelectionStart();
        uint32_t getSelectionEnd();
        bool hasSelection();
        std::wstring getSelection();

        wchar_t at(size_t pos);
        std::wstring toString();

        void addSpan(Span *span);
        void removeSpan(std::size_t index);
        void removeAllSpan();
        Span *getSpan(std::size_t index);
        std::size_t getSpanCount();

        void addEditWatcher(EditWatcher *watcher);
        void removeEditWatcher(EditWatcher *watcher);
    };

}

#endif  // UKIVE_TEXT_EDITABLE_H_