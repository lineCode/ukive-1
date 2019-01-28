#include "tsf_editor.h"

#include <tsattrs.h>

#include <sstream>

#include "ukive/log.h"
#include "ukive/text/input_connection.h"


namespace ukive {

    TsfEditor::TsfEditor(TsViewCookie tvc)
        :ref_count_(1),
        has_lock_(false),
        cur_lock_type_(0),
        input_conn_(nullptr),
        mViewCookie(tvc) {

        sink_record_.mask = 0;
    }

    TsfEditor::~TsfEditor() {
    }


    void TsfEditor::setInputConnection(InputConnection* ic) {
        input_conn_ = ic;
    }

    void TsfEditor::notifyTextChanged(bool correction, long start, long oldEnd, long newEnd) {
        if (input_conn_ && sink_record_.sink && !has_lock_) {
            TS_TEXTCHANGE textChange;
            textChange.acpStart = start;
            textChange.acpOldEnd = oldEnd;
            textChange.acpNewEnd = newEnd;

            sink_record_.sink->OnTextChange(
                correction ? TS_ST_CORRECTION : 0, &textChange);
        }
    }

    void TsfEditor::notifyTextLayoutChanged(TsLayoutCode reason) {
        if (input_conn_ && sink_record_.sink) {
            sink_record_.sink->OnLayoutChange(reason, mViewCookie);
        }
    }

    void TsfEditor::notifyTextSelectionChanged() {
        if (input_conn_ && sink_record_.sink && !has_lock_) {
            sink_record_.sink->OnSelectionChange();
        }
    }

    void TsfEditor::notifyStatusChanged(DWORD flags) {
        if (input_conn_ && sink_record_.sink) {
            sink_record_.sink->OnStatusChange(flags);
        }
    }

    void TsfEditor::notifyAttrsChanged(
        long start, long end,
        unsigned long attrsCount, const TS_ATTRID* attrs)
    {
        if (input_conn_ && sink_record_.sink) {
            sink_record_.sink->OnAttrsChange(start, end, attrsCount, attrs);
        }
    }


    bool TsfEditor::hasReadOnlyLock() const {
        return ((cur_lock_type_ & TS_LF_READ) == TS_LF_READ);
    }

    bool TsfEditor::hasReadWriteLock() const {
        return ((cur_lock_type_ & TS_LF_READWRITE) == TS_LF_READWRITE);
    }


    STDMETHODIMP TsfEditor::AdviseSink(REFIID riid, IUnknown* punk, DWORD dwMask) {
        ComPtr<IUnknown> punk_id;

        // Determine if the sink interface exists.
        // Get the pointer to the IUnknown interface and check if the IUnknown
        // pointer is the same as a pointer to an existing sink.
        // If the sink exists, update the existing sink with the
        // dwMask parameters passed to this method.
        HRESULT hr = QueryInterface(IID_IUnknown, reinterpret_cast<LPVOID*>(&punk_id));
        if (FAILED(hr)) {
            return E_INVALIDARG;
        }

        if (punk_id == sink_record_.punk_id) {
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
            punk->QueryInterface(IID_ITextStoreACPSink, reinterpret_cast<LPVOID*>(&sink_record_.sink));
            sink_record_.punk_id = punk_id;
            sink_record_.mask = dwMask;

            hr = S_OK;
        }

        return hr;
    }

    STDMETHODIMP TsfEditor::UnadviseSink(IUnknown* punk) {
        sink_record_.punk_id.reset();
        sink_record_.sink.reset();
        sink_record_.mask = 0;

        return S_OK;
    }

    STDMETHODIMP TsfEditor::RequestLock(DWORD dwLockFlags, HRESULT* phrSession) {
        //DLOG(Log::INFO) << "RequestLock()";

        if (!input_conn_) {
            return E_FAIL;
        }

        //The document is locked.
        if (!req_queue_.empty() || has_lock_) {
            if ((dwLockFlags & TS_LF_SYNC) == TS_LF_SYNC) {
                *phrSession = TS_E_SYNCHRONOUS;
                return S_OK;
            } else {
                *phrSession = TS_S_ASYNC;

                LockRecord* record = new LockRecord();
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

        while (!req_queue_.empty()) {
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

    STDMETHODIMP TsfEditor::GetStatus(TS_STATUS* pdcs) {
        if (!pdcs) {
            DCHECK(false);
            return E_INVALIDARG;
        }

        pdcs->dwStaticFlags = TS_SS_REGIONS;

        if (!input_conn_) {
            pdcs->dwDynamicFlags = TS_SD_LOADING;
        } else {
            if (input_conn_->isReadOnly()) {
                pdcs->dwDynamicFlags = TS_SD_READONLY;
            } else {
                pdcs->dwDynamicFlags = 0;
            }
        }

        //DLOG(Log::INFO) << "GetStatus()";

        return S_OK;
    }

    STDMETHODIMP TsfEditor::QueryInsert(
        LONG acpTestStart, LONG acpTestEnd, ULONG cch,
        LONG* pacpResultStart, LONG* pacpResultEnd)
    {
        long length = input_conn_->getTextLength();
        if (acpTestStart < 0 || acpTestStart > length || acpTestEnd < 0 || acpTestEnd > length) {
            DCHECK(false);
            return E_INVALIDARG;
        }

        input_conn_->determineInsert(
            acpTestStart, acpTestEnd, cch,
            pacpResultStart, pacpResultEnd);

        DLOG(Log::INFO) << "QueryInsert()";

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetSelection(
        ULONG ulIndex, ULONG ulCount,
        TS_SELECTION_ACP* pSelection, ULONG* pcFetched)
    {
        if (!hasReadOnlyLock()) {
            DCHECK(false);
            return TS_E_NOLOCK;
        }

        if (!input_conn_->getSelection(ulIndex, ulCount, pSelection, pcFetched)) {
            DCHECK(false);
            return TS_E_NOSELECTION;
        }

        DLOG(Log::INFO) << "GetSelection("
            << pSelection->acpStart << ", "
            << pSelection->acpEnd << ")";

        return S_OK;
    }

    STDMETHODIMP TsfEditor::SetSelection(ULONG ulCount, const TS_SELECTION_ACP* pSelection) {
        DLOG(Log::INFO) << "SetSelection("
            << ulCount << ", " << pSelection->acpStart << ", " << pSelection->acpEnd << ")";

        if (!hasReadWriteLock()) {
            DCHECK(false);
            return TS_E_NOLOCK;
        }

        if (!input_conn_->setSelection(ulCount, pSelection)) {
            DCHECK(false);
            return E_FAIL;
        }

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetText(
        LONG acpStart, LONG acpEnd,
        WCHAR* pchPlain, ULONG cchPlainReq, ULONG* pcchPlainRet,
        TS_RUNINFO* prgRunInfo, ULONG cRunInfoReq, ULONG* pcRunInfoRet, LONG* pacpNext)
    {
        if (!hasReadOnlyLock()) {
            DCHECK(false);
            return TS_E_NOLOCK;
        }

        long length = input_conn_->getTextLength();
        if (acpStart < 0 || acpStart > length || acpEnd < -1 || acpEnd > length) {
            DCHECK(false);
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
            prgRunInfo[0].uCount = *pcchPlainRet;
            prgRunInfo[0].type = TS_RT_PLAIN;
            *pcRunInfoRet = 1;
        }

        DLOG(Log::INFO) << "GetText("
            << acpStart << ", "
            << acpEnd << ", "
            << *pacpNext << ")";

        return S_OK;
    }

    STDMETHODIMP TsfEditor::SetText(
        DWORD dwFlags, LONG acpStart, LONG acpEnd,
        const WCHAR* pchText, ULONG cch, TS_TEXTCHANGE* pChange)
    {
        if (dwFlags == TS_ST_CORRECTION) {
            return S_OK;
        }

        if (input_conn_->isReadOnly()) {
            DCHECK(false);
            return TS_E_READONLY;
        }

        long length = input_conn_->getTextLength();
        if (acpStart < 0 || acpStart > length || acpEnd < 0 || acpEnd > length) {
            DCHECK(false);
            return TS_E_INVALIDPOS;
        }

        if (!hasReadWriteLock()) {
            DCHECK(false);
            return TS_E_NOLOCK;
        }

        std::wstring newText = L"";
        newText.append(pchText, cch);

        input_conn_->setText(acpStart, acpEnd, newText);

        pChange->acpStart = acpStart;
        pChange->acpOldEnd = acpEnd;
        pChange->acpNewEnd = acpEnd + newText.length() - (acpEnd - acpStart);

        DLOG(Log::INFO) << "SetText(" << dwFlags << ", "
            << newText << ", "
            << acpStart << ", "
            << acpEnd << ", "
            << pChange->acpStart << ", "
            << pChange->acpOldEnd << ", "
            << pChange->acpNewEnd << ")";

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetFormattedText(
        LONG acpStart, LONG acpEnd, IDataObject** ppDataObject)
    {
        DLOG(Log::INFO) << "GetFormattedText()";

        if (!hasReadWriteLock()) {
            return TS_E_NOLOCK;
        }

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetEmbedded(
        LONG acpPos, REFGUID rguidService, REFIID riid, IUnknown** ppunk)
    {
        DLOG(Log::INFO) << "GetEmbedded()";
        return S_OK;
    }

    STDMETHODIMP TsfEditor::QueryInsertEmbedded(
        const GUID* pguidService, const FORMATETC* pFormatEtc, BOOL* pfInsertable)
    {
        DLOG(Log::INFO) << "QueryInsertEmbedded()";
        return S_OK;
    }

    STDMETHODIMP TsfEditor::InsertEmbedded(
        DWORD dwFlags, LONG acpStart, LONG acpEnd, IDataObject* pDataObject, TS_TEXTCHANGE* pChange)
    {
        DLOG(Log::INFO) << "InsertEmbedded()";
        return S_OK;
    }

    STDMETHODIMP TsfEditor::RequestSupportedAttrs(
        DWORD dwFlags, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs)
    {
        /*DLOG(Log::INFO) << "RequestSupportedAttrs("
            << dwFlags << ", "
            << cFilterAttrs << ", "
            << paFilterAttrs->Data1 << "-"
            << paFilterAttrs->Data2 << "-"
            << paFilterAttrs->Data3 << "-"
            << paFilterAttrs->Data4 << ")";*/

        return S_OK;
    }

    STDMETHODIMP TsfEditor::RequestAttrsAtPosition(
        LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs, DWORD dwFlags)
    {
        DLOG(Log::INFO) << "RequestAttrsAtPosition()";
        return S_OK;
    }

    STDMETHODIMP TsfEditor::RequestAttrsTransitioningAtPosition(
        LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs, DWORD dwFlags)
    {
        DLOG(Log::INFO) << "RequestAttrsTransitioningAtPosition()";
        return S_OK;
    }

    STDMETHODIMP TsfEditor::FindNextAttrTransition(
        LONG acpStart, LONG acpHalt, ULONG cFilterAttrs,
        const TS_ATTRID* paFilterAttrs, DWORD dwFlags, LONG* pacpNext, BOOL* pfFound, LONG* plFoundOffset)
    {
        DLOG(Log::INFO) << "FindNextAttrTransition()";
        return S_OK;
    }

    STDMETHODIMP TsfEditor::RetrieveRequestedAttrs(
        ULONG ulCount, TS_ATTRVAL* paAttrVals, ULONG* pcFetched)
    {
        //DLOG(Log::INFO) << "RetrieveRequestedAttrs()";

        *pcFetched = 0;

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetEndACP(LONG* pacp) {
        DLOG(Log::INFO) << "GetEndACP()";

        *pacp = input_conn_->getTextLength();

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetActiveView(TsViewCookie* pvcView) {
        //DLOG(Log::INFO) << "GetActiveView()";

        *pvcView = mViewCookie;

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetACPFromPoint(
        TsViewCookie vcView, const POINT* pt, DWORD dwFlags, LONG* pacp)
    {
        DLOG(Log::INFO) << "GetACPFromPoint()";

        if (!input_conn_->getTextPositionAtPoint(pt, dwFlags, pacp)) {
            return TS_E_INVALIDPOINT;
        }

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetTextExt(
        TsViewCookie vcView, LONG acpStart, LONG acpEnd, RECT* prc, BOOL* pfClipped)
    {
        if ((cur_lock_type_ & TS_LF_READ) != TS_LF_READ) {
            DCHECK(false);
            return TS_E_NOLOCK;
        }

        long length = input_conn_->getTextLength();
        if (acpStart < 0 || acpStart > length || acpEnd < 0 || acpEnd > length) {
            DCHECK(false);
            return TS_E_INVALIDPOS;
        }

        if (!input_conn_->getTextBound(acpStart, acpEnd, prc, pfClipped)) {
            DCHECK(false);
            return TS_E_NOLAYOUT;
        }

        /*DLOG(Log::INFO) << "GetTextExt(" << vcView << ", "
            << acpStart << ", " << acpEnd << ", "
            << prc->left << " "
            << prc->top << " "
            << prc->right << " "
            << prc->bottom << ")";*/

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetScreenExt(TsViewCookie vcView, RECT* prc) {
        input_conn_->getTextViewBound(prc);

        /*DLOG(Log::INFO) << "GetScreenExt(" << vcView << ", "
            << prc->left << " "
            << prc->top << " "
            << prc->right << " "
            << prc->bottom << ")";*/

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetWnd(TsViewCookie vcView, HWND* phwnd) {
        *phwnd = input_conn_->getWindowHandle();

        //DLOG(Log::INFO) << "GetWnd(" << vcView << ", " << *phwnd << ")";

        return S_OK;
    }

    STDMETHODIMP TsfEditor::InsertTextAtSelection(
        DWORD dwFlags, const WCHAR* pchText, ULONG cch,
        LONG* pacpStart, LONG* pacpEnd, TS_TEXTCHANGE* pChange)
    {
        if (cch != 0 && !pchText) {
            DCHECK(false);
            return E_INVALIDARG;
        }

        if (!hasReadWriteLock()) {
            DCHECK(false);
            return TS_E_NOLOCK;
        }

        std::wstring text = L"";
        text.append(pchText, cch);

        input_conn_->insertTextAtSelection(dwFlags, text, pacpStart, pacpEnd, pChange);

        DLOG(Log::INFO) << "InsertTextAtSelection(" << dwFlags << ", "
            << text << ", "
            << *pacpStart << ", "
            << *pacpEnd << ", "
            << pChange->acpStart << ", "
            << pChange->acpOldEnd << ", "
            << pChange->acpNewEnd << ")";

        return S_OK;
    }

    STDMETHODIMP TsfEditor::InsertEmbeddedAtSelection(
        DWORD dwFlags, IDataObject* pDataObject, LONG* pacpStart, LONG* pacpEnd, TS_TEXTCHANGE* pChange)
    {
        return S_OK;
    }


    STDMETHODIMP TsfEditor::OnStartComposition(
        ITfCompositionView* pComposition, BOOL* pfOk)
    {
        *pfOk = TRUE;
        return S_OK;
    }

    STDMETHODIMP TsfEditor::OnUpdateComposition(
        ITfCompositionView* pComposition, ITfRange* pRangeNew)
    {
        return S_OK;
    }

    STDMETHODIMP TsfEditor::OnEndComposition(
        ITfCompositionView* pComposition)
    {
        return S_OK;
    }

    STDMETHODIMP_(ULONG) TsfEditor::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }

    STDMETHODIMP_(ULONG) TsfEditor::Release() {
        auto cr = InterlockedDecrement(&ref_count_);
        if (cr == 0) {
            delete this;
        }

        return cr;
    }

    STDMETHODIMP TsfEditor::QueryInterface(REFIID riid, void** ppvObj) {
        if (!ppvObj) {
            return E_POINTER;
        }

        if (IsEqualIID(riid, IID_IUnknown)) {
            *ppvObj = reinterpret_cast<IUnknown*>(this);
        } else if (IsEqualIID(riid, __uuidof(ITextStoreACP))) {
            *ppvObj = static_cast<ITextStoreACP*>(this);
        } else if (IsEqualIID(riid, __uuidof(ITfContextOwnerCompositionSink))) {
            *ppvObj = static_cast<ITfContextOwnerCompositionSink*>(this);
        } else {
            *ppvObj = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

}