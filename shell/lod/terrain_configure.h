#ifndef SHELL_LOD_TERRAIN_CONFIGURE_H_
#define SHELL_LOD_TERRAIN_CONFIGURE_H_

#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/utils/com_ptr.h"

#include "shell/third_party/directx_math/Inc/DirectXMath.h"


namespace shell {

    namespace dx = DirectX;

    struct TerrainVertexData {
        dx::XMFLOAT3 position;
        dx::XMFLOAT2 texcoord;

        TerrainVertexData()
            :position(dx::XMFLOAT3(0, 0, 0)),
            texcoord(dx::XMFLOAT2(0, 0)) {}
    };


    class TerrainConfigure {
    public:
        TerrainConfigure();
        ~TerrainConfigure();

        void init();
        void active();
        void close();

        void setMatrix(dx::XMFLOAT4X4 matrix);

    private:
        struct MatrixConstBuffer {
            dx::XMFLOAT4X4 wvp;
        };

        ukive::ComPtr<ID3D11Buffer> const_buffer_;
        ukive::ComPtr<ID3D11InputLayout> input_layout_;
        ukive::ComPtr<ID3D11PixelShader> pixel_shader_;
        ukive::ComPtr<ID3D11VertexShader> vertex_shader_;
    };

}

#endif  // SHELL_LOD_TERRAIN_CONFIGURE_H_