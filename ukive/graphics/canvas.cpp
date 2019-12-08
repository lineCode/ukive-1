#include "ukive/graphics/canvas.h"

#include "utils/log.h"
#include "utils/stl_utils.h"

#include "ukive/application.h"
#include "ukive/text/text_renderer.h"
#include "ukive/window/window.h"
#include "ukive/window/window_impl.h"
#include "ukive/graphics/rect.h"
#include "ukive/graphics/point.h"
#include "ukive/graphics/bitmap.h"
#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/offscreen_buffer.h"


namespace ukive {

    Canvas::Canvas(int width, int height)
        : opacity_(1.f),
          is_own_buffer_(true)
    {
        buffer_ = std::make_shared<OffscreenBuffer>(width, height);
        if (buffer_->onCreate(true)) {
            rt_ = buffer_->getRT();
            initCanvas();
        }
    }

    Canvas::Canvas(const std::shared_ptr<CyroBuffer>& buffer)
        : opacity_(1.f),
          is_own_buffer_(false),
          buffer_(buffer)
    {
        rt_ = buffer_->getRT();
        initCanvas();
    }

    Canvas::~Canvas() {
        if (is_own_buffer_) {
            buffer_->onDestroy();
        }
    }

    void Canvas::initCanvas() {
        layer_counter_ = 0;

        solid_brush_.reset();
        bitmap_brush_.reset();

        rt_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &solid_brush_);
        rt_->CreateBitmapBrush(nullptr, &bitmap_brush_);

        solid_brush_->SetOpacity(opacity_);
        bitmap_brush_->SetOpacity(opacity_);
    }

    void Canvas::releaseResources() {
        rt_.reset();
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
        rt_->Clear();
    }

    void Canvas::clear(const Color& color) {
        D2D1_COLOR_F d2d_color { color.r, color.g, color.b, color.a };
        rt_->Clear(d2d_color);
    }

    void Canvas::beginDraw() {
        buffer_->onBeginDraw();
    }

    bool Canvas::endDraw() {
        return buffer_->onEndDraw();
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

    void Canvas::release() {
        releaseResources();
    }

    void Canvas::refresh() {
        rt_ = buffer_->getRT();
        initCanvas();

        if (text_renderer_) {
            text_renderer_->setOpacity(opacity_);
        }

        solid_brush_->SetOpacity(opacity_);
        bitmap_brush_->SetOpacity(opacity_);
    }

    int Canvas::getWidth() const {
        return rt_->GetPixelSize().width;
    }

    int Canvas::getHeight() const {
        return rt_->GetPixelSize().height;
    }

    CyroBuffer* Canvas::getBuffer() const {
        return buffer_.get();
    }

    std::shared_ptr<Bitmap> Canvas::extractBitmap() const {
        return buffer_->onExtractBitmap();
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

    void Canvas::fillCircle(float cx, float cy, float radius, Bitmap* bmp) {
        bitmap_brush_->SetBitmap(bmp->getNative().get());
        bitmap_brush_->SetExtendModeX(D2D1_EXTEND_MODE_CLAMP);
        bitmap_brush_->SetExtendModeY(D2D1_EXTEND_MODE_CLAMP);
        rt_->FillEllipse(
            D2D1::Ellipse(D2D1::Point2F(cx, cy), radius, radius), bitmap_brush_.get());
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

    void Canvas::fillGeometry(ID2D1Geometry* geo, Bitmap* bmp) {
        bitmap_brush_->SetBitmap(bmp->getNative().get());
        bitmap_brush_->SetExtendModeX(D2D1_EXTEND_MODE_CLAMP);
        bitmap_brush_->SetExtendModeY(D2D1_EXTEND_MODE_CLAMP);
        rt_->FillGeometry(geo, bitmap_brush_.get());
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
            text.c_str(), utl::STLCU32(text.length()), textFormat, d2d_layout_rect, solid_brush_.get());
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
            text.c_str(), utl::STLCU32(text.length()), format, max_width, max_height, &layout);
        if (FAILED(hr)) {
            LOG(Log::WARNING) << "Failed to create text layout: " << hr;
            return {};
        }

        return layout;
    }


}