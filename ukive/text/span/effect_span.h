#ifndef UKIVE_TEXT_SPAN_EFFECT_SPAN_H_
#define UKIVE_TEXT_SPAN_EFFECT_SPAN_H_

#include "ukive/text/span/span.h"

#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    class View;

    // TODO: ¶ÀÁ¢ÓÚ Direct2D
    class EffectSpan : public Span {
    public:
        EffectSpan(unsigned int start, unsigned int end);
        ~EffectSpan();

        D2D1_COLOR_F mTextColor;
        D2D1_COLOR_F mUnderlineColor;
        D2D1_COLOR_F mStrikethroughColor;

        int getBaseType() const override;
        int getType() const override;

        virtual bool onDrawText(
            View *widget,
            float baselineOriginX, float baselineOriginY,
            DWRITE_GLYPH_RUN const* glyphRun, DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
            ID2D1RenderTarget *rt, ID2D1SolidColorBrush *brush);
        virtual bool onDrawUnderline(
            View *widget,
            float baselineOriginX, float baselineOriginY,
            DWRITE_UNDERLINE const* underline,
            ID2D1RenderTarget *rt, ID2D1SolidColorBrush *brush);
        virtual bool onDrawStrikethrough(
            View *widget,
            float baselineOriginX, float baselineOriginY,
            DWRITE_STRIKETHROUGH const* strikethrough,
            ID2D1RenderTarget *rt, ID2D1SolidColorBrush *brush);
        virtual bool onDrawInlineObject(
            View *widget,
            float originX, float originY,
            IDWriteInlineObject* inlineObject, BOOL isSideways, BOOL isRightToLeft,
            ID2D1RenderTarget *rt, ID2D1SolidColorBrush *brush);
    };

}

#endif  // UKIVE_TEXT_SPAN_EFFECT_SPAN_H_