#include "wallpaper_window.h"

#include "ukive/application.h"
#include "ukive/graphics/color.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/renderer.h"


namespace {
    D2D1_COLOR_F convColor(ukive::Color &color) {
        D2D1_COLOR_F _color = {
            color.r,
            color.g,
            color.b,
            color.a, };
        return _color;
    }
}

namespace shell {

    void WallpaperWindow::onCreate()
    {
        Window::onCreate();

        mInitText = L"System Initializing";
        mStandbyText = L"System Standby";

        mBottomText = mInitText;

        //UColor::parse(L"#02132c")
        setBackgroundColor(ukive::Color::parse(L"#33383b"));

        mAnimator = new ukive::Animator(getAnimationManager());

        int rotationNumber = 10;
        int rotationAngle = rotationNumber * 360;
        int rotationDuration = rotationNumber * 3;

        UI_ANIMATION_KEYFRAME delayKey;
        mAnimator->addKey(UI_ANIMATION_KEYFRAME_STORYBOARD_START, 0.5, &delayKey);
        mAnimator->addVariable(ROTATE_CIRCLE, 0, 0, rotationAngle);
        mAnimator->addTransition(ROTATE_CIRCLE, ukive::Transition::linearTransition(rotationDuration, rotationAngle), delayKey);

        double interval = 0.3;
        mAnimator->addVariable(VANISH_DOT, 0, 0, 1);
        mAnimator->addTransition(
            VANISH_DOT,
            ukive::Transition::discreteTransition(0, 1, interval),
            delayKey);
        for (int i = 0; i < 6; ++i)
        {
            mAnimator->addTransition(
                VANISH_DOT,
                ukive::Transition::discreteTransition(0, i % 2, interval));
        }
        mAnimator->setOnValueChangedListener(VANISH_DOT, this);

        UI_ANIMATION_KEYFRAME vanishDotEndKey;
        mAnimator->addKey(delayKey, interval * 7, &vanishDotEndKey);
        mAnimator->addVariable(ENABLE_ALPHA_TEXT, 0, 0, 1);
        mAnimator->addTransition(ENABLE_ALPHA_TEXT, ukive::Transition::instantaneousTransition(1), vanishDotEndKey);
        mAnimator->setOnValueChangedListener(ENABLE_ALPHA_TEXT, this);

        auto alphaTextTransition = ukive::Transition::sinusoidalTransitionFromRange(
            1.25, 0, 1, 0.5, ukive::Transition::UTSlope::INCREASING);

        mAnimator->addVariable(ALPHA_TEXT, 1, 0, 1);
        mAnimator->addTransition(ALPHA_TEXT, alphaTextTransition, vanishDotEndKey);

        UI_ANIMATION_KEYFRAME alphaTextEndKey;
        mAnimator->addKey(alphaTextTransition, &alphaTextEndKey);
        mAnimator->addVariable(EXTEND_DASHED, 0, 0, 2000);
        mAnimator->addTransition(EXTEND_DASHED, ukive::Transition::smoothStopTransition(1, 2000), alphaTextEndKey);

        mAnimator->addVariable(RADIUS_INNER, 0, 0, 100);
        mAnimator->addTransition(RADIUS_INNER, ukive::Transition::linearTransition(1, 40), vanishDotEndKey);



        createRadialGradientBrush();

        ukive::Application::getGraphicDeviceManager()->getDWriteFactory()->CreateTextFormat(
            L"Microsoft Yahei", nullptr,
            DWRITE_FONT_WEIGHT_BOLD,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            15, L"en-us", &mBottomTextFormat);
        mBottomTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        mInitTextWidth = getTextWidth(mInitText, mBottomTextFormat);
        mThreeDotWidth = getTextWidth(L" . . .", mBottomTextFormat);
        mStandbyTextWidth = getTextWidth(mStandbyText, mBottomTextFormat);

        mAnimator->start();
    }

    void WallpaperWindow::onDraw(ukive::Canvas *canvas)
    {
        Window::onDraw(canvas);

        float cx = getClientWidth() / 2.f;
        float cy = getClientHeight() / 2.f;

        mRadialBrush->SetCenter(D2D1::Point2F(cx, cy));

        auto dc = getRenderer()->getD2DDeviceContext();
        dc->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), 500, 500), mRadialBrush);

        canvas->save();
        canvas->rotate(mAnimator->getValue(ROTATE_CIRCLE), cx, cy);
        drawInnerArc(canvas, cx, cy, 60 - mAnimator->getValue(RADIUS_INNER));
        canvas->restore();

        float innerRadius = 75 - mAnimator->getValue(RADIUS_INNER);
        float outterRadius = 120;
        float strokeWidth = 3;
        float extAngle = 120;
        float extRadiusAdd = 3;

        canvas->save();
        canvas->rotate(-mAnimator->getValue(ROTATE_CIRCLE), cx, cy);
        drawDecoredCircle(
            canvas, cx, cy, innerRadius, extAngle, extRadiusAdd, strokeWidth,
            ukive::Color::parse(L"#FF3D00"));
        canvas->restore();

        canvas->save();
        canvas->rotate(mAnimator->getValue(ROTATE_CIRCLE), cx, cy);
        drawDecoredCircle(
            canvas, cx, cy, outterRadius, extAngle, extRadiusAdd, strokeWidth,
            ukive::Color::parse(L"#ECEFF1"));
        canvas->restore();

        drawText(canvas, mBottomText, mAnimator->getValue(ALPHA_TEXT));

        drawDashedLines(canvas, cx, cy, mAnimator->getValue(EXTEND_DASHED));
    }

    void WallpaperWindow::onDestroy()
    {
        Window::onDestroy();

        mAnimator->stop();
        delete mAnimator;
    }

    bool WallpaperWindow::onInputEvent(ukive::InputEvent *e)
    {
        return Window::onInputEvent(e);
    }


    void WallpaperWindow::createRadialGradientBrush()
    {
        auto dc = getRenderer()->getD2DDeviceContext();

        dc->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &mCircleBrush);

        // Create an array of gradient stops to put in the gradient stop
        // collection that will be used in the gradient brush.
        ID2D1GradientStopCollection *pGradientStops = NULL;

        D2D1_GRADIENT_STOP gradientStops[3];
        gradientStops[0].color = convColor(ukive::Color::parse(L"#aa638c99"));
        gradientStops[0].position = 0.0f;
        gradientStops[1].color = convColor(ukive::Color::parse(L"#44404b4f"));
        gradientStops[1].position = 0.7f;
        gradientStops[2].color = convColor(ukive::Color::parse(L"#0033383b"));
        gradientStops[2].position = 1.0f;

        // Create the ID2D1GradientStopCollection from a previously
        // declared array of D2D1_GRADIENT_STOP structs.
        HRESULT hr = dc->CreateGradientStopCollection(
            gradientStops,
            3,
            D2D1_GAMMA_2_2,
            D2D1_EXTEND_MODE_CLAMP,
            &pGradientStops
        );

        float cx = getClientWidth() / 2.f;
        float cy = getClientHeight() / 2.f;

        dc->CreateRadialGradientBrush(
            D2D1::RadialGradientBrushProperties(
                D2D1::Point2F(cx, cy),
                D2D1::Point2F(0, 0),
                500, 500),
            pGradientStops,
            &mRadialBrush);
    }

    float WallpaperWindow::getTextWidth(std::wstring text, IDWriteTextFormat* format)
    {
        float width = 0;
        IDWriteTextLayout *layout;
        HRESULT hr = ukive::Application::getGraphicDeviceManager()->getDWriteFactory()->CreateTextLayout(
            text.c_str(), text.length(), format, 0, 0, &layout);
        layout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        if (SUCCEEDED(hr))
        {
            DWRITE_TEXT_METRICS textMetrics;
            hr = layout->GetMetrics(&textMetrics);
            if (SUCCEEDED(hr))
                width = std::ceil(textMetrics.widthIncludingTrailingWhitespace);
        }

        layout->Release();
        return width;
    }


    void WallpaperWindow::drawInnerArc(ukive::Canvas *c, float cx, float cy, float radius)
    {
        float angle = 240;
        float strokeWidth = 6;

        D2D1_POINT_2F p0 = D2D1::Point2F(cx, cy - radius);
        D2D1_POINT_2F p1 = D2D1::Matrix3x2F::Rotation(angle, D2D1::Point2F(cx, cy)).TransformPoint(p0);

        HRESULT hr;
        ID2D1PathGeometry *circleGeo = nullptr;
        hr = ukive::Application::getGraphicDeviceManager()->getD2DFactory()->CreatePathGeometry(&circleGeo);
        if (SUCCEEDED(hr))
        {
            ID2D1GeometrySink *circleGeoSink = nullptr;
            hr = circleGeo->Open(&circleGeoSink);
            if (SUCCEEDED(hr))
            {
                circleGeoSink->BeginFigure(p0, D2D1_FIGURE_BEGIN_HOLLOW);

                circleGeoSink->AddArc(D2D1::ArcSegment(
                    p1,
                    D2D1::SizeF(radius, radius),
                    0.f,
                    D2D1_SWEEP_DIRECTION_CLOCKWISE,
                    D2D1_ARC_SIZE_LARGE
                ));

                circleGeoSink->EndFigure(D2D1_FIGURE_END_OPEN);
                circleGeoSink->Close();
            }

            ID2D1SolidColorBrush *brush = mCircleBrush;

            brush->SetColor(convColor(ukive::Color::parse(L"#ECEFF1")));
            c->getRT()->DrawGeometry(circleGeo, brush, strokeWidth);

            circleGeo->Release();
        }
    }

    void WallpaperWindow::drawDecoredCircle(
        ukive::Canvas *c, float cx, float cy, float radius,
        float extAngle, float extRadiusAdd, float strokeWidth, ukive::Color &color)
    {
        c->drawCircle(cx, cy, radius, strokeWidth, color);

        float extRadius = radius + extRadiusAdd;
        float extStrokeWidth = 2 * extRadiusAdd;

        D2D1_POINT_2F p0 = D2D1::Point2F(cx, cy - extRadius);
        D2D1_POINT_2F p1 = D2D1::Matrix3x2F::Rotation(extAngle, D2D1::Point2F(cx, cy)).TransformPoint(p0);
        D2D1_POINT_2F p2 = D2D1::Matrix3x2F::Rotation(180, D2D1::Point2F(cx, cy)).TransformPoint(p0);
        D2D1_POINT_2F p3 = D2D1::Matrix3x2F::Rotation(180, D2D1::Point2F(cx, cy)).TransformPoint(p1);

        HRESULT hr;
        ID2D1PathGeometry *circleGeo = nullptr;
        hr = ukive::Application::getGraphicDeviceManager()->getD2DFactory()->CreatePathGeometry(&circleGeo);
        if (SUCCEEDED(hr))
        {
            ID2D1GeometrySink *circleGeoSink = nullptr;
            hr = circleGeo->Open(&circleGeoSink);
            if (SUCCEEDED(hr))
            {
                circleGeoSink->BeginFigure(p0, D2D1_FIGURE_BEGIN_HOLLOW);

                circleGeoSink->AddArc(D2D1::ArcSegment(
                    p1,
                    D2D1::SizeF(extRadius, extRadius),
                    0.f,
                    D2D1_SWEEP_DIRECTION_CLOCKWISE,
                    D2D1_ARC_SIZE_SMALL
                ));

                circleGeoSink->EndFigure(D2D1_FIGURE_END_OPEN);

                circleGeoSink->BeginFigure(p2, D2D1_FIGURE_BEGIN_HOLLOW);

                circleGeoSink->AddArc(D2D1::ArcSegment(
                    p3,
                    D2D1::SizeF(extRadius, extRadius),
                    0.f,
                    D2D1_SWEEP_DIRECTION_CLOCKWISE,
                    D2D1_ARC_SIZE_SMALL
                ));

                circleGeoSink->EndFigure(D2D1_FIGURE_END_OPEN);

                circleGeoSink->Close();
            }

            ID2D1SolidColorBrush *brush = mCircleBrush;

            brush->SetColor(convColor(color));
            c->getRT()->DrawGeometry(circleGeo, brush, extStrokeWidth);

            circleGeo->Release();
        }
    }

    void WallpaperWindow::drawDashedLines(ukive::Canvas *c, float cx, float cy, float radius)
    {
        ID2D1SolidColorBrush *brush = mCircleBrush;
        brush->SetColor(convColor(ukive::Color::parse(L"#FF6F00")));

        ID2D1StrokeStyle1 *style;
        ukive::Application::getGraphicDeviceManager()->getD2DFactory()->CreateStrokeStyle(
            D2D1::StrokeStyleProperties1(
                D2D1_CAP_STYLE_FLAT,
                D2D1_CAP_STYLE_FLAT,
                D2D1_CAP_STYLE_FLAT,
                D2D1_LINE_JOIN_MITER, 0,
                D2D1_DASH_STYLE_DASH, 0
            ), nullptr, 0, &style);

        D2D1_POINT_2F p0 = D2D1::Point2F(cx - radius, cy);
        float angles[] = { 30, 80, 120, 170, 200, 250, 290, 330 };
        int angleNumber = sizeof(angles) / sizeof(angles[0]);

        auto dc = getRenderer()->getD2DDeviceContext();

        for (int i = 0; i < angleNumber; ++i)
        {
            dc->DrawLine(
                D2D1::Point2F(cx, cy),
                D2D1::Matrix3x2F::Rotation(angles[i], D2D1::Point2F(cx, cy)).TransformPoint(p0),
                brush, 2, style);
        }

        style->Release();
    }


    void WallpaperWindow::drawCenterCircle(ukive::Canvas *canvas, float radiusMinus)
    {
        float radius = 100;
        float cx = getClientWidth() / 2.f;
        float cy = getClientHeight() / 2.f;

        canvas->fillCircle(cx, cy, radius - radiusMinus, ukive::Color::parse(L"#88283593"));

        mRadialBrush->SetCenter(D2D1::Point2F(cx, cy));
        auto dc = getRenderer()->getD2DDeviceContext();
        dc->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), 500, 500), mRadialBrush);

        canvas->fillCircle(cx, cy, radius / 2, ukive::Color::parse(L"#ffffff"));
    }

    void WallpaperWindow::drawDeformedCircle(
        ukive::Canvas *canvas, float radiusMinus, bool showOutline, float outlineWidth)
    {
        float radius = 100 - radiusMinus;
        float cx = getClientWidth() / 2.f;
        float cy = getClientHeight() / 2.f;
        float gap = 7;
        float halfOfSine = std::sqrt(radius*radius - gap*gap / 4);

        D2D1_POINT_2F p0 = D2D1::Point2F(cx - halfOfSine, cy - gap / 2);
        D2D1_POINT_2F p1 = D2D1::Point2F(cx + halfOfSine, cy - gap / 2);
        D2D1_POINT_2F p2 = D2D1::Point2F(cx - halfOfSine, cy + gap / 2);
        D2D1_POINT_2F p3 = D2D1::Point2F(cx + halfOfSine, cy + gap / 2);

        HRESULT hr;
        ID2D1PathGeometry *circleGeo = nullptr;
        hr = ukive::Application::getGraphicDeviceManager()->getD2DFactory()->CreatePathGeometry(&circleGeo);
        if (SUCCEEDED(hr))
        {
            ID2D1GeometrySink *circleGeoSink = nullptr;
            hr = circleGeo->Open(&circleGeoSink);
            if (SUCCEEDED(hr))
            {
                //ÉÏ°ëÔ²
                circleGeoSink->BeginFigure(p0, D2D1_FIGURE_BEGIN_FILLED);

                circleGeoSink->AddArc(D2D1::ArcSegment(
                    p1,
                    D2D1::SizeF(radius, radius),
                    0.f,
                    D2D1_SWEEP_DIRECTION_CLOCKWISE,
                    D2D1_ARC_SIZE_SMALL
                ));

                circleGeoSink->EndFigure(D2D1_FIGURE_END_OPEN);

                //ÏÂ°ëÔ²
                circleGeoSink->BeginFigure(p3, D2D1_FIGURE_BEGIN_FILLED);

                circleGeoSink->AddArc(D2D1::ArcSegment(
                    p2,
                    D2D1::SizeF(radius, radius),
                    0.f,
                    D2D1_SWEEP_DIRECTION_CLOCKWISE,
                    D2D1_ARC_SIZE_SMALL
                ));

                circleGeoSink->EndFigure(D2D1_FIGURE_END_OPEN);

                circleGeoSink->Close();
            }

            ID2D1SolidColorBrush *brush = mCircleBrush;

            brush->SetColor(convColor(ukive::Color::parse(L"#339FA8DA")));
            canvas->getRT()->FillGeometry(circleGeo, brush);

            if (showOutline)
            {
                brush->SetColor(convColor(ukive::Color::parse(L"#5C6BC0")));
                canvas->getRT()->DrawGeometry(circleGeo, brush, 8 * outlineWidth);
            }

            circleGeo->Release();
        }
    }

    void WallpaperWindow::drawCursor(ukive::Canvas *canvas, bool split, float distance)
    {
        float radius = 150;
        float corner = 2.f;
        float cx = getClientWidth() / 2.f;
        float cy = getClientHeight() / 2.f;
        float width = 5;
        float height = width * 2;

        float topDistance = distance*0.6;

        if (split)
        {
            canvas->fillRoundRect(
                D2D1::RectF(
                    cx - width / 2 - topDistance,
                    cy - radius - height / 2,
                    cx + width / 2 - topDistance,
                    cy - radius + height / 2),
                corner,
                ukive::Color::White);
        }

        canvas->fillRoundRect(
            D2D1::RectF(
                cx - width / 2 + topDistance,
                cy - radius - height / 2,
                cx + width / 2 + topDistance,
                cy - radius + height / 2),
            corner,
            ukive::Color::White);

        if (split)
        {
            canvas->fillRoundRect(
                D2D1::RectF(
                    cx - width / 2 - distance,
                    cy + radius - height / 2,
                    cx + width / 2 - distance,
                    cy + radius + height / 2),
                corner,
                ukive::Color::White);
        }

        canvas->fillRoundRect(
            D2D1::RectF(
                cx - width / 2 + distance,
                cy + radius - height / 2,
                cx + width / 2 + distance,
                cy + radius + height / 2),
            corner,
            ukive::Color::White);
    }

    void WallpaperWindow::drawText(ukive::Canvas *canvas, std::wstring text, float alpha)
    {
        float width, appendix;

        float radius = 170;
        float height = 40;
        float cx = getClientWidth() / 2.f;
        float cy = getClientHeight() / 2.f;

        if (text.find(mInitText) != std::wstring::npos)
        {
            width = mInitTextWidth;
            appendix = mThreeDotWidth;
        }
        else
        {
            width = mStandbyTextWidth;
            appendix = 0;
        }

        canvas->drawText(
            mBottomText.c_str(), mBottomTextFormat,
            D2D1::RectF(
                cx - width / 2,
                cy + radius - height / 2,
                cx + width / 2 + appendix,
                cy + radius + height / 2),
            ukive::Color(alpha, 1.f, 1.f, 1.f));
    }


    void WallpaperWindow::onValueChanged(
        unsigned int varIndex,
        IUIAnimationStoryboard *storyboard,
        IUIAnimationVariable *variable,
        double newValue, double previousValue)
    {
        switch (varIndex)
        {
        case VANISH_DOT:
            if (mBottomText.find(L" . . .") == std::wstring::npos)
                mBottomText += L" .";
            else
                mBottomText = mInitText;
            break;

        case ENABLE_ALPHA_TEXT:
            mBottomText = mStandbyText;
            break;
        }
    }

    void WallpaperWindow::onIntegerValueChanged(
        unsigned int varIndex,
        IUIAnimationStoryboard *storyboard,
        IUIAnimationVariable *variable,
        int newValue, int previousValue)
    {
    }

}