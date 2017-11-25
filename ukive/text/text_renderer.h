#ifndef UKIVE_TEXT_TEXT_RENDER_H_
#define UKIVE_TEXT_TEXT_RENDER_H_

#include "ukive/graphics/color.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/utils/com_ptr.h"


namespace ukive {

    class TextRenderer : public IDWriteTextRenderer
    {
    private:
        ULONG mRefCount;
        float mOpacity;

        Color mDefaultTextColor;
        Color mDefaultUnderlineColor;
        Color mDefaultStrikethroughColor;

        ComPtr<ID2D1RenderTarget> mRenderTarget;
        ComPtr<ID2D1SolidColorBrush> mSolidBrush;

    public:
        TextRenderer(ComPtr<ID2D1RenderTarget> renderTarget);
        ~TextRenderer();

        void setOpacity(float opacity);
        void setTextColor(Color color);
        void setUnderlineColor(Color color);
        void setStrikethroughColor(Color color);

        STDMETHOD(IsPixelSnappingDisabled)(
            __maybenull void* clientDrawingContext,
            __out BOOL* isDisabled
            );

        STDMETHOD(GetCurrentTransform)(
            __maybenull void* clientDrawingContext,
            __out DWRITE_MATRIX* transform
            );

        STDMETHOD(GetPixelsPerDip)(
            __maybenull void* clientDrawingContext,
            __out FLOAT* pixelsPerDip
            );

        STDMETHOD(DrawGlyphRun)(
            __maybenull void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            DWRITE_MEASURING_MODE measuringMode,
            __in DWRITE_GLYPH_RUN const* glyphRun,
            __in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
            IUnknown* clientDrawingEffect
            );

        STDMETHOD(DrawUnderline)(
            __maybenull void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            __in DWRITE_UNDERLINE const* underline,
            IUnknown* clientDrawingEffect
            );

        STDMETHOD(DrawStrikethrough)(
            __maybenull void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            __in DWRITE_STRIKETHROUGH const* strikethrough,
            IUnknown* clientDrawingEffect
            );

        STDMETHOD(DrawInlineObject)(
            __maybenull void* clientDrawingContext,
            FLOAT originX,
            FLOAT originY,
            IDWriteInlineObject* inlineObject,
            BOOL isSideways,
            BOOL isRightToLeft,
            IUnknown* clientDrawingEffect
            );

    public:
        unsigned long STDMETHODCALLTYPE AddRef();
        unsigned long STDMETHODCALLTYPE Release();
        HRESULT STDMETHODCALLTYPE QueryInterface(
            IID const& riid,
            void** ppvObject
        );
    };

}

#endif  // UKIVE_TEXT_TEXT_RENDER_H_