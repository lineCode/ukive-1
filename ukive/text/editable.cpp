#include "editable.h"

#include "utils/log.h"
#include "utils/stl_utils.h"

#include "ukive/text/span/span.h"


namespace ukive {

    Editable::Editable(const string16& text)
        : text_(text),
          sel_beg_(0),
          sel_end_(0) {
    }

    Editable::~Editable() {
    }

    // 对文本的操作包括插入、删除和替换。
    // 举例来说，如果你将 "text" 中的第一个 "t" 替换为 "T"，acpStart=0, acpOldEnd=1, and acpNewEnd=1.
    // 如果你删除最后一个 "t"，acpStart=3, acpOldEnd=4, and acpNewEnd=3.
    // 如果将 "a" 放在 "e" 和 "x" 之间，acpStart=2, acpOldEnd=2, and acpNewEnd=3.
    void Editable::notifyTextChanged(
        uint32_t start, uint32_t old_end, uint32_t new_end, Reason r)
    {
        for (auto span : spans_) {
            uint32_t spanStart = span->getStart();
            uint32_t spanEnd = span->getEnd();

            if (spanStart >= start && spanStart < old_end) {
            }
        }

        for (auto watcher : watchers_) {
            watcher->onTextChanged(this, start, old_end, new_end, r);
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
        int text_start, int old_text_end, int new_text_end,
        uint32_t new_sel_start, uint32_t new_sel_end,
        uint32_t old_sel_start, uint32_t old_sel_end, Reason r)
    {
        for (auto watcher : watchers_) {
            watcher->onTextChanged(this, text_start, old_text_end, new_text_end, r);
            watcher->onSelectionChanged(new_sel_start, new_sel_end, old_sel_start, old_sel_end, r);
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
        return utl::STLCU32(text_.length());
    }

    void Editable::append(const string16& text, Reason r) {
        insert(text, utl::STLCU32(text_.length()), r);
    }

    void Editable::insert(const string16& text, uint32_t position, Reason r) {
        if (!text.empty()) {
            text_.insert(position, text);
            notifyTextChanged(position, position, position + utl::STLCU32(text.length()), r);
        }
    }

    void Editable::remove(uint32_t start, uint32_t length, Reason r) {
        if (!text_.empty()) {
            replace(L"", start, length, r);
        }
    }

    void Editable::replace(const string16& text, uint32_t start, uint32_t length, Reason r) {
        text_.replace(start, length, text);
        notifyTextChanged(start, start + 1, start + utl::STLCU32(text.length()) - length + 1, r);
    }

    void Editable::clear(Reason r) {
        if (!text_.empty()) {
            auto old_start = sel_beg_;
            auto old_end = sel_end_;
            auto old_length = utl::STLCInt(text_.length());

            text_.clear();

            sel_beg_ = 0;
            sel_end_ = 0;

            if (old_start != 0 || old_end != 0) {
                notifyEditWatcher(
                    0, old_length, 0,
                    sel_beg_, sel_end_,
                    old_start, old_end, r);
            } else {
                notifyTextChanged(0, old_length, 0, r);
            }
        }
    }

    void Editable::replace(const string16& find, const string16& replacement, Reason r) {
        auto first = text_.find(find);
        if (first != string16::npos) {
            text_.replace(first, find.length(), replacement);
            notifyTextChanged(
                utl::STLCU32(first),
                utl::STLCU32(first + 1),
                utl::STLCU32(first + replacement.length() - find.length() + 1), r);
        }
    }

    void Editable::replaceAll(const string16& find, const string16& replacement, Reason r) {
        auto first = text_.find(find);
        while (first != string16::npos) {
            text_.replace(first, find.length(), replacement);
            first = text_.find(find, first + replacement.length());
        }
    }

    void Editable::insert(const string16& text, Reason r) {
        auto old_start = sel_beg_;
        auto old_end = sel_end_;

        if (old_start == old_end) {
            if (text_.length() < old_end) {
                DCHECK(false);
                text_.insert(text_.length(), text);
            } else {
                text_.insert(old_end, text);
            }
            sel_beg_ += utl::STLCU32(text.length());
            sel_end_ += utl::STLCU32(text.length());

            notifyEditWatcher(
                old_start, old_start, old_start + utl::STLCU32(text.length()),
                sel_beg_, sel_end_, old_start, old_end, r);
        }
    }

    void Editable::remove(Reason r) {
        replace(L"", r);
    }

    void Editable::replace(const string16& text, Reason r) {
        auto old_start = sel_beg_;
        auto old_end = sel_end_;

        bool has_selection = old_start != old_end;

        if (has_selection) {
            text_.replace(old_start, old_end - old_start, text);
            sel_beg_ = sel_end_ = old_start + utl::STLCU32(text.length());

            notifyEditWatcher(
                old_start, old_start + 1, old_start + utl::STLCU32(text.length()) - (old_end - old_start) + 1,
                sel_beg_, sel_end_, old_start, old_end, r);
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
        if (start > text_.length()) start = utl::STLCU32(text_.length());

        if (end < 0) end = 0;
        if (end > text_.length()) end = utl::STLCU32(text_.length());

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
