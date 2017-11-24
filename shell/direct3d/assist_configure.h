#ifndef SHELL_DIRECT3D_ASSIST_CONFIGURE_H_
#define SHELL_DIRECT3D_ASSIST_CONFIGURE_H_

#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/utils/com_ptr.h"

#include "shell/third_party/directx_math/Inc/DirectXMath.h"


namespace shell {

    namespace dx = DirectX;

    struct AssistVertexData
    {
        dx::XMFLOAT3 position;
        dx::XMFLOAT4 color;
        dx::XMFLOAT2 texcoord;

        AssistVertexData()
        {
            position = dx::XMFLOAT3(0, 0, 0);
            color = dx::XMFLOAT4(0, 0, 0, 1);
            texcoord = dx::XMFLOAT2(0, 0);
        }
    };


    class AssistConfigure
    {
    private:
        ukive::ComPtr<ID3D11VertexShader> mVertexShader;
        ukive::ComPtr<ID3D11PixelShader> mPixelShader;
        ukive::ComPtr<ID3D11InputLayout> mInputLayout;
        ukive::ComPtr<ID3D11Buffer> mAssistConstBuffer;

    public:
        AssistConfigure();
        ~AssistConfigure();

        HRESULT init();
        void active();
        void reset();
        void close();

        void setMatrix(dx::XMFLOAT4X4 matrix);

    private:
        struct AssistConstBuffer {
            dx::XMFLOAT4X4 mWVP;
        };
    };

}

#endif  // SHELL_DIRECT3D_ASSIST_CONFIGURE_H_