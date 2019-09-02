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
        enum class Reason {
            USER_INPUT,
            API
        };

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
                int start, int oldEnd, int newEnd, Reason r) = 0;

            virtual void onSelectionChanged(
                uint32_t ns, uint32_t ne,
                uint32_t os, uint32_t oe, Reason r) = 0;

            virtual void onSpanChanged(
                Span* span, SpanChange action, Reason r) = 0;
        };

        explicit Editable(const string16& text);
        ~Editable();

        uint32_t length() const;

        void append(const string16& text, Reason r);
        void insert(const string16& text, uint32_t position, Reason r);
        void remove(uint32_t start, uint32_t length, Reason r);
        void replace(const string16& text, uint32_t start, uint32_t length, Reason r);
        void clear(Reason r);

        void replace(const string16& find, const string16& replacement, Reason r);
        void replaceAll(const string16& find, const string16& replacement, Reason r);

        void insert(const string16& text, Reason r);
        void remove(Reason r);
        void replace(const string16& text, Reason r);

        void setSelection(uint32_t selection, Reason r);
        void setSelection(uint32_t start, uint32_t end, Reason r);

        void setSelectionForceNotify(uint32_t selection, Reason r);
        void setSelectionForceNotify(uint32_t start, uint32_t end, Reason r);

        uint32_t getSelectionStart() const;
        uint32_t getSelectionEnd() const;
        bool hasSelection() const;
        string16 getSelection() const;

        wchar_t at(size_t pos) const;
        string16 toString() const;

        void addSpan(Span* span, Reason r);
        void removeSpan(std::size_t index, Reason r);
        void removeAllSpan(Reason r);
        Span* getSpan(std::size_t index) const;
        std::size_t getSpanCount() const;

        void addEditWatcher(EditWatcher* watcher);
        void removeEditWatcher(EditWatcher* watcher);

    private:
        void notifyTextChanged(
            uint32_t start, uint32_t oldEnd, uint32_t newEnd, Reason r);
        void notifySelectionChanged(
            uint32_t ns, uint32_t ne, uint32_t os, uint32_t oe, Reason r);
        void notifyEditWatcher(
            int start, int oldEnd, int newEnd,
            uint32_t ns, uint32_t ne, uint32_t os, uint32_t oe, Reason r);
        void notifySpanChanged(
            Span* span, EditWatcher::SpanChange action, Reason r);

        string16 text_;
        std::list<EditWatcher*> watchers_;
        std::vector<std::shared_ptr<Span>> spans_;

        uint32_t sel_beg_;
        uint32_t sel_end_;
    };

}

#endif  // UKIVE_TEXT_EDITABLE_H_