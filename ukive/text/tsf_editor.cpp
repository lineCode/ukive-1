#include "tsf_editor.h"

#include <tsattrs.h>

#include <sstream>

#include "ukive/text/input_connection.h"


namespace ukive {

    TsfEditor::TsfEditor() {
        mRefCount = 1;
        mInputConnection = nullptr;
        mCurLockType = 0;
        mHasLock = false;

        mAdviseSink.dwMask = 0;
        mAdviseSink.punkID = nullptr;
        mAdviseSink.textStoreACPSink = nullptr;
    }


    TsfEditor::~TsfEditor()
    {
    }


    void TsfEditor::setInputConnection(InputConnection *ic)
    {
        mInputConnection = ic;
    }

    void TsfEditor::notifyTextChanged(bool correction, long start, long oldEnd, long newEnd)
    {
        if (mInputConnection && mAdviseSink.textStoreACPSink && !mHasLock)
        {
            TS_TEXTCHANGE textChange;
            textChange.acpStart = start;
            textChange.acpOldEnd = oldEnd;
            textChange.acpNewEnd = newEnd;

            mAdviseSink.textStoreACPSink->OnTextChange(
                correction ? TS_ST_CORRECTION : 0, &textChange);
        }
    }

    void TsfEditor::notifyTextLayoutChanged(TsLayoutCode reason)
    {
        if (mInputConnection && mAdviseSink.textStoreACPSink)
            mAdviseSink.textStoreACPSink->OnLayoutChange(reason, mViewCookie);
    }

    void TsfEditor::notifyTextSelectionChanged()
    {
        if (mInputConnection && mAdviseSink.textStoreACPSink && !mHasLock)
            mAdviseSink.textStoreACPSink->OnSelectionChange();
    }

    void TsfEditor::notifyStatusChanged(DWORD flags)
    {
        if (mInputConnection && mAdviseSink.textStoreACPSink)
            mAdviseSink.textStoreACPSink->OnStatusChange(flags);
    }

    void TsfEditor::notifyAttrsChanged(
        long start, long end,
        unsigned long attrsCount, const TS_ATTRID *attrs)
    {
        if (mInputConnection && mAdviseSink.textStoreACPSink)
            mAdviseSink.textStoreACPSink->OnAttrsChange(start, end, attrsCount, attrs);
    }


    bool TsfEditor::hasReadOnlyLock()
    {
        return ((mCurLockType & TS_LF_READ) == TS_LF_READ);
    }

    bool TsfEditor::hasReadWriteLock()
    {
        return ((mCurLockType & TS_LF_READWRITE) == TS_LF_READWRITE);
    }


    STDMETHODIMP TsfEditor::AdviseSink(REFIID riid, IUnknown *punk, DWORD dwMask)
    {
        HRESULT hr;
        IUnknown *punkID;

        // Determine if the sink interface exists.
        // Get the pointer to the IUnknown interface and check if the IUnknown
        // pointer is the same as a pointer to an existing sink.
        // If the sink exists, update the existing sink with the
        // dwMask parameters passed to this method.
        hr = QueryInterface(IID_IUnknown, (LPVOID*)&punkID);
        if (FAILED(hr))
            return E_INVALIDARG;

        if (punkID == mAdviseSink.punkID)
        {
            mAdviseSink.dwMask = dwMask;
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

        if (IsEqualIID(riid, IID_ITextStoreACPSink))
        {
            punk->QueryInterface(IID_ITextStoreACPSink, (LPVOID*)&mAdviseSink.textStoreACPSink);
            mAdviseSink.punkID = punkID;
            mAdviseSink.dwMask = dwMask;
            punkID->AddRef();
            punkID->Release();

            hr = S_OK;
        }

        return hr;
    }

    STDMETHODIMP TsfEditor::UnadviseSink(IUnknown *punk)
    {
        if (mAdviseSink.punkID)
        {
            mAdviseSink.punkID->Release();
            mAdviseSink.punkID = nullptr;
        }
        if (mAdviseSink.textStoreACPSink)
        {
            mAdviseSink.textStoreACPSink->Release();
            mAdviseSink.textStoreACPSink = nullptr;
        }
        mAdviseSink.dwMask = 0;

        return S_OK;
    }

    STDMETHODIMP TsfEditor::RequestLock(DWORD dwLockFlags, HRESULT *phrSession)
    {
        if (!mInputConnection)
            return E_FAIL;

        //The document is locked.
        if (!mReqQueue.empty() || mHasLock)
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
                record->dwLockFlags = dwLockFlags;

                mReqQueue.push(std::shared_ptr<LockRecord>(record));

                return S_OK;
            }
        }

        mInputConnection->onBeginProcess();

        mHasLock = true;
        mCurLockType = dwLockFlags;

        *phrSession = mAdviseSink.textStoreACPSink->OnLockGranted(dwLockFlags);

        mHasLock = false;
        mCurLockType = 0;
        mInputConnection->onEndProcess();

        while (!mReqQueue.empty())
        {
            auto lockRecord = mReqQueue.front();
            mReqQueue.pop();

            mInputConnection->onBeginProcess();

            mHasLock = true;
            mCurLockType = ((lockRecord->dwLockFlags & TS_LF_READ) == TS_LF_READ);

            *phrSession = mAdviseSink.textStoreACPSink->OnLockGranted(lockRecord->dwLockFlags);

            mHasLock = false;
            mCurLockType = 0;
            mInputConnection->onEndProcess();
        }

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetStatus(TS_STATUS *pdcs)
    {
        if (pdcs == 0)
            return E_INVALIDARG;

        pdcs->dwStaticFlags = TS_SS_REGIONS;

        if (mInputConnection == nullptr)
            pdcs->dwDynamicFlags = TS_SD_LOADING;
        else {
            if (mInputConnection->isReadOnly())
                pdcs->dwDynamicFlags = TS_SD_READONLY;
            else
                pdcs->dwDynamicFlags = 0;
        }

        return S_OK;
    }

    STDMETHODIMP TsfEditor::QueryInsert(
        LONG acpTestStart, LONG acpTestEnd, ULONG cch,
        LONG *pacpResultStart, LONG *pacpResultEnd) {

        ::OutputDebugStringW(L"QueryInsert\n");

        long length = mInputConnection->getTextLength();
        if (acpTestStart < 0 || acpTestStart > length || acpTestEnd < 0 || acpTestEnd > length)
            return E_INVALIDARG;

        mInputConnection->determineInsert(acpTestStart, acpTestEnd, cch,
            pacpResultStart, pacpResultEnd);

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetSelection(
        ULONG ulIndex, ULONG ulCount,
        TS_SELECTION_ACP *pSelection, ULONG *pcFetched)
    {
        ::OutputDebugStringW(L"GetSelection-");

        if (!this->hasReadOnlyLock())
            return TS_E_NOLOCK;

        if (!mInputConnection->getSelection(ulIndex, ulCount, pSelection, pcFetched))
            return TS_E_NOSELECTION;

        std::wstringstream ss;
        ss << "GetSelection("
            << pSelection->acpStart << ", "
            << pSelection->acpEnd << ")\n";
        ::OutputDebugStringW(ss.str().c_str());

        return S_OK;
    }

    STDMETHODIMP TsfEditor::SetSelection(ULONG ulCount, const TS_SELECTION_ACP *pSelection)
    {
        std::wstringstream ss;
        ss << "SetSelection(" << pSelection->acpStart << ", " << pSelection->acpEnd << ")\n";
        ::OutputDebugStringW(ss.str().c_str());

        if (!this->hasReadWriteLock())
            return TS_E_NOLOCK;

        if (!mInputConnection->setSelection(ulCount, pSelection))
            return E_FAIL;

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
        ::OutputDebugStringW(L"GetText-");

        if (!this->hasReadOnlyLock())
            return TS_E_NOLOCK;

        long length = mInputConnection->getTextLength();
        if (acpStart < 0 || acpStart > length || acpEnd < -1 || acpEnd > length)
            return TF_E_INVALIDPOS;

        if (cchPlainReq == 0)
        {
            *pcchPlainRet = 0;
            *pacpNext = acpEnd;
        }
        else
        {
            std::wstring reqText = mInputConnection->getText(acpStart, acpEnd, cchPlainReq);
            reqText._Copy_s(pchPlain, cchPlainReq, reqText.length());
            *pcchPlainRet = reqText.length();

            if (acpEnd == -1)
                *pacpNext = reqText.length();
            else
                *pacpNext = acpEnd;
        }

        if (cRunInfoReq == 0)
            *pcRunInfoRet = 0;
        else
        {
            for (ULONG i = 0; i < cRunInfoReq; ++i)
            {
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
        ::OutputDebugStringW(ss.str().c_str());

        return S_OK;
    }

    STDMETHODIMP TsfEditor::SetText(
        DWORD dwFlags, LONG acpStart, LONG acpEnd,
        const WCHAR *pchText, ULONG cch, TS_TEXTCHANGE *pChange)
    {
        ::OutputDebugStringW(L"SetText-");

        if (dwFlags == TS_ST_CORRECTION)
            return S_OK;

        if (mInputConnection->isReadOnly())
            return TS_E_READONLY;

        long length = mInputConnection->getTextLength();
        if (acpStart < 0 || acpStart > length || acpEnd < 0 || acpEnd > length)
            return TS_E_INVALIDPOS;

        if (!this->hasReadWriteLock())
            return TS_E_NOLOCK;

        std::wstring newText = L"";
        newText.append(pchText, cch);

        mInputConnection->setText(acpStart, acpEnd, newText);

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
        ::OutputDebugStringW(ss.str().c_str());

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetFormattedText(LONG acpStart, LONG acpEnd, IDataObject **ppDataObject)
    {
        ::OutputDebugStringW(L"GetFormattedText\n");

        if (!this->hasReadWriteLock())
            return TS_E_NOLOCK;

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetEmbedded(LONG acpPos, REFGUID rguidService, REFIID riid, IUnknown **ppunk)
    {
        ::OutputDebugStringW(L"GetEmbedded\n");

        return S_OK;
    }

    STDMETHODIMP TsfEditor::QueryInsertEmbedded(const GUID *pguidService, const FORMATETC *pFormatEtc, BOOL *pfInsertable)
    {
        ::OutputDebugStringW(L"QueryInsertEmbedded\n");

        return S_OK;
    }

    STDMETHODIMP TsfEditor::InsertEmbedded(DWORD dwFlags, LONG acpStart, LONG acpEnd, IDataObject *pDataObject, TS_TEXTCHANGE *pChange)
    {
        ::OutputDebugStringW(L"InsertEmbedded\n");

        return S_OK;
    }

    STDMETHODIMP TsfEditor::RequestSupportedAttrs(DWORD dwFlags, ULONG cFilterAttrs, const TS_ATTRID *paFilterAttrs)
    {
        std::wstringstream ss;
        ss << "RequestSupportedAttrs("
            << dwFlags << ", "
            << cFilterAttrs << ", "
            << paFilterAttrs->Data1 << "-" << paFilterAttrs->Data2 << "-" << paFilterAttrs->Data3 << "-" << paFilterAttrs->Data4 << ")\n";
        ::OutputDebugStringW(ss.str().c_str());

        return S_OK;
    }

    STDMETHODIMP TsfEditor::RequestAttrsAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID *paFilterAttrs, DWORD dwFlags)
    {
        ::OutputDebugStringW(L"RequestAttrsAtPosition\n");

        return S_OK;
    }

    STDMETHODIMP TsfEditor::RequestAttrsTransitioningAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID *paFilterAttrs, DWORD dwFlags)
    {
        ::OutputDebugStringW(L"RequestAttrsTransitioningAtPosition\n");

        return S_OK;
    }

    STDMETHODIMP TsfEditor::FindNextAttrTransition(
        LONG acpStart, LONG acpHalt, ULONG cFilterAttrs,
        const TS_ATTRID *paFilterAttrs, DWORD dwFlags, LONG *pacpNext, BOOL *pfFound, LONG *plFoundOffset)
    {
        ::OutputDebugStringW(L"FindNextAttrTransition\n");

        return S_OK;
    }

    STDMETHODIMP TsfEditor::RetrieveRequestedAttrs(ULONG ulCount, TS_ATTRVAL *paAttrVals, ULONG *pcFetched)
    {
        ::OutputDebugStringW(L"RetrieveRequestedAttrs\n");

        *pcFetched = 0;

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetEndACP(LONG *pacp)
    {
        ::OutputDebugStringW(L"GetEndACP\n");

        *pacp = mInputConnection->getTextLength();

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetActiveView(TsViewCookie *pvcView)
    {
        *pvcView = mViewCookie;

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetACPFromPoint(TsViewCookie vcView, const POINT *pt, DWORD dwFlags, LONG *pacp)
    {
        ::OutputDebugStringW(L"GetACPFromPoint\n");

        if (!mInputConnection->getTextPositionAtPoint(pt, dwFlags, pacp))
            return TS_E_INVALIDPOINT;

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetTextExt(TsViewCookie vcView, LONG acpStart, LONG acpEnd, RECT *prc, BOOL *pfClipped)
    {
        if ((mCurLockType & TS_LF_READ) != TS_LF_READ)
            return TS_E_NOLOCK;

        long length = mInputConnection->getTextLength();
        if (acpStart < 0 || acpStart > length || acpEnd < 0 || acpEnd > length)
            return TS_E_INVALIDPOS;

        if (!mInputConnection->getTextBound(acpStart, acpEnd, prc, pfClipped))
            return TS_E_NOLAYOUT;

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetScreenExt(TsViewCookie vcView, RECT *prc)
    {
        mInputConnection->getTextViewBound(prc);

        return S_OK;
    }

    STDMETHODIMP TsfEditor::GetWnd(TsViewCookie vcView, HWND *phwnd)
    {
        *phwnd = mInputConnection->getWindowHandle();

        return S_OK;
    }

    STDMETHODIMP TsfEditor::InsertTextAtSelection(
        DWORD dwFlags, const WCHAR *pchText, ULONG cch,
        LONG *pacpStart, LONG *pacpEnd, TS_TEXTCHANGE *pChange)
    {
        ::OutputDebugStringW(L"InsertTextAtSelection-");

        if (cch != 0 && pchText == 0)
            return E_INVALIDARG;

        if (!this->hasReadWriteLock())
            return TS_E_NOLOCK;

        std::wstring text = L"";
        text.append(pchText, cch);

        mInputConnection->insertTextAtSelection(dwFlags, text, pacpStart, pacpEnd, pChange);

        std::wstringstream ss;
        ss << "InsertTextAtSelection(" << dwFlags << ", "
            << text << ", "
            << *pacpStart << ", "
            << *pacpEnd << ", "
            << pChange->acpStart << ", "
            << pChange->acpOldEnd << ", "
            << pChange->acpNewEnd << ")\n";
        ::OutputDebugStringW(ss.str().c_str());

        return S_OK;
    }

    STDMETHODIMP TsfEditor::InsertEmbeddedAtSelection(DWORD dwFlags, IDataObject *pDataObject, LONG *pacpStart, LONG *pacpEnd, TS_TEXTCHANGE *pChange)
    {
        return S_OK;
    }


    STDMETHODIMP TsfEditor::OnStartComposition(
        __RPC__in_opt ITfCompositionView *pComposition,
        __RPC__out BOOL *pfOk)
    {
        *pfOk = TRUE;

        return S_OK;
    }

    STDMETHODIMP TsfEditor::OnUpdateComposition(
        __RPC__in_opt ITfCompositionView *pComposition,
        __RPC__in_opt ITfRange *pRangeNew)
    {
        return S_OK;
    }

    STDMETHODIMP TsfEditor::OnEndComposition(
        __RPC__in_opt ITfCompositionView *pComposition)
    {
        return S_OK;
    }


    STDMETHODIMP TsfEditor::QueryInterface(REFIID riid, void **ppvObj)
    {
        if (ppvObj == 0)
            return E_INVALIDARG;

        *ppvObj = 0;

        if (IsEqualIID(riid, IID_IUnknown))
        {
            *ppvObj = reinterpret_cast<IUnknown*>(this);
        }
        else if (IsEqualIID(riid, __uuidof(ITextStoreACP)))
        {
            *ppvObj = (ITextStoreACP*)this;
        }
        else if (IsEqualIID(riid, __uuidof(ITfContextOwnerCompositionSink)))
        {
            *ppvObj = (ITfContextOwnerCompositionSink*)this;
        }

        if (*ppvObj)
        {
            AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) TsfEditor::AddRef(void)
    {
        return InterlockedIncrement(&mRefCount);
    }

    STDMETHODIMP_(ULONG) TsfEditor::Release(void)
    {
        LONG cr = InterlockedDecrement(&mRefCount);

        if (mRefCount == 0)
            delete this;

        return cr;
    }

}