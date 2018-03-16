#include "assist_configure.h"

#include "ukive/graphics/direct3d/space.h"
#include "ukive/utils/string_utils.h"
#include "ukive/utils/hresult_utils.h"


namespace shell {

    AssistConfigure::AssistConfigure() {
    }

    AssistConfigure::~AssistConfigure() {
    }

    HRESULT AssistConfigure::init() {
        UINT numElements;
        D3D11_INPUT_ELEMENT_DESC polygonLayout[3];

        polygonLayout[0].SemanticName = "POSITION";
        polygonLayout[0].SemanticIndex = 0;
        polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        polygonLayout[0].InputSlot = 0;
        polygonLayout[0].AlignedByteOffset = 0;
        polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        polygonLayout[0].InstanceDataStepRate = 0;

        polygonLayout[1].SemanticName = "COLOR";
        polygonLayout[1].SemanticIndex = 0;
        polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        polygonLayout[1].InputSlot = 0;
        polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        polygonLayout[1].InstanceDataStepRate = 0;

        polygonLayout[2].SemanticName = "TEXCOORD";
        polygonLayout[2].SemanticIndex = 0;
        polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
        polygonLayout[2].InputSlot = 0;
        polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        polygonLayout[2].InstanceDataStepRate = 0;

        numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

        ukive::string16 shaderFileName(::_wgetcwd(nullptr, 0));

        RH(ukive::Space::createVertexShader(
            shaderFileName + L"\\assist_vertex_shader.cso",
            polygonLayout, numElements, &mVertexShader, &mInputLayout));

        RH(ukive::Space::createPixelShader(
            shaderFileName + L"\\assist_pixel_shader.cso",
            &mPixelShader));

        RH(ukive::Space::createConstantBuffer(
            sizeof(AssistConstBuffer), &mAssistConstBuffer));

        return S_OK;
    }

    void AssistConfigure::active() {
        ukive::Space::setVertexShader(mVertexShader.get());
        ukive::Space::setPixelShader(mPixelShader.get());
        ukive::Space::setInputLayout(mInputLayout.get());
    }

    void AssistConfigure::reset() {
        ukive::Space::setVertexShader(mVertexShader.get());
        ukive::Space::setPixelShader(mPixelShader.get());
        ukive::Space::setInputLayout(mInputLayout.get());
    }

    void AssistConfigure::close() {
    }

    void AssistConfigure::setMatrix(dx::XMFLOAT4X4 matrix) {
        D3D11_MAPPED_SUBRESOURCE resource;

        resource = ukive::Space::lockResource(mAssistConstBuffer.get());
        ((AssistConstBuffer*)resource.pData)->mWVP = matrix;
        ukive::Space::unlockResource(mAssistConstBuffer.get());

        ukive::Space::setConstantBuffers(0, 1, &mAssistConstBuffer);
    }

}