#include "tsf_editor.h"

#include <tsattrs.h>

#include <sstream>

#include "ukive/log.h"
#include "ukive/text/input_connection.h"


namespace ukive {

    TsfEditor::TsfEditor(TsViewCookie tvc)
        :ref_count_(1),
        mViewCookie(tvc),
        input_conn_(nullptr),
        cur_lock_type_(0),
        has_lock_(false) {

        sink_record_.mask = 0;
        sink_record_.punk_id = nullptr;
        sink_record_.sink = nullptr;
    }


    TsfEditor::~TsfEditor() {
    }


    void TsfEditor::setInputConnection(InputConnection *ic) {
        input_conn_ = ic;
    }

    void TsfEditor::notifyTextChanged(bool correction, long start, long oldEnd, long newEnd)
    {
        if (input_conn_ && sink_record_.sink && !has_lock_) {
            TS_TEXTCHANGE textChange;
            textChange.acpStart = start;
            textChange.acpOldEnd = oldEnd;
            textChange.acpNewEnd = newEnd;

            sink_record_.sink->OnTextChange(
                correction ? TS_ST_CORRECTION : 0, &textChange);
        }
    }

    void TsfEditor::notifyTextLayoutChanged(TsLayoutCode reason)
    {
        if (input_conn_ && sink_record_.sink) {
            sink_record_.sink->OnLayoutChange(reason, mViewCookie);
        }
    }

    void TsfEditor::notifyTextSelectionChanged()
    {
        if (input_conn_ && sink_record_.sink && !has_lock_) {
            sink_record_.sink->OnSelectionChange();
        }
    }

    void TsfEditor::notifyStatusChanged(DWORD flags)
    {
        if (input_conn_ && sink_record_.sink) {
            sink_record_.sink->OnStatusChange(flags);
        }
    }

    void TsfEditor::notifyAttrsChanged(
        long start, long end,
        unsigned long attrsCount, const TS_ATTRID *attrs)
    {
        if (input_conn_ && sink_record_.sink)
            sink_record_.sink->OnAttrsChange(start, end, attrsCount, attrs);
    }


    bool TsfEditor::hasReadOnlyLock()
    {
        return ((cur_lock_type_ & TS_LF_READ) == TS_LF_READ);
    }

    bool TsfEditor::hasReadWriteLock()
    {
        return ((cur_lock_type_ & TS_LF_READWRITE) == TS_LF_READWRITE);
    }


    STDMETHODIMP TsfEditor::AdviseSink(REFIID riid, IUnknown *punk, DWORD dwMask)
    {
        HRESULT hr;
        IUnknown *punkID = nullptr;

        // Determine if the sink interface exists.
        // Get the pointer to the IUnknown interface and check if the IUnknown
        // pointer is the same as a pointer to an existing sink.
        // If the sink exists, update the existing sink with the
        // dwMask parameters passed to this method.
        hr = QueryInterface(IID_IUnknown, (LPVOID*)&punkID);
        if (FAILED(hr)) {
            return E_INVALIDARG;
        }

        if (punkID == sink_record_.punk_id) {
            sink_record_.mask = dwMask;
            return S_OK;
        }

        // If the sink does not exist, do the following:
        // 1. Install a new sink.
        // 2. Keep the pointer to the IUnknown interface to uniquely
        //        identify this advise sink.
        // 3. Set the dwMask parameter of this new sink to the dwMask
        //    parameters passed to this method.
        // 4. Increment the reference count.
        // 5. Release the IUnknown pointer, since this pointer is no
        //        longer required.

        if (IsEqualIID(riid, IID_ITextStoreACPSink)) {
            punk->QueryInterface(IID_ITextStoreACPSink, (LPVOID*)&sink_record_.sink);
            sink_record_.punk_id = punkID;
            sink_record_.mask = dwMask;
            punkID->AddRef();
            punkID->Release();

            hr = S_OK;
        }

        return hr;
    }

    STDMETHODIMP TsfEditor::UnadviseSink(IUnknown *punk)
    {
        if (sink_record_.punk_id) {
            sink_record_.punk_id->Release();
            sink_record_.punk_id = nullptr;
        }

        if (sink_record_.sink) {
            sink_record_.sink->Release();
            sink_record_.sink = nullptr;
        }
        sink_record_.mask = 0;

        return S_OK;
    }

    STDMETHODIMP TsfEditor::RequestLock(DWORD dwLockFlags, HRESULT *phrSession)
    {
        Log::d(L"TsfEditor", L"RequestLock()\n");

        if (!input_conn_) {
            return E_FAIL;
        }

        //The document is locked.
        if (!req_queue_.empty() || has_lock_)
        {
            if ((dwLockFlags & TS_LF_SYNC) == TS_LF_SYNC)
            {
                *phrSession = TS_E_SYNCHRONOUS;
                return S_OK;
            }
            else
            {
                *phrSession = TS_S_ASYNC;

                LockRecord *record = new LockRecord();
                record->lock_flags = dwLockFlags;

                req_queue_.push(std::shared_ptr<LockRecord>(record));

                return S_OK;
            }
        }

        input_conn_->onBeginProcess();

        has_lock_ = true;
        cur_lock_type_ = dwLockFlags;

        *phrSession = sink_record_.sink->OnLockGranted(dwLockFlags);

        has_lock_ = false;
        cur_lock_type_ = 0;
        input_conn_->onEndProcess();

        while (!req_queue_.empty())
        {
            auto lockRecord = req_queue_.front();
            req_queue_.pop();

            input_conn_->onBeginProcess();

            has_lock_ = true;
            cur_lock_type_ = ((lockRecord->lock_flags & TS_LF_READ) == TS_LF_READ);

            *phrSession = sink_record_.sink->OnLockGranted(lockRecord->lock_flags);

            has_lock_ = false;
            cur_lock_type_ = 0;
            input_conn_->onEndProcess();
        }

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetStatus(TS_STATUS *pdcs)
    {
        Log::d(L"TsfEditor", L"GetStatus-\n");

        if (pdcs == nullptr) {
            return E_INVALIDARG;
        }

        pdcs->dwStaticFlags = TS_SS_REGIONS;

        if (input_conn_ == nullptr) {
            pdcs->dwDynamicFlags = TS_SD_LOADING;
        } else {
            if (input_conn_->isReadOnly()) {
                pdcs->dwDynamicFlags = TS_SD_READONLY;
            } else {
                pdcs->dwDynamicFlags = 0;
            }
        }

        Log::d(L"TsfEditor", L"GetStatus()\n");

        return S_OK;
    }

    STDMETHODIMP TsfEditor::QueryInsert(
        LONG acpTestStart, LONG acpTestEnd, ULONG cch,
        LONG *pacpResultStart, LONG *pacpResultEnd) {

        Log::d(L"TsfEditor", L"QueryInsert\n");

        long length = input_conn_->getTextLength();
        if (acpTestStart < 0 || acpTestStart > length || acpTestEnd < 0 || acpTestEnd > length) {
            return E_INVALIDARG;
        }

        input_conn_->determineInsert(acpTestStart, acpTestEnd, cch,
            pacpResultStart, pacpResultEnd);

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetSelection(
        ULONG ulIndex, ULONG ulCount,
        TS_SELECTION_ACP *pSelection, ULONG *pcFetched)
    {
        Log::d(L"TsfEditor", L"GetSelection-");

        if (!this->hasReadOnlyLock()) {
            return TS_E_NOLOCK;
        }

        if (!input_conn_->getSelection(ulIndex, ulCount, pSelection, pcFetched)) {
            return TS_E_NOSELECTION;
        }

        /*DLOG(1) << "GetSelection("
            << pSelection->acpStart << ", "
            << pSelection->acpEnd << ")";*/

        return S_OK;
    }

    STDMETHODIMP TsfEditor::SetSelection(ULONG ulCount, const TS_SELECTION_ACP *pSelection)
    {
        std::wstringstream ss;
        ss << "SetSelection(" << pSelection->acpStart << ", " << pSelection->acpEnd << ")\n";
        Log::d(L"TsfEditor", ss.str());

        if (!this->hasReadWriteLock()) {
            return TS_E_NOLOCK;
        }

        if (!input_conn_->setSelection(ulCount, pSelection)) {
            return E_FAIL;
        }

        return S_OK;
    }

    //acpStart [in] Specifies the starting character position.
    //acpEnd [in] Specifies the ending character position. If this parameter is -1, then return all text in the text store.
    //pchPlain [out] Specifies the buffer to receive the plain text data.If this parameter is NULL, then the cchPlainReq parameter must be 0.
    //cchPlainReq [in] Specifies the number of plain text characters passed to the method.
    //pcchPlainRet [out] Receives the number of characters copied into the plain text buffer.This parameter cannot be NULL.Use a parameter if values are not required.
    //prgRunInfo [out] Receives an array of TS_RUNINFO structures.May be NULL only if cRunInfoReq = 0.
    //cRunInfoReq [in] Specifies the size, in characters, of the text run buffer.
    //pcRunInfoRet [out] Receives the number of TS_RUNINFO structures written to the text run buffer.This parameter cannot be NULL.
    //pacpNext [out] Receives the character position of the next unread character.Cannot be NULL.
    STDMETHODIMP TsfEditor::GetText(
        LONG acpStart, LONG acpEnd,
        WCHAR *pchPlain, ULONG cchPlainReq, ULONG *pcchPlainRet,
        TS_RUNINFO *prgRunInfo, ULONG cRunInfoReq, ULONG *pcRunInfoRet, LONG *pacpNext)
    {
        Log::d(L"TsfEditor", L"GetText-");

        if (!this->hasReadOnlyLock()) {
            return TS_E_NOLOCK;
        }

        long length = input_conn_->getTextLength();
        if (acpStart < 0 || acpStart > length || acpEnd < -1 || acpEnd > length) {
            return TF_E_INVALIDPOS;
        }

        if (cchPlainReq == 0) {
            *pcchPlainRet = 0;
            *pacpNext = acpEnd;
        } else {
            std::wstring reqText = input_conn_->getText(acpStart, acpEnd, cchPlainReq);
            reqText._Copy_s(pchPlain, cchPlainReq, reqText.length());
            *pcchPlainRet = reqText.length();

            if (acpEnd == -1) {
                *pacpNext = reqText.length();
            } else {
                *pacpNext = acpEnd;
            }
        }

        if (cRunInfoReq == 0) {
            *pcRunInfoRet = 0;
        } else {
            for (ULONG i = 0; i < cRunInfoReq; ++i) {
                ULONG count = prgRunInfo[i].uCount;
                TsRunType type = prgRunInfo[i].type;
            }

            *pcRunInfoRet = cRunInfoReq;
        }

        std::wstringstream ss;
        ss << "GetText("
            << acpStart << ", "
            << acpEnd << ", "
            << *pacpNext << ")\n";
        Log::d(L"TsfEditor", ss.str());

        return S_OK;
    }

    STDMETHODIMP TsfEditor::SetText(
        DWORD dwFlags, LONG acpStart, LONG acpEnd,
        const WCHAR *pchText, ULONG cch, TS_TEXTCHANGE *pChange)
    {
        Log::d(L"TsfEditor", L"SetText-");

        if (dwFlags == TS_ST_CORRECTION) {
            return S_OK;
        }

        if (input_conn_->isReadOnly()) {
            return TS_E_READONLY;
        }

        long length = input_conn_->getTextLength();
        if (acpStart < 0 || acpStart > length || acpEnd < 0 || acpEnd > length) {
            return TS_E_INVALIDPOS;
        }

        if (!this->hasReadWriteLock()) {
            return TS_E_NOLOCK;
        }

        std::wstring newText = L"";
        newText.append(pchText, cch);

        input_conn_->setText(acpStart, acpEnd, newText);

        pChange->acpStart = acpStart;
        pChange->acpOldEnd = acpEnd;
        pChange->acpNewEnd = acpEnd + newText.length() - (acpEnd - acpStart);

        std::wstringstream ss;
        ss << "SetText(" << dwFlags << ", "
            << newText << ", "
            << acpStart << ", "
            << acpEnd << ", "
            << pChange->acpStart << ", "
            << pChange->acpOldEnd << ", "
            << pChange->acpNewEnd << ")\n";
        Log::d(L"TsfEditor", ss.str());

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetFormattedText(LONG acpStart, LONG acpEnd, IDataObject **ppDataObject)
    {
        Log::d(L"TsfEditor", L"GetFormattedText\n");

        if (!this->hasReadWriteLock()) {
            return TS_E_NOLOCK;
        }

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetEmbedded(LONG acpPos, REFGUID rguidService, REFIID riid, IUnknown **ppunk)
    {
        Log::d(L"TsfEditor", L"GetEmbedded\n");

        return S_OK;
    }

    STDMETHODIMP TsfEditor::QueryInsertEmbedded(const GUID *pguidService, const FORMATETC *pFormatEtc, BOOL *pfInsertable)
    {
        Log::d(L"TsfEditor", L"QueryInsertEmbedded\n");

        return S_OK;
    }

    STDMETHODIMP TsfEditor::InsertEmbedded(DWORD dwFlags, LONG acpStart, LONG acpEnd, IDataObject *pDataObject, TS_TEXTCHANGE *pChange)
    {
        Log::d(L"TsfEditor", L"InsertEmbedded\n");

        return S_OK;
    }

    STDMETHODIMP TsfEditor::RequestSupportedAttrs(DWORD dwFlags, ULONG cFilterAttrs, const TS_ATTRID *paFilterAttrs)
    {
        std::wstringstream ss;
        ss << "RequestSupportedAttrs("
            << dwFlags << ", "
            << cFilterAttrs << ", "
            << paFilterAttrs->Data1 << "-" << paFilterAttrs->Data2 << "-" << paFilterAttrs->Data3 << "-" << paFilterAttrs->Data4 << ")\n";
        Log::d(L"TsfEditor", ss.str());

        return S_OK;
    }

    STDMETHODIMP TsfEditor::RequestAttrsAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID *paFilterAttrs, DWORD dwFlags)
    {
        Log::d(L"TsfEditor", L"RequestAttrsAtPosition\n");

        return S_OK;
    }

    STDMETHODIMP TsfEditor::RequestAttrsTransitioningAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID *paFilterAttrs, DWORD dwFlags)
    {
        Log::d(L"TsfEditor", L"RequestAttrsTransitioningAtPosition\n");

        return S_OK;
    }

    STDMETHODIMP TsfEditor::FindNextAttrTransition(
        LONG acpStart, LONG acpHalt, ULONG cFilterAttrs,
        const TS_ATTRID *paFilterAttrs, DWORD dwFlags, LONG *pacpNext, BOOL *pfFound, LONG *plFoundOffset)
    {
        Log::d(L"TsfEditor", L"FindNextAttrTransition\n");

        return S_OK;
    }

    STDMETHODIMP TsfEditor::RetrieveRequestedAttrs(ULONG ulCount, TS_ATTRVAL *paAttrVals, ULONG *pcFetched)
    {
        Log::d(L"TsfEditor", L"RetrieveRequestedAttrs\n");

        *pcFetched = 0;

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetEndACP(LONG *pacp)
    {
        Log::d(L"TsfEditor", L"GetEndACP\n");

        *pacp = input_conn_->getTextLength();

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetActiveView(TsViewCookie *pvcView)
    {
        Log::d(L"TsfEditor", L"GetActiveView\n");

        *pvcView = mViewCookie;

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetACPFromPoint(TsViewCookie vcView, const POINT *pt, DWORD dwFlags, LONG *pacp)
    {
        Log::d(L"TsfEditor", L"GetACPFromPoint\n");

        if (!input_conn_->getTextPositionAtPoint(pt, dwFlags, pacp)) {
            return TS_E_INVALIDPOINT;
        }

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetTextExt(TsViewCookie vcView, LONG acpStart, LONG acpEnd, RECT *prc, BOOL *pfClipped)
    {
        Log::d(L"TsfEditor", L"GetTextExt-");

        if ((cur_lock_type_ & TS_LF_READ) != TS_LF_READ) {
            return TS_E_NOLOCK;
        }

        long length = input_conn_->getTextLength();
        if (acpStart < 0 || acpStart > length || acpEnd < 0 || acpEnd > length) {
            return TS_E_INVALIDPOS;
        }

        if (!input_conn_->getTextBound(acpStart, acpEnd, prc, pfClipped)) {
            return TS_E_NOLAYOUT;
        }

        std::wstringstream ss;
        ss << "GetTextExt(" << vcView << ", "
            << acpStart << ", " << acpEnd << ", "
            << prc->left << " "
            << prc->top << " "
            << prc->right << " "
            << prc->bottom << ")" << std::endl;
        Log::d(L"TsfEditor", ss.str());

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetScreenExt(TsViewCookie vcView, RECT *prc)
    {
        input_conn_->getTextViewBound(prc);

        std::wstringstream ss;
        ss << "GetScreenExt(" << vcView << ", "
            << prc->left << " "
            << prc->top << " "
            << prc->right << " "
            << prc->bottom << ")" << std::endl;
        Log::d(L"TsfEditor", ss.str());

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetWnd(TsViewCookie vcView, HWND *phwnd)
    {
        *phwnd = input_conn_->getWindowHandle();

        std::wstringstream ss;
        ss << "GetWnd(" << vcView << ", " << *phwnd << ")" << std::endl;
        Log::d(L"TsfEditor", ss.str());

        return S_OK;
    }

    STDMETHODIMP TsfEditor::InsertTextAtSelection(
        DWORD dwFlags, const WCHAR *pchText, ULONG cch,
        LONG *pacpStart, LONG *pacpEnd, TS_TEXTCHANGE *pChange)
    {
        Log::d(L"TsfEditor", L"InsertTextAtSelection-");

        if (cch != 0 && pchText == 0) {
            return E_INVALIDARG;
        }

        if (!hasReadWriteLock()) {
            return TS_E_NOLOCK;
        }

        std::wstring text = L"";
        text.append(pchText, cch);

        input_conn_->insertTextAtSelection(dwFlags, text, pacpStart, pacpEnd, pChange);

        std::wstringstream ss;
        ss << "InsertTextAtSelection(" << dwFlags << ", "
            << text << ", "
            << *pacpStart << ", "
            << *pacpEnd << ", "
            << pChange->acpStart << ", "
            << pChange->acpOldEnd << ", "
            << pChange->acpNewEnd << ")\n";
        Log::d(L"TsfEditor", ss.str());

        return S_OK;
    }

    STDMETHODIMP TsfEditor::InsertEmbeddedAtSelection(DWORD dwFlags, IDataObject *pDataObject, LONG *pacpStart, LONG *pacpEnd, TS_TEXTCHANGE *pChange)
    {
        return S_OK;
    }


    STDMETHODIMP TsfEditor::OnStartComposition(
        __RPC__in_opt ITfCompositionView *pComposition,
        __RPC__out BOOL *pfOk) {

        *pfOk = TRUE;
        return S_OK;
    }

    STDMETHODIMP TsfEditor::OnUpdateComposition(
        __RPC__in_opt ITfCompositionView *pComposition,
        __RPC__in_opt ITfRange *pRangeNew) {
        return S_OK;
    }

    STDMETHODIMP TsfEditor::OnEndComposition(
        __RPC__in_opt ITfCompositionView *pComposition) {
        return S_OK;
    }

    STDMETHODIMP_(ULONG) TsfEditor::AddRef(void) {
        return InterlockedIncrement(&ref_count_);
    }

    STDMETHODIMP_(ULONG) TsfEditor::Release(void) {
        auto cr = InterlockedDecrement(&ref_count_);
        if (cr == 0) {
            delete this;
        }

        return cr;
    }

    STDMETHODIMP TsfEditor::QueryInterface(REFIID riid, void **ppvObj) {
        if (ppvObj == nullptr) {
            return E_INVALIDARG;
        }

        if (IsEqualIID(riid, IID_IUnknown)) {
            *ppvObj = reinterpret_cast<IUnknown*>(this);
        } else if (IsEqualIID(riid, __uuidof(ITextStoreACP))) {
            *ppvObj = static_cast<ITextStoreACP*>(this);
        } else if (IsEqualIID(riid, __uuidof(ITfContextOwnerCompositionSink))) {
            *ppvObj = static_cast<ITfContextOwnerCompositionSink*>(this);
        } else {
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

}