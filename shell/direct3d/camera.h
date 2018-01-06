#ifndef SHELL_DIRECT3D_CAMERA_H_
#define SHELL_DIRECT3D_CAMERA_H_

#define NEAR_PLANE 10.0f
#define FAR_PLANE 100000.0f

#include "shell/third_party/directx_math/Inc/DirectXMath.h"


namespace shell {

    namespace dx = DirectX;

    class Camera {
    public:
        Camera();
        ~Camera();

        void init(unsigned int width, unsigned int height);
        void close();
        void resize(unsigned int width, unsigned int height);

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

        const dx::XMFLOAT3* getCameraPos();
        const dx::XMFLOAT3* getCameraLookAt();
        const dx::XMFLOAT3* getCameraUp();

        const dx::XMFLOAT4X4* getWorldMatrix();
        const dx::XMFLOAT4X4* getViewMatrix();
        const dx::XMFLOAT4X4* getProjectionMatrix();
        const dx::XMFLOAT4X4* getOrthoMatrix();

        void getWVPMatrix(dx::XMFLOAT4X4* wvp);
        void getWVOMatrix(dx::XMFLOAT4X4* wvo);

    private:
        dx::XMFLOAT3 pos_;
        dx::XMFLOAT3 look_at_;
        dx::XMFLOAT3 up_;
        dx::XMFLOAT3 right_;
        dx::XMFLOAT3 look_;
        dx::XMFLOAT3 z_vector_;
        dx::XMFLOAT3 y_vector_;

        dx::XMFLOAT4X4 world_matrix_;
        dx::XMFLOAT4X4 view_matrix_;
        dx::XMFLOAT4X4 projection_matrix_;
        dx::XMFLOAT4X4 ortho_matrix_;

        float radius_;
        unsigned int width_;
        unsigned int height_;
    };

}

#endif  // SHELL_DIRECT3D_CAMERA_H_