#include "tsf_manager.h"

#include "ukive/utils/hresult_utils.h"


namespace ukive {

    TsfManager::TsfManager()
        : sink_(nullptr),
          client_id_(0),
          mAlpnSinkCookie(TF_INVALID_COOKIE),
          mOpenModeSinkCookie(TF_INVALID_COOKIE),
          mConvModeSinkCookie(TF_INVALID_COOKIE) {
    }

    TsfManager::~TsfManager() {}


    HRESULT TsfManager::init() {
        RH(::CoCreateInstance(
            CLSID_TF_ThreadMgr,
            nullptr, CLSCTX_INPROC_SERVER,
            __uuidof(ITfThreadMgr),
            reinterpret_cast<void**>(&thread_mgr_)));

        RH(thread_mgr_->Activate(&client_id_));

        return S_OK;
    }

    void TsfManager::close() {
        thread_mgr_->Deactivate();
    }

    TfClientId TsfManager::getClientId() const {
        return client_id_;
    }

    ComPtr<ITfThreadMgr> TsfManager::getThreadManager() const {
        return thread_mgr_;
    }


    HRESULT TsfManager::setupCompartmentSinks(ITfCompartment *openMode, ITfCompartment *convMode) {
        HRESULT hr = E_FAIL;
        ComPtr<ITfSource> srcOpenMode;
        ComPtr<ITfSource> srcConvMode;

        if (!openMode || !convMode) {
            return E_INVALIDARG;
        }

        if (SUCCEEDED(hr = openMode->QueryInterface(
            IID_ITfSource, reinterpret_cast<void**>(&srcOpenMode))))
        {
            if (mOpenModeSinkCookie != TF_INVALID_COOKIE) {
                srcOpenMode->UnadviseSink(mOpenModeSinkCookie);
                mOpenModeSinkCookie = TF_INVALID_COOKIE;
            }

            if (SUCCEEDED(hr = srcOpenMode->AdviseSink(
                IID_ITfCompartmentEventSink, static_cast<ITfCompartmentEventSink*>(sink_), &mOpenModeSinkCookie)))
            {
                if (SUCCEEDED(hr = convMode->QueryInterface(
                    IID_ITfSource, reinterpret_cast<void**>(&srcConvMode))))
                {
                    if (mConvModeSinkCookie != TF_INVALID_COOKIE) {
                        srcConvMode->UnadviseSink(mConvModeSinkCookie);
                        mConvModeSinkCookie = TF_INVALID_COOKIE;
                    }

                    hr = srcConvMode->AdviseSink(
                        IID_ITfCompartmentEventSink,
                        static_cast<ITfCompartmentEventSink*>(sink_),
                        &mConvModeSinkCookie);
                }
                srcConvMode->Release();
            }
            srcOpenMode->Release();
        }

        return hr;
    }

    HRESULT TsfManager::releaseCompartmentSinks() {
        HRESULT hr = E_FAIL;

        ComPtr<ITfCompartmentMgr> cm;
        ComPtr<ITfCompartment> openMode;
        ComPtr<ITfCompartment> convMode;

        hr = getCompartments(&cm, &openMode, &convMode);

        ITfSource *srcOpenMode = nullptr;
        if (SUCCEEDED(hr)) {
            hr = openMode->QueryInterface(IID_ITfSource, reinterpret_cast<void**>(&srcOpenMode));

            if (mOpenModeSinkCookie != TF_INVALID_COOKIE) {
                srcOpenMode->UnadviseSink(mOpenModeSinkCookie);
                mOpenModeSinkCookie = TF_INVALID_COOKIE;
            }
        }

        ITfSource *srcConvMode = nullptr;
        if (SUCCEEDED(hr)) {
            hr = convMode->QueryInterface(IID_ITfSource, reinterpret_cast<void**>(&srcConvMode));

            if (mConvModeSinkCookie != TF_INVALID_COOKIE) {
                srcConvMode->UnadviseSink(mConvModeSinkCookie);
                mConvModeSinkCookie = TF_INVALID_COOKIE;
            }
        }

        return hr;
    }

    HRESULT TsfManager::getCompartments(
        ITfCompartmentMgr **cm, ITfCompartment **openMode, ITfCompartment **convMode)
    {
        HRESULT hr = E_FAIL;
        ITfCompartmentMgr *_cm = nullptr;
        ITfCompartment *_openMode = nullptr;
        ITfCompartment *_convMode = nullptr;

        if (thread_mgr_ == nullptr)
            return E_FAIL;

        hr = thread_mgr_->QueryInterface(IID_ITfCompartmentMgr, reinterpret_cast<void**>(&_cm));
        if (FAILED(hr))
            return hr;

        hr = _cm->GetCompartment(GUID_COMPARTMENT_KEYBOARD_OPENCLOSE, &_openMode);
        if (FAILED(hr))
            return hr;

        hr = _cm->GetCompartment(GUID_COMPARTMENT_KEYBOARD_INPUTMODE_CONVERSION, &_convMode);
        if (FAILED(hr))
            return hr;

        *cm = _cm;
        *openMode = _openMode;
        *convMode = _convMode;

        return S_OK;
    }

    HRESULT TsfManager::updateImeState() {
        ITfCompartmentMgr *cm = nullptr;
        ITfCompartment *openMode = nullptr;
        ITfCompartment *convMode = nullptr;

        if (SUCCEEDED(getCompartments(&cm, &openMode, &convMode))) {
            VARIANT valOpenMode;
            VARIANT valConvMode;
            VariantInit(&valOpenMode);
            VariantInit(&valConvMode);

            HRESULT hr = openMode->GetValue(&valOpenMode);
            hr = convMode->GetValue(&valConvMode);

            valOpenMode.vt = VT_I4;
            valConvMode.vt = VT_I4;

            valOpenMode.lVal = TRUE;
            valConvMode.lVal = 0;

            hr = openMode->SetValue(client_id_, &valOpenMode);
            hr = convMode->SetValue(client_id_, &valConvMode);

            VariantClear(&valOpenMode);
            VariantClear(&valConvMode);

            setupCompartmentSinks(openMode, convMode);

            openMode->Release();
            convMode->Release();
            cm->Release();
        }

        return S_OK;
    }

    HRESULT TsfManager::setupSinks() {
        HRESULT hr = E_FAIL;

        ComPtr<ITfCompartmentMgr> cm;
        ComPtr<ITfCompartment> openMode;
        ComPtr<ITfCompartment> convMode;

        sink_ = new TsfSink(this);

        auto src = thread_mgr_.cast<ITfSource>();
        if (src) {
            hr = src->AdviseSink(
                __uuidof(ITfInputProcessorProfileActivationSink),
                static_cast<ITfInputProcessorProfileActivationSink*>(sink_), &mAlpnSinkCookie);
        }

        if (SUCCEEDED(hr)) {
            hr = getCompartments(&cm, &openMode, &convMode);

            VARIANT valOpenMode;
            VARIANT valConvMode;
            VariantInit(&valOpenMode);
            VariantInit(&valConvMode);

            hr = openMode->GetValue(&valOpenMode);
            hr = convMode->GetValue(&valConvMode);

            valOpenMode.vt = VT_I4;
            valConvMode.vt = VT_I4;

            valOpenMode.lVal = TRUE;
            valConvMode.lVal = 0;

            hr = openMode->SetValue(client_id_, &valOpenMode);
            hr = convMode->SetValue(client_id_, &valConvMode);

            VariantClear(&valOpenMode);
            VariantClear(&valConvMode);

            hr = setupCompartmentSinks(openMode.get(), convMode.get());
        }

        return hr;
    }

    HRESULT TsfManager::releaseSinks() {
        HRESULT hr = E_FAIL;
        auto source = thread_mgr_.cast<ITfSource>();

        if (thread_mgr_.get() && source) {
            hr = source->UnadviseSink(mAlpnSinkCookie);
            hr = releaseCompartmentSinks();

            mAlpnSinkCookie = TF_INVALID_COOKIE;
        }

        return hr;
    }


    TsfSink::TsfSink(TsfManager *tsfMgr)
        : ref_count_(1),
          tsf_mgr_(tsfMgr),
          composition_view_(nullptr) {
    }

    TsfSink::~TsfSink() {}


    //切换输入法时响应
    STDMETHODIMP TsfSink::OnActivated(
        DWORD dwProfileType, LANGID langid, REFCLSID clsid, REFGUID catid, REFGUID guidProfile,
        HKL hkl, DWORD dwFlags)
    {
        switch (dwProfileType) {
        case TF_PROFILETYPE_INPUTPROCESSOR:
            break;
        case TF_PROFILETYPE_KEYBOARDLAYOUT:
            break;
        default:
            break;
        }

        switch (dwFlags) {
        case 2:
            break;
        case 3:
            //tsf_mgr_->updateImeState();
            break;
        default:
            break;
        }

        return S_OK;
    }

    //同一输入法中-英切换，日-英切换等等
    STDMETHODIMP TsfSink::OnChange(REFGUID rguid) {
        return S_OK;
    }

    STDMETHODIMP_(ULONG) TsfSink::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }

    STDMETHODIMP_(ULONG) TsfSink::Release() {
        auto nc = InterlockedDecrement(&ref_count_);
        if (nc == 0) {
            delete this;
        }

        return nc;
    }

    STDMETHODIMP TsfSink::QueryInterface(REFIID riid, void **ppvObj) {
        if (!ppvObj) {
            return E_POINTER;
        }

        if (IsEqualIID(riid, IID_IUnknown)) {
            *ppvObj = reinterpret_cast<IUnknown*>(this);
        } else if (IsEqualIID(riid, __uuidof(ITfInputProcessorProfileActivationSink))) {
            *ppvObj = static_cast<ITfInputProcessorProfileActivationSink*>(this);
        } else if (IsEqualIID(riid, __uuidof(ITfCompartmentEventSink))) {
            *ppvObj = static_cast<ITfCompartmentEventSink*>(this);
        } else {
            *ppvObj = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

}