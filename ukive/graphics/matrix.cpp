#include "matrix.h"

#include "ukive/graphics/rect.h"


namespace ukive {

    Matrix::Matrix()
        :matrix_native_(D2D1::Matrix3x2F::Identity()) {
    }

    Matrix::Matrix(const Matrix &rhs) {
        matrix_native_ = rhs.matrix_native_;
    }

    Matrix& Matrix::operator=(const Matrix &rhs) {
        matrix_native_ = rhs.matrix_native_;
        return *this;
    }

    Matrix Matrix::operator*(const Matrix &rhs) const {
        Matrix m;
        m.set(matrix_native_ * rhs.matrix_native_);
        return m;
    }

    void Matrix::set(const D2D1::Matrix3x2F &native) {
        matrix_native_ = native;
    }

    void Matrix::set(const D2D1_MATRIX_3X2_F &native) {
        matrix_native_._11 = native._11;
        matrix_native_._12 = native._12;
        matrix_native_._21 = native._21;
        matrix_native_._22 = native._22;
        matrix_native_._31 = native._31;
        matrix_native_._32 = native._32;
    }

    float Matrix::get(Element e) {
        switch (e) {
        case TRANS_X:
            return matrix_native_.dx;
        case TRANS_Y:
            return matrix_native_.dy;
        case SCALE_X:
            return matrix_native_.m11;
        case SCALE_Y:
            return matrix_native_.m22;
        }

        return 0.f;
    }

    void Matrix::identity() {
        matrix_native_ = D2D1::Matrix3x2F::Identity();
    }


    void Matrix::preTranslate(float dx, float dy) {
        auto tmp = D2D1::Matrix3x2F::Translation(dx, dy);
        matrix_native_ = matrix_native_*tmp;
    }

    void Matrix::postTranslate(float dx, float dy) {
        auto tmp = D2D1::Matrix3x2F::Translation(dx, dy);
        matrix_native_ = tmp*matrix_native_;
    }


    void Matrix::preScale(float sx, float sy) {
        auto tmp = D2D1::Matrix3x2F::Scale(sx, sy);
        matrix_native_ = matrix_native_*tmp;
    }

    void Matrix::postScale(float sx, float sy) {
        auto tmp = D2D1::Matrix3x2F::Scale(sx, sy);
        matrix_native_ = tmp*matrix_native_;
    }

    void Matrix::preScale(float sx, float sy, float cx, float cy) {
        auto tmp = D2D1::Matrix3x2F::Scale(sx, sy, D2D1::Point2F(cx, cy));
        matrix_native_ = matrix_native_*tmp;
    }

    void Matrix::postScale(float sx, float sy, float cx, float cy) {
        auto tmp = D2D1::Matrix3x2F::Scale(sx, sy, D2D1::Point2F(cx, cy));
        matrix_native_ = tmp*matrix_native_;
    }


    void Matrix::preRotate(float angle) {
        auto tmp = D2D1::Matrix3x2F::Rotation(angle);
        matrix_native_ = matrix_native_*tmp;
    }

    void Matrix::postRotate(float angle) {
        auto tmp = D2D1::Matrix3x2F::Rotation(angle);
        matrix_native_ = tmp*matrix_native_;
    }

    void Matrix::preRotate(float angle, float cx, float cy) {
        auto tmp = D2D1::Matrix3x2F::Rotation(angle, D2D1::Point2F(cx, cy));
        matrix_native_ = matrix_native_*tmp;
    }

    void Matrix::postRotate(float angle, float cx, float cy) {
        auto tmp = D2D1::Matrix3x2F::Rotation(angle, D2D1::Point2F(cx, cy));
        matrix_native_ = tmp*matrix_native_;
    }


    void Matrix::preSkew(float angle_x, float angle_y) {
        auto tmp = D2D1::Matrix3x2F::Skew(angle_x, angle_y);
        matrix_native_ = matrix_native_*tmp;
    }

    void Matrix::postSkew(float angle_x, float angle_y) {
        auto tmp = D2D1::Matrix3x2F::Skew(angle_x, angle_y);
        matrix_native_ = tmp*matrix_native_;
    }

    void Matrix::preSkew(float angle_x, float angle_y, float cx, float cy) {
        auto tmp = D2D1::Matrix3x2F::Skew(angle_x, angle_y, D2D1::Point2F(cx, cy));
        matrix_native_ = matrix_native_*tmp;
    }

    void Matrix::postSkew(float angle_x, float angle_y, float cx, float cy) {
        auto tmp = D2D1::Matrix3x2F::Skew(angle_x, angle_y, D2D1::Point2F(cx, cy));
        matrix_native_ = tmp*matrix_native_;
    }


    D2D1::Matrix3x2F Matrix::getNative() {
        return matrix_native_;
    }

    void Matrix::transformRect(RectF* rect) {
        D2D1_POINT_2F left_top_point = { rect->left, rect->top };
        D2D1_POINT_2F right_bottom_point = { rect->right, rect->bottom };
        left_top_point = matrix_native_.TransformPoint(left_top_point);
        right_bottom_point = matrix_native_.TransformPoint(right_bottom_point);

        rect->left = left_top_point.x;
        rect->top = left_top_point.y;
        rect->right = right_bottom_point.x;
        rect->bottom = right_bottom_point.y;
    }

}