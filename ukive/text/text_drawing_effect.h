#ifndef UKIVE_TEXT_TEXT_DRAWING_EFFECT_H_
#define UKIVE_TEXT_TEXT_DRAWING_EFFECT_H_

#include <Unknwn.h>


namespace ukive {

    class EffectSpan;

    class DECLSPEC_UUID("06C2F086-4818-4DF9-880D-FF7C9F796AFA") TextDrawingEffect : public IUnknown
    {
    private:
        ULONG mRefCount;

    public:
        TextDrawingEffect();
        ~TextDrawingEffect();

        EffectSpan *mEffectSpan;

    public:
        unsigned long STDMETHODCALLTYPE AddRef();
        unsigned long STDMETHODCALLTYPE Release();
        HRESULT STDMETHODCALLTYPE QueryInterface(
            IID const& riid,
            void** ppvObject
        );
    };

}

#endif  // UKIVE_TEXT_TEXT_DRAWING_EFFECT_H_