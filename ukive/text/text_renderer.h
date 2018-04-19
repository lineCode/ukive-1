#ifndef UKIVE_TEXT_TEXT_RENDER_H_
#define UKIVE_TEXT_TEXT_RENDER_H_

#include "ukive/graphics/color.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/utils/com_ptr.h"


namespace ukive {

    class TextRenderer : public IDWriteTextRenderer {
    public:
        TextRenderer(ComPtr<ID2D1RenderTarget> rt);
        ~TextRenderer();

        void setOpacity(float opacity);
        void setTextColor(const Color& color);
        void setUnderlineColor(const Color& color);
        void setStrikethroughColor(const Color& color);

        STDMETHOD(IsPixelSnappingDisabled)(
            void* clientDrawingContext,
            BOOL* isDisabled);

        STDMETHOD(GetCurrentTransform)(
            void* clientDrawingContext,
            DWRITE_MATRIX* transform);

        STDMETHOD(GetPixelsPerDip)(
            void* clientDrawingContext,
            FLOAT* pixelsPerDip);

        STDMETHOD(DrawGlyphRun)(
            void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            DWRITE_MEASURING_MODE measuringMode,
            DWRITE_GLYPH_RUN const* glyphRun,
            DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
            IUnknown* clientDrawingEffect);

        STDMETHOD(DrawUnderline)(
            void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            DWRITE_UNDERLINE const* underline,
            IUnknown* clientDrawingEffect);

        STDMETHOD(DrawStrikethrough)(
            void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            DWRITE_STRIKETHROUGH const* strikethrough,
            IUnknown* clientDrawingEffect);

        STDMETHOD(DrawInlineObject)(
            void* clientDrawingContext,
            FLOAT originX,
            FLOAT originY,
            IDWriteInlineObject* inlineObject,
            BOOL isSideways,
            BOOL isRightToLeft,
            IUnknown* clientDrawingEffect);

        ULONG STDMETHODCALLTYPE AddRef();
        ULONG STDMETHODCALLTYPE Release();
        HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID riid, void** ppvObject);

    private:
        ULONG ref_count_;
        float opacity_;

        Color def_text_color_;
        Color def_underline_color_;
        Color def_strikethrough_color_;

        ComPtr<ID2D1RenderTarget> rt_;
        ComPtr<ID2D1SolidColorBrush> brush_;
    };

}

#endif  // UKIVE_TEXT_TEXT_RENDER_H_