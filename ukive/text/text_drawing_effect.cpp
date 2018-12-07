#include "text_drawing_effect.h"


namespace ukive {

    TextDrawingEffect::TextDrawingEffect()
        :effect_span_(nullptr),
        ref_count_(1) {
    }


    TextDrawingEffect::~TextDrawingEffect() {
    }


    STDMETHODIMP_(unsigned long) TextDrawingEffect::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }

    STDMETHODIMP_(unsigned long) TextDrawingEffect::Release() {
        auto nc = InterlockedDecrement(&ref_count_);
        if (nc == 0) {
            delete this;
        }

        return nc;
    }

    STDMETHODIMP TextDrawingEffect::QueryInterface(
        REFIID riid, void** ppvObject) {

        if (!ppvObject) {
            return E_POINTER;
        }

        if (__uuidof(TextDrawingEffect) == riid) {
            *ppvObject = static_cast<TextDrawingEffect*>(this);
        } else if (__uuidof(IUnknown) == riid) {
            *ppvObject = reinterpret_cast<IUnknown*>(this);
        } else {
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

}