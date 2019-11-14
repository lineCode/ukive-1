#include "ukive/graphics/canvas.h"

#include <algorithm>

#include "ukive/application.h"
#include "ukive/text/text_renderer.h"
#include "ukive/window/window.h"
#include "ukive/window/window_impl.h"
#include "ukive/graphics/rect.h"
#include "ukive/graphics/point.h"
#include "ukive/graphics/bitmap.h"
#include "ukive/log.h"
#include "ukive/utils/stl_utils.h"
#include "ukive/utils/win10_version.h"


namespace ukive {

    Canvas::Canvas(int width, int height)
        : is_texture_target_(true),
          opacity_(1.f)
    {
        d3d_tex2d_ = createTexture2D(width, height, false);
        if (!d3d_tex2d_) {
            return;
        }

        auto dxgi_surface = d3d_tex2d_.cast<IDXGISurface>();
        if (!dxgi_surface) {
            LOG(Log::WARNING) << "Failed to query DXGI surface.";
            return;
        }

        rt_ = createDXGIRenderTarget(dxgi_surface.get(), false);
        if (!rt_) {
            return;
        }

        initCanvas();
    }

    Canvas::Canvas(Window* w, bool hw_acc)
        : is_texture_target_(false),
          opacity_(1.f),
          is_hardware_acc_(hw_acc),
          owner_window_(w)
    {
        is_layered_ = owner_window_->isTranslucent();

        ComPtr<ID2D1RenderTarget> rt;
        if (is_layered_) {
            if (is_hardware_acc_) {
                rt = createHardwareBRT();
            } else {
                rt = createSoftwareBRT();
            }
        } else {
            rt = createSwapchainBRT();
        }

        if (!rt) {
            return;
        }

        rt_ = rt;
        initCanvas();
    }

    Canvas::~Canvas() {}

    void Canvas::initCanvas() {
        layer_counter_ = 0;

        rt_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &solid_brush_);
        rt_->CreateBitmapBrush(nullptr, &bitmap_brush_);

        solid_brush_->SetOpacity(opacity_);
        bitmap_brush_->SetOpacity(opacity_);
    }

    ComPtr<ID2D1RenderTarget> Canvas::createHardwareBRT() {
        D3D11_TEXTURE2D_DESC tex_desc = { 0 };
        tex_desc.ArraySize = 1;
        tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET;
        tex_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        tex_desc.Width = owner_window_->getWidth();
        tex_desc.Height = owner_window_->getHeight();
        tex_desc.MipLevels = 1;
        tex_desc.SampleDesc.Count = 1;
        tex_desc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;

        ComPtr<ID3D11Texture2D> d3d_texture;
        HRESULT hr = Application::getGraphicDeviceManager()->getD3DDevice()->
            CreateTexture2D(&tex_desc, nullptr, &d3d_texture);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create tex2d.";
            return {};
        }

        auto dxgi_surface = d3d_texture.cast<IDXGISurface>();
        if (!dxgi_surface) {
            LOG(Log::WARNING) << "Failed to cast tex2d to dxgi surface.";
            return {};
        }

        return createDXGIRenderTarget(dxgi_surface.get(), true);
    }

    ComPtr<ID2D1RenderTarget> Canvas::createSoftwareBRT() {
        auto wic_bmp = Application::getWICManager()->createBitmap(
            owner_window_->getWidth(),
            owner_window_->getHeight());
        if (!wic_bmp) {
            LOG(Log::WARNING) << "Failed to create wic bitmap.";
            return {};
        }

        return createWICRenderTarget(wic_bmp.get());
    }

    ComPtr<ID2D1RenderTarget> Canvas::createSwapchainBRT() {
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.OutputWindow = owner_window_->getImpl()->getHandle();
        swapChainDesc.Windowed = TRUE;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        auto gdm = Application::getGraphicDeviceManager();

        HRESULT hr = gdm->getDXGIFactory()->CreateSwapChain(
            gdm->getD3DDevice().get(),
            &swapChainDesc, &swapchain_);
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to create swap chain: " << hr;
            return {};
        }

        ComPtr<IDXGISurface> back_buffer;
        hr = swapchain_->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<LPVOID*>(&back_buffer));
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to query DXGI surface: " << hr;
            return {};
        }

        return createDXGIRenderTarget(back_buffer.get(), false);
    }

    bool Canvas::resizeHardwareBRT() {
        if (owner_window_->getWidth() <= 0 ||
            owner_window_->getHeight() <= 0)
        {
            return true;
        }

        releaseResources();

        rt_.reset();
        rt_ = createHardwareBRT();

        initCanvas();

        return rt_ != nullptr;
    }

    bool Canvas::resizeSoftwareBRT() {
        if (owner_window_->getWidth() <= 0 ||
            owner_window_->getHeight() <= 0)
        {
            return true;
        }

        rt_.reset();
        rt_ = createSoftwareBRT();

        return rt_ != nullptr;
    }

    bool Canvas::resizeSwapchainBRT() {
        releaseResources();

        rt_.reset();

        // 在某些系统上，需要传入完整大小
        static bool need_total = win::isWin10Ver1703OrGreater();

        int width = owner_window_->getClientWidth(need_total);
        int height = owner_window_->getClientHeight(need_total);
        if (width <= 0 || height <= 0) {
            return true;
        }

        HRESULT hr = swapchain_->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to resize swap chain: " << hr;
            return false;
        }

        ComPtr<IDXGISurface> back_buffer;
        hr = swapchain_->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<LPVOID*>(&back_buffer));
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to query DXGI surface: " << hr;
            return false;
        }

        rt_ = createDXGIRenderTarget(back_buffer.get(), false);

        initCanvas();

        return rt_ != nullptr;
    }

    bool Canvas::drawLayered() {
        auto gdi_rt = rt_.cast<ID2D1GdiInteropRenderTarget>();
        if (!gdi_rt) {
            LOG(Log::FATAL) << "Failed to cast ID2D1RenderTarget to GDI RT.";
            return false;
        }

        HDC hdc = nullptr;
        HRESULT hr = gdi_rt->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &hdc);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to get DC: " << hr;
            return false;
        }

        RECT wr;
        ::GetWindowRect(owner_window_->getImpl()->getHandle(), &wr);
        POINT zero = { 0, 0 };
        SIZE size = { wr.right - wr.left, wr.bottom - wr.top };
        POINT position = { wr.left, wr.top };
        BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        BOOL ret = ::UpdateLayeredWindow(
            owner_window_->getImpl()->getHandle(),
            nullptr, &position, &size, hdc, &zero,
            RGB(0xFF, 0xFF, 0xFF), &blend, ULW_ALPHA);
        if (ret == 0) {
            LOG(Log::ERR) << "Failed to update layered window: " << ::GetLastError();
        }

        RECT rect = {};
        hr = gdi_rt->ReleaseDC(&rect);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to release DC: " << hr;
            return false;
        }

        return true;
    }

    bool Canvas::drawSwapchain() {
        HRESULT hr = swapchain_->Present(Application::isVSyncEnabled() ? 1 : 0, 0);
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to present.";
            return false;
        }

        return true;
    }

    void Canvas::releaseResources() {
        layer_counter_ = 0;
        matrix_.identity();
        while (!opacity_stack_.empty()) {
            opacity_stack_.pop();
        }
        while (!drawing_state_stack_.empty()) {
            drawing_state_stack_.pop();
        }

        text_renderer_.reset();
        layer_.reset();
        solid_brush_.reset();
        bitmap_brush_.reset();
        d3d_tex2d_.reset();
    }

    void Canvas::setOpacity(float opacity) {
        if (opacity == opacity_) {
            return;
        }

        opacity_ = opacity;

        if (text_renderer_) {
            text_renderer_->setOpacity(opacity_);
        }

        solid_brush_->SetOpacity(opacity_);
        bitmap_brush_->SetOpacity(opacity_);
    }

    float Canvas::getOpacity() {
        return opacity_;
    }

    bool Canvas::resize() {
        bool ret;
        if (is_texture_target_) {
            // TODO:
            ret = true;
        } else {
            if (is_layered_) {
                if (is_hardware_acc_) {
                    ret = resizeHardwareBRT();
                } else {
                    ret = resizeSoftwareBRT();
                }
            } else {
                ret = resizeSwapchainBRT();
            }
        }

        return ret;
    }

    void Canvas::clear() {
        rt_->Clear();
    }

    void Canvas::clear(const Color& color) {
        D2D1_COLOR_F d2d_color { color.r, color.g, color.b, color.a };
        rt_->Clear(d2d_color);
    }

    void Canvas::beginDraw() {
        rt_->BeginDraw();
    }

    void Canvas::endDraw() {
        if (is_texture_target_) {
            HRESULT hr = rt_->EndDraw();
            if (FAILED(hr)) {
                LOG(Log::ERR) << "Failed to end draw.";
            }
        } else {
            if (is_layered_) {
                drawLayered();
            }

            HRESULT hr = rt_->EndDraw();
            if (FAILED(hr)) {
                DCHECK(false);
                LOG(Log::ERR) << "Failed to draw d2d content.";
            }

            if (!is_layered_) {
                drawSwapchain();
            }
        }
    }

    void Canvas::pushClip(const Rect& rect) {
        D2D1_RECT_F d2d_rect { float(rect.left), float(rect.top), float(rect.right), float(rect.bottom) };
        rt_->PushAxisAlignedClip(
            d2d_rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    }

    void Canvas::popClip() {
        rt_->PopAxisAlignedClip();
    }

    void Canvas::pushLayer(ID2D1Geometry* clipGeometry) {
        if (layer_counter_ > 0) {
            ++layer_counter_;
            return;
        }

        if (!layer_) {
            HRESULT hr = rt_->CreateLayer(&layer_);
            if (FAILED(hr)) {
                LOG(Log::WARNING) << "Failed to create layer: " << hr;
                return;
            }
        }

        rt_->PushLayer(
            D2D1::LayerParameters(D2D1::InfiniteRect(), clipGeometry),
            layer_.get());

        ++layer_counter_;
    }

    void Canvas::pushLayer(const RectF& content_bound, ID2D1Geometry* clipGeometry) {
        if (layer_counter_ > 0) {
            ++layer_counter_;
            return;
        }

        D2D1_RECT_F d2d_rect {
            content_bound.left, content_bound.top,
            content_bound.right, content_bound.bottom };

        if (!layer_) {
            HRESULT hr = rt_->CreateLayer(&layer_);
            if (FAILED(hr)) {
                LOG(Log::WARNING) << "Failed to create layer: " << hr;
                return;
            }
        }

        rt_->PushLayer(
            D2D1::LayerParameters(d2d_rect, clipGeometry),
            layer_.get());

        ++layer_counter_;
    }

    void Canvas::popLayer() {
        if (layer_counter_ > 1) {
            --layer_counter_;
            return;
        }

        rt_->PopLayer();

        --layer_counter_;
    }

    void Canvas::save() {
        ComPtr<ID2D1Factory> factory;
        rt_->GetFactory(&factory);

        ComPtr<ID2D1DrawingStateBlock> drawingStateBlock;
        factory->CreateDrawingStateBlock(&drawingStateBlock);
        rt_->SaveDrawingState(drawingStateBlock.get());

        opacity_stack_.push(opacity_);
        drawing_state_stack_.push(drawingStateBlock);
    }

    void Canvas::restore() {
        if (drawing_state_stack_.empty()) {
            return;
        }

        opacity_ = opacity_stack_.top();

        auto drawingStateBlock = drawing_state_stack_.top().get();

        D2D1_DRAWING_STATE_DESCRIPTION desc;
        drawingStateBlock->GetDescription(&desc);

        matrix_.set(desc.transform);

        rt_->RestoreDrawingState(drawingStateBlock);

        opacity_stack_.pop();
        drawing_state_stack_.pop();

        if (text_renderer_) {
            text_renderer_->setOpacity(opacity_);
        }

        solid_brush_->SetOpacity(opacity_);
        bitmap_brush_->SetOpacity(opacity_);
    }

    ID2D1RenderTarget* Canvas::getRT() {
        return rt_.get();
    }

    ComPtr<ID3D11Texture2D> Canvas::getTexture() {
        return d3d_tex2d_;
    }

    std::shared_ptr<Bitmap> Canvas::extractBitmap() {
        if (is_texture_target_) {
            ComPtr<ID2D1Bitmap> bitmap;
            D2D1_BITMAP_PROPERTIES bmp_prop = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
            HRESULT hr = rt_->CreateSharedBitmap(
                __uuidof(IDXGISurface), d3d_tex2d_.cast<IDXGISurface>().get(), &bmp_prop, &bitmap);
            if (FAILED(hr)) {
                DCHECK(false);
                LOG(Log::WARNING) << "Failed to create shared bitmap: " << hr;
                return {};
            }

            return std::make_shared<Bitmap>(bitmap);
        }

        return {};
    }

    void Canvas::scale(float sx, float sy) {
        scale(sx, sy, 0.f, 0.f);
    }

    void Canvas::scale(float sx, float sy, float cx, float cy) {
        matrix_.postScale(sx, sy, cx, cy);
        rt_->SetTransform(matrix_.getNative());
    }

    void Canvas::rotate(float angle) {
        rotate(angle, 0.f, 0.f);
    }

    void Canvas::rotate(float angle, float cx, float cy) {
        matrix_.postRotate(angle, cx, cy);
        rt_->SetTransform(matrix_.getNative());
    }

    void Canvas::translate(float dx, float dy) {
        matrix_.postTranslate(dx, dy);
        rt_->SetTransform(matrix_.getNative());
    }

    void Canvas::setMatrix(const Matrix& matrix) {
        matrix_ = matrix_ * matrix;
        rt_->SetTransform(matrix_.getNative());
    }

    Matrix Canvas::getMatrix() {
        return matrix_;
    }

    void Canvas::fillOpacityMask(
        float width, float height,
        Bitmap* mask, Bitmap* content)
    {
        bitmap_brush_->SetBitmap(content->getNative().get());
        bitmap_brush_->SetExtendModeX(D2D1_EXTEND_MODE_CLAMP);
        bitmap_brush_->SetExtendModeY(D2D1_EXTEND_MODE_CLAMP);

        D2D1_RECT_F rect = D2D1::RectF(0, 0, width, height);

        auto mode = rt_->GetAntialiasMode();
        rt_->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
        rt_->FillOpacityMask(
            mask->getNative().get(), bitmap_brush_.get(), D2D1_OPACITY_MASK_CONTENT_GRAPHICS, rect, rect);
        rt_->SetAntialiasMode(mode);
    }

    void Canvas::fillBitmapRepeat(const RectF& rect, Bitmap* content) {
        bitmap_brush_->SetBitmap(content->getNative().get());
        bitmap_brush_->SetExtendModeX(D2D1_EXTEND_MODE_WRAP);
        bitmap_brush_->SetExtendModeY(D2D1_EXTEND_MODE_WRAP);

        D2D1_RECT_F d2d_rect = D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom);
        rt_->FillRectangle(d2d_rect, bitmap_brush_.get());
    }

    void Canvas::drawLine(
        const PointF& start, const PointF& end, const Color& color)
    {
        drawLine(start, end, 1, color);
    }

    void Canvas::drawLine(
        const PointF& start, const PointF& end, float stroke_width, const Color& color)
    {
        D2D1_COLOR_F d2d_color {
            color.r, color.g, color.b, color.a, };

        solid_brush_->SetColor(d2d_color);
        rt_->DrawLine(
            D2D1::Point2F(start.x, start.y),
            D2D1::Point2F(end.x, end.y),
            solid_brush_.get(), stroke_width);
    }

    void Canvas::drawRect(const RectF& rect, const Color& color) {
        D2D1_COLOR_F d2d_color {
            color.r, color.g, color.b, color.a, };
        D2D1_RECT_F d2d_rect {
            rect.left, rect.top, rect.right, rect.bottom };

        solid_brush_->SetColor(d2d_color);
        rt_->DrawRectangle(d2d_rect, solid_brush_.get());
    }

    void Canvas::drawRect(const RectF& rect, float stroke_width, const Color& color) {
        D2D1_COLOR_F d2d_color {
            color.r, color.g, color.b, color.a, };
        D2D1_RECT_F d2d_rect {
            rect.left, rect.top, rect.right, rect.bottom };

        solid_brush_->SetColor(d2d_color);
        rt_->DrawRectangle(d2d_rect, solid_brush_.get(), stroke_width);
    }

    void Canvas::fillRect(const RectF& rect, const Color& color) {
        D2D1_COLOR_F d2d_color {
            color.r, color.g, color.b, color.a, };
        D2D1_RECT_F d2d_rect {
            rect.left, rect.top, rect.right, rect.bottom };

        solid_brush_->SetColor(d2d_color);
        rt_->FillRectangle(d2d_rect, solid_brush_.get());
    }

    void Canvas::drawRoundRect(
        const RectF& rect, float radius, const Color& color)
    {
        D2D1_COLOR_F d2d_color {
            color.r, color.g, color.b, color.a, };
        D2D1_RECT_F d2d_rect {
            rect.left, rect.top, rect.right, rect.bottom };

        solid_brush_->SetColor(d2d_color);
        rt_->DrawRoundedRectangle(
            D2D1::RoundedRect(d2d_rect, radius, radius), solid_brush_.get());
    }

    void Canvas::drawRoundRect(
        const RectF& rect, float stroke_width,
        float radius, const Color& color)
    {
        D2D1_COLOR_F d2d_color {
            color.r, color.g, color.b, color.a, };
        D2D1_RECT_F d2d_rect {
            rect.left, rect.top, rect.right, rect.bottom };

        solid_brush_->SetColor(d2d_color);
        rt_->DrawRoundedRectangle(
            D2D1::RoundedRect(d2d_rect, radius, radius), solid_brush_.get(), stroke_width);
    }

    void Canvas::fillRoundRect(
        const RectF& rect, float radius, const Color& color)
    {
        D2D1_COLOR_F d2d_color {
            color.r, color.g, color.b, color.a, };
        D2D1_RECT_F d2d_rect {
            rect.left, rect.top, rect.right, rect.bottom };

        solid_brush_->SetColor(d2d_color);
        rt_->FillRoundedRectangle(
            D2D1::RoundedRect(d2d_rect, radius, radius), solid_brush_.get());
    }

    void Canvas::drawCircle(float cx, float cy, float radius, const Color& color) {
        drawOval(cx, cy, radius, radius, color);
    }

    void Canvas::drawCircle(float cx, float cy, float radius, float stroke_width, const Color& color) {
        drawOval(cx, cy, radius, radius, stroke_width, color);
    }

    void Canvas::fillCircle(float cx, float cy, float radius, const Color& color) {
        fillOval(cx, cy, radius, radius, color);
    }

    void Canvas::drawCircle(const RectF& rect, const Color& color) {
        float cx = rect.left + rect.width() / 2;
        float cy = rect.top + rect.height() / 2;
        float radius = std::min(rect.width() / 2, rect.height() / 2);

        drawOval(cx, cy, radius, radius, color);
    }

    void Canvas::drawCircle(const RectF& rect, float stroke_width, const Color& color) {
        float cx = rect.left + rect.width() / 2;
        float cy = rect.top + rect.height() / 2;
        float radius = std::min(rect.width() / 2, rect.height() / 2);

        drawOval(cx, cy, radius, radius, stroke_width, color);
    }

    void Canvas::fillCircle(const RectF& rect, const Color& color) {
        float cx = rect.left + rect.width() / 2;
        float cy = rect.top + rect.height() / 2;
        float radius = std::min(rect.width() / 2, rect.height() / 2);

        fillOval(cx, cy, radius, radius, color);
    }

    void Canvas::drawOval(float cx, float cy, float rx, float ry, const Color& color) {
        D2D1_COLOR_F _color = {
            color.r, color.g, color.b, color.a, };
        solid_brush_->SetColor(_color);
        rt_->DrawEllipse(
            D2D1::Ellipse(D2D1::Point2F(cx, cy), rx, ry),
            solid_brush_.get());
    }

    void Canvas::drawOval(float cx, float cy, float rx, float ry, float stroke_width, const Color& color) {
        D2D1_COLOR_F _color {
            color.r, color.g, color.b, color.a, };
        solid_brush_->SetColor(_color);
        rt_->DrawEllipse(
            D2D1::Ellipse(
                D2D1::Point2F(cx, cy),
                rx, ry),
            solid_brush_.get(), stroke_width);
    }

    void Canvas::fillOval(float cx, float cy, float rx, float ry, const Color& color) {
        D2D1_COLOR_F _color {
            color.r, color.g, color.b, color.a, };
        solid_brush_->SetColor(_color);
        rt_->FillEllipse(
            D2D1::Ellipse(D2D1::Point2F(cx, cy), rx, ry), solid_brush_.get());
    }

    void Canvas::fillGeometry(ID2D1Geometry* geo, const Color& color) {
        D2D1_COLOR_F _color{
            color.r, color.g, color.b, color.a, };
        solid_brush_->SetColor(_color);
        rt_->FillGeometry(geo, solid_brush_.get());
    }

    void Canvas::fillGeometry(ID2D1Geometry* geo, ID2D1Brush* brush) {
        rt_->FillGeometry(geo, brush);
    }

    void Canvas::drawBitmap(Bitmap* bitmap) {
        if (!bitmap) {
            return;
        }

        RectF src(0.f, 0.f, bitmap->getWidth(), bitmap->getHeight());

        drawBitmap(src, src, 1.f, bitmap);
    }

    void Canvas::drawBitmap(float x, float y, Bitmap* bitmap) {
        if (!bitmap) {
            return;
        }

        auto width = bitmap->getWidth();
        auto height = bitmap->getHeight();
        RectF src(0.f, 0.f, width, height);
        RectF dst(x, y, width, height);

        drawBitmap(src, dst, 1.f, bitmap);
    }

    void Canvas::drawBitmap(float opacity, Bitmap* bitmap) {
        if (!bitmap) {
            return;
        }

        RectF src(0.f, 0.f, bitmap->getWidth(), bitmap->getHeight());

        drawBitmap(src, src, opacity, bitmap);
    }

    void Canvas::drawBitmap(const RectF& dst, float opacity, Bitmap* bitmap) {
        if (!bitmap) {
            return;
        }

        RectF src(0.f, 0.f, bitmap->getWidth(), bitmap->getHeight());

        drawBitmap(src, dst, opacity, bitmap);
    }

    void Canvas::drawBitmap(const RectF& src, const RectF& dst, float opacity, Bitmap* bitmap) {
        if (!bitmap) {
            return;
        }

        D2D1_RECT_F d2d_src_rect {
            src.left, src.top, src.right, src.bottom };
        D2D1_RECT_F d2d_dst_rect {
            dst.left, dst.top, dst.right, dst.bottom };

        rt_->DrawBitmap(
            bitmap->getNative().get(), d2d_dst_rect, opacity,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            d2d_src_rect);
    }

    void Canvas::drawText(
        const string16& text, IDWriteTextFormat* textFormat,
        const RectF& layoutRect, const Color& color)
    {
        if (!textFormat) {
            return;
        }

        D2D1_COLOR_F d2d_color {
            color.r, color.g, color.b, color.a, };
        D2D1_RECT_F d2d_layout_rect {
            layoutRect.left, layoutRect.top, layoutRect.right, layoutRect.bottom };

        solid_brush_->SetColor(d2d_color);
        rt_->DrawTextW(
            text.c_str(), STLCU32(text.length()), textFormat, d2d_layout_rect, solid_brush_.get());
    }

    void Canvas::drawTextLayout(
        float x, float y,
        IDWriteTextLayout* textLayout, const Color& color)
    {
        if (!textLayout) {
            return;
        }

        D2D1_COLOR_F d2d_color {
            color.r, color.g, color.b, color.a, };
        solid_brush_->SetColor(d2d_color);
        rt_->DrawTextLayout(D2D1::Point2F(x, y), textLayout, solid_brush_.get());
    }

    void Canvas::drawTextLayoutWithEffect(
        View* v, float x, float y,
        IDWriteTextLayout* textLayout, const Color& color)
    {
        if (!text_renderer_) {
            text_renderer_ = new TextRenderer(rt_);
            text_renderer_->setOpacity(opacity_);
        }

        text_renderer_->setTextColor(color);
        textLayout->Draw(v, text_renderer_.get(), x, y);
    }

    // static
    ComPtr<ID2D1RenderTarget> Canvas::createWICRenderTarget(IWICBitmap* wic_bitmap) {
        ComPtr<ID2D1RenderTarget> render_target;
        auto d2d_factory = Application::getGraphicDeviceManager()->getD2DFactory();
        if (d2d_factory) {
            const D2D1_PIXEL_FORMAT format =
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);

            const D2D1_RENDER_TARGET_PROPERTIES properties
                = D2D1::RenderTargetProperties(
                    D2D1_RENDER_TARGET_TYPE_SOFTWARE,
                    format, 96, 96,
                    D2D1_RENDER_TARGET_USAGE_NONE);

            HRESULT hr = d2d_factory->CreateWicBitmapRenderTarget(
                wic_bitmap, properties, &render_target);
            if (FAILED(hr)) {
                DCHECK(false);
                LOG(Log::WARNING) << "Failed to create WICBitmap RenderTarget: " << hr;
                return {};
            }
        }

        return render_target;
    }

    ComPtr<ID2D1DCRenderTarget> Canvas::createDCRenderTarget() {
        ComPtr<ID2D1DCRenderTarget> render_target;
        auto d2d_factory = Application::getGraphicDeviceManager()->getD2DFactory();

        if (d2d_factory) {
            const D2D1_PIXEL_FORMAT format =
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                    D2D1_ALPHA_MODE_PREMULTIPLIED);

            // DPI 固定为 96
            const D2D1_RENDER_TARGET_PROPERTIES properties =
                D2D1::RenderTargetProperties(
                    D2D1_RENDER_TARGET_TYPE_SOFTWARE,
                    format);

            HRESULT hr = d2d_factory->CreateDCRenderTarget(
                &properties, &render_target);
            if (FAILED(hr)) {
                LOG(Log::WARNING) << "Failed to create DC RenderTarget: " << hr;
                return {};
            }
        }

        return render_target;
    }

    ComPtr<ID3D11Texture2D> Canvas::createTexture2D(int width, int height, bool gdi_compat) {
        auto d3d_device = Application::getGraphicDeviceManager()->getD3DDevice();

        D3D11_TEXTURE2D_DESC tex_desc = { 0 };
        tex_desc.ArraySize = 1;
        tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        tex_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        tex_desc.Width = width;
        tex_desc.Height = height;
        tex_desc.MipLevels = 1;
        tex_desc.SampleDesc.Count = 1;
        tex_desc.SampleDesc.Quality = 0;
        tex_desc.MiscFlags = gdi_compat ? D3D11_RESOURCE_MISC_GDI_COMPATIBLE : 0;

        ComPtr<ID3D11Texture2D> d3d_texture;
        HRESULT hr = d3d_device->CreateTexture2D(&tex_desc, nullptr, &d3d_texture);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create 2d texture: " << hr;
            return {};
        }

        return d3d_texture;
    }

    ComPtr<ID2D1RenderTarget> Canvas::createDXGIRenderTarget(IDXGISurface* surface, bool gdi_compat) {
        ComPtr<ID2D1RenderTarget> render_target;
        auto d2d_factory = Application::getGraphicDeviceManager()->getD2DFactory();
        if (d2d_factory) {
            D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                96, 96, gdi_compat ? D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE : D2D1_RENDER_TARGET_USAGE_NONE);

            HRESULT hr = d2d_factory->CreateDxgiSurfaceRenderTarget(surface, props, &render_target);
            if (FAILED(hr)) {
                DCHECK(false);
            }
        }

        return render_target;
    }

    ComPtr<IDWriteTextFormat> Canvas::createTextFormat(
        const string16& font_family_name, float font_size, const string16& locale_name)
    {
        auto dwrite_factory = Application::getGraphicDeviceManager()->getDWriteFactory();

        ComPtr<IDWriteTextFormat> format;
        HRESULT hr = dwrite_factory->CreateTextFormat(
            font_family_name.c_str(), nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            font_size,
            locale_name.c_str(),
            &format);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create text format: " << hr;
            return {};
        }

        return format;
    }

    ComPtr<IDWriteTextLayout> Canvas::createTextLayout(
        const string16& text,
        IDWriteTextFormat* format,
        float max_width, float max_height)
    {
        auto dwrite_factory = Application::getGraphicDeviceManager()->getDWriteFactory();

        ComPtr<IDWriteTextLayout> layout;
        HRESULT hr = dwrite_factory->CreateTextLayout(
            text.c_str(), STLCU32(text.length()), format, max_width, max_height, &layout);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create text layout: " << hr;
            return {};
        }

        return layout;
    }


}