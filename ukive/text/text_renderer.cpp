#include "text_renderer.h"

#include "utils/log.h"

#include "ukive/graphics/color.h"
#include "ukive/text/span/effect_span.h"
#include "ukive/text/span/underline_span.h"
#include "ukive/text/text_drawing_effect.h"
#include "ukive/views/text_view.h"
#include "ukive/application.h"


namespace ukive {

    TextRenderer::TextRenderer(const ComPtr<ID2D1RenderTarget>& rt)
        :ref_count_(1),
        opacity_(1.f),
        def_text_color_(Color::Black),
        def_underline_color_(Color::Black),
        def_strikethrough_color_(Color::Black),
        rt_(rt)  {

        D2D1_COLOR_F color = {
            def_text_color_.r,
            def_text_color_.g,
            def_text_color_.b,
            def_text_color_.a, };
        rt_->CreateSolidColorBrush(color, &brush_);
    }

    TextRenderer::~TextRenderer() {
    }


    void TextRenderer::setOpacity(float opacity) {
        opacity_ = opacity;
        brush_->SetOpacity(opacity);
    }

    void TextRenderer::setTextColor(const Color& color) {
        def_text_color_ = color;
    }

    void TextRenderer::setUnderlineColor(const Color& color) {
        def_underline_color_ = color;
    }

    void TextRenderer::setStrikethroughColor(const Color& color) {
        def_strikethrough_color_ = color;
    }


    /******************************************************************
    *                                                                 *
    *  CustomTextRenderer::DrawGlyphRun                               *
    *                                                                 *
    *  Gets GlyphRun outlines via IDWriteFontFace::GetGlyphRunOutline *
    *  and then draws and fills them using Direct2D path geometries   *
    *                                                                 *
    ******************************************************************/

    STDMETHODIMP TextRenderer::DrawGlyphRun(
        void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_MEASURING_MODE measuringMode,
        DWRITE_GLYPH_RUN const* glyphRun,
        DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
        IUnknown* clientDrawingEffect) {

        if (clientDrawingEffect) {
            ComPtr<TextDrawingEffect> drawingEffect;
            HRESULT hr = clientDrawingEffect->QueryInterface(&drawingEffect);
            DCHECK(SUCCEEDED(hr));

            EffectSpan* span = drawingEffect->effect_span_;
            brush_->SetColor(span->mTextColor);

            if (!span->onDrawText(
                reinterpret_cast<View*>(clientDrawingContext),
                baselineOriginX, baselineOriginY,
                glyphRun, glyphRunDescription,
                rt_.get(), brush_.get()))
            {
                rt_->DrawGlyphRun(
                    D2D1::Point2F(baselineOriginX, baselineOriginY),
                    glyphRun, brush_.get(), measuringMode);
            }
        } else {
            D2D1_COLOR_F color = {
                def_text_color_.r,
                def_text_color_.g,
                def_text_color_.b,
                def_text_color_.a, };
            brush_->SetColor(color);

            rt_->DrawGlyphRun(
                D2D1::Point2F(baselineOriginX, baselineOriginY),
                glyphRun, brush_.get(), measuringMode);
        }

        return S_OK;
    }

    /******************************************************************
    *                                                                 *
    *  CustomTextRenderer::DrawUnderline                              *
    *                                                                 *
    *  Draws underlines below the text using a Direct2D recatangle    *
    *  geometry                                                       *
    *                                                                 *
    ******************************************************************/

    STDMETHODIMP TextRenderer::DrawUnderline(
        void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_UNDERLINE const* underline,
        IUnknown* clientDrawingEffect) {

        D2D1_RECT_F rect = D2D1::RectF(
            0 + baselineOriginX,
            underline->offset + baselineOriginY,
            underline->width + baselineOriginX,
            underline->offset + underline->thickness + baselineOriginY);

        if (clientDrawingEffect) {
            ComPtr<TextDrawingEffect> drawingEffect;
            clientDrawingEffect->QueryInterface(&drawingEffect);

            EffectSpan* span = drawingEffect->effect_span_;
            brush_->SetColor(span->mUnderlineColor);

            if (!span->onDrawUnderline(
                reinterpret_cast<View*>(clientDrawingContext),
                baselineOriginX, baselineOriginY,
                underline, rt_.get(), brush_.get())) {

                rt_->FillRectangle(rect, brush_.get());
            }
        } else {
            D2D1_COLOR_F color = {
                def_underline_color_.r,
                def_underline_color_.g,
                def_underline_color_.b,
                def_underline_color_.a, };
            brush_->SetColor(color);
            rt_->FillRectangle(rect, brush_.get());
        }

        return S_OK;
    }

    /******************************************************************
    *                                                                 *
    *  CustomTextRenderer::DrawStrikethrough                          *
    *                                                                 *
    *  Draws strikethroughs below the text using a Direct2D           *
    *  recatangle geometry                                            *
    *                                                                 *
    ******************************************************************/

    STDMETHODIMP TextRenderer::DrawStrikethrough(
        void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_STRIKETHROUGH const* strikethrough,
        IUnknown* clientDrawingEffect) {

        D2D1_RECT_F rect = D2D1::RectF(
            0 + baselineOriginX,
            strikethrough->offset + baselineOriginY,
            strikethrough->width + baselineOriginX,
            strikethrough->offset + strikethrough->thickness + baselineOriginY);

        if (clientDrawingEffect) {
            ComPtr<TextDrawingEffect> drawingEffect;
            HRESULT hr = clientDrawingEffect->QueryInterface(&drawingEffect);
            DCHECK(SUCCEEDED(hr));

            EffectSpan* span = drawingEffect->effect_span_;
            brush_->SetColor(span->mStrikethroughColor);

            if (!span->onDrawStrikethrough(
                reinterpret_cast<View*>(clientDrawingContext),
                baselineOriginX, baselineOriginY,
                strikethrough,
                rt_.get(), brush_.get())) {
                rt_->FillRectangle(rect, brush_.get());
            }
        } else {
            D2D1_COLOR_F color = {
                def_strikethrough_color_.r,
                def_strikethrough_color_.g,
                def_strikethrough_color_.b,
                def_strikethrough_color_.a, };
            brush_->SetColor(color);
            rt_->FillRectangle(rect, brush_.get());
        }

        return S_OK;
    }

    /******************************************************************
    *                                                                 *
    *  CustomTextRenderer::DrawInlineObject                           *
    *                                                                 *
    *  This function is not implemented for the purposes of this      *
    *  sample.                                                        *
    *                                                                 *
    ******************************************************************/

    STDMETHODIMP TextRenderer::DrawInlineObject(
        void* clientDrawingContext,
        FLOAT originX,
        FLOAT originY,
        IDWriteInlineObject* inlineObject,
        BOOL isSideways,
        BOOL isRightToLeft,
        IUnknown* clientDrawingEffect) {

        // Not implemented
        return E_NOTIMPL;
    }

    /******************************************************************
    *                                                                 *
    *  CustomTextRenderer::IsPixelSnappingDisabled                    *
    *                                                                 *
    *  Determines whether pixel snapping is disabled. The recommended *
    *  default is FALSE, unless doing animation that requires         *
    *  subpixel vertical placement.                                   *
    *                                                                 *
    ******************************************************************/

    STDMETHODIMP TextRenderer::IsPixelSnappingDisabled(
        void* clientDrawingContext,
        BOOL* isDisabled) {

        *isDisabled = FALSE;
        return S_OK;
    }

    /******************************************************************
    *                                                                 *
    *  CustomTextRenderer::GetCurrentTransform                        *
    *                                                                 *
    *  Returns the current transform applied to the render target..   *
    *                                                                 *
    ******************************************************************/

    STDMETHODIMP TextRenderer::GetCurrentTransform(
        void* clientDrawingContext,
        DWRITE_MATRIX* transform) {

        //forward the render target's transform
        rt_->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
        return S_OK;
    }

    /******************************************************************
    *                                                                 *
    *  CustomTextRenderer::GetPixelsPerDip                            *
    *                                                                 *
    *  This returns the number of pixels per DIP.                     *
    *                                                                 *
    ******************************************************************/

    STDMETHODIMP TextRenderer::GetPixelsPerDip(
        void* clientDrawingContext,
        FLOAT* pixelsPerDip) {

        float x, yUnused;

        rt_->GetDpi(&x, &yUnused);
        *pixelsPerDip = x / 96;

        return S_OK;
    }

    STDMETHODIMP_(ULONG) TextRenderer::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }

    STDMETHODIMP_(ULONG) TextRenderer::Release() {
        auto rc = InterlockedDecrement(&ref_count_);
        if (rc == 0) {
            delete this;
        }

        return rc;
    }

    STDMETHODIMP TextRenderer::QueryInterface(
        REFIID riid, void** ppvObject) {

        if (ppvObject == nullptr) {
            return E_POINTER;
        }

        if (__uuidof(IDWriteTextRenderer) == riid) {
            *ppvObject = static_cast<IDWriteTextRenderer*>(this);
        } else if (__uuidof(IDWritePixelSnapping) == riid) {
            *ppvObject = static_cast<IDWritePixelSnapping*>(this);
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