#ifndef UKIVE_TEXT_EDITABLE_H_
#define UKIVE_TEXT_EDITABLE_H_

#include <list>
#include <memory>
#include <vector>

#include "ukive/utils/string_utils.h"


namespace ukive {

    class Span;

    class Editable {
    public:
        class EditWatcher {
        public:
            virtual ~EditWatcher() = default;

            enum SpanChange {
                ADD,
                REMOVE,
                EDIT,
            };

            virtual void onTextChanged(
                Editable* editable,
                int start, int oldEnd, int newEnd) = 0;

            virtual void onSelectionChanged(
                uint32_t ns, uint32_t ne,
                uint32_t os, uint32_t oe) = 0;

            virtual void onSpanChanged(
                Span* span, SpanChange action) = 0;
        };

        Editable(const string16& text);
        ~Editable();

        uint32_t length() const;

        void append(const string16& text);
        void insert(const string16& text, uint32_t position);
        void remove(uint32_t start, uint32_t length);
        void replace(const string16& text, uint32_t start, uint32_t length);
        void clear();

        void replace(const string16& find, const string16& replacement);
        void replaceAll(const string16& find, const string16& replacement);

        void insert(const string16& text);
        void remove();
        void replace(const string16& text);

        void setSelection(uint32_t selection);
        void setSelection(uint32_t start, uint32_t end);

        void setSelectionForceNotify(uint32_t selection);
        void setSelectionForceNotify(uint32_t start, uint32_t end);

        uint32_t getSelectionStart() const;
        uint32_t getSelectionEnd() const;
        bool hasSelection() const;
        string16 getSelection() const;

        wchar_t at(size_t pos) const;
        string16 toString() const;

        void addSpan(Span* span);
        void removeSpan(std::size_t index);
        void removeAllSpan();
        Span* getSpan(std::size_t index);
        std::size_t getSpanCount() const;

        void addEditWatcher(EditWatcher* watcher);
        void removeEditWatcher(EditWatcher* watcher);

    private:
        void notifyTextChanged(
            uint32_t start, uint32_t oldEnd, uint32_t newEnd);
        void notifySelectionChanged(
            uint32_t ns, uint32_t ne, uint32_t os, uint32_t oe);
        void notifyEditWatcher(
            int start, int oldEnd, int newEnd,
            uint32_t ns, uint32_t ne, uint32_t os, uint32_t oe);
        void notifySpanChanged(
            Span* span, EditWatcher::SpanChange action);

        string16 text_;
        std::list<EditWatcher*> watchers_;
        std::vector<std::shared_ptr<Span>> spans_;

        uint32_t sel_beg_;
        uint32_t sel_end_;
    };

}

#endif  // UKIVE_TEXT_EDITABLE_H_