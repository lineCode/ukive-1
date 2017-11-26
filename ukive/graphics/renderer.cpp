#include "renderer.h"

#include <fstream>

#include "ukive/application.h"
#include "ukive/graphics/swapchain_resize_notifier.h"
#include "ukive/graphics/direct3d_render_listener.h"
#include "ukive/log.h"
#include "ukive/utils/hresult_utils.h"
#include "ukive/window/window_impl.h"


namespace ukive {

    Renderer::Renderer()
        :d3d_render_listener_(nullptr) {
    }

    Renderer::~Renderer() {
    }


    HRESULT Renderer::init(WindowImpl *window)
    {
        owner_window_ = window;
        d2d_dc_ = Application::getGraphicDeviceManager()->createD2DDeviceContext();

        return createRenderResource();
    }

    HRESULT Renderer::createRenderResource()
    {
        RH(d2d_dc_->CreateEffect(CLSID_D2D1Shadow, &shadow_effect_));
        RH(d2d_dc_->CreateEffect(CLSID_D2D12DAffineTransform, &affinetrans_effect_));

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

        swapChainDesc.BufferCount = 2;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;

        auto gdm = Application::getGraphicDeviceManager();

        RH(gdm->getDXGIFactory()->CreateSwapChainForHwnd(
            gdm->getD3DDevice().get(),
            owner_window_->getHandle(),
            &swapChainDesc, 0, 0, &swapchain_));

        ComPtr<IDXGISurface> backBufferPtr;
        RH(swapchain_->GetBuffer(0, __uuidof(IDXGISurface), (LPVOID*)&backBufferPtr));
        RH(createBitmapRenderTarget(backBufferPtr.get(), &bitmap_render_target_));

        float dpiX, dpiY;
        bitmap_render_target_->GetDpi(&dpiX, &dpiY);
        d2d_dc_->SetDpi(dpiX, dpiY);

        d2d_dc_->SetTarget(bitmap_render_target_.get());

        DXGI_SWAP_CHAIN_DESC1 checkDesc;
        swapchain_->GetDesc1(&checkDesc);

        width_ = checkDesc.Width;
        height_ = checkDesc.Height;

        return S_OK;
    }

    void Renderer::releaseRenderResource()
    {
        bitmap_render_target_.reset();
        swapchain_.reset();
        shadow_effect_.reset();
        affinetrans_effect_.reset();
    }

    HRESULT Renderer::resize()
    {
        d2d_dc_->SetTarget(0);
        bitmap_render_target_.reset();

        for (auto it = sc_resize_notifier_list_.begin();
            it != sc_resize_notifier_list_.end(); ++it)
        {
            (*it)->onSwapChainResize();
        }

        RH(swapchain_->ResizeBuffers(
            0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));

        ComPtr<IDXGISurface> backBufferPtr;
        RH(swapchain_->GetBuffer(0, __uuidof(IDXGISurface), (LPVOID*)&backBufferPtr));
        RH(createBitmapRenderTarget(backBufferPtr.get(), &bitmap_render_target_));

        d2d_dc_->SetTarget(bitmap_render_target_.get());

        DXGI_SWAP_CHAIN_DESC1 checkDesc;
        swapchain_->GetDesc1(&checkDesc);

        width_ = checkDesc.Width;
        height_ = checkDesc.Height;

        for (auto it = sc_resize_notifier_list_.begin();
            it != sc_resize_notifier_list_.end(); ++it)
        {
            (*it)->onSwapChainResized();
        }

        return S_OK;
    }

    bool Renderer::render(
        Color bkColor,
        std::function<void()> renderCallback)
    {
        HRESULT hr;

        if (d3d_render_listener_)
            d3d_render_listener_->onDirect3DClear();

        d2d_dc_->BeginDraw();
        D2D1_COLOR_F color = {
            bkColor.r,
            bkColor.g,
            bkColor.b,
            bkColor.a, };
        d2d_dc_->Clear(color);

        renderCallback();

        hr = d2d_dc_->EndDraw();

        if (d3d_render_listener_)
            d3d_render_listener_->onDirect3DRender();

        hr = swapchain_->Present(Application::isVSyncEnabled() ? 1 : 0, 0);
        if (FAILED(hr)) {
            Log::e(L"failed to present.");
        }

        return !FAILED(hr);
    }

    void Renderer::close()
    {
        releaseRenderResource();
    }


    HRESULT Renderer::drawWithShadow(
        float elevation,
        float width, float height,
        std::function<void(ComPtr<ID2D1RenderTarget> rt)> drawer)
    {
        ComPtr<ID2D1BitmapRenderTarget> bmpRenderTarget;
        RH(d2d_dc_->CreateCompatibleRenderTarget(
            D2D1::SizeF(width, height), &bmpRenderTarget));

        bmpRenderTarget->BeginDraw();
        bmpRenderTarget->Clear(D2D1::ColorF(0, 0));

        drawer(bmpRenderTarget.cast<ID2D1RenderTarget>());

        RH(bmpRenderTarget->EndDraw());

        ComPtr<ID2D1Bitmap> bkBitmap;
        RH(bmpRenderTarget->GetBitmap(&bkBitmap));

        shadow_effect_->SetInput(0, bkBitmap.get());
        RH(shadow_effect_->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, elevation));
        RH(shadow_effect_->SetValue(D2D1_SHADOW_PROP_COLOR, D2D1::Vector4F(0, 0, 0, .4f)));

        D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Translation(0, elevation / 2.f);
        affinetrans_effect_->SetInputEffect(0, shadow_effect_.get());
        RH(affinetrans_effect_->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, matrix));

        d2d_dc_->DrawImage(affinetrans_effect_.get());
        d2d_dc_->DrawBitmap(bkBitmap.get());

        return S_OK;
    }

    HRESULT Renderer::drawShadow(float elevation, float alpha, ID2D1Bitmap *bitmap)
    {
        //在 Alpha 动画时，令阴影更快消退。
        float shadowAlpha;
        if (alpha == 0.f)
            shadowAlpha = 0.f;
        else if (alpha == 1.f)
            shadowAlpha = .38f;
        else
            shadowAlpha = static_cast<float>(.38f*::pow(2, 8 * (alpha - 1)) / 1.f);

        shadow_effect_->SetInput(0, bitmap);
        RH(shadow_effect_->SetValue(D2D1_SHADOW_PROP_OPTIMIZATION, D2D1_SHADOW_OPTIMIZATION_BALANCED));
        RH(shadow_effect_->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, elevation));
        RH(shadow_effect_->SetValue(D2D1_SHADOW_PROP_COLOR, D2D1::Vector4F(0, 0, 0, shadowAlpha)));

        D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Translation(0, elevation / 1.5f);
        affinetrans_effect_->SetInputEffect(0, shadow_effect_.get());
        RH(affinetrans_effect_->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, matrix));

        d2d_dc_->DrawImage(affinetrans_effect_.get());

        return S_OK;
    }

    HRESULT Renderer::drawOnBitmap(
        float width, float height, ID2D1Bitmap **bitmap,
        std::function<void(ComPtr<ID2D1RenderTarget> rt)> drawer)
    {
        ComPtr<ID2D1BitmapRenderTarget> bmpRenderTarget;
        HRESULT hr = d2d_dc_->CreateCompatibleRenderTarget(
            D2D1::SizeF(width, height), &bmpRenderTarget);
        if (FAILED(hr)) {
            return hr;
        }

        bmpRenderTarget->BeginDraw();
        bmpRenderTarget->Clear(D2D1::ColorF(0, 0));

        drawer(bmpRenderTarget.cast<ID2D1RenderTarget>());

        RH(bmpRenderTarget->EndDraw());
        RH(bmpRenderTarget->GetBitmap(bitmap));

        return S_OK;
    }

    void Renderer::drawObjects(DrawingObjectManager::DrawingObject *object) {
        if (object == nullptr) {
            return;
        }

        auto gdm = Application::getGraphicDeviceManager();

        gdm->getD3DDeviceContext()->IASetVertexBuffers(
            0, 1, &object->vertexBuffer, &object->vertexStructSize, &object->vertexDataOffset);
        gdm->getD3DDeviceContext()->IASetIndexBuffer(object->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        gdm->getD3DDeviceContext()->DrawIndexed(object->indexCount, 0, 0);
    }

    void Renderer::draw(ID3D11Buffer* vertices, ID3D11Buffer* indices, int structSize, int indexCount)
    {
        UINT vertexDataOffset = 0;
        UINT vertexStructSize = structSize;

        auto gdm = Application::getGraphicDeviceManager();

        gdm->getD3DDeviceContext()->IASetVertexBuffers(
            0, 1, &vertices, &vertexStructSize, &vertexDataOffset);
        gdm->getD3DDeviceContext()->IASetIndexBuffer(indices, DXGI_FORMAT_R32_UINT, 0);
        gdm->getD3DDeviceContext()->DrawIndexed(indexCount, 0, 0);
    }


    void Renderer::addSwapChainResizeNotifier(SwapChainResizeNotifier *notifier)
    {
        sc_resize_notifier_list_.push_back(notifier);
    }

    void Renderer::removeSwapChainResizeNotifier(SwapChainResizeNotifier *notifier)
    {
        for (auto it = sc_resize_notifier_list_.begin();
            it != sc_resize_notifier_list_.end();)
        {
            if ((*it) == notifier)
                it = sc_resize_notifier_list_.erase(it);
            else
                ++it;
        }
    }

    void Renderer::removeAllSwapChainResizeNotifier()
    {
        sc_resize_notifier_list_.clear();
    }

    void Renderer::setDirect3DRenderListener(Direct3DRenderListener *listener)
    {
        d3d_render_listener_ = listener;
    }


    void Renderer::setVertexShader(ID3D11VertexShader *shader) {
        auto gdm = Application::getGraphicDeviceManager();
        gdm->getD3DDeviceContext()->VSSetShader(shader, 0, 0);
    }

    void Renderer::setPixelShader(ID3D11PixelShader *shader) {
        auto gdm = Application::getGraphicDeviceManager();
        gdm->getD3DDeviceContext()->PSSetShader(shader, 0, 0);
    }

    void Renderer::setInputLayout(ID3D11InputLayout *inputLayout) {
        auto gdm = Application::getGraphicDeviceManager();
        gdm->getD3DDeviceContext()->IASetInputLayout(inputLayout);
    }

    void Renderer::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology) {
        auto gdm = Application::getGraphicDeviceManager();
        gdm->getD3DDeviceContext()->IASetPrimitiveTopology(topology);
    }

    void Renderer::setConstantBuffers(
        UINT startSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers) {
        auto gdm = Application::getGraphicDeviceManager();
        gdm->getD3DDeviceContext()->VSSetConstantBuffers(
            startSlot, NumBuffers, ppConstantBuffers);
    }


    UINT Renderer::getScWidth() {
        return width_;
    }

    UINT Renderer::getScHeight() {
        return height_;
    }

    ComPtr<ID2D1Effect> Renderer::getShadowEffect() {
        return shadow_effect_;
    }

    ComPtr<ID2D1Effect> Renderer::getAffineTransEffect() {
        return affinetrans_effect_;
    }

    ComPtr<IDXGISwapChain1> Renderer::getSwapChain() {
        return swapchain_;
    }

    ComPtr<ID2D1DeviceContext> Renderer::getD2DDeviceContext() {
        return d2d_dc_;
    }


    HRESULT Renderer::createBitmapRenderTarget(IDXGISurface *dxgiSurface, ID2D1Bitmap1 **bitmap) {
        HRESULT hr = S_OK;

        FLOAT dpiX;
        FLOAT dpiY;
        Application::getGraphicDeviceManager()->getD2DFactory()->GetDesktopDpi(&dpiX, &dpiY);

        D2D1_BITMAP_PROPERTIES1 bitmapProperties =
            D2D1::BitmapProperties1(
                D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
                dpiX, dpiY);

        return d2d_dc_->
            CreateBitmapFromDxgiSurface(dxgiSurface, bitmapProperties, bitmap);
    }

    HRESULT Renderer::createCompatBitmapRenderTarget(
        float width, float height, ID2D1BitmapRenderTarget **bRT)
    {
        ComPtr<ID2D1BitmapRenderTarget> bmpRenderTarget;
        RH(d2d_dc_->CreateCompatibleRenderTarget(
            D2D1::SizeF(width, height), bRT));

        return S_OK;
    }

    HRESULT Renderer::createDXGISurfaceRenderTarget(
        IDXGISurface *dxgiSurface, ID2D1RenderTarget **renderTarget)
    {
        HRESULT hr = S_OK;
        auto gdm = Application::getGraphicDeviceManager();

        FLOAT dpiX;
        FLOAT dpiY;
        gdm->getD2DFactory()->GetDesktopDpi(&dpiX, &dpiY);

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
            dpiX, dpiY);

        return gdm->getD2DFactory()->CreateDxgiSurfaceRenderTarget(dxgiSurface, props, renderTarget);
    }

    HRESULT Renderer::createWindowRenderTarget(
        HWND handle, unsigned int width, unsigned int height, ID2D1HwndRenderTarget **renderTarget)
    {
        HRESULT hr = S_OK;
        auto gdm = Application::getGraphicDeviceManager();

        FLOAT dpiX;
        FLOAT dpiY;
        gdm->getD2DFactory()->GetDesktopDpi(&dpiX, &dpiY);

        D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = D2D1::RenderTargetProperties();
        // Set the DPI to be the default system DPI to allow direct mapping
        // between image pixels and desktop pixels in different system DPI settings
        renderTargetProperties.dpiX = dpiX;
        renderTargetProperties.dpiY = dpiY;

        return gdm->getD2DFactory()->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(handle, D2D1::SizeU(width, height)),
            renderTarget);
    }

    HRESULT Renderer::createTextFormat(
        string16 fontFamilyName,
        float fontSize, string16 localeName,
        IDWriteTextFormat **textFormat)
    {
        return Application::getGraphicDeviceManager()->getDWriteFactory()->CreateTextFormat(
            fontFamilyName.c_str(), nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            fontSize,
            localeName.c_str(),
            textFormat);
    }

    HRESULT Renderer::createTextLayout(
        string16 text,
        IDWriteTextFormat *textFormat,
        float maxWidth, float maxHeight,
        IDWriteTextLayout **textLayout)
    {
        return Application::getGraphicDeviceManager()->getDWriteFactory()->CreateTextLayout(
            text.c_str(), text.length(), textFormat, maxWidth, maxHeight, textLayout);
    }


    HRESULT Renderer::createVertexShader(
        string16 fileName,
        D3D11_INPUT_ELEMENT_DESC *polygonLayout,
        UINT numElements,
        ID3D11VertexShader **vertexShader,
        ID3D11InputLayout **inputLayout)
    {
        std::ifstream reader(fileName.c_str(), std::ios::binary);
        auto cpos = reader.tellg();
        reader.seekg(0, std::ios_base::end);
        size_t charSize = (size_t)reader.tellg();
        reader.seekg(cpos);

        char *shaderBuf = new char[charSize];
        reader.read(shaderBuf, charSize);

        auto gdm = Application::getGraphicDeviceManager();

        RH(gdm->getD3DDevice()->CreateVertexShader(
            shaderBuf, charSize, 0, vertexShader));

        RH(gdm->getD3DDevice()->CreateInputLayout(
            polygonLayout, numElements,
            shaderBuf, charSize, inputLayout));

        delete[] shaderBuf;

        return S_OK;
    }

    HRESULT Renderer::createPixelShader(
        string16 fileName,
        ID3D11PixelShader **pixelShader)
    {
        std::ifstream reader(fileName.c_str(), std::ios::binary);
        auto cpos = reader.tellg();
        reader.seekg(0, std::ios_base::end);
        size_t charSize = (size_t)reader.tellg();
        reader.seekg(cpos);

        char *shaderBuf = new char[charSize];
        reader.read(shaderBuf, charSize);

        RH(Application::getGraphicDeviceManager()->getD3DDevice()->CreatePixelShader(
            shaderBuf, charSize, 0, pixelShader));

        delete[] shaderBuf;

        return S_OK;
    }


    HRESULT Renderer::createVertexBuffer(
        void *vertices, UINT structSize, UINT vertexCount, ID3D11Buffer *&vertexBuffer)
    {
        D3D11_BUFFER_DESC vertexBufferDesc;
        D3D11_SUBRESOURCE_DATA vertexData;

        vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        vertexBufferDesc.ByteWidth = structSize * vertexCount;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        vertexBufferDesc.MiscFlags = 0;
        vertexBufferDesc.StructureByteStride = 0;

        vertexData.pSysMem = vertices;
        vertexData.SysMemPitch = 0;
        vertexData.SysMemSlicePitch = 0;

        RH(Application::getGraphicDeviceManager()->getD3DDevice()
            ->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer));

        return S_OK;
    }

    HRESULT Renderer::createIndexBuffer(int *indices, UINT indexCount, ID3D11Buffer *&indexBuffer)
    {
        D3D11_BUFFER_DESC indexBufferDesc;
        D3D11_SUBRESOURCE_DATA indexData;

        // 设置索引缓冲描述.
        indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        indexBufferDesc.ByteWidth = sizeof(int)* indexCount;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        indexBufferDesc.MiscFlags = 0;
        indexBufferDesc.StructureByteStride = 0;

        // 指向存临时索引缓冲.
        indexData.pSysMem = indices;
        indexData.SysMemPitch = 0;
        indexData.SysMemSlicePitch = 0;

        // 创建索引缓冲.
        RH(Application::getGraphicDeviceManager()->getD3DDevice()
            ->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer));

        return S_OK;
    }

    HRESULT Renderer::createConstantBuffer(UINT size, ID3D11Buffer **buffer)
    {
        HRESULT hr = E_FAIL;
        ID3D11Buffer *_buffer = 0;
        D3D11_BUFFER_DESC constBufferDesc;

        constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        constBufferDesc.ByteWidth = size;
        constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        constBufferDesc.MiscFlags = 0;
        constBufferDesc.StructureByteStride = 0;

        hr = Application::getGraphicDeviceManager()->getD3DDevice()
            ->CreateBuffer(&constBufferDesc, 0, &_buffer);

        *buffer = _buffer;

        return hr;
    }

    D3D11_MAPPED_SUBRESOURCE Renderer::lockResource(ID3D11Resource *resource)
    {
        HRESULT hr = E_FAIL;
        D3D11_MAPPED_SUBRESOURCE mappedResource;

        hr = Application::getGraphicDeviceManager()->getD3DDeviceContext()
            ->Map(resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (SUCCEEDED(hr))
            return mappedResource;

        mappedResource.pData = nullptr;
        return mappedResource;
    }

    void Renderer::unlockResource(ID3D11Resource *resource)
    {
        Application::getGraphicDeviceManager()->getD3DDeviceContext()->Unmap(resource, 0);
    }

}