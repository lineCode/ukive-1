#ifndef SHELL_DIRECT3D_ASSIST_CONFIGURE_H_
#define SHELL_DIRECT3D_ASSIST_CONFIGURE_H_

#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/system/com_ptr.hpp"

#include "shell/third_party/directx_math/Inc/DirectXMath.h"


namespace shell {

    namespace dx = DirectX;

    struct AssistVertexData {
        dx::XMFLOAT3 position;
        dx::XMFLOAT4 color;
        dx::XMFLOAT2 texcoord;

        AssistVertexData()
            :position(dx::XMFLOAT3(0, 0, 0)),
            color(dx::XMFLOAT4(0, 0, 0, 1)),
            texcoord(dx::XMFLOAT2(0, 0)) {}
    };


    class AssistConfigure {
    public:
        AssistConfigure();
        ~AssistConfigure();

        void init();
        void active();
        void close();

        void setMatrix(const dx::XMFLOAT4X4& matrix);

    private:
        struct AssistConstBuffer {
            dx::XMFLOAT4X4 wvp;
        };

        ukive::ComPtr<ID3D11Buffer> const_buffer_;
        ukive::ComPtr<ID3D11InputLayout> input_layout_;
        ukive::ComPtr<ID3D11PixelShader> pixel_shader_;
        ukive::ComPtr<ID3D11VertexShader> vertex_shader_;
    };

}

#endif  // SHELL_DIRECT3D_ASSIST_CONFIGURE_H_