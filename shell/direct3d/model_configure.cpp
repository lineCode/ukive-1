#include "model_configure.h"

#include "ukive/application.h"
#include "ukive/graphics/direct3d/space.h"
#include "ukive/utils/string_utils.h"


namespace shell {

    ModelConfigure::ModelConfigure() {}

    ModelConfigure::~ModelConfigure() {}


    void ModelConfigure::init() {
        D3D11_INPUT_ELEMENT_DESC layout[4];

        layout[0].SemanticName = "POSITION";
        layout[0].SemanticIndex = 0;
        layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        layout[0].InputSlot = 0;
        layout[0].AlignedByteOffset = 0;
        layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        layout[0].InstanceDataStepRate = 0;

        layout[1].SemanticName = "COLOR";
        layout[1].SemanticIndex = 0;
        layout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        layout[1].InputSlot = 0;
        layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        layout[1].InstanceDataStepRate = 0;

        layout[2].SemanticName = "NORMAL";
        layout[2].SemanticIndex = 0;
        layout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        layout[2].InputSlot = 0;
        layout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        layout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        layout[2].InstanceDataStepRate = 0;

        layout[3].SemanticName = "TEXCOORD";
        layout[3].SemanticIndex = 0;
        layout[3].Format = DXGI_FORMAT_R32G32_FLOAT;
        layout[3].InputSlot = 0;
        layout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        layout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        layout[3].InstanceDataStepRate = 0;

        ukive::string16 shader_path = ukive::Application::getExecFileName(true);

        ukive::Space::createVertexShader(
            shader_path + L"\\shaders\\model_vertex_shader.cso",
            layout, ARRAYSIZE(layout), &vertex_shader_, &input_layout_);

        ukive::Space::createPixelShader(
            shader_path + L"\\shaders\\model_pixel_shader.cso",
            &pixel_shader_);

        const_buffer_ = ukive::Space::createConstantBuffer(sizeof(MatrixConstBuffer));
        light_const_buffer_ = ukive::Space::createConstantBuffer(sizeof(PhongLightConstBuffer));
    }

    void ModelConfigure::active() {
        ukive::Space::setVertexShader(vertex_shader_.get());
        ukive::Space::setPixelShader(pixel_shader_.get());
        ukive::Space::setInputLayout(input_layout_.get());
    }

    void ModelConfigure::close() {}

    void ModelConfigure::setMatrix(const dx::XMFLOAT4X4& matrix) {
        auto resource = ukive::Space::lockResource(const_buffer_.get());
        (reinterpret_cast<MatrixConstBuffer*>(resource.pData))->wvp = matrix;
        ukive::Space::unlockResource(const_buffer_.get());

        ukive::Space::setConstantBuffers(0, 1, &const_buffer_);
    }

}
