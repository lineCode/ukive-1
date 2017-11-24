#ifndef UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_H_
#define UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_H_

#include <memory>

#include "ukive/views/layout/frame_layout.h"


namespace ukive {

    class Window;
    class LayoutParams;
    class LinearLayout;

    class BaseLayout : public FrameLayout
    {
    private:
        std::shared_ptr<FrameLayout> mShadeLayout;
        std::shared_ptr<LinearLayout> mContentLayout;

        void initBaseLayout();

    protected:
        virtual LayoutParams *generateLayoutParams(LayoutParams *lp) override;
        virtual LayoutParams *generateDefaultLayoutParams() override;
        virtual bool checkLayoutParams(LayoutParams *lp) override;

    public:
        BaseLayout(Window *wnd);
        BaseLayout(Window *wnd, int id);
        ~BaseLayout();

        void addShade(View *shade);
        void removeShade(View *shade);

        void addContent(View *content);

        virtual View *findWidgetById(int id) override;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_H_