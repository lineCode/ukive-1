#include "camera.h"

#include <algorithm>


namespace shell {

    Camera::Camera(int width, int height)
        :width_(width >= 1 ? width : 1),
        height_(height >= 1 ? height : 1) {

        //摄像机位置。
        pos_ = dx::XMFLOAT3(0.0f, 0.0f, -10.0f);
        //摄像机看向的位置。
        look_at_ = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);

        up_ = dx::XMFLOAT3(0.0f, 1.0f, 0.0f);
        right_ = dx::XMFLOAT3(1.0f, 0.0f, 0.0f);
        look_ = dx::XMFLOAT3(0.0f, 0.0f, 1.0f);

        z_vector_ = dx::XMFLOAT3(0.0f, 0.0f, 1.0f);
        y_vector_ = dx::XMFLOAT3(0.0f, 1.0f, 0.0f);

        dx::XMVECTOR upVec = dx::XMLoadFloat3(&up_);
        dx::XMVECTOR posVec = dx::XMLoadFloat3(&pos_);
        dx::XMVECTOR lookAtVec = dx::XMLoadFloat3(&look_at_);

        dx::XMStoreFloat(&radius_, dx::XMVector3Length(dx::XMVectorSubtract(lookAtVec, posVec)));

        dx::XMStoreFloat4x4(&world_matrix_, dx::XMMatrixIdentity());
        dx::XMStoreFloat4x4(&view_matrix_, dx::XMMatrixLookAtLH(posVec, lookAtVec, upVec));

        dx::XMStoreFloat4x4(&projection_matrix_,
            dx::XMMatrixPerspectiveFovLH(dx::XM_PIDIV4, width_ * 1.f / height_, NEAR_PLANE, FAR_PLANE));
        dx::XMStoreFloat4x4(&ortho_matrix_,
            dx::XMMatrixOrthographicLH(width_, height_, NEAR_PLANE, FAR_PLANE));
    }

    Camera::~Camera() {}


    void Camera::resize(int width, int height) {
        width_ = std::max(width, 1);
        height_ = std::max(height, 1);

        dx::XMStoreFloat4x4(&projection_matrix_,
            dx::XMMatrixPerspectiveFovLH(dx::XM_PIDIV4, width_ * 1.f / height_, NEAR_PLANE, FAR_PLANE));

        dx::XMStoreFloat4x4(&ortho_matrix_,
            dx::XMMatrixOrthographicLH(width_, height_, NEAR_PLANE, FAR_PLANE));
    }

    void Camera::moveCamera(float dx, float dy) {
        dx::XMVECTOR frontVec = dx::XMVectorSet(look_.x, 0, look_.z, 0);
        frontVec = dx::XMVector3Normalize(frontVec);

        dx::XMVECTOR upVec = dx::XMLoadFloat3(&up_);
        dx::XMVECTOR rightVec = dx::XMLoadFloat3(&right_);
        dx::XMVECTOR posVec = dx::XMLoadFloat3(&pos_);
        dx::XMVECTOR lookAtVec = dx::XMLoadFloat3(&look_at_);

        dx::XMVECTOR deltaVec = dx::XMVectorAdd(
            dx::XMVectorScale(frontVec, dy),
            dx::XMVectorScale(rightVec, dx));

        posVec = dx::XMVectorAdd(posVec, deltaVec);
        lookAtVec = dx::XMVectorAdd(lookAtVec, deltaVec);

        dx::XMStoreFloat4x4(&view_matrix_,
            dx::XMMatrixLookAtLH(posVec, lookAtVec, upVec));

        dx::XMStoreFloat3(&pos_, posVec);
        dx::XMStoreFloat3(&look_at_, lookAtVec);
    }

    void Camera::scaleCamera(float factor) {
        dx::XMVECTOR upVec = dx::XMLoadFloat3(&up_);
        dx::XMVECTOR lookVec = dx::XMLoadFloat3(&look_);
        dx::XMVECTOR lookAtVec = dx::XMLoadFloat3(&look_at_);

        radius_ *= factor;

        dx::XMVECTOR posVec = dx::XMVectorAdd(lookAtVec,
            dx::XMVectorScale(lookVec, -radius_));

        dx::XMStoreFloat4x4(&view_matrix_,
            dx::XMMatrixLookAtLH(posVec, lookAtVec, upVec));

        dx::XMStoreFloat3(&pos_, posVec);
    }

    void Camera::circuleCamera(float dxAngle, float dyAngle) {
        dx::XMVECTOR yVec = dx::XMLoadFloat3(&y_vector_);
        dx::XMVECTOR upVec = dx::XMLoadFloat3(&up_);
        dx::XMVECTOR rightVec = dx::XMLoadFloat3(&right_);
        dx::XMVECTOR posVec = dx::XMLoadFloat3(&pos_);
        dx::XMVECTOR lookAtVec = dx::XMLoadFloat3(&look_at_);

        dx::XMMATRIX rotateMatrix
            = dx::XMMatrixRotationAxis(yVec, dxAngle);
        posVec = dx::XMVector3TransformCoord(posVec, rotateMatrix);
        rightVec = dx::XMVector3TransformNormal(rightVec, rotateMatrix);

        rotateMatrix = dx::XMMatrixRotationAxis(rightVec, dyAngle);
        posVec = dx::XMVector3TransformCoord(posVec, rotateMatrix);

        dx::XMVECTOR lookVec
            = dx::XMVectorSubtract(lookAtVec, posVec);

        upVec = dx::XMVector3Cross(lookVec, rightVec);
        upVec = dx::XMVector3Normalize(upVec);
        lookVec = dx::XMVector3Normalize(lookVec);

        dx::XMStoreFloat4x4(&view_matrix_,
            dx::XMMatrixLookAtLH(posVec, lookAtVec, upVec));

        dx::XMStoreFloat3(&up_, upVec);
        dx::XMStoreFloat3(&right_, rightVec);
        dx::XMStoreFloat3(&pos_, posVec);
        dx::XMStoreFloat3(&look_, lookVec);
    }

    void Camera::circuleCamera2(float dxAngle, float dyAngle) {
        dx::XMVECTOR yVec = dx::XMLoadFloat3(&y_vector_);
        dx::XMVECTOR rightVec = dx::XMLoadFloat3(&right_);
        dx::XMVECTOR posVec = dx::XMLoadFloat3(&pos_);
        dx::XMVECTOR lookVec = dx::XMLoadFloat3(&look_);

        dx::XMMATRIX rotateMatrix
            = dx::XMMatrixRotationAxis(yVec, dxAngle);
        lookVec = dx::XMVector3TransformCoord(lookVec, rotateMatrix);
        rightVec = dx::XMVector3TransformNormal(rightVec, rotateMatrix);

        dx::XMVECTOR lookAtVec
            = dx::XMVectorAdd(posVec,
                dx::XMVectorScale(lookVec, radius_));

        rotateMatrix
            = dx::XMMatrixRotationAxis(rightVec, dyAngle);
        lookVec = dx::XMVector3TransformCoord(lookVec, rotateMatrix);

        lookAtVec = dx::XMVectorAdd(posVec,
            dx::XMVectorScale(lookVec, radius_));

        dx::XMVECTOR upVec = dx::XMVector3Cross(lookVec, rightVec);
        upVec = dx::XMVector3Normalize(upVec);
        lookVec = dx::XMVector3Normalize(lookVec);

        dx::XMStoreFloat4x4(&view_matrix_,
            dx::XMMatrixLookAtLH(posVec, lookAtVec, upVec));

        dx::XMStoreFloat3(&up_, upVec);
        dx::XMStoreFloat3(&right_, rightVec);
        dx::XMStoreFloat3(&look_, lookVec);
        dx::XMStoreFloat3(&look_at_, lookAtVec);
    }


    void Camera::moveWorld(float dx, float dy) {
        dx::XMVECTOR frontVec
            = dx::XMVectorSet(look_.x, 0, look_.z, 0);
        frontVec = dx::XMVector3Normalize(frontVec);
        frontVec = dx::XMVectorScale(frontVec, dy);

        dx::XMVECTOR rightVec
            = dx::XMLoadFloat3(&right_);
        rightVec = dx::XMVectorScale(rightVec, dx);

        dx::XMMATRIX transMatrix
            = dx::XMMatrixTranslation(
                dx::XMVectorGetX(frontVec) + dx::XMVectorGetX(rightVec),
                0, dx::XMVectorGetZ(frontVec) + dx::XMVectorGetZ(rightVec));

        dx::XMMATRIX worldMatrix
            = dx::XMLoadFloat4x4(&world_matrix_);
        dx::XMMatrixMultiply(worldMatrix, transMatrix);
        dx::XMStoreFloat4x4(&world_matrix_, worldMatrix);
    }

    void Camera::scaleWorld(int direction) {
        float scaleFactor;

        if (direction > 0)
            scaleFactor = 1.1f;
        else
            scaleFactor = 0.9f;

        dx::XMMATRIX scaleMatrix
            = dx::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

        dx::XMMATRIX worldMatrix
            = dx::XMLoadFloat4x4(&world_matrix_);
        dx::XMMatrixMultiply(worldMatrix, scaleMatrix);
        dx::XMStoreFloat4x4(&world_matrix_, worldMatrix);
    }

    void Camera::rotateWorld(float dxAngle, float dyAngle) {
        dx::XMMATRIX rotateMatrix
            = dx::XMMatrixRotationY(dxAngle);

        dx::XMMATRIX worldMatrix
            = dx::XMLoadFloat4x4(&world_matrix_);
        dx::XMMatrixMultiply(worldMatrix, rotateMatrix);
        dx::XMStoreFloat4x4(&world_matrix_, worldMatrix);
    }

    void Camera::setCameraPosition(float x, float y, float z) {
        dx::XMVECTOR upVec = dx::XMLoadFloat3(&up_);
        dx::XMVECTOR lookAtVec = dx::XMLoadFloat3(&look_at_);
        dx::XMVECTOR posVec = dx::XMVectorSet(x, y, z, 0);
        dx::XMVECTOR lookVec = dx::XMVectorSubtract(lookAtVec, posVec);

        dx::XMStoreFloat(&radius_, dx::XMVector3Length(lookVec));

        // TODO:此处假设在变换前后 up 不变。当 up 在变换后改变，需要额外变量确定 right。
        dx::XMVECTOR rightVec = dx::XMVector3Cross(upVec, lookVec);
        upVec = dx::XMVector3Cross(lookVec, rightVec);

        upVec = dx::XMVector3Normalize(upVec);
        lookVec = dx::XMVector3Normalize(lookVec);
        rightVec = dx::XMVector3Normalize(rightVec);

        dx::XMStoreFloat4x4(&view_matrix_,
            dx::XMMatrixLookAtLH(posVec, lookAtVec, upVec));

        dx::XMStoreFloat3(&up_, upVec);
        dx::XMStoreFloat3(&right_, rightVec);
        dx::XMStoreFloat3(&pos_, posVec);
        dx::XMStoreFloat3(&look_, lookVec);
    }

    void Camera::setCameraLookAt(float x, float y, float z) {
        dx::XMVECTOR upVec = dx::XMLoadFloat3(&up_);
        dx::XMVECTOR posVec = dx::XMLoadFloat3(&pos_);
        dx::XMVECTOR lookAtVec = dx::XMVectorSet(x, y, z, 0);
        dx::XMVECTOR lookVec = dx::XMVectorSubtract(lookAtVec, posVec);

        dx::XMStoreFloat(&radius_, dx::XMVector3Length(lookVec));

        // TODO:此处假设在变换前后 up 不变。当 up 在变换后改变，需要额外变量确定 right。
        dx::XMVECTOR rightVec = dx::XMVector3Cross(upVec, lookVec);
        upVec = dx::XMVector3Cross(lookVec, rightVec);

        upVec = dx::XMVector3Normalize(upVec);
        lookVec = dx::XMVector3Normalize(lookVec);
        rightVec = dx::XMVector3Normalize(rightVec);

        dx::XMStoreFloat4x4(&view_matrix_,
            dx::XMMatrixLookAtLH(posVec, lookAtVec, upVec));

        dx::XMStoreFloat3(&up_, upVec);
        dx::XMStoreFloat3(&right_, rightVec);
        dx::XMStoreFloat3(&look_, lookVec);
        dx::XMStoreFloat3(&look_at_, lookVec);
    }

    const dx::XMFLOAT3* Camera::getCameraPos() const {
        return &pos_;
    }

    const dx::XMFLOAT3* Camera::getCameraLookAt() const {
        return &look_at_;
    }

    const dx::XMFLOAT3* Camera::getCameraUp() const {
        return &up_;
    }

    const dx::XMFLOAT4X4* Camera::getWorldMatrix() const {
        return &world_matrix_;
    }

    const dx::XMFLOAT4X4* Camera::getViewMatrix() const {
        return &view_matrix_;
    }

    const dx::XMFLOAT4X4* Camera::getProjectionMatrix() const {
        return &projection_matrix_;
    }

    const dx::XMFLOAT4X4* Camera::getOrthoMatrix() const {
        return &ortho_matrix_;
    }

    void Camera::getWVPMatrix(dx::XMFLOAT4X4 *wvp) const {
        dx::XMMATRIX world = dx::XMLoadFloat4x4(&world_matrix_);
        dx::XMMATRIX view = dx::XMLoadFloat4x4(&view_matrix_);
        dx::XMMATRIX projection = dx::XMLoadFloat4x4(&projection_matrix_);

        dx::XMStoreFloat4x4(wvp,
            dx::XMMatrixMultiply(
                dx::XMMatrixMultiply(world, view), projection));
    }

    void Camera::getWVOMatrix(dx::XMFLOAT4X4 *wvo) const {
        dx::XMMATRIX world = dx::XMLoadFloat4x4(&world_matrix_);
        dx::XMMATRIX view = dx::XMLoadFloat4x4(&view_matrix_);
        dx::XMMATRIX ortho = dx::XMLoadFloat4x4(&ortho_matrix_);

        dx::XMStoreFloat4x4(wvo,
            dx::XMMatrixMultiply(
                dx::XMMatrixMultiply(world, view), ortho));
    }

}