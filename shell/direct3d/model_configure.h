#ifndef SHELL_DIRECT3D_MODEL_CONFIGURE_H_
#define SHELL_DIRECT3D_MODEL_CONFIGURE_H_

#include <Windows.h>

#include "ukive/utils/com_ptr.h"
#include "ukive/graphics/graphic_device_manager.h"

#include "shell/third_party/directx_math/Inc/DirectXMath.h"


namespace shell {

    namespace dx = DirectX;

    struct ModelVertexData
    {
        dx::XMFLOAT3 position;
        dx::XMFLOAT4 color;
        dx::XMFLOAT3 normal;
        dx::XMFLOAT2 texcoord;

        ModelVertexData()
        {
            position = dx::XMFLOAT3(0, 0, 0);
            color = dx::XMFLOAT4(0, 0, 0, 1);
            normal = dx::XMFLOAT3(0, 0, 0);
            texcoord = dx::XMFLOAT2(0, 0);
        }
    };


    class ModelConfigure
    {
    private:
        struct MatrixConstBuffer
        {
            dx::XMFLOAT4X4 mWVP;
        };

        struct PhongLightConstBuffer
        {
            dx::XMFLOAT4X4 mWVP;
        };

    private:
        ukive::ComPtr<ID3D11VertexShader> mVertexShader;
        ukive::ComPtr<ID3D11PixelShader> mPixelShader;
        ukive::ComPtr<ID3D11InputLayout> mInputLayout;

        ukive::ComPtr<ID3D11Buffer> mMatrixConstBuffer;
        ukive::ComPtr<ID3D11Buffer> mModelLightConstBuffer;

    public:
        ModelConfigure();
        ~ModelConfigure();

        HRESULT init();
        void active();
        void reset();
        void close();

        void setMatrix(dx::XMFLOAT4X4 matrix);
    };

}

#endif  // SHELL_DIRECT3D_MODEL_CONFIGURE_H_