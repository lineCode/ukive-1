#ifndef SHELL_LOD_TERRAIN_CONFIGURE_H_
#define SHELL_LOD_TERRAIN_CONFIGURE_H_

#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/utils/com_ptr.h"

#include "shell/third_party/directx_math/Inc/DirectXMath.h"


namespace shell {

    namespace dx = DirectX;

    struct TerrainVertexData
    {
        dx::XMFLOAT3 position;
        dx::XMFLOAT2 texcoord;

        TerrainVertexData()
        {
            position = dx::XMFLOAT3(0, 0, 0);
            texcoord = dx::XMFLOAT2(0, 0);
        }
    };


    class TerrainConfigure
    {
    private:
        struct MatrixConstBuffer
        {
            dx::XMFLOAT4X4 mWVP;
        };

    private:
        ukive::ComPtr<ID3D11VertexShader> mVertexShader;
        ukive::ComPtr<ID3D11PixelShader> mPixelShader;
        ukive::ComPtr<ID3D11InputLayout> mInputLayout;

        ukive::ComPtr<ID3D11Buffer> mMatrixConstBuffer;

    public:
        TerrainConfigure();
        ~TerrainConfigure();

        HRESULT init();
        void active();
        void reset();
        void close();

        void setMatrix(dx::XMFLOAT4X4 matrix);
    };

}

#endif  // SHELL_LOD_TERRAIN_CONFIGURE_H_