#include "text_drawing_effect.h"


namespace ukive {

    TextDrawingEffect::TextDrawingEffect()
        :mRefCount(1),
        mEffectSpan(nullptr)
    {
    }


    TextDrawingEffect::~TextDrawingEffect()
    {
    }


    STDMETHODIMP_(unsigned long) TextDrawingEffect::AddRef()
    {
        return InterlockedIncrement(&mRefCount);
    }


    STDMETHODIMP_(unsigned long) TextDrawingEffect::Release()
    {
        unsigned long newCount = InterlockedDecrement(&mRefCount);

        if (newCount == 0)
        {
            delete this;
            return 0;
        }

        return newCount;
    }


    STDMETHODIMP TextDrawingEffect::QueryInterface(
        IID const& riid,
        void** ppvObject
    )
    {
        if (__uuidof(TextDrawingEffect) == riid)
        {
            *ppvObject = this;
        }
        else if (__uuidof(IUnknown) == riid)
        {
            *ppvObject = this;
        }
        else
        {
            *ppvObject = NULL;
            return E_FAIL;
        }

        AddRef();

        return S_OK;
    }

}