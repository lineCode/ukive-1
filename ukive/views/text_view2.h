#ifndef UKIVE_VIEWS_TEXT_VIEW2_H_
#define UKIVE_VIEWS_TEXT_VIEW2_H_

#include <Windows.h>

#include "ukive/views/view.h"
#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    class TextView2 : public View
    {
    private:

        void initTextView();

        HRESULT createFontFace(PCWCHAR fontName, IDWriteFontFace **fontFace);

    public:
        TextView2(Window *wnd);
        TextView2(Window *wnd, int id);
        ~TextView2();
    };

}

#endif  // UKIVE_VIEWS_TEXT_VIEW2_H_