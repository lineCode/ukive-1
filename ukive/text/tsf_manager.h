#ifndef UKIVE_TEXT_TSF_MANAGER_H_
#define UKIVE_TEXT_TSF_MANAGER_H_

#include <msctf.h>

#include "ukive/utils/com_ptr.h"


namespace ukive {

    class TsfSink;
    class TsfEditor;
    class InputConnection;

    class TsfManager {
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

    private:
        TsfSink* sink_;
        TfClientId client_id_;
        ComPtr<ITfThreadMgr> thread_mgr_;

        DWORD mAlpnSinkCookie;
        DWORD mOpenModeSinkCookie;
        DWORD mConvModeSinkCookie;

        HRESULT setupCompartmentSinks(ITfCompartment* openMode, ITfCompartment* convMode);
        HRESULT releaseCompartmentSinks();
        HRESULT getCompartments(
            ITfCompartmentMgr** cm, ITfCompartment** openMode, ITfCompartment** convMode);
    };

    class TsfSink : public ITfInputProcessorProfileActivationSink, public ITfCompartmentEventSink {
    public:
        TsfSink(TsfManager* tsfMgr);
        ~TsfSink();

        // ITfInputProcessorProfileActivationSink
        // Notification for keyboard input locale change
        STDMETHODIMP OnActivated(
            DWORD dwProfileType, LANGID langid, REFCLSID clsid, REFGUID catid, REFGUID guidProfile,
            HKL hkl, DWORD dwFlags) override;

        // ITfCompartmentEventSink
        // Notification for open mode (toggle state) change
        STDMETHODIMP OnChange(REFGUID rguid) override;

        STDMETHODIMP_(ULONG) AddRef() override;
        STDMETHODIMP_(ULONG) Release() override;
        STDMETHODIMP QueryInterface(REFIID riid, void** ppvObj) override;

    private:
        ULONG ref_count_;
        TsfManager* tsf_mgr_;
        ITfCompositionView* composition_view_;
    };

}

#endif  // UKIVE_TEXT_TSF_MANAGER_H_