#ifndef UKIVE_VIEWS_IMAGE_VIEW_H_
#define UKIVE_VIEWS_IMAGE_VIEW_H_

#include <memory>

#include "ukive/views/view.h"
#include "ukive/graphics/matrix.h"


namespace ukive {

    class Bitmap;
    class BitmapDrawable;

    class ImageView : public View {
    public:
        enum ScaleType {
            FULL,
            FIT_ALWAYS,
            FIT_WHEN_LARGE,
            MATRIX,
        };

        explicit ImageView(Window* w);
        ImageView(Window* w, AttrsRef attrs);
        ~ImageView();

        void onMeasure(int width, int height, int width_mode, int height_mode) override;
        void onSizeChanged(int width, int height, int old_w, int old_h) override;
        void onDraw(Canvas* canvas) override;
        bool onInputEvent(InputEvent* e) override;

        void setMatrix(const Matrix& m);
        void setScaleType(ScaleType type);
        void setImageBitmap(std::shared_ptr<Bitmap> bitmap);

        Matrix getMatrix() const;
        ScaleType getScaleType() const;
        std::shared_ptr<Bitmap> getImageBitmap() const;

    private:
        void setImageBounds(int width, int height);
        void fitImageBounds(int width, int height, bool always);

        Matrix matrix_;
        ScaleType scale_type_;
        BitmapDrawable* bitmap_drawable_;
    };

}

#endif  // UKIVE_VIEWS_IMAGE_VIEW_H_