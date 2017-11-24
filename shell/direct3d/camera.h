#ifndef SHELL_DIRECT3D_CAMERA_H_
#define SHELL_DIRECT3D_CAMERA_H_

#define NEAR_PLANE 10.0f
#define FAR_PLANE 100000.0f

#include "shell/third_party/directx_math/Inc/DirectXMath.h"


namespace shell {

    namespace dx = DirectX;

    class Camera
    {
    private:
        dx::XMFLOAT3 mPos;
        dx::XMFLOAT3 mLookAt;
        dx::XMFLOAT3 mUp;
        dx::XMFLOAT3 mRight;
        dx::XMFLOAT3 mLook;
        dx::XMFLOAT3 mZVector;
        dx::XMFLOAT3 mYVector;

        dx::XMFLOAT4X4 mWorldMatrix;
        dx::XMFLOAT4X4 mViewMatrix;
        dx::XMFLOAT4X4 mProjectionMatrix;
        dx::XMFLOAT4X4 mOrthoMatrix;

        float mRadius;
        unsigned int mWidth;
        unsigned int mHeight;

    public:
        Camera();
        ~Camera();

        void init(unsigned int wWidth, unsigned int wHeight);
        void close();
        void resize(unsigned int wWidth, unsigned int wHeight);

        void moveCamera(float dx, float dy);
        void scaleCamera(float factor);
        void circuleCamera(float dxAngle, float dyAngle);
        void circuleCamera2(float dxAngle, float dyAngle);

        void moveWorld(float dx, float dy);
        void scaleWorld(int direction);
        void rotateWorld(float dxAngle, float dyAngle);

        //该方法有缺陷，请看方法内注释。
        void setCameraPosition(float x, float y, float z);
        //该方法有缺陷，请看方法内注释。
        void setCameraLookAt(float x, float y, float z);

        const dx::XMFLOAT3 *getCameraPos();
        const dx::XMFLOAT3 *getCameraLookAt();
        const dx::XMFLOAT3 *getCameraUp();

        const dx::XMFLOAT4X4 *getWorldMatrix();
        const dx::XMFLOAT4X4 *getViewMatrix();
        const dx::XMFLOAT4X4 *getProjectionMatrix();
        const dx::XMFLOAT4X4 *getOrthoMatrix();

        void getWVPMatrix(dx::XMFLOAT4X4 *wvp);
        void getWVOMatrix(dx::XMFLOAT4X4 *wvo);
    };

}

#endif  // SHELL_DIRECT3D_CAMERA_H_