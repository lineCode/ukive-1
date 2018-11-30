#ifndef UKIVE_VIEWS_CHECK_BOX_H_
#define UKIVE_VIEWS_CHECK_BOX_H_

#include "ukive/views/text_view.h"


namespace ukive {

    class CheckBox : public TextView {
    public:
        CheckBox(Window* w);

        void setChecked(bool checked);

    private:
        bool checked_;
    };

}

#endif  // UKIVE_VIEWS_CHECK_BOX_H_