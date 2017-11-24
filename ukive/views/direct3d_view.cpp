#include "direct3d_view.h"

#include <algorithm>

#include "ukive/application.h"
#include "ukive/graphics/renderer.h"
#include "ukive/window/window.h"
#include "ukive/utils/hresult_utils.h"


namespace ukive {

    Direct3DView::Direct3DView(Window *wnd)
        :View(wnd)
    {
        initDirect3DView();
    }

    Direct3DView::Direct3DView(Window *wnd, int id)
        : View(wnd, id)
    {
        initDirect3DView();
    }

    Direct3DView::~Direct3DView()
    {
        releaseState();
        releaseResourceView();

        mWindow->getRenderer()->setDirect3DRenderListener(nullptr);
        mWindow->getRenderer()->removeSwapChainResizeNotifier(this);
    }


    void Direct3DView::initDirect3DView()
    {
        setFocusable(true);
        setMinimumWidth(1);
        setMinimumHeight(1);

        mWindow->getRenderer()->setDirect3DRenderListener(this);
        mWindow->getRenderer()->addSwapChainResizeNotifier(this);

        HRESULT hr = createResourceView();
        if (FAILED(hr))
            throw std::runtime_error("UDirect3DView::initDirect3DView(): init res view failed.");

        hr = createState();
        if (FAILED(hr))
            throw std::runtime_error("UDirect3DView::initDirect3DView(): init state failed.");

        setViewports(0, 0, 1, 1);
    }


    HRESULT Direct3DView::createState()
    {
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
        RH(d3dDevice->CreateDepthStencilState(&depthStencilDesc, &mNormalDepthStencilState));


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
        RH(d3dDevice->CreateRasterizerState(&rasterDesc, &mNormalRasterizerState));


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

        RH(d3dDevice->CreateSamplerState(&samplerDesc, &mNormalSamplerState));

        d3dDeviceContext->OMSetDepthStencilState(mNormalDepthStencilState.get(), 1);
        d3dDeviceContext->RSSetState(mNormalRasterizerState.get());
        d3dDeviceContext->PSSetSamplers(0, 1, &mNormalSamplerState);

        return S_OK;
    }

    void Direct3DView::releaseState()
    {
        mNormalDepthStencilState.reset();
        mNormalRasterizerState.reset();
        mNormalSamplerState.reset();
    }


    void Direct3DView::setViewports(
        float x, float y,
        float width, float height)
    {
        auto d3dDeviceContext =
            Application::getGraphicDeviceManager()->getD3DDeviceContext();

        mViewport.Width = width;
        mViewport.Height = height;
        mViewport.MinDepth = 0.0f;
        mViewport.MaxDepth = 1.0f;
        mViewport.TopLeftX = x;
        mViewport.TopLeftY = y;

        d3dDeviceContext->RSSetViewports(1, &mViewport);
    }


    HRESULT Direct3DView::createResourceView()
    {
        auto d3dDevice =
            Application::getGraphicDeviceManager()->getD3DDevice();
        auto d3dDeviceContext =
            Application::getGraphicDeviceManager()->getD3DDeviceContext();
        auto swapchain = mWindow->getRenderer()->getSwapChain();

        ComPtr<ID3D11Texture2D> backBufferPtr;

        RH(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr));
        RH(d3dDevice->CreateRenderTargetView(backBufferPtr.get(), 0, &mNormalRenderTargetView));

        D3D11_TEXTURE2D_DESC depthBufferDesc;
        ::memset(&depthBufferDesc, 0, sizeof(depthBufferDesc));

        depthBufferDesc.Width = mWindow->getRenderer()->getScWidth();
        depthBufferDesc.Height = mWindow->getRenderer()->getScHeight();
        depthBufferDesc.MipLevels = 1; //对于深度缓冲为1
        depthBufferDesc.ArraySize = 1; //对于深度缓冲为1，对于纹理，这2个参数有更多用途
        depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthBufferDesc.SampleDesc.Count = 1;
        depthBufferDesc.SampleDesc.Quality = 0;
        depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthBufferDesc.CPUAccessFlags = 0;
        depthBufferDesc.MiscFlags = 0;

        RH(d3dDevice->CreateTexture2D(&depthBufferDesc, 0, &mNormalDepthStencilBuffer));

        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
        ::memset(&depthStencilViewDesc, 0, sizeof(depthStencilViewDesc));

        // 设置深度模版视图描述.
        depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Texture2D.MipSlice = 0;

        // 创建深度模版视图.
        RH(d3dDevice->CreateDepthStencilView(
            mNormalDepthStencilBuffer.get(), &depthStencilViewDesc, &mNormalDepthStencilView));

        //std::wstring ddsFileName(::_wgetcwd(nullptr, 0));
        //RH(DirectX::CreateDDSTextureFromFile(d3dDevice.get(), ddsFileName + L"\\top.dds", 0, &mShaderResourceView));

        d3dDeviceContext->OMSetRenderTargets(1, &mNormalRenderTargetView, mNormalDepthStencilView.get());
        //d3dDeviceContext->PSSetShaderResources(0, 1, &mShaderResourceView);

        return S_OK;
    }

    void Direct3DView::releaseResourceView()
    {
        mNormalDepthStencilBuffer.reset();
        mNormalRenderTargetView.reset();
        mNormalDepthStencilView.reset();
        mShaderResourceView.reset();
    }


    void Direct3DView::onDirect3DClear()
    {
        D2D1_COLOR_F d2dColor = mWindow->getBackgroundColor();
        float backColor[4] = { d2dColor.r, d2dColor.g, d2dColor.b, d2dColor.a };

        Application::getGraphicDeviceManager()->getD3DDeviceContext()->
            ClearRenderTargetView(mNormalRenderTargetView.get(), backColor);
        Application::getGraphicDeviceManager()->getD3DDeviceContext()->
            ClearDepthStencilView(mNormalDepthStencilView.get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    }

    void Direct3DView::onDirect3DRender()
    {
    }

    void Direct3DView::onSwapChainResize()
    {
        releaseState();
        releaseResourceView();
    }

    void Direct3DView::onSwapChainResized()
    {
        HRESULT hr;

        hr = createResourceView();
        if (FAILED(hr))
            throw std::runtime_error("UDirect3DView::initDirect3DView(): init res view failed.");

        hr = createState();
        if (FAILED(hr))
            throw std::runtime_error("UDirect3DView::initDirect3DView(): init state failed.");
    }


    void Direct3DView::onMeasure(
        int width, int height,
        int widthSpec, int heightSpec)
    {
        int finalWidth = 0;
        int finalHeight = 0;

        switch (widthSpec)
        {
        case FIT:
        case EXACTLY:
            finalWidth = std::max(width, getMinimumWidth());
            break;
        case UNKNOWN:
            finalWidth = std::max(0, getMinimumWidth());
            break;
        }

        switch (heightSpec)
        {
        case FIT:
        case EXACTLY:
            finalHeight = std::max(height, getMinimumHeight());
            break;
        case UNKNOWN:
            finalHeight = std::max(0, getMinimumHeight());
            break;
        }

        setMeasuredDimension(finalWidth, finalHeight);
    }

    void Direct3DView::onDraw(Canvas *canvas)
    {
        View::onDraw(canvas);
    }

    bool Direct3DView::onInputEvent(InputEvent *e)
    {
        return View::onInputEvent(e);
    }


    void Direct3DView::onLayout(
        bool changed, bool sizeChanged,
        int left, int top, int right, int bottom)
    {
        if (changed)
        {
            Rect rect = getBoundInWindow();
            int _left = rect.left + getPaddingLeft();
            int _top = rect.top + getPaddingTop();
            int width = getMeasuredWidth() - getPaddingLeft() - getPaddingRight();
            int height = getMeasuredHeight() - getPaddingTop() - getPaddingBottom();

            setViewports(
                static_cast<float>(_left), static_cast<float>(_top),
                static_cast<float>(width), static_cast<float>(height));
        }
    }

    void Direct3DView::onSizeChanged(
        int width, int height, int oldWidth, int oldHeight)
    {
        View::onSizeChanged(width, height, oldWidth, oldHeight);
    }

}