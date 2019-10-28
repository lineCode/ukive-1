#include "shadow_effect.h"

#include "ukive/application.h"
#include "ukive/log.h"
#include "ukive/graphics/direct3d/space.h"
#include "ukive/graphics/renderer.h"
#include "ukive/window/window.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/bitmap.h"


namespace {

    float getWeight(float x, float sigma) {
        float exponent = -std::pow(x, 2) / (2 * std::pow(sigma, 2));
        return std::exp(exponent) / (std::sqrt(2 * 3.1416f) * sigma);
    }

}

namespace ukive {

    ShadowEffect::ShadowEffect()
        : width_(0),
          height_(0),
          view_width_(0),
          view_height_(0),
          radius_(0),
          elevation_(0.f),
          wvo_matrix_(),
          world_matrix_(),
          view_matrix_(),
          ortho_matrix_(),
          viewport_()
    {
        D3D11_INPUT_ELEMENT_DESC layout[1];

        layout[0].SemanticName = "POSITION";
        layout[0].SemanticIndex = 0;
        layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        layout[0].InputSlot = 0;
        layout[0].AlignedByteOffset = 0;
        layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        layout[0].InstanceDataStepRate = 0;

        string16 shader_path = Application::getExecFileName(true);

        Space::createVertexShader(
            shader_path + L"\\shaders\\shadow_effect_vs.cso",
            layout, ARRAYSIZE(layout), &vertex_shader_, &input_layout_);

        Space::createPixelShader(
            shader_path + L"\\shaders\\shadow_effect_ps.cso", &pixel_shader_);

        const_buffer_ = Space::createConstantBuffer(sizeof(ConstBuffer));
        ps_const_buffer_ = Space::createConstantBuffer(sizeof(PSConstBuffer));

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
            LOG(Log::WARNING) << "Failed to create rasterizer state: " << hr;
        }
    }

    void ShadowEffect::draw() {
        Space::setVertexShader(vertex_shader_.get());
        Space::setPixelShader(pixel_shader_.get());
        Space::setInputLayout(input_layout_.get());
        Space::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        auto d3d_dc = Application::getGraphicDeviceManager()->getD3DDeviceContext();

        d3d_dc->RSSetState(rasterizer_state_.get());
        d3d_dc->RSSetViewports(1, &viewport_);

        {
            ID3D11ShaderResourceView* srvs[] = { bg_srv_.get(), kernel_srv_.get() };
            d3d_dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
            d3d_dc->OMSetRenderTargets(1, &shadow1_rtv_, nullptr);
        }

        // VS ConstBuffer
        auto resource = Space::lockResource(const_buffer_.get());
        (reinterpret_cast<ConstBuffer*>(resource.pData))->wvo = wvo_matrix_;
        Space::unlockResource(const_buffer_.get());

        Space::setConstantBuffers(0, 1, &const_buffer_);

        // PS ConstBuffer
        resource = ukive::Space::lockResource(ps_const_buffer_.get());
        (reinterpret_cast<PSConstBuffer*>(resource.pData))->vertical = 0;
        Space::unlockResource(ps_const_buffer_.get());

        d3d_dc->PSSetConstantBuffers(0, 1, &ps_const_buffer_);

        // Render
        FLOAT transparent[4] = {0, 0, 0, 0};
        d3d_dc->ClearRenderTargetView(shadow1_rtv_.get(), transparent);

        UINT vertexDataOffset = 0;
        UINT vertexStructSize = sizeof(VertexData);
        d3d_dc->IASetVertexBuffers(0, 1, &vert_buffer_, &vertexStructSize, &vertexDataOffset);
        d3d_dc->IASetIndexBuffer(index_buffer_.get(), DXGI_FORMAT_R32_UINT, 0);
        d3d_dc->DrawIndexed(6, 0, 0);

        // PS ConstBuffer
        resource = Space::lockResource(ps_const_buffer_.get());
        (reinterpret_cast<PSConstBuffer*>(resource.pData))->vertical = 1;
        Space::unlockResource(ps_const_buffer_.get());

        {
            d3d_dc->OMSetRenderTargets(1, &shadow2_rtv_, nullptr);
            ID3D11ShaderResourceView* srvs[] = { shadow1_srv_.get(), kernel_srv_.get() };
            d3d_dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
        }

        // Render
        d3d_dc->ClearRenderTargetView(shadow2_rtv_.get(), transparent);
        d3d_dc->DrawIndexed(6, 0, 0);
    }

    void ShadowEffect::draw(Canvas* c) {
        draw();
        auto shadow_bmp = getOutput(c->getRT());

        c->save();
        c->translate(-std::floor(elevation_ * 2), -std::floor(elevation_ * 2));
        c->drawBitmap(c->getOpacity(), &Bitmap(shadow_bmp));
        c->restore();
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

    void ShadowEffect::setRadius(int radius) {
        if (radius == radius_ || radius <= 0) {
            return;
        }

        radius_ = radius;
        elevation_ = radius_ / 2.f;

        createKernelTexture();
    }

    void ShadowEffect::setContent(ID3D11Texture2D* texture) {
        D3D11_TEXTURE2D_DESC desc;
        texture->GetDesc(&desc);
        view_width_ = desc.Width;
        view_height_ = desc.Height;

        auto device = Application::getGraphicDeviceManager()->getD3DDevice();

        bg_srv_.reset();
        HRESULT hr = device->CreateShaderResourceView(texture, nullptr, &bg_srv_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create SRV: " << hr;
            return;
        }

        bg_rtv_.reset();
        hr = device->CreateRenderTargetView(texture, nullptr, &bg_rtv_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create RTV: " << hr;
            return;
        }

        setSize(view_width_ + radius_ * 2, view_height_ + radius_ * 2);
    }

    int ShadowEffect::getRadius() const {
        return radius_;
    }

    ComPtr<ID2D1Bitmap> ShadowEffect::getOutput(ID2D1RenderTarget* rt) {
        D2D1_BITMAP_PROPERTIES bmp_prop = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

        ComPtr<ID2D1Bitmap> bitmap;
        HRESULT hr = rt->CreateSharedBitmap(
            __uuidof(IDXGISurface), shadow2_tex2d_.cast<IDXGISurface>().get(), &bmp_prop, &bitmap);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create shared bitmap: " << hr;
            return {};
        }

        return bitmap;
    }


    void ShadowEffect::createTexture(
        ComPtr<ID3D11Texture2D>& tex,
        ComPtr<ID3D11RenderTargetView>& rtv,
        ComPtr<ID3D11ShaderResourceView>& srv)
    {
        tex = Renderer::createTexture2D(width_, height_);
        auto device = Application::getGraphicDeviceManager()->getD3DDevice();

        srv.reset();
        HRESULT hr = device->CreateShaderResourceView(tex.get(), nullptr, &srv);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create SRV: " << hr;
            return;
        }

        rtv.reset();
        hr = device->CreateRenderTargetView(tex.get(), nullptr, &rtv);
        if (FAILED(hr)) {
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
            LOG(Log::WARNING) << "Failed to create 2d texture: " << hr;
            return;
        }

        hr = d3d_device->CreateShaderResourceView(kernel_tex2d_.get(), nullptr, &kernel_srv_);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create SRV: " << hr;
            return;
        }
    }
}