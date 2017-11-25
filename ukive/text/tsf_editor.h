#ifndef UKIVE_TEXT_TSF_EDITOR_H_
#define UKIVE_TEXT_TSF_EDITOR_H_

#include <msctf.h>
#include <TextStor.h>

#include <memory>
#include <queue>


namespace ukive {

    class InputConnection;

    class TsfEditor : public ITextStoreACP, public ITfContextOwnerCompositionSink
    {
    private:
        struct ADVISE_SINK {
            IUnknown *punkID;
            ITextStoreACPSink *textStoreACPSink;
            DWORD dwMask;
        };

        struct LockRecord {
            DWORD dwLockFlags;
        };

        LONG mRefCount;

        bool mHasLock;
        DWORD mCurLockType;
        ADVISE_SINK mAdviseSink;
        InputConnection *mInputConnection;

        std::queue<std::shared_ptr<LockRecord>> mReqQueue;

        const TsViewCookie mViewCookie = 0xBEEFBEEF;

    public:
        TsfEditor();
        ~TsfEditor();

        void setInputConnection(InputConnection *ic);

        void notifyTextChanged(bool correction, long start, long oldEnd, long newEnd);
        void notifyTextLayoutChanged(TsLayoutCode reason);
        void notifyTextSelectionChanged();
        void notifyStatusChanged(DWORD flags);
        void notifyAttrsChanged(long start, long end, unsigned long attrsCount, const TS_ATTRID *attrs);

        bool hasReadOnlyLock();
        bool hasReadWriteLock();

        //ITextStoreACP methods.
        STDMETHODIMP AdviseSink(REFIID riid, IUnknown *punk, DWORD dwMask) override;
        STDMETHODIMP UnadviseSink(IUnknown *punk) override;

        //This method is called by the TSF manager to provide a document lock in order to modify the document.
        //This method calls the ITextStoreACPSink::OnLockGranted method to create the document lock.
        STDMETHODIMP RequestLock(DWORD dwLockFlags, HRESULT *phrSession) override;

        //This method obtains the document status.
        //The document status is returned through the TS_STATUS structure.
        STDMETHODIMP GetStatus(TS_STATUS *pdcs) override;

        //This method determines whether the specified start and end character positions are valid.
        //Use this method to adjust an edit to a document before executing the edit.
        //The method must not return values outside the range of the document.
        STDMETHODIMP QueryInsert(
            LONG acpTestStart, LONG acpTestEnd, ULONG cch,
            LONG *pacpResultStart, LONG *pacpResultEnd) override;

        //This method returns the character position of a text selection in a document.
        //This method supports multiple text selections. The caller must have a read-only lock on the document before calling this method.
        STDMETHODIMP GetSelection(
            ULONG ulIndex, ULONG ulCount,
            TS_SELECTION_ACP *pSelection, ULONG *pcFetched) override;
        STDMETHODIMP SetSelection(ULONG ulCount, const TS_SELECTION_ACP *pSelection) override;
        STDMETHODIMP GetText(
            LONG acpStart, LONG acpEnd,
            WCHAR *pchPlain, ULONG cchPlainReq, ULONG *pcchPlainRet,
            TS_RUNINFO *prgRunInfo, ULONG cRunInfoReq, ULONG *pcRunInfoRet, LONG *pacpNext) override;
        STDMETHODIMP SetText(
            DWORD dwFlags, LONG acpStart, LONG acpEnd,
            const WCHAR *pchText, ULONG cch, TS_TEXTCHANGE *pChange) override;
        STDMETHODIMP GetFormattedText(LONG acpStart, LONG acpEnd, IDataObject **ppDataObject) override;
        STDMETHODIMP GetEmbedded(LONG acpPos, REFGUID rguidService, REFIID riid, IUnknown **ppunk) override;
        STDMETHODIMP QueryInsertEmbedded(const GUID *pguidService, const FORMATETC *pFormatEtc, BOOL *pfInsertable) override;
        STDMETHODIMP InsertEmbedded(DWORD dwFlags, LONG acpStart, LONG acpEnd, IDataObject *pDataObject, TS_TEXTCHANGE *pChange) override;
        STDMETHODIMP RequestSupportedAttrs(DWORD dwFlags, ULONG cFilterAttrs, const TS_ATTRID *paFilterAttrs) override;
        STDMETHODIMP RequestAttrsAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID *paFilterAttrs, DWORD dwFlags) override;
        STDMETHODIMP RequestAttrsTransitioningAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID *paFilterAttrs, DWORD dwFlags) override;
        STDMETHODIMP FindNextAttrTransition(
            LONG acpStart, LONG acpHalt,
            ULONG cFilterAttrs, const TS_ATTRID *paFilterAttrs,
            DWORD dwFlags, LONG *pacpNext, BOOL *pfFound, LONG *plFoundOffset) override;
        STDMETHODIMP RetrieveRequestedAttrs(ULONG ulCount, TS_ATTRVAL *paAttrVals, ULONG *pcFetched) override;
        STDMETHODIMP GetEndACP(LONG *pacp) override;
        STDMETHODIMP GetActiveView(TsViewCookie *pvcView) override;
        STDMETHODIMP GetACPFromPoint(TsViewCookie vcView, const POINT *pt, DWORD dwFlags, LONG *pacp) override;
        STDMETHODIMP GetTextExt(TsViewCookie vcView, LONG acpStart, LONG acpEnd, RECT *prc, BOOL *pfClipped) override;
        STDMETHODIMP GetScreenExt(TsViewCookie vcView, RECT *prc) override;
        STDMETHODIMP GetWnd(TsViewCookie vcView, HWND *phwnd) override;
        STDMETHODIMP InsertTextAtSelection(
            DWORD dwFlags, const WCHAR *pchText, ULONG cch, LONG *pacpStart, LONG *pacpEnd, TS_TEXTCHANGE *pChange) override;
        STDMETHODIMP InsertEmbeddedAtSelection(
            DWORD dwFlags, IDataObject *pDataObject, LONG *pacpStart, LONG *pacpEnd, TS_TEXTCHANGE *pChange) override;

        //ITfContextOwnerCompositionSink methods.
        STDMETHODIMP OnStartComposition(
            __RPC__in_opt ITfCompositionView *pComposition,
            __RPC__out BOOL *pfOk) override;

        STDMETHODIMP OnUpdateComposition(
            __RPC__in_opt ITfCompositionView *pComposition,
            __RPC__in_opt ITfRange *pRangeNew) override;

        STDMETHODIMP OnEndComposition(
            __RPC__in_opt ITfCompositionView *pComposition) override;

        //IUnknown methods.
        STDMETHODIMP QueryInterface(REFIID riid, void **ppvObj) override;
        STDMETHODIMP_(ULONG) AddRef() override;
        STDMETHODIMP_(ULONG) Release() override;
    };

}

#endif  // UKIVE_TEXT_TSF_EDITOR_H_