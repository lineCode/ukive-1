#include "input_connection.h"

#include "ukive/log.h"
#include "ukive/application.h"
#include "ukive/text/tsf_editor.h"
#include "ukive/text/tsf_manager.h"
#include "ukive/views/text_view.h"
#include "ukive/window/window.h"


namespace ukive {

    InputConnection::InputConnection(TextView* textView)
        : text_view_(textView),
          tsf_editor_(nullptr),
          editor_cookie_(0),
          is_initialized_(false),
          is_editor_pushed_(false) {
    }

    InputConnection::~InputConnection() {
    }

    HRESULT InputConnection::initialization() {
        if (is_initialized_) {
            return S_OK;
        }

        TsfManager* tsfMgr = Application::getTsfManager();

        tsf_editor_ = new TsfEditor(0xbeef);
        tsf_editor_->setInputConnection(this);

        HRESULT hr = tsfMgr->getThreadManager()->CreateDocumentMgr(&doc_mgr_);
        if (FAILED(hr)) {
            DLOG(Log::ERR) << "Create doc mgr failed: " << hr;
            return hr;
        }

        hr = doc_mgr_->CreateContext(
            tsfMgr->getClientId(), 0,
            static_cast<ITextStoreACP*>(tsf_editor_),
            &editor_context_, &editor_cookie_);
        if (FAILED(hr)) {
            DLOG(Log::ERR) << "Create context failed: " << hr;
            return hr;
        }

        hr = editor_context_->QueryInterface(
            IID_ITfContextOwnerCompositionServices, reinterpret_cast<void**>(&comp_service_));
        if (FAILED(hr)) {
            DLOG(Log::ERR) << "Create composition service failed: " << hr;
            return hr;
        }

        is_initialized_ = true;

        return S_OK;
    }

    void InputConnection::pushEditor() {
        if (!is_initialized_ || is_editor_pushed_) {
            return;
        }

        HRESULT hr = doc_mgr_->Push(editor_context_.get());
        if (FAILED(hr)) {
            DLOG(Log::ERR) << "pushEditor() failed: " << hr;
            return;
        }

        is_editor_pushed_ = true;
    }

    void InputConnection::popEditor() {
        if (!is_initialized_ || !is_editor_pushed_) {
            return;
        }

        HRESULT hr = doc_mgr_->Pop(TF_POPF_ALL);
        if (FAILED(hr)) {
            DLOG(Log::ERR) << "popEditor() failed: " << hr;
            return;
        }

        is_editor_pushed_ = false;
    }

    bool InputConnection::mount() {
        if (!is_initialized_) {
            return false;
        }

        auto mgr = Application::getTsfManager();
        HRESULT hr = mgr->getThreadManager()->SetFocus(doc_mgr_.get());
        if (FAILED(hr)) {
            DLOG(Log::ERR) << "mount() failed: " << hr;
            return false;
        }

        return true;
    }

    bool InputConnection::unmount() {
        if (!is_initialized_) {
            return false;
        }

        auto tsfMgr = Application::getTsfManager();
        HRESULT hr = tsfMgr->getThreadManager()->SetFocus(nullptr);
        if (FAILED(hr)) {
            DLOG(Log::ERR) << "unmount() failed: " << hr;
            return false;
        }

        return true;
    }

    bool InputConnection::terminateComposition() {
        if (!is_initialized_) {
            return false;
        }

        HRESULT hr = comp_service_->TerminateComposition(nullptr);
        if (FAILED(hr)) {
            DLOG(Log::ERR) << "terminateComposition() failed: " << hr;
            return false;
        }

        return true;
    }

    void InputConnection::notifyStatusChanged(DWORD flags) {
        tsf_editor_->notifyStatusChanged(flags);
    }

    void InputConnection::notifyTextChanged(bool correction, long start, long oldEnd, long newEnd) {
        tsf_editor_->notifyTextChanged(correction, start, oldEnd, newEnd);
    }

    void InputConnection::notifyTextLayoutChanged(TsLayoutCode reason) {
        tsf_editor_->notifyTextLayoutChanged(reason);
    }

    void InputConnection::notifyTextSelectionChanged() {
        tsf_editor_->notifyTextSelectionChanged();
    }

    void InputConnection::onBeginProcess() {
        text_view_->onBeginProcess();
    }

    void InputConnection::onEndProcess() {
        text_view_->onEndProcess();
    }

    bool InputConnection::isReadOnly() const {
        return !text_view_->isEditable();
    }

    void InputConnection::determineInsert(
        long start, long end, unsigned long repLength,
        long* resStart, long* resEnd)
    {
        *resStart = start;
        *resEnd = end;
    }

    bool InputConnection::getSelection(
        unsigned long startIndex, unsigned long maxCount,
        TS_SELECTION_ACP* selections, unsigned long* fetchedCount)
    {
        if (startIndex != TF_DEFAULT_SELECTION || maxCount != 1) {
            return false;
        }

        int selStart = text_view_->getSelectionStart();
        int selEnd = text_view_->getSelectionEnd();

        selections[0].acpStart = selStart;
        selections[0].acpEnd = selEnd;
        selections[0].style.fInterimChar = FALSE;
        selections[0].style.ase = TS_AE_END;

        *fetchedCount = 1;

        return true;
    }

    bool InputConnection::setSelection(unsigned long count, const TS_SELECTION_ACP* selections) {
        if (count != 1) {
            return false;
        }

        int selStart = selections[0].acpStart;
        int selEnd = selections[0].acpEnd;

        if (selStart == selEnd) {
            text_view_->getEditable()->setSelectionForceNotify(selStart, Editable::Reason::USER_INPUT);
        } else {
            text_view_->getEditable()->setSelectionForceNotify(selStart, selEnd, Editable::Reason::USER_INPUT);
        }

        return true;
    }

    std::wstring InputConnection::getText(long start, long end, long maxLength) {
        std::wstring totalText = text_view_->getText();

        if (end == -1) {
            if (static_cast<unsigned long>(start + maxLength) <= totalText.length()) {
                return totalText.substr(start, maxLength);
            } else {
                return totalText.substr(start, totalText.length() - start);
            }
        } else {
            if (end - start >= maxLength) {
                return totalText.substr(start, maxLength);
            } else {
                return totalText.substr(start, end - start);
            }
        }
    }

    void InputConnection::setText(long start, long end, std::wstring newText) {
        if (start == end) {
            text_view_->getEditable()->insert(newText, start, Editable::Reason::USER_INPUT);
        } else {
            text_view_->getEditable()->replace(newText, start, end - start, Editable::Reason::USER_INPUT);
        }
    }

    long InputConnection::getTextLength() {
        return text_view_->getText().length();
    }

    bool InputConnection::getTextPositionAtPoint(const POINT* pt, DWORD dwFlags, long* pacp) {
        return true;
    }

    bool InputConnection::getTextBound(long start, long end, RECT* prc, BOOL* pfClipped) {
        Rect bound = text_view_->getBoundsInScreen();
        RectF textBound = text_view_->getSelectionBound(start, end);

        prc->left = static_cast<long>(
            bound.left + textBound.left + text_view_->getPaddingLeft() - text_view_->getScrollX());
        prc->top = static_cast<long>(
            bound.top + textBound.top + text_view_->getPaddingTop() - text_view_->getScrollY());
        prc->right = static_cast<long>(
            bound.left + textBound.right + text_view_->getPaddingLeft() - text_view_->getScrollX());
        prc->bottom = static_cast<long>(
            bound.top + textBound.bottom + text_view_->getPaddingTop() - text_view_->getScrollY());

        return true;
    }

    void InputConnection::getTextViewBound(RECT* prc) {
        Rect bound = text_view_->getBoundsInScreen();
        prc->left = bound.left;
        prc->top = bound.top;
        prc->right = bound.right;
        prc->bottom = bound.bottom;
    }

    void InputConnection::insertTextAtSelection(
        DWORD dwFlags, std::wstring text,
        LONG* pacpStart, LONG* pacpEnd, TS_TEXTCHANGE* pChange)
    {
        switch (dwFlags) {
        case 0: {
            int sel_start = text_view_->getSelectionStart();
            int sel_end = text_view_->getSelectionEnd();

            *pacpStart = sel_start;
            *pacpEnd = sel_end;

            pChange->acpStart = sel_start;
            pChange->acpOldEnd = sel_end;
            pChange->acpNewEnd = sel_start + (text.length() - (sel_end - sel_start));
            break;
        }

        case TF_IAS_NOQUERY: {
            int sel_start = text_view_->getSelectionStart();
            int sel_end = text_view_->getSelectionEnd();

            pChange->acpStart = sel_start;
            pChange->acpOldEnd = sel_end;
            pChange->acpNewEnd = sel_end;
            break;
        }

        case TF_IAS_QUERYONLY:
            *pacpStart = text_view_->getSelectionStart();
            *pacpEnd = text_view_->getSelectionEnd();
            break;

        default:
            DCHECK(false);
            break;
        }
    }

    HWND InputConnection::getWindowHandle() const {
        return text_view_->getWindow()->getHandle();
    }

}