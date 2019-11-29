#ifndef SHELL_DIRECT3D_MODEL_CONFIGURE_H_
#define SHELL_DIRECT3D_MODEL_CONFIGURE_H_

#include "ukive/system/com_ptr.hpp"
#include "ukive/graphics/graphic_device_manager.h"

#include "shell/third_party/directx_math/Inc/DirectXMath.h"


namespace shell {

    namespace dx = DirectX;

    struct ModelVertexData {
        dx::XMFLOAT3 position;
        dx::XMFLOAT4 color;
        dx::XMFLOAT3 normal;
        dx::XMFLOAT2 texcoord;

        ModelVertexData()
            :position(dx::XMFLOAT3(0, 0, 0)),
            color(dx::XMFLOAT4(0, 0, 0, 1)),
            normal(dx::XMFLOAT3(0, 0, 0)),
            texcoord(dx::XMFLOAT2(0, 0)) {}
    };


    class ModelConfigure {
    public:
        ModelConfigure();
        ~ModelConfigure();

        void init();
        void active();
        void close();

        void setMatrix(const dx::XMFLOAT4X4& matrix);

    private:
        struct MatrixConstBuffer {
            dx::XMFLOAT4X4 wvp;
        };

        struct PhongLightConstBuffer {
            dx::XMFLOAT4X4 wvp;
        };

        ukive::ComPtr<ID3D11InputLayout> input_layout_;
        ukive::ComPtr<ID3D11PixelShader> pixel_shader_;
        ukive::ComPtr<ID3D11VertexShader> vertex_shader_;

        ukive::ComPtr<ID3D11Buffer> const_buffer_;
        ukive::ComPtr<ID3D11Buffer> light_const_buffer_;
    };

}

#endif  // SHELL_DIRECT3D_MODEL_CONFIGURE_H_