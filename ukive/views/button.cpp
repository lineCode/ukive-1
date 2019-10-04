#include "button.h"

#include "ukive/window/window.h"
#include "ukive/drawable/ripple_drawable.h"
#include "ukive/graphics/color.h"
#include "ukive/graphics/canvas.h"
#include "ukive/event/input_event.h"
#include "ukive/application.h"

#include "oigka/layout_constants.h"


namespace ukive {

    // Button
    Button::Button(Window* w)
        : Button(w, {})
    {}

    Button::Button(Window *w, AttrsRef attrs)
        : TextView(w, attrs)
    {
        if (attrs.find(oigka::kAttrTextViewText) == attrs.end()) {
            setText(L"Button");
        }
        setTextSize(13);
        setIsEditable(false);
        setIsSelectable(false);

        initButton();
    }

    void Button::initButton() {
        shape_drawable_ = new ShapeDrawable(ShapeDrawable::ROUND_RECT);
        shape_drawable_->setRadius(getWindow()->dpToPxX(2.f));
        shape_drawable_->setSolidEnable(true);
        shape_drawable_->setSolidColor(Color::White);

        ripple_background_ = new RippleDrawable();
        ripple_background_->addDrawable(shape_drawable_);

        setPadding(
            getWindow()->dpToPxX(24),
            getWindow()->dpToPxX(6),
            getWindow()->dpToPxX(24),
            getWindow()->dpToPxX(6));
        setBackground(ripple_background_);
        setElevation(getWindow()->dpToPxX(2.0f));
    }

    void Button::setButtonColor(Color color) {
        shape_drawable_->setSolidColor(color);
        invalidate();
    }

    void Button::setButtonShape(ShapeDrawable::Shape shape) {
        switch (shape) {
        case ShapeDrawable::OVAL:
            setOutline(OUTLINE_OVAL);
            break;

        case ShapeDrawable::RECT:
        case ShapeDrawable::ROUND_RECT:
        default:
            setOutline(OUTLINE_RECT);
            break;
        }
        shape_drawable_->setShape(shape);
        invalidate();
    }


    // DropdownButton
    DropdownButton::DropdownButton(Window* w)
        : View(w)
    {
        initButton();
    }

    void DropdownButton::initButton() {
        auto shape_drawable = new ShapeDrawable(ShapeDrawable::OVAL);
        shape_drawable->setSolidEnable(true);
        shape_drawable->setSolidColor(Color::White);

        auto ripple_background = new RippleDrawable();
        ripple_background->setDrawMaskEnabled(false);
        ripple_background->addDrawable(shape_drawable);
        setBackground(ripple_background);

        tri_length_ = getWindow()->dpToPxX(10);
        tri_height_ = getWindow()->dpToPxX(6);
        button_size_ = getWindow()->dpToPxX(28);

        // 设为偶数
        if (tri_length_ & 1) {
            ++tri_length_;
        }
        if (tri_height_ & 1) {
            ++tri_height_;
        }
        if (button_size_ & 1) {
            ++button_size_;
        }

        ComPtr<ID2D1GeometrySink> sink;
        Application::getGraphicDeviceManager()->getD2DFactory()->CreatePathGeometry(&tri_geo_);
        if (SUCCEEDED(tri_geo_->Open(&sink))) {
            sink->BeginFigure(D2D1::Point2F(0, 0), D2D1_FIGURE_BEGIN_FILLED);
            sink->AddLine(D2D1::Point2F(tri_length_, 0));
            sink->AddLine(D2D1::Point2F(tri_length_ / 2, tri_height_));
            sink->AddLine(D2D1::Point2F(0, 0));
            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
            sink->Close();
        }
    }

    void DropdownButton::onMeasure(int width, int height, int width_mode, int height_mode) {
        setMeasuredSize(button_size_, button_size_);
    }

    void DropdownButton::onDraw(Canvas* canvas) {
        View::onDraw(canvas);

        int x = (getWidth() - tri_length_) / 2;
        int y = (getHeight() - tri_height_) / 2;

        ComPtr<ID2D1TransformedGeometry> geo;
        Application::getGraphicDeviceManager()->getD2DFactory()->CreateTransformedGeometry(
            tri_geo_.get(), D2D1::Matrix3x2F::Translation(x, y + 1), &geo);

        canvas->fillGeometry(geo.get(), Color::Grey500);
    }

    bool DropdownButton::onInputEvent(InputEvent* e) {
        View::onInputEvent(e);
        return true;
    }

}
