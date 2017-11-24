#include "camera.h"


namespace shell {

    Camera::Camera()
    {
        mWidth = 1;
        mHeight = 1;
    }

    Camera::~Camera()
    {
    }

    void Camera::init(unsigned int wWidth, unsigned int wHeight)
    {
        mWidth = wWidth;
        mHeight = wHeight;

        //摄像机位置。
        mPos = dx::XMFLOAT3(0.0f, 0.0f, -10.0f);
        //摄像机看向的位置。
        mLookAt = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);

        mUp = dx::XMFLOAT3(0.0f, 1.0f, 0.0f);
        mRight = dx::XMFLOAT3(1.0f, 0.0f, 0.0f);
        mLook = dx::XMFLOAT3(0.0f, 0.0f, 1.0f);

        mZVector = dx::XMFLOAT3(0.0f, 0.0f, 1.0f);
        mYVector = dx::XMFLOAT3(0.0f, 1.0f, 0.0f);

        dx::XMVECTOR upVec = dx::XMLoadFloat3(&mUp);
        dx::XMVECTOR posVec = dx::XMLoadFloat3(&mPos);
        dx::XMVECTOR lookAtVec = dx::XMLoadFloat3(&mLookAt);

        dx::XMStoreFloat(&mRadius, dx::XMVector3Length(dx::XMVectorSubtract(lookAtVec, posVec)));

        dx::XMStoreFloat4x4(&mWorldMatrix, dx::XMMatrixIdentity());
        dx::XMStoreFloat4x4(&mViewMatrix, dx::XMMatrixLookAtLH(posVec, lookAtVec, upVec));

        dx::XMStoreFloat4x4(&mProjectionMatrix,
            dx::XMMatrixPerspectiveFovLH(dx::XM_PIDIV4, (float)mWidth / mHeight, NEAR_PLANE, FAR_PLANE));
        dx::XMStoreFloat4x4(&mOrthoMatrix,
            dx::XMMatrixOrthographicLH((float)mWidth, (float)mHeight, NEAR_PLANE, FAR_PLANE));
    }

    void Camera::close()
    {
    }

    void Camera::resize(unsigned int wWidth, unsigned int wHeight)
    {
        mWidth = wWidth;
        mHeight = wHeight;

        dx::XMStoreFloat4x4(&mProjectionMatrix,
            dx::XMMatrixPerspectiveFovLH(dx::XM_PIDIV4, (float)mWidth / mHeight, NEAR_PLANE, FAR_PLANE));

        dx::XMStoreFloat4x4(&mOrthoMatrix,
            dx::XMMatrixOrthographicLH((float)mWidth, (float)mHeight, NEAR_PLANE, FAR_PLANE));
    }


    void Camera::moveCamera(float dx, float dy)
    {
        dx::XMVECTOR frontVec = dx::XMVectorSet(mLook.x, 0, mLook.z, 0);
        frontVec = dx::XMVector3Normalize(frontVec);

        dx::XMVECTOR upVec = dx::XMLoadFloat3(&mUp);
        dx::XMVECTOR rightVec = dx::XMLoadFloat3(&mRight);
        dx::XMVECTOR posVec = dx::XMLoadFloat3(&mPos);
        dx::XMVECTOR lookAtVec = dx::XMLoadFloat3(&mLookAt);

        dx::XMVECTOR deltaVec = dx::XMVectorAdd(
            dx::XMVectorScale(frontVec, dy),
            dx::XMVectorScale(rightVec, dx));

        posVec = dx::XMVectorAdd(posVec, deltaVec);
        lookAtVec = dx::XMVectorAdd(lookAtVec, deltaVec);

        dx::XMStoreFloat4x4(&mViewMatrix,
            dx::XMMatrixLookAtLH(posVec, lookAtVec, upVec));

        dx::XMStoreFloat3(&mPos, posVec);
        dx::XMStoreFloat3(&mLookAt, lookAtVec);
    }

    void Camera::scaleCamera(float factor)
    {
        dx::XMVECTOR upVec = dx::XMLoadFloat3(&mUp);
        dx::XMVECTOR lookVec = dx::XMLoadFloat3(&mLook);
        dx::XMVECTOR lookAtVec = dx::XMLoadFloat3(&mLookAt);

        mRadius *= factor;

        dx::XMVECTOR posVec = dx::XMVectorAdd(lookAtVec,
            dx::XMVectorScale(lookVec, -mRadius));

        dx::XMStoreFloat4x4(&mViewMatrix,
            dx::XMMatrixLookAtLH(posVec, lookAtVec, upVec));

        dx::XMStoreFloat3(&mPos, posVec);
    }

    void Camera::circuleCamera(float dxAngle, float dyAngle)
    {
        dx::XMVECTOR yVec = dx::XMLoadFloat3(&mYVector);
        dx::XMVECTOR upVec = dx::XMLoadFloat3(&mUp);
        dx::XMVECTOR rightVec = dx::XMLoadFloat3(&mRight);
        dx::XMVECTOR posVec = dx::XMLoadFloat3(&mPos);
        dx::XMVECTOR lookAtVec = dx::XMLoadFloat3(&mLookAt);

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

        dx::XMStoreFloat4x4(&mViewMatrix,
            dx::XMMatrixLookAtLH(posVec, lookAtVec, upVec));

        dx::XMStoreFloat3(&mUp, upVec);
        dx::XMStoreFloat3(&mRight, rightVec);
        dx::XMStoreFloat3(&mPos, posVec);
        dx::XMStoreFloat3(&mLook, lookVec);
    }

    void Camera::circuleCamera2(float dxAngle, float dyAngle)
    {
        dx::XMVECTOR yVec = dx::XMLoadFloat3(&mYVector);
        dx::XMVECTOR rightVec = dx::XMLoadFloat3(&mRight);
        dx::XMVECTOR posVec = dx::XMLoadFloat3(&mPos);
        dx::XMVECTOR lookVec = dx::XMLoadFloat3(&mLook);

        dx::XMMATRIX rotateMatrix
            = dx::XMMatrixRotationAxis(yVec, dxAngle);
        lookVec = dx::XMVector3TransformCoord(lookVec, rotateMatrix);
        rightVec = dx::XMVector3TransformNormal(rightVec, rotateMatrix);

        dx::XMVECTOR lookAtVec
            = dx::XMVectorAdd(posVec,
                dx::XMVectorScale(lookVec, mRadius));

        rotateMatrix
            = dx::XMMatrixRotationAxis(rightVec, dyAngle);
        lookVec = dx::XMVector3TransformCoord(lookVec, rotateMatrix);

        lookAtVec = dx::XMVectorAdd(posVec,
            dx::XMVectorScale(lookVec, mRadius));

        dx::XMVECTOR upVec = dx::XMVector3Cross(lookVec, rightVec);
        upVec = dx::XMVector3Normalize(upVec);
        lookVec = dx::XMVector3Normalize(lookVec);

        dx::XMStoreFloat4x4(&mViewMatrix,
            dx::XMMatrixLookAtLH(posVec, lookAtVec, upVec));

        dx::XMStoreFloat3(&mUp, upVec);
        dx::XMStoreFloat3(&mRight, rightVec);
        dx::XMStoreFloat3(&mLook, lookVec);
        dx::XMStoreFloat3(&mLookAt, lookAtVec);
    }


    void Camera::moveWorld(float dx, float dy)
    {
        dx::XMVECTOR frontVec
            = dx::XMVectorSet(mLook.x, 0, mLook.z, 0);
        frontVec = dx::XMVector3Normalize(frontVec);
        frontVec = dx::XMVectorScale(frontVec, dy);

        dx::XMVECTOR rightVec
            = dx::XMLoadFloat3(&mRight);
        rightVec = dx::XMVectorScale(rightVec, dx);

        dx::XMMATRIX transMatrix
            = dx::XMMatrixTranslation(
                dx::XMVectorGetX(frontVec) + dx::XMVectorGetX(rightVec),
                0, dx::XMVectorGetZ(frontVec) + dx::XMVectorGetZ(rightVec));

        dx::XMMATRIX worldMatrix
            = dx::XMLoadFloat4x4(&mWorldMatrix);
        dx::XMMatrixMultiply(worldMatrix, transMatrix);
        dx::XMStoreFloat4x4(&mWorldMatrix, worldMatrix);
    }

    void Camera::scaleWorld(int direction)
    {
        float scaleFactor;

        if (direction > 0)
            scaleFactor = 1.1f;
        else
            scaleFactor = 0.9f;

        dx::XMMATRIX scaleMatrix
            = dx::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

        dx::XMMATRIX worldMatrix
            = dx::XMLoadFloat4x4(&mWorldMatrix);
        dx::XMMatrixMultiply(worldMatrix, scaleMatrix);
        dx::XMStoreFloat4x4(&mWorldMatrix, worldMatrix);
    }

    void Camera::rotateWorld(float dxAngle, float dyAngle)
    {
        dx::XMMATRIX rotateMatrix
            = dx::XMMatrixRotationY(dxAngle);

        dx::XMMATRIX worldMatrix
            = dx::XMLoadFloat4x4(&mWorldMatrix);
        dx::XMMatrixMultiply(worldMatrix, rotateMatrix);
        dx::XMStoreFloat4x4(&mWorldMatrix, worldMatrix);
    }

    void Camera::setCameraPosition(float x, float y, float z)
    {
        dx::XMVECTOR upVec = dx::XMLoadFloat3(&mUp);
        dx::XMVECTOR lookAtVec = dx::XMLoadFloat3(&mLookAt);
        dx::XMVECTOR posVec = dx::XMVectorSet(x, y, z, 0);
        dx::XMVECTOR lookVec = dx::XMVectorSubtract(lookAtVec, posVec);

        dx::XMStoreFloat(&mRadius, dx::XMVector3Length(lookVec));

        //TODO:此处假设在变换前后up不变。当up在变换后改变，需要额外变量确定right。
        dx::XMVECTOR rightVec = dx::XMVector3Cross(upVec, lookVec);
        upVec = dx::XMVector3Cross(lookVec, rightVec);

        upVec = dx::XMVector3Normalize(upVec);
        lookVec = dx::XMVector3Normalize(lookVec);
        rightVec = dx::XMVector3Normalize(rightVec);

        dx::XMStoreFloat4x4(&mViewMatrix,
            dx::XMMatrixLookAtLH(posVec, lookAtVec, upVec));

        dx::XMStoreFloat3(&mUp, upVec);
        dx::XMStoreFloat3(&mRight, rightVec);
        dx::XMStoreFloat3(&mPos, posVec);
        dx::XMStoreFloat3(&mLook, lookVec);
    }

    void Camera::setCameraLookAt(float x, float y, float z)
    {
        dx::XMVECTOR upVec = dx::XMLoadFloat3(&mUp);
        dx::XMVECTOR posVec = dx::XMLoadFloat3(&mPos);
        dx::XMVECTOR lookAtVec = dx::XMVectorSet(x, y, z, 0);
        dx::XMVECTOR lookVec = dx::XMVectorSubtract(lookAtVec, posVec);

        dx::XMStoreFloat(&mRadius, dx::XMVector3Length(lookVec));

        //TODO:此处假设在变换前后up不变。当up在变换后改变，需要额外变量确定right。
        dx::XMVECTOR rightVec = dx::XMVector3Cross(upVec, lookVec);
        upVec = dx::XMVector3Cross(lookVec, rightVec);

        upVec = dx::XMVector3Normalize(upVec);
        lookVec = dx::XMVector3Normalize(lookVec);
        rightVec = dx::XMVector3Normalize(rightVec);

        dx::XMStoreFloat4x4(&mViewMatrix,
            dx::XMMatrixLookAtLH(posVec, lookAtVec, upVec));

        dx::XMStoreFloat3(&mUp, upVec);
        dx::XMStoreFloat3(&mRight, rightVec);
        dx::XMStoreFloat3(&mLook, lookVec);
        dx::XMStoreFloat3(&mLookAt, lookVec);
    }

    const dx::XMFLOAT3 *Camera::getCameraPos()
    {
        return &mPos;
    }

    const dx::XMFLOAT3 *Camera::getCameraLookAt()
    {
        return &mLookAt;
    }

    const dx::XMFLOAT3 *Camera::getCameraUp()
    {
        return &mUp;
    }

    const dx::XMFLOAT4X4 *Camera::getWorldMatrix()
    {
        return &mWorldMatrix;
    }

    const dx::XMFLOAT4X4 *Camera::getViewMatrix()
    {
        return &mViewMatrix;
    }

    const dx::XMFLOAT4X4 *Camera::getProjectionMatrix()
    {
        return &mProjectionMatrix;
    }

    const dx::XMFLOAT4X4 *Camera::getOrthoMatrix()
    {
        return &mOrthoMatrix;
    }

    void Camera::getWVPMatrix(dx::XMFLOAT4X4 *wvp)
    {
        dx::XMMATRIX world = dx::XMLoadFloat4x4(&mWorldMatrix);
        dx::XMMATRIX view = dx::XMLoadFloat4x4(&mViewMatrix);
        dx::XMMATRIX projection = dx::XMLoadFloat4x4(&mProjectionMatrix);

        dx::XMStoreFloat4x4(wvp,
            dx::XMMatrixMultiply(
                dx::XMMatrixMultiply(world, view), projection));
    }

    void Camera::getWVOMatrix(dx::XMFLOAT4X4 *wvo)
    {
        dx::XMMATRIX world = dx::XMLoadFloat4x4(&mWorldMatrix);
        dx::XMMATRIX view = dx::XMLoadFloat4x4(&mViewMatrix);
        dx::XMMATRIX ortho = dx::XMLoadFloat4x4(&mOrthoMatrix);

        dx::XMStoreFloat4x4(wvo,
            dx::XMMatrixMultiply(
                dx::XMMatrixMultiply(world, view), ortho));
    }

}