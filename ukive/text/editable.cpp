#include "editable.h"

#include "ukive/text/span/span.h"
#include "ukive/log.h"
#include "ukive/utils/stl_utils.h"


namespace ukive {

    Editable::Editable(const string16& text)
        : text_(text),
          sel_beg_(0),
          sel_end_(0) {
    }

    Editable::~Editable() {
    }


    // The possible text changes include insert, delete, and replace.
    // For example, if you replace the first "t" of "text" with "T", acpStart =0, acpOldEnd =1, and acpNewEnd =1.
    // If you delete the last "t", acpStart =3, acpOldEnd =4, and acpNewEnd =3.
    // If an "a" is inserted between "e" and "x", acpStart =2, acpOldEnd =2, and acpNewEnd =3.
    void Editable::notifyTextChanged(
        uint32_t start, uint32_t oldEnd, uint32_t newEnd, Reason r)
    {
        for (auto span : spans_) {
            uint32_t spanStart = span->getStart();
            uint32_t spanEnd = span->getEnd();

            if (spanStart >= start && spanStart < oldEnd) {
            }
        }

        for (auto watcher : watchers_) {
            watcher->onTextChanged(this, start, oldEnd, newEnd, r);
        }
    }

    void Editable::notifySelectionChanged(
        uint32_t ns, uint32_t ne,
        uint32_t os, uint32_t oe, Reason r)
    {
        for (auto watcher : watchers_) {
            watcher->onSelectionChanged(ns, ne, os, oe, r);
        }
    }

    void Editable::notifyEditWatcher(
        int start, int oldEnd, int newEnd,
        uint32_t ns, uint32_t ne,
        uint32_t os, uint32_t oe, Reason r)
    {
        for (auto watcher : watchers_) {
            watcher->onTextChanged(this, start, oldEnd, newEnd, r);
            watcher->onSelectionChanged(ns, ne, os, oe, r);
        }
    }

    void Editable::notifySpanChanged(
        Span* span, EditWatcher::SpanChange action, Reason r)
    {
        for (auto watcher : watchers_) {
            watcher->onSpanChanged(span, action, r);
        }
    }


    uint32_t Editable::length() const {
        return text_.length();
    }


    void Editable::append(const string16& text, Reason r) {
        insert(text, text_.length(), r);
    }

    void Editable::insert(const string16& text, uint32_t position, Reason r) {
        if (!text.empty()) {
            text_.insert(position, text);
            notifyTextChanged(position, position, position + text.length(), r);
        }
    }

    void Editable::remove(uint32_t start, uint32_t length, Reason r) {
        if (!text_.empty()) {
            replace(L"", start, length, r);
        }
    }

    void Editable::replace(const string16& text, uint32_t start, uint32_t length, Reason r) {
        text_.replace(start, length, text);
        notifyTextChanged(start, start + 1, start + text.length() - length + 1, r);
    }

    void Editable::clear(Reason r) {
        if (!text_.empty()) {
            uint32_t oldStart = sel_beg_;
            uint32_t oldEnd = sel_end_;
            uint32_t oldLength = text_.length();

            text_.clear();

            sel_beg_ = 0;
            sel_end_ = 0;

            if (oldStart != 0 || oldEnd != 0) {
                notifyEditWatcher(
                    0, oldLength, 0,
                    sel_beg_, sel_end_,
                    oldStart, oldEnd, r);
            } else {
                notifyTextChanged(0, oldLength, 0, r);
            }
        }
    }


    void Editable::replace(const string16& find, const string16& replacement, Reason r) {
        size_t first = text_.find(find);
        if (first != string16::npos) {
            text_.replace(first, find.length(), replacement);
            notifyTextChanged(first, first + 1, first + replacement.length() - find.length() + 1, r);
        }
    }

    void Editable::replaceAll(const string16& find, const string16& replacement, Reason r) {
        size_t first = text_.find(find);
        while (first != string16::npos) {
            text_.replace(first, find.length(), replacement);
            first = text_.find(find, first + replacement.length());
        }
    }


    void Editable::insert(const string16& text, Reason r) {
        uint32_t oldStart = sel_beg_;
        uint32_t oldEnd = sel_end_;

        if (oldStart == oldEnd) {
            if (text_.length() < oldEnd) {
                DCHECK(false);
                text_.insert(text_.length(), text);
            } else {
                text_.insert(oldEnd, text);
            }
            sel_beg_ += text.length();
            sel_end_ += text.length();

            notifyEditWatcher(
                sel_beg_, sel_beg_, sel_beg_ + text.length(),
                sel_beg_, sel_end_, oldStart, oldEnd, r);
        }
    }

    void Editable::remove(Reason r) {
        replace(L"", r);
    }

    void Editable::replace(const string16& text, Reason r) {
        uint32_t oldStart = sel_beg_;
        uint32_t oldEnd = sel_end_;

        bool hasSelection = oldStart != oldEnd;

        if (hasSelection) {
            text_.replace(oldStart, oldEnd - oldStart, text);
            sel_beg_ = sel_end_ = oldStart + text.length();

            notifyEditWatcher(
                oldStart, oldStart + 1, oldStart + text.length() - (oldEnd - oldStart) + 1,
                sel_beg_, sel_end_, oldStart, oldEnd, r);
        }
    }


    void Editable::setSelection(uint32_t selection, Reason r) {
        setSelection(selection, selection, r);
    }

    void Editable::setSelection(uint32_t start, uint32_t end, Reason r) {
        if (start == sel_beg_ && end == sel_end_) {
            return;
        }

        setSelectionForceNotify(start, end, r);
    }

    void Editable::setSelectionForceNotify(uint32_t selection, Reason r) {
        setSelectionForceNotify(selection, selection, r);
    }

    void Editable::setSelectionForceNotify(uint32_t start, uint32_t end, Reason r) {
        if (start < 0) start = 0;
        if (start > text_.length()) start = text_.length();

        if (end < 0) end = 0;
        if (end > text_.length()) end = text_.length();

        if (start > end) {
            auto tmp = start;
            start = end;
            end = tmp;
        }

        int oldSeStart = sel_beg_;
        int oldSeEnd = sel_end_;

        sel_beg_ = start;
        sel_end_ = end;

        notifySelectionChanged(start, end, oldSeStart, oldSeEnd, r);
    }


    uint32_t Editable::getSelectionStart() const {
        return sel_beg_;
    }

    uint32_t Editable::getSelectionEnd() const {
        return sel_end_;
    }

    bool Editable::hasSelection() const {
        return sel_beg_ != sel_end_;
    }

    string16 Editable::getSelection() const {
        if (sel_beg_ == sel_end_) {
            return L"";
        }

        return text_.substr(sel_beg_, sel_end_ - sel_beg_);
    }


    wchar_t Editable::at(size_t pos) const {
        return text_.at(pos);
    }

    string16 Editable::toString() const {
        return text_;
    }


    void Editable::addSpan(Span* span, Reason r) {
        spans_.push_back(std::shared_ptr<Span>(span));

        notifySpanChanged(
            span, EditWatcher::SpanChange::ADD, r);
    }

    void Editable::removeSpan(std::size_t index, Reason r) {
        Span* span = getSpan(index);
        if (span) {
            notifySpanChanged(
                span, EditWatcher::SpanChange::REMOVE, r);

            spans_.erase(spans_.begin() + index);
        }
    }

    void Editable::removeAllSpan(Reason r) {
        while (!spans_.empty()) {
            removeSpan(0, r);
        }
    }

    Span* Editable::getSpan(size_t index) const {
        return spans_.at(index).get();
    }

    std::size_t Editable::getSpanCount() const {
        return spans_.size();
    }

    void Editable::addEditWatcher(EditWatcher* watcher) {
        watchers_.push_back(watcher);
    }

    void Editable::removeEditWatcher(EditWatcher* watcher) {
        for (auto it = watchers_.begin();
            it != watchers_.end();)
        {
            if (watcher == (*it)) {
                it = watchers_.erase(it);
            } else {
                ++it;
            }
        }
    }

}
