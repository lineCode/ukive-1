#include "ukive/views/check_box.h"

#include "ukive/drawable/check_drawable.h"
#include "ukive/window/window.h"


namespace ukive {

    CheckBox::CheckBox(Window* w)
        : CheckBox(w, {}) {}

    CheckBox::CheckBox(Window* w, AttrsRef attrs)
        : TextView(w, attrs),
          checked_(false)
    {
        setPadding(w->dpToPxX(24), w->dpToPxX(8), w->dpToPxX(8), w->dpToPxX(8));
        setBackground(new CheckDrawable(w));
    }

    void CheckBox::setChecked(bool checked) {
        if (checked == checked_) {
            return;
        }
        checked_ = checked;
        // TODO:
        static_cast<CheckDrawable*>(getBackground())->setChecked(checked);
        invalidate();
    }
}
