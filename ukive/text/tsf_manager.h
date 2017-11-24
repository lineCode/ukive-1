#ifndef UKIVE_TEXT_TSF_MANAGER_H_
#define UKIVE_TEXT_TSF_MANAGER_H_

#include <msctf.h>

#include "ukive/utils/com_ptr.h"


namespace ukive {

    class TsfSink;
    class TsfEditor;
    class InputConnection;

    class TsfManager
    {
    private:
        TsfSink *mSink;
        TfClientId mClientId;
        ComPtr<ITfThreadMgr> mThreadMgr;

        DWORD mAlpnSinkCookie;
        DWORD mOpenModeSinkCookie;
        DWORD mConvModeSinkCookie;

        HRESULT setupCompartmentSinks(ITfCompartment *openMode, ITfCompartment *convMode);
        HRESULT releaseCompartmentSinks();

        HRESULT getCompartments(ITfCompartmentMgr **cm, ITfCompartment **openMode, ITfCompartment **convMode);

    public:
        TsfManager();
        ~TsfManager();

        HRESULT init();
        void close();

        TfClientId getClientId();
        ComPtr<ITfThreadMgr> getThreadManager();

        HRESULT setupSinks();
        HRESULT releaseSinks();

        HRESULT updateImeState();
    };

    class TsfSink : public ITfInputProcessorProfileActivationSink, public ITfCompartmentEventSink
    {
    private:
        LONG mRefCount;
        TsfManager *mTsfMgr;
        ITfCompositionView *pComposition;

    public:
        TsfSink(TsfManager *tsfMgr);
        ~TsfSink();

        STDMETHODIMP QueryInterface(REFIID riid, void **ppvObj) override;
        STDMETHODIMP_(ULONG) AddRef(void) override;
        STDMETHODIMP_(ULONG) Release(void) override;

        // ITfInputProcessorProfileActivationSink
        // Notification for keyboard input locale change
        STDMETHODIMP OnActivated(
            DWORD dwProfileType, LANGID langid, REFCLSID clsid, REFGUID catid, REFGUID guidProfile, HKL hkl, DWORD dwFlags) override;

        // ITfCompartmentEventSink
        // Notification for open mode (toggle state) change
        STDMETHODIMP OnChange(REFGUID rguid) override;
    };

}

#endif  // UKIVE_TEXT_TSF_MANAGER_H_