#include "direct3d_view.h"

#include <algorithm>

#include "ukive/log.h"
#include "ukive/application.h"
#include "ukive/graphics/renderer.h"
#include "ukive/window/window.h"
#include "ukive/utils/hresult_utils.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/bitmap.h"
#include "ukive/graphics/direct3d/scene.h"


namespace ukive {

    Direct3DView::Direct3DView(Window* w, Scene* scene)
        :View(w),
        scene_(scene) {

        setFocusable(true);
        setMinimumWidth(1);
        setMinimumHeight(1);

        getWindow()->getRenderer()->addSwapChainResizeNotifier(this);
        scene_->onSceneCreate(this);
    }

    Direct3DView::~Direct3DView() {
        scene_->onSceneDestroy();

        releaseState();
        releaseResourceView();

        getWindow()->getRenderer()->removeSwapChainResizeNotifier(this);
    }


    void Direct3DView::createState() {
        auto d3d_device = Application::getGraphicDeviceManager()->getD3DDevice();
        auto d3d_dc = Application::getGraphicDeviceManager()->getD3DDeviceContext();

        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

        // 设置深度模版状态描述
        depthStencilDesc.DepthEnable = TRUE;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

        // D3D11_DEPTH_WRITE_MASK_ZERO 禁止写深度缓冲
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
        depthStencilDesc.StencilEnable = TRUE;
        depthStencilDesc.StencilReadMask = 0xFF;
        depthStencilDesc.StencilWriteMask = 0xFF;

        // 对于front face 像素使用的模版操作操作
        depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // 对于back face像素使用的模版操作模式
        depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // 创建深度模版状态
        HRESULT hr = d3d_device->CreateDepthStencilState(&depthStencilDesc, &depth_stencil_state_);
        if (FAILED(hr)) {
            DCHECK(false);
            LOG(Log::WARNING) << "Failed to create depth stencil state: " << hr;
        }

        D3D11_RASTERIZER_DESC rasterDesc;
        ZeroMemory(&rasterDesc, sizeof(rasterDesc));

        // 设置光栅化描述，指定多边形如何被渲染.
        rasterDesc.AntialiasedLineEnable = FALSE;
        rasterDesc.CullMode = D3D11_CULL_BACK;
        rasterDesc.DepthBias = 0;
        rasterDesc.DepthBiasClamp = 0.0f;
        rasterDesc.DepthClipEnable = TRUE;
        rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
        rasterDesc.FrontCounterClockwise = FALSE;
        rasterDesc.MultisampleEnable = FALSE;
        rasterDesc.ScissorEnable = FALSE;
        rasterDesc.SlopeScaledDepthBias = 0.0f;

        // 创建光栅化状态.
        hr = d3d_device->CreateRasterizerState(&rasterDesc, &rasterizer_state_);
        if (FAILED(hr)) {
            DCHECK(false);
            LOG(Log::WARNING) << "Failed to create rasterizer state: " << hr;
        }

        D3D11_SAMPLER_DESC samplerDesc;
        ZeroMemory(&samplerDesc, sizeof(samplerDesc));

        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        hr = d3d_device->CreateSamplerState(&samplerDesc, &sampler_state_);
        if (FAILED(hr)) {
            DCHECK(false);
            LOG(Log::WARNING) << "Failed to create sampler state: " << hr;
        }
    }

    void Direct3DView::releaseState() {
        depth_stencil_state_.reset();
        rasterizer_state_.reset();
        sampler_state_.reset();
    }


    void Direct3DView::setViewports(
        float x, float y, float width, float height) {

        viewport_.Width = width;
        viewport_.Height = height;
        viewport_.MinDepth = 0.0f;
        viewport_.MaxDepth = 1.0f;
        viewport_.TopLeftX = x;
        viewport_.TopLeftY = y;
    }


    void Direct3DView::createResourceView(int width, int height) {
        if (width <= 0 || height <= 0) {
            return;
        }

        auto d3d_device = Application::getGraphicDeviceManager()->getD3DDevice();
        auto d3d_dc = Application::getGraphicDeviceManager()->getD3DDeviceContext();
        auto d2d_dc = getWindow()->getRenderer()->getD2DDeviceContext();

        // RTT
        D3D11_TEXTURE2D_DESC rtt_tex2d_desc;
        ZeroMemory(&rtt_tex2d_desc, sizeof(rtt_tex2d_desc));

        rtt_tex2d_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        rtt_tex2d_desc.Width = width;
        rtt_tex2d_desc.Height = height;
        rtt_tex2d_desc.ArraySize = 1;
        rtt_tex2d_desc.MipLevels = 1;  // 为 0 时，无法 query 出 IDXGISurface.
        rtt_tex2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        rtt_tex2d_desc.Usage = D3D11_USAGE_DEFAULT;
        rtt_tex2d_desc.CPUAccessFlags = 0;
        rtt_tex2d_desc.SampleDesc.Count = 1;
        rtt_tex2d_desc.SampleDesc.Quality = 0;
        rtt_tex2d_desc.MiscFlags = 0;

        HRESULT hr = d3d_device->CreateTexture2D(
            &rtt_tex2d_desc, nullptr, &d3d_content_surface_);
        if (FAILED(hr)) {
            DCHECK(false);
            LOG(Log::WARNING) << "Failed to create 2d texture: " << hr;
            return;
        }

        hr = d3d_device->CreateRenderTargetView(
            d3d_content_surface_.get(), nullptr, &render_target_view_);
        if (FAILED(hr)) {
            DCHECK(false);
            LOG(Log::WARNING) << "Failed to create render target view: " << hr;
            return;
        }

        auto dxgi_surface = d3d_content_surface_.cast<IDXGISurface>();
        if (!dxgi_surface) {
            DCHECK(false);
            LOG(Log::WARNING) << "Failed to query DXGI surface.";
            return;
        }

        D2D1_BITMAP_PROPERTIES bmp_prop = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

        hr = d2d_dc->CreateSharedBitmap(
            __uuidof(IDXGISurface), dxgi_surface.get(), &bmp_prop, &d3d_content_);
        if (FAILED(hr)) {
            DCHECK(false);
            LOG(Log::WARNING) << "Failed to create shared bitmap: " << hr;
            return;
        }

        // Depth buffer
        D3D11_TEXTURE2D_DESC db_tex2d_desc;
        ZeroMemory(&db_tex2d_desc, sizeof(db_tex2d_desc));

        db_tex2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        db_tex2d_desc.Width = width;
        db_tex2d_desc.Height = height;
        db_tex2d_desc.ArraySize = 1;
        db_tex2d_desc.MipLevels = 1;
        db_tex2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        db_tex2d_desc.Usage = D3D11_USAGE_DEFAULT;
        db_tex2d_desc.CPUAccessFlags = 0;
        db_tex2d_desc.SampleDesc.Count = 1;
        db_tex2d_desc.SampleDesc.Quality = 0;
        db_tex2d_desc.MiscFlags = 0;

        hr = d3d_device->CreateTexture2D(&db_tex2d_desc, 0, &depth_stencil_buffer_);
        if (FAILED(hr)) {
            DCHECK(false);
            LOG(Log::WARNING) << "Failed to create 2d texture: " << hr;
            return;
        }

        // 设置深度模版视图描述
        D3D11_DEPTH_STENCIL_VIEW_DESC ds_view_desc;
        ZeroMemory(&ds_view_desc, sizeof(ds_view_desc));

        ds_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        ds_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        ds_view_desc.Flags = 0;
        ds_view_desc.Texture2D.MipSlice = 0;

        hr = d3d_device->CreateDepthStencilView(
            depth_stencil_buffer_.get(), &ds_view_desc, &depth_stencil_view_);
        if (FAILED(hr)) {
            DCHECK(false);
            LOG(Log::WARNING) << "Failed to create depth stencil view: " << hr;
            return;
        }

        // std::wstring ddsFileName(::_wgetcwd(nullptr, 0));
        // RH(DirectX::CreateDDSTextureFromFile(
        // d3d_device.get(), ddsFileName + L"\\top.dds", 0, &shader_resource_view_));
    }

    void Direct3DView::releaseResourceView() {
        d3d_content_.reset();
        d3d_content_surface_.reset();
        depth_stencil_buffer_.reset();
        render_target_view_.reset();
        depth_stencil_view_.reset();
        shader_resource_view_.reset();
    }


    void Direct3DView::onPreSwapChainResize() {
        releaseState();
        releaseResourceView();
    }

    void Direct3DView::onPostSwapChainResize() {
        createState();
    }


    void Direct3DView::onMeasure(
        int width, int height, int width_spec, int height_spec) {

        int final_width = 0;
        int final_height = 0;

        switch (width_spec) {
        case FIT:
        case EXACTLY:
            final_width = std::max(width, getMinimumWidth());
            break;
        case UNKNOWN:
            final_width = std::max(0, getMinimumWidth());
            break;
        }

        switch (height_spec) {
        case FIT:
        case EXACTLY:
            final_height = std::max(height, getMinimumHeight());
            break;
        case UNKNOWN:
            final_height = std::max(0, getMinimumHeight());
            break;
        }

        setMeasuredDimension(final_width, final_height);
    }

    void Direct3DView::onLayout(
        bool changed, bool size_changed,
        int left, int top, int right, int bottom) {

        int width = right - left - getPaddingLeft() - getPaddingRight();
        int height = bottom - top - getPaddingTop() - getPaddingBottom();

        if (d3d_content_) {
            releaseResourceView();
        }

        createResourceView(width, height);

        if (size_changed) {
            setViewports(0, 0, width, height);
            scene_->onSceneResize(width, height);
        }
    }

    void Direct3DView::onDraw(Canvas* canvas) {
        View::onDraw(canvas);

        if (d3d_content_) {
            auto d3d_dc = Application::getGraphicDeviceManager()->getD3DDeviceContext();

            //d3d_dc->OMSetDepthStencilState(depth_stencil_state_.get(), 1);
            d3d_dc->RSSetState(rasterizer_state_.get());
            //d3d_dc->PSSetSamplers(0, 1, &sampler_state_);

            d3d_dc->OMSetRenderTargets(1, &render_target_view_, nullptr);
            // d3d_dc->PSSetShaderResources(0, 1, &shader_resource_view_);

            d3d_dc->RSSetViewports(1, &viewport_);

            Color d2d_color = getWindow()->getBackgroundColor();
            float bg_color[4] = { d2d_color.r, d2d_color.g, d2d_color.b, d2d_color.a };

            d3d_dc->ClearRenderTargetView(render_target_view_.get(), bg_color);
            //d3d_dc->ClearDepthStencilView(depth_stencil_view_.get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

            scene_->onSceneRender();

            Bitmap bmp(d3d_content_);
            canvas->drawBitmap(&bmp);
        }
    }

    bool Direct3DView::onInputEvent(InputEvent* e) {
        scene_->onSceneInput(e);
        return View::onInputEvent(e);
    }

    void Direct3DView::onSizeChanged(
        int width, int height, int old_width, int old_height) {
        View::onSizeChanged(width, height, old_width, old_height);
    }

}