#ifndef UKIVE_GRAPHICS_MATRIX_H_
#define UKIVE_GRAPHICS_MATRIX_H_

#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    class RectF;

    class Matrix {
    public:
        enum Element {
            TRANS_X,
            TRANS_Y,
            SCALE_X,
            SCALE_Y
        };

        Matrix();
        Matrix(const Matrix &rhs);
        Matrix& operator=(const Matrix &rhs);

        void set(const D2D1::Matrix3x2F &native);
        void set(const D2D1_MATRIX_3X2_F &native);
        float get(Element e);
        D2D1::Matrix3x2F getNative();

        void identity();

        void preTranslate(float dx, float dy);
        void postTranslate(float dx, float dy);

        void preScale(float sx, float sy);
        void postScale(float sx, float sy);
        void preScale(float sx, float sy, float cx, float cy);
        void postScale(float sx, float sy, float cx, float cy);

        void preRotate(float angle);
        void postRotate(float angle);
        void preRotate(float angle, float cx, float cy);
        void postRotate(float angle, float cx, float cy);

        void preSkew(float angle_x, float angle_y);
        void postSkew(float angle_x, float angle_y);
        void preSkew(float angle_x, float angle_y, float cx, float cy);
        void postSkew(float angle_x, float angle_y, float cx, float cy);

        void transformRect(RectF* rect);

    private:
        D2D1::Matrix3x2F matrix_native_;
    };

}

#endif  // UKIVE_GRAPHICS_MATRIX_H_