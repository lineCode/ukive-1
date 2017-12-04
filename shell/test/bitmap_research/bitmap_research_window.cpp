#include "bitmap_research_window.h"

#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout/restraint_layout_params.h"
#include "ukive/views/layout/frame_layout.h"
#include "ukive/graphics/bitmap_factory.h"

#include "shell/test/bitmap_research/bitmap_dump_view.h"


namespace shell {

    namespace {
        static const int BLOCK_SIZE = 108;
        static const int BITMAP_DATA = 233;

        struct BitmapData {
            int start_x;
            int start_y;
            int width;
            int height;
            int available;
            unsigned int pixel_data[BLOCK_SIZE];
        };
    }

    void BitmapResearchWindow::onCreate() {
        Window::onCreate();

        typedef ukive::RestraintLayoutParams Rlp;

        ukive::RestraintLayout *layout = new ukive::RestraintLayout(this);

        Rlp *lp = Rlp::Builder(
            Rlp::MATCH_PARENT,
            Rlp::MATCH_PARENT)
            .start(layout->getId(), Rlp::START, 8)
            .top(layout->getId(), Rlp::TOP, 8)
            .end(layout->getId(), Rlp::END, 8)
            .bottom(layout->getId(), Rlp::BOTTOM, 8).build();

        bitmap_dump_view_ = new BitmapDumpView(this);

        layout->addView(bitmap_dump_view_, lp);

        setContentView(layout);

        bitmap_dump_view_->setBitmapSize(16, 16);
        for (int h = 0; h < 16; ++h) {
            for (int w = 0; w < 16; ++w) {
                bitmap_dump_view_->setBitmapColor(3435973836, w, h);
            }
        }
    }

    bool BitmapResearchWindow::onDataCopy(unsigned int id, unsigned int size, void *data) {
        Window::onDataCopy(id, size, data);

        if (id == BITMAP_DATA && size == sizeof(BitmapData)) {
            BitmapData *bmp_data = reinterpret_cast<BitmapData*>(data);
            int cur_x = bmp_data->start_x;
            int cur_y = bmp_data->start_y;

            bitmap_dump_view_->setBitmapSize(bmp_data->width, bmp_data->height);
            for (int i = 0; i < bmp_data->available; ++i) {
                bitmap_dump_view_->setBitmapColor(bmp_data->pixel_data[i], cur_x, cur_y);

                if (cur_x == bmp_data->width - 1) {
                    cur_x = 0;
                    ++cur_y;
                }
                else {
                    ++cur_x;
                }
            }
        }

        return false;
    }

}