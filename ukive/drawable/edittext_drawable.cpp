#include "edittext_drawable.h"

#include "ukive/window/window.h"
#include "ukive/graphics/canvas.h"


namespace ukive {

    EditTextDrawable::EditTextDrawable(Window* win)
        :Drawable(),
        owner_win_(win),
        panel_line_color_(Color::Grey400) {

        panel_line_width_ = std::ceil(win->dpToPx(1.5f));
    }

    EditTextDrawable::~EditTextDrawable() {}


    bool EditTextDrawable::onFocusChanged(bool focus) {
        if (focus) {
            //panel_line_width_ = 2;
            panel_line_color_ = Color::Blue500;
        } else {
            //panel_line_width_ = 2;
            panel_line_color_ = Color::Grey400;
        }

        return true;
    }

    void EditTextDrawable::draw(Canvas* canvas) {
        auto bound = getBounds();

        canvas->fillRect(
            RectF(
                bound.left, bound.bottom - panel_line_width_,
                bound.width(), panel_line_width_),
            panel_line_color_);
    }

    float EditTextDrawable::getOpacity() const {
        return 1.f;
    }

}