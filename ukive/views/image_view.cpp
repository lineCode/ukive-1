#include "image_view.h"

#include <algorithm>

#include "ukive/drawable/bitmap_drawable.h"
#include "ukive/graphics/bitmap.h"
#include "ukive/graphics/canvas.h"


namespace ukive {

    ImageView::ImageView(Window* wnd)
        : View(wnd),
          scale_type_(FIT_WHEN_LARGE),
          bitmap_drawable_(nullptr) {
    }

    ImageView::~ImageView() {
    }


    void ImageView::onMeasure(int width, int height, int width_mode, int height_mode) {
        int final_width = 0;
        int final_height = 0;

        int vert_padding = getPaddingTop() + getPaddingBottom();
        int hori_padding = getPaddingLeft() + getPaddingRight();

        switch (width_mode) {
        case FIT:
            if (bitmap_drawable_) {
                final_width = bitmap_drawable_->getIncWidth();
            }

            final_width = std::max(getMinimumWidth(), final_width + hori_padding);
            final_width = std::min(width, final_width);
            break;

        case UNKNOWN:
            if (bitmap_drawable_) {
                final_width = bitmap_drawable_->getIncWidth();
            }

            final_width = std::max(getMinimumWidth(), final_width + hori_padding);
            break;

        case EXACTLY:
            final_width = width;
            break;

        default:
            break;
        }

        switch (height_mode) {
        case FIT:
            if (bitmap_drawable_ != nullptr) {
                final_height = bitmap_drawable_->getIncHeight();
            }

            final_height = std::max(getMinimumHeight(), final_height + vert_padding);
            final_height = std::min(height, final_height);
            break;

        case UNKNOWN:
            if (bitmap_drawable_ != nullptr) {
                final_height = bitmap_drawable_->getIncHeight();
            }

            final_height = std::max(getMinimumHeight(), final_height + vert_padding);
            break;

        case EXACTLY:
            final_height = height;
            break;

        default:
            break;
        }

        setMeasuredDimension(final_width, final_height);
    }

    void ImageView::onDraw(Canvas* canvas) {
        View::onDraw(canvas);

        if (scale_type_ == MATRIX) {
            canvas->save();
            canvas->setMatrix(matrix_);

            if (bitmap_drawable_) {
                bitmap_drawable_->draw(canvas);
            }

            canvas->restore();
        } else {
            if (bitmap_drawable_) {
                bitmap_drawable_->draw(canvas);
            }
        }
    }

    bool ImageView::onInputEvent(InputEvent* e) {
        return View::onInputEvent(e);
    }

    void ImageView::onSizeChanged(int width, int height, int old_w, int old_h) {
        View::onSizeChanged(width, height, old_w, old_h);
        setImageBounds(width, height);
    }

    void ImageView::setMatrix(const Matrix& m) {
        matrix_ = m;
        invalidate();
    }

    void ImageView::setScaleType(ScaleType type) {
        if (scale_type_ == type) {
            return;
        }

        scale_type_ = type;
        setImageBounds(getWidth(), getHeight());

        invalidate();
    }

    void ImageView::setImageBitmap(std::shared_ptr<Bitmap> bitmap) {
        if (bitmap != nullptr) {
            bitmap_drawable_ = new BitmapDrawable(bitmap);
            setImageBounds(getWidth(), getHeight());
            invalidate();
        }
    }

    void ImageView::setImageBounds(int width, int height) {
        if (!bitmap_drawable_ || width <= 0 || height <= 0) {
            return;
        }

        switch (scale_type_) {
        case FULL:
            bitmap_drawable_->setBounds(0, 0, width, height);
            break;

        case FIT_ALWAYS:
            fitImageBounds(width, height, true);
            break;

        case FIT_WHEN_LARGE:
            fitImageBounds(width, height, false);
            break;

        case MATRIX:
        default:
            bitmap_drawable_->setBounds(
                0, 0,
                bitmap_drawable_->getIncWidth(),
                bitmap_drawable_->getIncHeight());
            break;
        }
    }

    void ImageView::fitImageBounds(int width, int height, bool always) {
        int bmp_width = bitmap_drawable_->getIncWidth();
        int bmp_height = bitmap_drawable_->getIncHeight();
        if (bmp_width <= 0 || bmp_height <= 0) {
            return;
        }

        if (always || (bmp_width > width || bmp_height > height)) {
            float scale = std::min(
                float(width) / bmp_width,
                float(height) / bmp_height);

            bmp_width *= scale;
            bmp_height *= scale;
        }

        float bmp_x = (width - bmp_width) / 2.f;
        float bmp_y = (height - bmp_height) / 2.f;

        bitmap_drawable_->setBounds(
            bmp_x, bmp_y,
            bmp_width, bmp_height);
    }

    Matrix ImageView::getMatrix() const {
        return matrix_;
    }

    ImageView::ScaleType ImageView::getScaleType() const {
        return scale_type_;
    }

    std::shared_ptr<Bitmap> ImageView::getImageBitmap() const {
        return bitmap_drawable_->getBitmap();
    }

}