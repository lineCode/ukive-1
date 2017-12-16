#ifndef SHELL_TEST_BITMAP_RESEARCH_BITMAP_DUMP_VIEW_H_
#define SHELL_TEST_BITMAP_RESEARCH_BITMAP_DUMP_VIEW_H_

#include <vector>

#include "ukive/graphics/color.h"
#include "ukive/graphics/matrix.h"
#include "ukive/graphics/bitmap.h"
#include "ukive/views/view.h"
#include "ukive/utils/com_ptr.h"


namespace ukive {
    class Canvas;
    class Window;
    class InputEvent;
}

namespace shell {

    class BitmapDumpView : public ukive::View {
    public:
        BitmapDumpView(ukive::Window *win);
        BitmapDumpView(ukive::Window *win, int id);
        ~BitmapDumpView();

        void onDraw(ukive::Canvas *canvas) override;
        bool onInputEvent(ukive::InputEvent *e) override;

        void setBitmapSize(int width, int height);
        void setBitmapColor(unsigned int argb, int x, int y);

    private:
        void initView();

        bool shouldShowText();
        bool isCellVisible(const ukive::RectF &rect);

        int cell_width_;

        int bitmap_width_;
        int bitmap_height_;

        int prev_x_;
        int prev_y_;
        int start_x_;
        int start_y_;
        int wheel_start_x_;
        int wheel_start_y_;
        int total_trans_x_;
        int total_trans_y_;
        float scale_factor_;
        bool is_mouse_down_;

        unsigned int **pixels_;
        ukive::Matrix matrix_;
        ukive::ComPtr<IDWriteTextFormat> text_format_;
    };

}

#endif  // SHELL_TEST_BITMAP_RESEARCH_BITMAP_DUMP_VIEW_H_