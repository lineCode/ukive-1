#include "shadow_effect.h"

#include "ukive/application.h"
#include "ukive/log.h"
#include "ukive/graphics/direct3d/space.h"
#include "ukive/graphics/renderer.h"


namespace {

    float getWeight(float x, float sigma) {
        float exponent = -std::pow(x, 2) / (2 * std::pow(sigma, 2));
        return std::exp(exponent) / (std::sqrt(2 * 3.1416f) * sigma);
    }

}

namespace ukive {

    ShadowEffect::ShadowEffect(int radius)
        :width_(0),
        height_(0) {

        radius_ = radius;
        elevation_ = radius_ / 2.f;

        D3D11_INPUT_ELEMENT_DESC layout[2];

        layout[0].SemanticName = "POSITION";
        layout[0].SemanticIndex = 0;
        layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        layout[0].InputSlot = 0;
        layout[0].AlignedByteOffset = 0;
        layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        layout[0].InstanceDataStepRate = 0;

        layout[1].SemanticName = "TEXCOORD";
        layout[1].SemanticIndex = 0;
        layout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        layout[1].InputSlot = 0;
        layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        layout[1].InstanceDataStepRate = 0;

        ukive::string16 shader_path(::_wgetcwd(nullptr, 0));

        ukive::Space::createVertexShader(
            shader_path + L"\\shadow_effect_vs.cso",
            layout, ARRAYSIZE(layout), &vertex_shader_, &input_layout_);

        ukive::Space::createPixelShader(
            shader_path + L"\\shadow_effect_ps.cso", &pixel_shader_);

        const_buffer_ = ukive::Space::createConstantBuffer(sizeof(ConstBuffer));
        ps_const_buffer_ = ukive::Space::createConstantBuffer(sizeof(PSConstBuffer));

        D3D11_RASTERIZER_DESC rasterDesc;
        ZeroMemory(&rasterDesc, sizeof(rasterDesc));

        // 设置光栅化描述，指定多边形如何被渲染.
        rasterDesc.AntialiasedLineEnable = FALSE;
        rasterDesc.CullMode = D3D11_CULL_BACK;
        rasterDesc.DepthBias = 0;
        rasterDesc.DepthBiasClamp = 0.0f;
        rasterDesc.DepthClipEnable = TRUE;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterDesc.FrontCounterClockwise = FALSE;
        rasterDesc.MultisampleEnable = FALSE;
        rasterDesc.ScissorEnable = FALSE;
        rasterDesc.SlopeScaledDepthBias = 0.0f;

        auto device = Application::getGraphicDeviceManager()->getD3DDevice();

        // 创建光栅化状态.
        HRESULT hr = device->CreateRasterizerState(&rasterDesc, &rasterizer_state_);
        if (FAILED(hr)) {
            DCHECK(false);
            LOG(Log::WARNING) << "Failed to create rasterizer state: " << hr;
        }

        D3D11_SAMPLER_DESC samplerDesc;
        ZeroMemory(&samplerDesc, sizeof(samplerDesc));

        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        hr = device->CreateSamplerState(&samplerDesc, &sampler_state_);
        if (FAILED(hr)) {
            DCHECK(false);
            LOG(Log::WARNING) << "Failed to create sampler state: " << hr;
        }

        createKernelTexture();
    }


    void ShadowEffect::draw() {
        ukive::Space::setVertexShader(vertex_shader_.get());
        ukive::Space::setPixelShader(pixel_shader_.get());
        ukive::Space::setInputLayout(input_layout_.get());
        ukive::Space::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        auto d3d_dc = Application::getGraphicDeviceManager()->getD3DDeviceContext();

        d3d_dc->RSSetState(rasterizer_state_.get());
        d3d_dc->PSSetSamplers(0, 1, &sampler_state_);
        d3d_dc->RSSetViewports(1, &viewport_);

        {
            ID3D11ShaderResourceView* srvs[] = { bg_srv_.get(), kernel_srv_.get() };
            d3d_dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
            d3d_dc->OMSetRenderTargets(1, &shadow1_rtv_, nullptr);
        }

        // VS ConstBuffer
        auto resource = ukive::Space::lockResource(const_buffer_.get());
        (reinterpret_cast<ConstBuffer*>(resource.pData))->wvo = wvo_matrix_;
        ukive::Space::unlockResource(const_buffer_.get());

        ukive::Space::setConstantBuffers(0, 1, &const_buffer_);

        // PS ConstBuffer
        resource = ukive::Space::lockResource(ps_const_buffer_.get());
        (reinterpret_cast<PSConstBuffer*>(resource.pData))->vertical = 0;
        ukive::Space::unlockResource(ps_const_buffer_.get());

        d3d_dc->PSSetConstantBuffers(0, 1, &ps_const_buffer_);

        // Render
        UINT vertexDataOffset = 0;
        UINT vertexStructSize = sizeof(VertexData);
        d3d_dc->IASetVertexBuffers(0, 1, &vert_buffer_, &vertexStructSize, &vertexDataOffset);
        d3d_dc->IASetIndexBuffer(index_buffer_.get(), DXGI_FORMAT_R32_UINT, 0);
        d3d_dc->DrawIndexed(6, 0, 0);

        // PS ConstBuffer
        resource = ukive::Space::lockResource(ps_const_buffer_.get());
        (reinterpret_cast<PSConstBuffer*>(resource.pData))->vertical = 1;
        ukive::Space::unlockResource(ps_const_buffer_.get());

        {
            d3d_dc->OMSetRenderTargets(1, &shadow2_rtv_, nullptr);
            ID3D11ShaderResourceView* srvs[] = { shadow1_srv_.get(), kernel_srv_.get() };
            d3d_dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
        }

        // Render
        d3d_dc->DrawIndexed(6, 0, 0);
    }

    void ShadowEffect::setSize(int width, int height) {
        if (width_ == width && height_ == height) {
            return;
        }

        width_ = width;
        height_ = height;

        viewport_.Width = width_;
        viewport_.Height = height_;
        viewport_.MinDepth = 0.0f;
        viewport_.MaxDepth = 1.0f;
        viewport_.TopLeftX = 0;
        viewport_.TopLeftY = 0;

        VertexData vertices[] = {
            { dx::XMFLOAT3(0, height, 0), dx::XMFLOAT2(0, 0) },
            { dx::XMFLOAT3(width, height, 0), dx::XMFLOAT2(1, 0) },
            { dx::XMFLOAT3(width, 0, 0), dx::XMFLOAT2(1, 1) },
            { dx::XMFLOAT3(0, 0, 0), dx::XMFLOAT2(0, 1) },
        };

        vert_buffer_ = Space::createVertexBuffer(vertices, sizeof(VertexData), ARRAYSIZE(vertices));

        int indices[] = {0, 1, 2, 0, 2, 3};

        index_buffer_ = Space::createIndexBuffer(indices, ARRAYSIZE(indices));

        float pos_x = width_ / 2.f;
        float pos_y = height_ / 2.f;

        // 摄像机位置。
        auto pos = dx::XMFLOAT3(pos_x, pos_y, -2);
        // 摄像机看向的位置。
        auto look_at = dx::XMFLOAT3(pos_x, pos_y, 0);
        // 摄像机上向量
        auto up = dx::XMFLOAT3(0.0f, 1.0f, 0.0f);

        dx::XMStoreFloat4x4(&world_matrix_, dx::XMMatrixIdentity());
        dx::XMStoreFloat4x4(&view_matrix_, dx::XMMatrixLookAtLH(
            dx::XMLoadFloat3(&pos),
            dx::XMLoadFloat3(&look_at),
            dx::XMLoadFloat3(&up)));
        dx::XMStoreFloat4x4(&ortho_matrix_,
            dx::XMMatrixOrthographicLH(width_, height_, 1, 2));

        dx::XMMATRIX world = dx::XMLoadFloat4x4(&world_matrix_);
        dx::XMMATRIX view = dx::XMLoadFloat4x4(&view_matrix_);
        dx::XMMATRIX ortho = dx::XMLoadFloat4x4(&ortho_matrix_);

        dx::XMStoreFloat4x4(&wvo_matrix_,
            dx::XMMatrixMultiply(
                dx::XMMatrixMultiply(world, view), ortho));

        DirectX::XMStoreFloat4x4(&wvo_matrix_, DirectX::XMMatrixTranspose(
            DirectX::XMLoadFloat4x4(&wvo_matrix_)));

        createTexture(shadow1_tex2d_, shadow1_rtv_, shadow1_srv_);
        createTexture(shadow2_tex2d_, shadow2_rtv_, shadow2_srv_);
    }

    void ShadowEffect::setContent(ID3D11Texture2D* texture) {
        auto device = Application::getGraphicDeviceManager()->getD3DDevice();

        bg_srv_.reset();
        HRESULT hr = device->CreateShaderResourceView(texture, nullptr, &bg_srv_);
        if (FAILED(hr)) {
            DCHECK(false);
            LOG(Log::WARNING) << "Failed to create SRV: " << hr;
            return;
        }

        bg_rtv_.reset();
        hr = device->CreateRenderTargetView(texture, nullptr, &bg_rtv_);
        if (FAILED(hr)) {
            DCHECK(false);
            LOG(Log::WARNING) << "Failed to create RTV: " << hr;
            return;
        }
    }

    ComPtr<ID3D11Texture2D> ShadowEffect::getOutput() {
        return shadow2_tex2d_;
    }


    void ShadowEffect::createTexture(
        ComPtr<ID3D11Texture2D>& tex,
        ComPtr<ID3D11RenderTargetView>& rtv,
        ComPtr<ID3D11ShaderResourceView>& srv) {

        tex = Renderer::createTexture2D(width_, height_);
        auto device = Application::getGraphicDeviceManager()->getD3DDevice();

        srv.reset();
        HRESULT hr = device->CreateShaderResourceView(tex.get(), nullptr, &srv);
        if (FAILED(hr)) {
            DCHECK(false);
            LOG(Log::WARNING) << "Failed to create SRV: " << hr;
            return;
        }

        rtv.reset();
        hr = device->CreateRenderTargetView(tex.get(), nullptr, &rtv);
        if (FAILED(hr)) {
            DCHECK(false);
            LOG(Log::WARNING) << "Failed to create RTV: " << hr;
            return;
        }
    }

    void ShadowEffect::createKernelTexture() {
        float total_weight = 0;
        std::unique_ptr<float[]> weight_matrix(new float[radius_ + 1]());
        for (int i = 0; i < radius_ + 1; ++i) {
            float w = getWeight(radius_ - i, elevation_);
            weight_matrix[i] = w;
            if (i != radius_) {
                total_weight += w;
            }
        }

        total_weight *= 2;
        total_weight += weight_matrix[radius_];
        for (int i = 0; i < radius_ + 1; ++i) {
            weight_matrix[i] /= total_weight;
        }

        auto d3d_device = Application::getGraphicDeviceManager()->getD3DDevice();

        D3D11_TEXTURE2D_DESC tex_desc = { 0 };
        tex_desc.ArraySize = 1;
        tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        tex_desc.Format = DXGI_FORMAT_R32_FLOAT;
        tex_desc.Width = radius_ + 1;
        tex_desc.Height = 1;
        tex_desc.MipLevels = 1;
        tex_desc.SampleDesc.Count = 1;
        tex_desc.SampleDesc.Quality = 0;
        tex_desc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = weight_matrix.get();
        data.SysMemPitch = sizeof(float) * (radius_ + 1);
        data.SysMemSlicePitch = 0;

        HRESULT hr = d3d_device->CreateTexture2D(&tex_desc, &data, &kernel_tex2d_);
        if (FAILED(hr)) {
            DCHECK(false);
            LOG(Log::WARNING) << "Failed to create 2d texture: " << hr;
            return;
        }

        hr = d3d_device->CreateShaderResourceView(kernel_tex2d_.get(), nullptr, &kernel_srv_);
        if (FAILED(hr)) {
            DCHECK(false);
            LOG(Log::WARNING) << "Failed to create SRV: " << hr;
            return;
        }
    }
}