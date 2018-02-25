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

    Direct3DView::Direct3DView(Window *w, Scene* scene)
        :View(w),
        scene_(scene) {
        initDirect3DView();
    }

    Direct3DView::~Direct3DView() {
        scene_->onSceneDestroy();

        releaseState();
        releaseResourceView();

        getWindow()->getRenderer()->removeSwapChainResizeNotifier(this);
    }


    void Direct3DView::initDirect3DView() {
        setFocusable(true);
        setMinimumWidth(1);
        setMinimumHeight(1);

        getWindow()->getRenderer()->addSwapChainResizeNotifier(this);
        scene_->onSceneCreate(this);
    }


    HRESULT Direct3DView::createState() {
        auto d3dDevice =
            Application::getGraphicDeviceManager()->getD3DDevice();
        auto d3dDeviceContext =
            Application::getGraphicDeviceManager()->getD3DDeviceContext();

        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        ::memset(&depthStencilDesc, 0, sizeof(depthStencilDesc));

        // 设置深度模版状态描述
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

        //D3D11_DEPTH_WRITE_MASK_ZERO禁止写深度缓冲
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
        depthStencilDesc.StencilEnable = true;
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

        //创建深度模版状态
        RH(d3dDevice->CreateDepthStencilState(&depthStencilDesc, &depth_stencil_state_));


        D3D11_RASTERIZER_DESC rasterDesc;
        ::memset(&rasterDesc, 0, sizeof(rasterDesc));

        // 设置光栅化描述，指定多边形如何被渲染.
        rasterDesc.AntialiasedLineEnable = false;
        rasterDesc.CullMode = D3D11_CULL_BACK;
        rasterDesc.DepthBias = 0;
        rasterDesc.DepthBiasClamp = 0.0f;
        rasterDesc.DepthClipEnable = true;
        rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
        rasterDesc.FrontCounterClockwise = false;
        rasterDesc.MultisampleEnable = false;
        rasterDesc.ScissorEnable = false;
        rasterDesc.SlopeScaledDepthBias = 0.0f;

        // 创建光栅化状态.
        RH(d3dDevice->CreateRasterizerState(&rasterDesc, &rasterizer_state_));


        D3D11_SAMPLER_DESC samplerDesc;
        ::memset(&samplerDesc, 0, sizeof(samplerDesc));

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

        RH(d3dDevice->CreateSamplerState(&samplerDesc, &sampler_state_));

        d3dDeviceContext->OMSetDepthStencilState(depth_stencil_state_.get(), 1);
        d3dDeviceContext->RSSetState(rasterizer_state_.get());
        d3dDeviceContext->PSSetSamplers(0, 1, &sampler_state_);

        return S_OK;
    }

    void Direct3DView::releaseState() {
        depth_stencil_state_.reset();
        rasterizer_state_.reset();
        sampler_state_.reset();
    }


    void Direct3DView::setViewports(
        float x, float y, float width, float height) {

        auto d3d_dc_ =
            Application::getGraphicDeviceManager()->getD3DDeviceContext();

        viewport_.Width = width;
        viewport_.Height = height;
        viewport_.MinDepth = 0.0f;
        viewport_.MaxDepth = 1.0f;
        viewport_.TopLeftX = x;
        viewport_.TopLeftY = y;

        d3d_dc_->RSSetViewports(1, &viewport_);
    }


    HRESULT Direct3DView::createResourceView(unsigned int width, unsigned int height)
    {
        if (width <= 0 || height <= 0) {
            return S_OK;
        }

        auto d3d_device = Application::getGraphicDeviceManager()->getD3DDevice();
        auto d3d_dc = Application::getGraphicDeviceManager()->getD3DDeviceContext();
        auto d2d_dc = getWindow()->getRenderer()->getD2DDeviceContext();

        // RTT
        CD3D11_TEXTURE2D_DESC target_surface_desc = CD3D11_TEXTURE2D_DESC(
            DXGI_FORMAT_B8G8R8A8_UNORM, width, height, 1, 1,
            D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0, 1, 0, 0);
        RH(d3d_device->CreateTexture2D(&target_surface_desc, nullptr, &d3d_content_surface_));
        RH(d3d_device->CreateRenderTargetView(d3d_content_surface_.get(), nullptr, &render_target_view_));

        D2D1_BITMAP_PROPERTIES bmp_prop = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

        auto dxgi_surface = d3d_content_surface_.cast<IDXGISurface>();
        RH(d2d_dc->CreateSharedBitmap(__uuidof(IDXGISurface), dxgi_surface.get(), &bmp_prop, &d3d_content_));

        // Depth buffer
        CD3D11_TEXTURE2D_DESC depth_buffer_desc = CD3D11_TEXTURE2D_DESC(
            DXGI_FORMAT_D24_UNORM_S8_UINT, width, height, 1, 1,
            D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 0, 1, 0, 0);
        RH(d3d_device->CreateTexture2D(&depth_buffer_desc, 0, &depth_stencil_buffer_));

        // 设置深度模版视图描述
        CD3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = CD3D11_DEPTH_STENCIL_VIEW_DESC(
            D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D24_UNORM_S8_UINT, 0, 0, 0, 0);
        RH(d3d_device->CreateDepthStencilView(
            depth_stencil_buffer_.get(), &depth_stencil_view_desc, &depth_stencil_view_));

        // std::wstring ddsFileName(::_wgetcwd(nullptr, 0));
        // RH(DirectX::CreateDDSTextureFromFile(d3d_device.get(), ddsFileName + L"\\top.dds", 0, &shader_resource_view_));

        d3d_dc->OMSetRenderTargets(1, &render_target_view_, depth_stencil_view_.get());
        // d3d_dc->PSSetShaderResources(0, 1, &shader_resource_view_);

        return S_OK;
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
        HRESULT hr = createState();
        if (FAILED(hr)) {
            Log::e(L"Direct3DView", L"init state failed.");
            return;
        }
    }


    void Direct3DView::onMeasure(
        int width, int height, int widthSpec, int heightSpec) {

        int final_width = 0;
        int final_height = 0;

        switch (widthSpec) {
        case FIT:
        case EXACTLY:
            final_width = std::max(width, getMinimumWidth());
            break;
        case UNKNOWN:
            final_width = std::max(0, getMinimumWidth());
            break;
        }

        switch (heightSpec) {
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

        HRESULT hr = createResourceView(width, height);
        if (FAILED(hr)) {
            Log::e(L"Direct3DView", L"init res view failed.");
            return;
        }

        if (size_changed) {
            setViewports(0, 0, width, height);
            scene_->onSceneResize(width, height);
        }
    }

    void Direct3DView::onDraw(Canvas *canvas) {
        View::onDraw(canvas);

        if (d3d_content_) {
            Color d2dColor = getWindow()->getBackgroundColor();
            float backColor[4] = { d2dColor.r, d2dColor.g, d2dColor.b, d2dColor.a };

            auto d3d_dc_ = Application::getGraphicDeviceManager()->getD3DDeviceContext();

            d3d_dc_->ClearRenderTargetView(render_target_view_.get(), backColor);
            d3d_dc_->ClearDepthStencilView(depth_stencil_view_.get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

            scene_->onSceneRender();

            Bitmap bmp(d3d_content_);
            canvas->drawBitmap(&bmp);
        }
    }

    bool Direct3DView::onInputEvent(InputEvent *e) {
        scene_->onSceneInput(e);
        return View::onInputEvent(e);
    }

    void Direct3DView::onSizeChanged(
        int width, int height, int oldWidth, int oldHeight) {
        View::onSizeChanged(width, height, oldWidth, oldHeight);
    }

}