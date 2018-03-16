#include "terrain_configure.h"

#include "ukive/graphics/direct3d/space.h"
#include "ukive/utils/string_utils.h"
#include "ukive/utils/hresult_utils.h"


namespace shell {

    TerrainConfigure::TerrainConfigure()
    {
    }

    TerrainConfigure::~TerrainConfigure()
    {
    }


    HRESULT TerrainConfigure::init()
    {
        UINT numElements;
        D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

        polygonLayout[0].SemanticName = "POSITION";
        polygonLayout[0].SemanticIndex = 0;
        polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        polygonLayout[0].InputSlot = 0;
        polygonLayout[0].AlignedByteOffset = 0;
        polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        polygonLayout[0].InstanceDataStepRate = 0;

        polygonLayout[1].SemanticName = "TEXCOORD";
        polygonLayout[1].SemanticIndex = 0;
        polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        polygonLayout[1].InputSlot = 0;
        polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        polygonLayout[1].InstanceDataStepRate = 0;

        numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

        ukive::string16 shaderFileName(::_wgetcwd(nullptr, 0));

        RH(ukive::Space::createVertexShader(
            shaderFileName + L"\\terrain_vertex_shader.cso",
            polygonLayout, numElements, &mVertexShader, &mInputLayout));

        RH(ukive::Space::createPixelShader(
            shaderFileName + L"\\terrain_pixel_shader.cso",
            &mPixelShader));

        RH(ukive::Space::createConstantBuffer(
            sizeof(MatrixConstBuffer), &mMatrixConstBuffer));

        return S_OK;
    }

    void TerrainConfigure::active()
    {
        ukive::Space::setVertexShader(mVertexShader.get());
        ukive::Space::setPixelShader(mPixelShader.get());
        ukive::Space::setInputLayout(mInputLayout.get());
    }

    void TerrainConfigure::reset()
    {
        ukive::Space::setVertexShader(mVertexShader.get());
        ukive::Space::setPixelShader(mPixelShader.get());
        ukive::Space::setInputLayout(mInputLayout.get());
    }

    void TerrainConfigure::close()
    {
    }

    void TerrainConfigure::setMatrix(dx::XMFLOAT4X4 matrix)
    {
        D3D11_MAPPED_SUBRESOURCE resource;

        resource = ukive::Space::lockResource(mMatrixConstBuffer.get());
        ((MatrixConstBuffer*)resource.pData)->mWVP = matrix;
        ukive::Space::unlockResource(mMatrixConstBuffer.get());

        ukive::Space::setConstantBuffers(0, 1, &mMatrixConstBuffer);
    }

}