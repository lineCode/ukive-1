#include "input_connection.h"

#include "ukive/log.h"
#include "ukive/application.h"
#include "ukive/text/tsf_editor.h"
#include "ukive/text/tsf_manager.h"
#include "ukive/utils/hresult_utils.h"
#include "ukive/views/text_view.h"
#include "ukive/window/window.h"


namespace ukive {

    DWORD InputConnection::cookie_ = 0xeec;

    InputConnection::InputConnection(TextView *textView)
    {
        mTextView = textView;

        mTsfEditor = nullptr;
        mIsInitialized = false;
        mIsEditorPushed = false;
    }

    InputConnection::~InputConnection() {
    }


    HRESULT InputConnection::initialization()
    {
        if (mIsInitialized) {
            return S_OK;
        }

        TsfManager *tsfMgr = Application::getTsfManager();

        mTsfEditor = new TsfEditor(cookie_++);
        mTsfEditor->setInputConnection(this);

        RH(tsfMgr->getThreadManager()->CreateDocumentMgr(&mDocumentMgr));

        RH(mDocumentMgr->CreateContext(tsfMgr->getClientId(), 0,
            static_cast<ITextStoreACP*>(mTsfEditor),
            &mEditorContext, &mEditorCookie));

        RH(mEditorContext->QueryInterface(
            IID_ITfContextOwnerCompositionServices, reinterpret_cast<void**>(&mCompServices)));

        mIsInitialized = true;

        return S_OK;
    }

    void InputConnection::pushEditor()
    {
        if (!mIsInitialized || mIsEditorPushed) {
            return;
        }

        HRESULT hr = mDocumentMgr->Push(mEditorContext.get());
        if (FAILED(hr)) {
            Log::e(L"InputConnection", L"pushEditor() failed.\n");
            return;
        }

        mIsEditorPushed = true;
    }

    void InputConnection::popEditor()
    {
        if (!mIsInitialized || !mIsEditorPushed) {
            return;
        }

        HRESULT hr = mDocumentMgr->Pop(TF_POPF_ALL);
        if (FAILED(hr)) {
            Log::e(L"InputConnection", L"popEditor() failed.\n");
            return;
        }

        mIsEditorPushed = false;
    }

    bool InputConnection::mount()
    {
        DLOG(1) << "mount";

        if (!mIsInitialized) {
            return false;
        }

        TsfManager *tsfMgr = Application::getTsfManager();

        HRESULT hr = tsfMgr->getThreadManager()->SetFocus(mDocumentMgr.get());
        if (FAILED(hr)) {
            Log::e(L"InputConnection", L"mount() failed.\n");
            return false;
        }

        return true;
    }

    bool InputConnection::unmount()
    {
        DLOG(1) << "unmount";

        if (!mIsInitialized) {
            return false;
        }

        TsfManager *tsfMgr = Application::getTsfManager();

        HRESULT hr = tsfMgr->getThreadManager()->SetFocus(nullptr);
        if (FAILED(hr)) {
            Log::e(L"InputConnection", L"unmount() failed.\n");
            return false;
        }

        return true;
    }

    bool InputConnection::terminateComposition()
    {
        if (!mIsInitialized) {
            return false;
        }

        HRESULT hr = mCompServices->TerminateComposition(nullptr);
        if (FAILED(hr)) {
            Log::e(L"InputConnection", L"terminateComposition() failed.\n");
            return false;
        }

        return true;
    }


    void InputConnection::notifyStatusChanged(DWORD flags) {
        mTsfEditor->notifyStatusChanged(flags);
    }

    void InputConnection::notifyTextChanged(bool correction, long start, long oldEnd, long newEnd) {
        mTsfEditor->notifyTextChanged(correction, start, oldEnd, newEnd);
    }

    void InputConnection::notifyTextLayoutChanged(TsLayoutCode reason) {
        mTsfEditor->notifyTextLayoutChanged(reason);
    }

    void InputConnection::notifyTextSelectionChanged() {
        mTsfEditor->notifyTextSelectionChanged();
    }


    void InputConnection::onBeginProcess() {
        mTextView->onBeginProcess();
    }

    void InputConnection::onEndProcess() {
        mTextView->onEndProcess();
    }

    bool InputConnection::isReadOnly() {
        return !mTextView->isEditable();
    }

    void InputConnection::determineInsert(
        long start, long end, unsigned long repLength,
        long *resStart, long *resEnd)
    {
        *resStart = start;
        *resEnd = end;
    }

    bool InputConnection::getSelection(
        unsigned long startIndex, unsigned long maxCount,
        TS_SELECTION_ACP *selections, unsigned long *fetchedCount)
    {
        if (startIndex != TF_DEFAULT_SELECTION || maxCount != 1) {
            return false;
        }

        int selStart = mTextView->getSelectionStart();
        int selEnd = mTextView->getSelectionEnd();

        selections[0].acpStart = selStart;
        selections[0].acpEnd = selEnd;
        selections[0].style.fInterimChar = FALSE;
        selections[0].style.ase = TS_AE_NONE;

        *fetchedCount = 1;

        return true;
    }

    bool InputConnection::setSelection(unsigned long count, const TS_SELECTION_ACP *selections)
    {
        if (count != 1) {
            return false;
        }

        int selStart = selections[0].acpStart;
        int selEnd = selections[0].acpEnd;

        if (selStart == selEnd) {
            mTextView->getEditable()->setSelectionForceNotify(selStart);
        } else {
            mTextView->getEditable()->setSelectionForceNotify(selStart, selEnd);
        }

        return true;
    }

    std::wstring InputConnection::getText(long start, long end, long maxLength)
    {
        std::wstring totalText = mTextView->getText();

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

    void InputConnection::setText(long start, long end, std::wstring newText)
    {
        if (start == end) {
            mTextView->getEditable()->insert(newText, start);
        } else {
            mTextView->getEditable()->replace(newText, start, end - start);
        }
    }

    long InputConnection::getTextLength() {
        return mTextView->getText().length();
    }

    bool InputConnection::getTextPositionAtPoint(const POINT *pt, DWORD dwFlags, long *pacp) {
        return true;
    }

    bool InputConnection::getTextBound(long start, long end, RECT *prc, BOOL *pfClipped)
    {
        Rect bound = mTextView->getBoundsInScreen();
        RectF textBound = mTextView->getSelectionBound(start, end);

        prc->left = static_cast<long>(
            bound.left + textBound.left + mTextView->getPaddingLeft() - mTextView->getScrollX());
        prc->top = static_cast<long>(
            bound.top + textBound.top + mTextView->getPaddingTop() - mTextView->getScrollY());
        prc->right = static_cast<long>(
            bound.left + textBound.right + mTextView->getPaddingLeft() - mTextView->getScrollX());
        prc->bottom = static_cast<long>(
            bound.top + textBound.bottom + mTextView->getPaddingTop() - mTextView->getScrollY());

        return true;
    }

    void InputConnection::getTextViewBound(RECT *prc)
    {
        Rect bound = mTextView->getBoundsInScreen();
        prc->left = bound.left;
        prc->top = bound.top;
        prc->right = bound.right;
        prc->bottom = bound.bottom;
    }

    void InputConnection::insertTextAtSelection(
        DWORD dwFlags, std::wstring text,
        LONG *pacpStart, LONG *pacpEnd, TS_TEXTCHANGE *pChange)
    {
        switch (dwFlags)
        {
        case 0:
            *pacpStart = mTextView->getSelectionStart();
            *pacpEnd = mTextView->getSelectionEnd();
            break;

        case TF_IAS_NOQUERY:
            pacpStart = 0;
            pacpEnd = 0;
            break;

        case TF_IAS_QUERYONLY:
            *pacpStart = mTextView->getSelectionStart();
            *pacpEnd = mTextView->getSelectionEnd();
            break;
        }
    }

    HWND InputConnection::getWindowHandle() {
        return mTextView->getWindow()->getHandle();
    }

}