#include "text_drawing_effect.h"


namespace ukive {

    TextDrawingEffect::TextDrawingEffect()
        :ref_count_(1),
        effect_span_(nullptr) {
    }


    TextDrawingEffect::~TextDrawingEffect() {
    }


    STDMETHODIMP_(unsigned long) TextDrawingEffect::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }


    STDMETHODIMP_(unsigned long) TextDrawingEffect::Release() {
        auto rc = InterlockedDecrement(&ref_count_);
        if (rc == 0) {
            delete this;
        }

        return rc;
    }


    STDMETHODIMP TextDrawingEffect::QueryInterface(
        IID const& riid, void** ppvObject) {

        if (ppvObject == NULL) {
            return E_POINTER;
        }

        if (__uuidof(TextDrawingEffect) == riid) {
            *ppvObject = this;
        } else if (__uuidof(IUnknown) == riid) {
            *ppvObject = this;
        } else {
            *ppvObject = NULL;
            return E_FAIL;
        }

        AddRef();
        return S_OK;
    }

}