#include "canvas.h"

#include <algorithm>

#include "ukive/application.h"
#include "ukive/graphics/bitmap_factory.h"
#include "ukive/text/text_renderer.h"
#include "ukive/window/window.h"
#include "ukive/graphics/renderer.h"
#include "ukive/graphics/rect.h"
#include "ukive/graphics/point.h"
#include "ukive/graphics/bitmap.h"
#include "ukive/log.h"


namespace ukive {

    Canvas::Canvas(int width, int height)
        : is_texture_target_(true)
    {
        d3d_tex2d_ = Renderer::createTexture2D(width, height);
        auto dxgi_surface = d3d_tex2d_.cast<IDXGISurface>();
        if (!dxgi_surface) {
            DCHECK(false);
            LOG(ukive::Log::WARNING) << "Failed to query DXGI surface.";
            return;
        }

        auto off_d2d_rt = Renderer::createDXGIRenderTarget(dxgi_surface.get(), true);
        initCanvas(off_d2d_rt);
    }

    Canvas::Canvas(ComPtr<ID2D1RenderTarget> rt)
        : is_texture_target_(false)
    {
        initCanvas(rt);
    }

    Canvas::~Canvas() {}

    void Canvas::initCanvas(ComPtr<ID2D1RenderTarget> rt) {
        opacity_ = 1.f;
        layer_counter_ = 0;

        render_target_ = rt;
        render_target_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &solid_brush_);
        render_target_->CreateBitmapBrush(nullptr, &bitmap_brush_);
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

    void Canvas::clear() {
        render_target_->Clear();
    }

    void Canvas::clear(const Color& color) {
        D2D1_COLOR_F d2d_color { color.r, color.g, color.b, color.a };
        render_target_->Clear(d2d_color);
    }

    void Canvas::beginDraw() {
        render_target_->BeginDraw();
    }

    void Canvas::endDraw() {
        HRESULT hr = render_target_->EndDraw();
        if (FAILED(hr)) {
            LOG(Log::ERR) << "Failed to end draw.";
        }
    }

    void Canvas::popClip() {
        render_target_->PopAxisAlignedClip();
    }

    void Canvas::pushClip(const RectF& rect) {
        D2D1_RECT_F d2d_rect { rect.left, rect.top, rect.right, rect.bottom };
        render_target_->PushAxisAlignedClip(
            d2d_rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    }

    void Canvas::pushLayer(ID2D1Geometry* clipGeometry) {
        if (layer_counter_ > 0) {
            ++layer_counter_;
            return;
        }

        if (!layer_) {
            HRESULT hr = render_target_->CreateLayer(&layer_);
            if (FAILED(hr)) {
                LOG(Log::WARNING) << "Failed to create layer: " << hr;
                return;
            }
        }

        render_target_->PushLayer(
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
            HRESULT hr = render_target_->CreateLayer(&layer_);
            if (FAILED(hr)) {
                LOG(Log::WARNING) << "Failed to create layer: " << hr;
                return;
            }
        }

        render_target_->PushLayer(
            D2D1::LayerParameters(d2d_rect, clipGeometry),
            layer_.get());

        ++layer_counter_;
    }

    void Canvas::popLayer() {
        if (layer_counter_ > 1) {
            --layer_counter_;
            return;
        }

        render_target_->PopLayer();

        --layer_counter_;
    }

    void Canvas::save() {
        ComPtr<ID2D1Factory> factory;
        render_target_->GetFactory(&factory);

        ComPtr<ID2D1DrawingStateBlock> drawingStateBlock;
        factory->CreateDrawingStateBlock(&drawingStateBlock);
        render_target_->SaveDrawingState(drawingStateBlock.get());

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

        render_target_->RestoreDrawingState(drawingStateBlock);

        opacity_stack_.pop();
        drawing_state_stack_.pop();

        if (text_renderer_) {
            text_renderer_->setOpacity(opacity_);
        }

        solid_brush_->SetOpacity(opacity_);
        bitmap_brush_->SetOpacity(opacity_);
    }

    ID2D1RenderTarget* Canvas::getRT() {
        return render_target_.get();
    }

    ComPtr<ID3D11Texture2D> Canvas::getTexture() {
        return d3d_tex2d_;
    }

    std::shared_ptr<Bitmap> Canvas::extractBitmap() {
        if (is_texture_target_) {
            ComPtr<ID2D1Bitmap> bitmap;
            D2D1_BITMAP_PROPERTIES bmp_prop = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
            HRESULT hr = render_target_->CreateSharedBitmap(
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
        render_target_->SetTransform(matrix_.getNative());
    }

    void Canvas::rotate(float angle) {
        rotate(angle, 0.f, 0.f);
    }

    void Canvas::rotate(float angle, float cx, float cy) {
        matrix_.postRotate(angle, cx, cy);
        render_target_->SetTransform(matrix_.getNative());
    }

    void Canvas::translate(float dx, float dy) {
        matrix_.postTranslate(dx, dy);
        render_target_->SetTransform(matrix_.getNative());
    }

    void Canvas::setMatrix(const Matrix& matrix) {
        matrix_ = matrix_ * matrix;
        render_target_->SetTransform(matrix_.getNative());
    }

    Matrix Canvas::getMatrix() {
        return matrix_;
    }

    void Canvas::fillOpacityMask(
        float width, float height,
        Bitmap* mask, Bitmap* content)
    {
        bitmap_brush_->SetBitmap(content->getNative().get());

        D2D1_RECT_F rect = D2D1::RectF(0, 0, width, height);

        auto mode = render_target_->GetAntialiasMode();
        render_target_->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
        render_target_->FillOpacityMask(
            mask->getNative().get(), bitmap_brush_.get(), D2D1_OPACITY_MASK_CONTENT_GRAPHICS, rect, rect);
        render_target_->SetAntialiasMode(mode);
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
        render_target_->DrawLine(
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
        render_target_->DrawRectangle(d2d_rect, solid_brush_.get());
    }

    void Canvas::drawRect(const RectF& rect, float stroke_width, const Color& color) {
        D2D1_COLOR_F d2d_color {
            color.r, color.g, color.b, color.a, };
        D2D1_RECT_F d2d_rect {
            rect.left, rect.top, rect.right, rect.bottom };

        solid_brush_->SetColor(d2d_color);
        render_target_->DrawRectangle(d2d_rect, solid_brush_.get(), stroke_width);
    }

    void Canvas::fillRect(const RectF& rect, const Color& color) {
        D2D1_COLOR_F d2d_color {
            color.r, color.g, color.b, color.a, };
        D2D1_RECT_F d2d_rect {
            rect.left, rect.top, rect.right, rect.bottom };

        solid_brush_->SetColor(d2d_color);
        render_target_->FillRectangle(d2d_rect, solid_brush_.get());
    }

    void Canvas::drawRoundRect(
        const RectF& rect, float radius, const Color& color)
    {
        D2D1_COLOR_F d2d_color {
            color.r, color.g, color.b, color.a, };
        D2D1_RECT_F d2d_rect {
            rect.left, rect.top, rect.right, rect.bottom };

        solid_brush_->SetColor(d2d_color);
        render_target_->DrawRoundedRectangle(
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
        render_target_->DrawRoundedRectangle(
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
        render_target_->FillRoundedRectangle(
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
        render_target_->DrawEllipse(
            D2D1::Ellipse(D2D1::Point2F(cx, cy), rx, ry),
            solid_brush_.get());
    }

    void Canvas::drawOval(float cx, float cy, float rx, float ry, float stroke_width, const Color& color) {
        D2D1_COLOR_F _color {
            color.r, color.g, color.b, color.a, };
        solid_brush_->SetColor(_color);
        render_target_->DrawEllipse(
            D2D1::Ellipse(
                D2D1::Point2F(cx, cy),
                rx, ry),
            solid_brush_.get(), stroke_width);
    }

    void Canvas::fillOval(float cx, float cy, float rx, float ry, const Color& color) {
        D2D1_COLOR_F _color {
            color.r, color.g, color.b, color.a, };
        solid_brush_->SetColor(_color);
        render_target_->FillEllipse(
            D2D1::Ellipse(D2D1::Point2F(cx, cy), rx, ry), solid_brush_.get());
    }


    void Canvas::fillGeometry(ID2D1Geometry* geo, ID2D1Brush* brush) {
        render_target_->FillGeometry(geo, brush);
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

        render_target_->DrawBitmap(
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
        render_target_->DrawTextW(
            text.c_str(), text.length(), textFormat, d2d_layout_rect, solid_brush_.get());
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
        render_target_->DrawTextLayout(D2D1::Point2F(x, y), textLayout, solid_brush_.get());
    }

    void Canvas::drawTextLayoutWithEffect(
        View* v, float x, float y,
        IDWriteTextLayout* textLayout, const Color& color)
    {
        if (!text_renderer_) {
            text_renderer_ = new TextRenderer(render_target_);
            text_renderer_->setOpacity(opacity_);
        }

        text_renderer_->setTextColor(color);
        textLayout->Draw(v, text_renderer_.get(), x, y);
    }

}