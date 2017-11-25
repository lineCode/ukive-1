#include "text_renderer.h"

#include "ukive/graphics/color.h"
#include "ukive/text/span/effect_span.h"
#include "ukive/text/span/underline_span.h"
#include "ukive/text/text_drawing_effect.h"
#include "ukive/views/text_view.h"
#include "ukive/application.h"


namespace ukive {

    TextRenderer::TextRenderer(ComPtr<ID2D1RenderTarget> renderTarget)
        :mRefCount(1),
        mOpacity(1.f)
    {
        mDefaultTextColor = Color::Black;
        mDefaultUnderlineColor = Color::Black;
        mDefaultStrikethroughColor = Color::Black;

        mRenderTarget = renderTarget;
        D2D1_COLOR_F color = {
            mDefaultTextColor.r,
            mDefaultTextColor.g,
            mDefaultTextColor.b,
            mDefaultTextColor.a, };
        mRenderTarget->CreateSolidColorBrush(color, &mSolidBrush);
    }


    TextRenderer::~TextRenderer()
    {
    }


    void TextRenderer::setOpacity(float opacity)
    {
        mOpacity = opacity;
        mSolidBrush->SetOpacity(opacity);
    }

    void TextRenderer::setTextColor(Color color)
    {
        mDefaultTextColor = color;
    }

    void TextRenderer::setUnderlineColor(Color color)
    {
        mDefaultUnderlineColor = color;
    }

    void TextRenderer::setStrikethroughColor(Color color)
    {
        mDefaultStrikethroughColor = color;
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
        __maybenull void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_MEASURING_MODE measuringMode,
        __in DWRITE_GLYPH_RUN const* glyphRun,
        __in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
        IUnknown* clientDrawingEffect
    )
    {
        if (clientDrawingContext != nullptr)
        {
            RectF region;
            TextView *textView = (TextView*)clientDrawingContext;
            textView->computeVisibleRegion(&region);

            if ((int)std::floor(baselineOriginX) > (int)std::ceil(region.right))
                return S_OK;
            if ((int)std::ceil(baselineOriginX) < (int)std::floor(region.left))
                return S_OK;
            if ((int)std::ceil(baselineOriginY) < (int)std::floor(region.top))
                return S_OK;
            if ((int)std::floor(baselineOriginY - glyphRun->fontEmSize) > (int)std::ceil(region.bottom))
                return S_OK;
        }

        if (clientDrawingEffect != nullptr)
        {
            ComPtr<TextDrawingEffect> drawingEffect;
            clientDrawingEffect->QueryInterface(__uuidof(TextDrawingEffect), reinterpret_cast<void**>(&drawingEffect));

            EffectSpan *span = drawingEffect->mEffectSpan;
            mSolidBrush->SetColor(span->mTextColor);

            if (!span->onDrawText(
                (View*)clientDrawingContext,
                baselineOriginX, baselineOriginY,
                glyphRun, glyphRunDescription,
                mRenderTarget.get(), mSolidBrush.get()))
            {
                mRenderTarget->DrawGlyphRun(
                    D2D1::Point2F(baselineOriginX, baselineOriginY),
                    glyphRun, mSolidBrush.get(), measuringMode);
            }
        }
        else
        {
            D2D1_COLOR_F color = {
                mDefaultTextColor.r,
                mDefaultTextColor.g,
                mDefaultTextColor.b,
                mDefaultTextColor.a, };
            mSolidBrush->SetColor(color);

            mRenderTarget->DrawGlyphRun(
                D2D1::Point2F(baselineOriginX, baselineOriginY),
                glyphRun, mSolidBrush.get(), measuringMode);
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
        __maybenull void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        __in DWRITE_UNDERLINE const* underline,
        IUnknown* clientDrawingEffect
    )
    {
        D2D1_RECT_F rect = D2D1::RectF(
            0 + baselineOriginX,
            underline->offset + baselineOriginY,
            underline->width + baselineOriginX,
            underline->offset + underline->thickness + baselineOriginY);

        if (clientDrawingEffect != 0)
        {
            ComPtr<TextDrawingEffect> drawingEffect;
            clientDrawingEffect->QueryInterface(__uuidof(TextDrawingEffect), reinterpret_cast<void**>(&drawingEffect));

            EffectSpan *span = drawingEffect->mEffectSpan;
            mSolidBrush->SetColor(span->mUnderlineColor);

            if (!span->onDrawUnderline(
                (View*)clientDrawingContext,
                baselineOriginX, baselineOriginY,
                underline,
                mRenderTarget.get(), mSolidBrush.get()))
                mRenderTarget->FillRectangle(rect, mSolidBrush.get());
        }
        else
        {
            D2D1_COLOR_F color = {
                mDefaultUnderlineColor.r,
                mDefaultUnderlineColor.g,
                mDefaultUnderlineColor.b,
                mDefaultUnderlineColor.a, };
            mSolidBrush->SetColor(color);
            mRenderTarget->FillRectangle(rect, mSolidBrush.get());
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
        __maybenull void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        __in DWRITE_STRIKETHROUGH const* strikethrough,
        IUnknown* clientDrawingEffect
    )
    {
        D2D1_RECT_F rect = D2D1::RectF(
            0 + baselineOriginX,
            strikethrough->offset + baselineOriginY,
            strikethrough->width + baselineOriginX,
            strikethrough->offset + strikethrough->thickness + baselineOriginY);

        if (clientDrawingEffect != 0)
        {
            ComPtr<TextDrawingEffect> drawingEffect;
            clientDrawingEffect->QueryInterface(__uuidof(TextDrawingEffect), reinterpret_cast<void**>(&drawingEffect));

            EffectSpan *span = drawingEffect->mEffectSpan;
            mSolidBrush->SetColor(span->mStrikethroughColor);

            if (!span->onDrawStrikethrough(
                (View*)clientDrawingContext,
                baselineOriginX, baselineOriginY,
                strikethrough,
                mRenderTarget.get(), mSolidBrush.get()))
                mRenderTarget->FillRectangle(rect, mSolidBrush.get());
        }
        else
        {
            D2D1_COLOR_F color = {
                mDefaultStrikethroughColor.r,
                mDefaultStrikethroughColor.g,
                mDefaultStrikethroughColor.b,
                mDefaultStrikethroughColor.a, };
            mSolidBrush->SetColor(color);
            mRenderTarget->FillRectangle(rect, mSolidBrush.get());
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
        __maybenull void* clientDrawingContext,
        FLOAT originX,
        FLOAT originY,
        IDWriteInlineObject* inlineObject,
        BOOL isSideways,
        BOOL isRightToLeft,
        IUnknown* clientDrawingEffect
    )
    {
        // Not implemented
        return E_NOTIMPL;
    }


    STDMETHODIMP_(unsigned long) TextRenderer::AddRef()
    {
        return InterlockedIncrement(&mRefCount);
    }


    STDMETHODIMP_(unsigned long) TextRenderer::Release()
    {
        unsigned long newCount = InterlockedDecrement(&mRefCount);

        if (newCount == 0)
        {
            delete this;
            return 0;
        }

        return newCount;
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
        __maybenull void* clientDrawingContext,
        __out BOOL* isDisabled
    )
    {
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
        __maybenull void* clientDrawingContext,
        __out DWRITE_MATRIX* transform
    )
    {
        //forward the render target's transform
        mRenderTarget->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
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
        __maybenull void* clientDrawingContext,
        __out FLOAT* pixelsPerDip
    )
    {
        float x, yUnused;

        mRenderTarget->GetDpi(&x, &yUnused);
        *pixelsPerDip = x / 96;

        return S_OK;
    }

    /******************************************************************
    *                                                                 *
    *  CustomTextRenderer::QueryInterface                             *
    *                                                                 *
    *  Query interface implementation                                 *
    *                                                                 *
    ******************************************************************/

    STDMETHODIMP TextRenderer::QueryInterface(
        IID const& riid,
        void** ppvObject
    )
    {
        if (__uuidof(IDWriteTextRenderer) == riid)
        {
            *ppvObject = this;
        }
        else if (__uuidof(IDWritePixelSnapping) == riid)
        {
            *ppvObject = this;
        }
        else if (__uuidof(IUnknown) == riid)
        {
            *ppvObject = this;
        }
        else
        {
            *ppvObject = 0;
            return E_FAIL;
        }

        AddRef();

        return S_OK;
    }

}