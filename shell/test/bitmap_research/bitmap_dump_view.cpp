#include "bitmap_dump_view.h"

#include <sstream>

#include "ukive/graphics/canvas.h"
#include "ukive/event/input_event.h"
#include "ukive/window/window.h"
#include "ukive/graphics/renderer.h"
#include "ukive/graphics/rect.h"


namespace shell {

    namespace {
        const int kCellWidth = 96;
    }

    BitmapDumpView::BitmapDumpView(ukive::Window *w)
        :View(w) {
        initView();
    }

    BitmapDumpView::~BitmapDumpView() {
        if (pixels_) {
            if (bitmap_height_ > 0 && bitmap_width_ > 0) {
                for (int h = 0; h < bitmap_height_; ++h) {
                    delete[] pixels_[h];
                }

                delete[] pixels_;
            }
        }
    }


    void BitmapDumpView::initView() {
        bitmap_width_ = 0;
        bitmap_height_ = 0;
        pixels_ = nullptr;

        total_trans_x_ = 0;
        total_trans_y_ = 0;
        is_mouse_down_ = false;
        scale_factor_ = 1.f;
        cell_width_ = kCellWidth;

        text_format_ = ukive::Renderer::createTextFormat(
            L"Consolas", 15, L"en-US");
        text_format_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        text_format_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

    void BitmapDumpView::setBitmapSize(int width, int height) {
        if (width != bitmap_width_ || height != bitmap_height_) {
            if (bitmap_height_ > 0 && bitmap_width_ > 0) {
                for (int h = 0; h < bitmap_height_; ++h) {
                    delete[] pixels_[h];
                }

                delete[] pixels_;
            }

            bitmap_width_ = width;
            bitmap_height_ = height;

            if (bitmap_height_ > 0 && bitmap_width_ > 0) {
                pixels_ = new unsigned int*[bitmap_height_];

                for (int h = 0; h < bitmap_height_; ++h) {
                    pixels_[h] = new unsigned int[bitmap_width_];
                }
            }

            invalidate();
        }
    }

    void BitmapDumpView::setBitmapColor(unsigned int argb, int x, int y) {
        pixels_[y][x] = argb;
        invalidate();
    }

    void BitmapDumpView::onDraw(ukive::Canvas *canvas) {
        View::onDraw(canvas);
        if (bitmap_width_ <= 0 || bitmap_height_ <= 0) {
            return;
        }

        canvas->save();
        canvas->setMatrix(matrix_);

        int cur_x = 0, cur_y = 0;
        for (int h = 0; h < bitmap_height_; ++h) {
            for (int w = 0; w < bitmap_width_; ++w) {
                unsigned int color_int = pixels_[h][w];
                ukive::Color color = ukive::Color::ofARGB(color_int);

                ukive::RectF cell_rect(
                    cur_x, cur_y, cell_width_, cell_width_);

                if (isCellVisible(cell_rect)) {
                    canvas->fillRect(cell_rect, color);

                    if (shouldShowText()) {
                        std::wstringstream ss;
                        ss << L"A " << ukive::Color::GetA(color_int) << L"\n"
                            << L"R " << ukive::Color::GetR(color_int) << L"\n"
                            << L"G " << ukive::Color::GetG(color_int) << L"\n"
                            << L"B " << ukive::Color::GetB(color_int);
                        canvas->drawText(ss.str(), text_format_.get(), cell_rect, ukive::Color::Black);
                    }
                }

                if (w == bitmap_width_ - 1) {
                    cur_x = 0;
                    cur_y += cell_width_;
                }
                else {
                    cur_x += cell_width_;
                }
            }
        }

        canvas->restore();
    }

    bool BitmapDumpView::onInputEvent(ukive::InputEvent *e) {
        switch (e->getEvent()) {
        case ukive::InputEvent::EVM_WHEEL:
            wheel_start_x_ = e->getX();
            wheel_start_y_ = e->getY();
            if (e->getMouseWheel() > 0) {
                scale_factor_ = 1.1f;
            }
            else {
                scale_factor_ = 0.9f;
            }
            matrix_.preScale(scale_factor_, scale_factor_, wheel_start_x_, wheel_start_y_);
            invalidate();
            break;

        case ukive::InputEvent::EVM_DOWN:
            is_mouse_down_ = true;
            prev_x_ = start_x_ = e->getX();
            prev_y_ = start_y_ = e->getY();
            break;
        case ukive::InputEvent::EVM_MOVE:
            if (is_mouse_down_) {
                int dx = e->getX() - prev_x_;
                int dy = e->getY() - prev_y_;

                total_trans_x_ += dx;
                total_trans_y_ += dy;

                prev_x_ = e->getX();
                prev_y_ = e->getY();

                matrix_.preTranslate(dx, dy);
                invalidate();
            }
            break;
        case ukive::InputEvent::EVM_UP:
            is_mouse_down_ = false;
            break;
        default:
            break;
        }
        return View::onInputEvent(e);
    }

    bool BitmapDumpView::shouldShowText() {
        return matrix_.get(ukive::Matrix::SCALE_X) > 0.5f;
    }

    bool BitmapDumpView::isCellVisible(const ukive::RectF &rect) {
        ukive::RectF real_rect(rect.left, rect.top,
            rect.right - rect.left, rect.bottom - rect.top);
        matrix_.transformRect(&real_rect);

        return getContentBounds().intersect(real_rect.toRect());
    }

}