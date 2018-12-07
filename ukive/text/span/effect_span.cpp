#include "effect_span.h"


namespace ukive {

    EffectSpan::EffectSpan(unsigned int start, unsigned int end)
        :Span(start, end),
        mTextColor(D2D1::ColorF(D2D1::ColorF::Black)),
        mUnderlineColor(D2D1::ColorF(D2D1::ColorF::Black)),
        mStrikethroughColor(D2D1::ColorF(D2D1::ColorF::Black)) {
    }

    EffectSpan::~EffectSpan() {
    }


    int EffectSpan::getType() const {
        return Span::EFFECT_NORMAL;
    }

    int EffectSpan::getBaseType() const {
        return Span::EFFECT;
    }


    bool EffectSpan::onDrawText(
        View *widget,
        float baselineOriginX, float baselineOriginY,
        DWRITE_GLYPH_RUN const* glyphRun, DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
        ID2D1RenderTarget *rt, ID2D1SolidColorBrush *brush) {
        return false;
    }

    bool EffectSpan::onDrawUnderline(
        View *widget,
        float baselineOriginX, float baselineOriginY,
        DWRITE_UNDERLINE const* underline,
        ID2D1RenderTarget *rt, ID2D1SolidColorBrush *brush) {
        return false;
    }

    bool EffectSpan::onDrawStrikethrough(
        View *widget,
        float baselineOriginX, float baselineOriginY,
        DWRITE_STRIKETHROUGH const* strikethrough,
        ID2D1RenderTarget *rt, ID2D1SolidColorBrush *brush) {
        return false;
    }

    bool EffectSpan::onDrawInlineObject(
        View *widget,
        float originX, float originY,
        IDWriteInlineObject* inlineObject, BOOL isSideways, BOOL isRightToLeft,
        ID2D1RenderTarget *rt, ID2D1SolidColorBrush *brush) {
        return false;
    }

}