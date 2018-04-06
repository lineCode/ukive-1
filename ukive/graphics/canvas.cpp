#include "canvas.h"

#include <algorithm>

#include "ukive/application.h"
#include "ukive/graphics/bitmap_factory.h"
#include "ukive/text/text_renderer.h"
#include "ukive/window/window.h"
#include "ukive/graphics/renderer.h"
#include "ukive/graphics/rect.h"
#include "ukive/graphics/bitmap.h"
#include "ukive/log.h"


namespace ukive {

    Canvas::Canvas(Window* win, int width, int height)
        :is_bmp_target_(true) {

        ComPtr<ID2D1BitmapRenderTarget> bmp_target;
        HRESULT hr = win->getRenderer()->getD2DDeviceContext()->CreateCompatibleRenderTarget(
            D2D1::SizeF(width, height), &bmp_target);
        if (FAILED(hr)) {
            Log::e(L"Canvas", L"cannot create bitmap render target.");
            return;
        }

        initCanvas(bmp_target.cast<ID2D1RenderTarget>());
    }

    Canvas::Canvas(ComPtr<ID2D1RenderTarget> renderTarget)
        :is_bmp_target_(false) {
        initCanvas(renderTarget);
    }


    Canvas::~Canvas() {
    }

    void Canvas::initCanvas(ComPtr<ID2D1RenderTarget> renderTarget) {
        opacity_ = 1.f;
        layer_counter_ = 0;

        render_target_ = renderTarget;
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
        D2D1_COLOR_F d2d_color = { color.r, color.g, color.b, color.a };
        render_target_->Clear(d2d_color);
    }


    void Canvas::beginDraw() {
        render_target_->BeginDraw();
    }

    void Canvas::endDraw() {
        HRESULT hr = render_target_->EndDraw();
        if (FAILED(hr)) {
            Log::e(L"Canvas", L"failed to end draw.");
        }
    }


    void Canvas::popClip() {
        render_target_->PopAxisAlignedClip();
    }

    void Canvas::pushClip(const RectF& rect) {
        D2D1_RECT_F d2d_rect = { rect.left, rect.top, rect.right, rect.bottom };
        render_target_->PushAxisAlignedClip(
            d2d_rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    }


    void Canvas::pushLayer(ID2D1Geometry* clipGeometry) {
        if (layer_counter_ > 0) {
            ++layer_counter_;
            return;
        }

        auto d2d_dc = render_target_.cast<ID2D1DeviceContext>();
        if (d2d_dc == nullptr) {
            if (layer_ == nullptr) {
                HRESULT hr = render_target_->CreateLayer(&layer_);
                if (FAILED(hr))
                    throw std::runtime_error("Canvas-Constructor(): Create layer failed.");
            }

            render_target_->PushLayer(
                D2D1::LayerParameters(D2D1::InfiniteRect(), clipGeometry),
                layer_.get());
        } else {
            d2d_dc->PushLayer(
                D2D1::LayerParameters1(D2D1::InfiniteRect(), clipGeometry),
                nullptr);
        }

        ++layer_counter_;
    }

    void Canvas::pushLayer(const RectF& content_bound, ID2D1Geometry* clipGeometry) {
        if (layer_counter_ > 0) {
            ++layer_counter_;
            return;
        }

        D2D1_RECT_F d2d_rect = {
            content_bound.left, content_bound.top,
            content_bound.right, content_bound.bottom };

        auto d2d_dc = render_target_.cast<ID2D1DeviceContext>();
        if (d2d_dc == nullptr) {
            if (layer_ == nullptr) {
                HRESULT hr = render_target_->CreateLayer(&layer_);
                if (FAILED(hr)) {
                    throw std::runtime_error("Canvas-Constructor(): Create layer failed.");
                }
            }

            render_target_->PushLayer(
                D2D1::LayerParameters(d2d_rect, clipGeometry),
                layer_.get());
        } else {
            d2d_dc->PushLayer(
                D2D1::LayerParameters1(d2d_rect, clipGeometry),
                nullptr);
        }

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

        ID2D1DrawingStateBlock* drawingStateBlock
            = drawing_state_stack_.top().get();

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

    std::shared_ptr<Bitmap> Canvas::extractBitmap() {
        if (is_bmp_target_) {
            auto bmp_target = render_target_.cast<ID2D1BitmapRenderTarget>();

            ComPtr<ID2D1Bitmap> bitmap;
            if (FAILED(bmp_target->GetBitmap(&bitmap))) {
                Log::e(L"Canvas", L"failed to extract bitmap.");
                return std::shared_ptr<Bitmap>();
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
        matrix_ = matrix;
        render_target_->SetTransform(matrix_.getNative());
    }

    Matrix Canvas::getMatrix() {
        return matrix_;
    }


    void Canvas::fillOpacityMask(
        float width, float height,
        Bitmap* mask, Bitmap* content) {

        bitmap_brush_->SetBitmap(content->getNative().get());

        D2D1_RECT_F rect = D2D1::RectF(0, 0, width, height);

        auto mode = render_target_->GetAntialiasMode();
        render_target_->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
        render_target_->FillOpacityMask(
            mask->getNative().get(), bitmap_brush_.get(), D2D1_OPACITY_MASK_CONTENT_GRAPHICS, rect, rect);
        render_target_->SetAntialiasMode(mode);
    }


    void Canvas::drawRect(const RectF& rect, const Color& color) {
        D2D1_COLOR_F d2d_color = {
            color.r, color.g, color.b, color.a, };
        D2D1_RECT_F d2d_rect = {
            rect.left, rect.top, rect.right, rect.bottom };

        solid_brush_->SetColor(d2d_color);
        render_target_->DrawRectangle(d2d_rect, solid_brush_.get());
    }

    void Canvas::drawRect(const RectF& rect, float strokeWidth, const Color& color) {
        D2D1_COLOR_F d2d_color = {
            color.r, color.g, color.b, color.a, };
        D2D1_RECT_F d2d_rect = {
            rect.left, rect.top, rect.right, rect.bottom };

        solid_brush_->SetColor(d2d_color);
        render_target_->DrawRectangle(d2d_rect, solid_brush_.get(), strokeWidth);
    }


    void Canvas::fillRect(const RectF& rect, const Color& color) {
        D2D1_COLOR_F d2d_color = {
            color.r, color.g, color.b, color.a, };
        D2D1_RECT_F d2d_rect = {
            rect.left, rect.top, rect.right, rect.bottom };

        solid_brush_->SetColor(d2d_color);
        render_target_->FillRectangle(d2d_rect, solid_brush_.get());
    }


    void Canvas::drawRoundRect(
        const RectF& rect, float radius, const Color& color) {

        D2D1_COLOR_F d2d_color = {
            color.r, color.g, color.b, color.a, };
        D2D1_RECT_F d2d_rect = {
            rect.left, rect.top, rect.right, rect.bottom };

        solid_brush_->SetColor(d2d_color);
        render_target_->DrawRoundedRectangle(
            D2D1::RoundedRect(d2d_rect, radius, radius), solid_brush_.get());
    }

    void Canvas::drawRoundRect(
        const RectF& rect, float strokeWidth,
        float radius, const Color& color) {

        D2D1_COLOR_F d2d_color = {
            color.r, color.g, color.b, color.a, };
        D2D1_RECT_F d2d_rect = {
            rect.left, rect.top, rect.right, rect.bottom };

        solid_brush_->SetColor(d2d_color);
        render_target_->DrawRoundedRectangle(
            D2D1::RoundedRect(d2d_rect, radius, radius), solid_brush_.get(), strokeWidth);
    }

    void Canvas::fillRoundRect(
        const RectF& rect, float radius, const Color& color) {

        D2D1_COLOR_F d2d_color = {
            color.r, color.g, color.b, color.a, };
        D2D1_RECT_F d2d_rect = {
            rect.left, rect.top, rect.right, rect.bottom };

        solid_brush_->SetColor(d2d_color);
        render_target_->FillRoundedRectangle(
            D2D1::RoundedRect(d2d_rect, radius, radius), solid_brush_.get());
    }


    void Canvas::drawCircle(float cx, float cy, float radius, const Color& color) {
        drawOval(cx, cy, radius, radius, color);
    }

    void Canvas::drawCircle(float cx, float cy, float radius, float strokeWidth, const Color& color) {
        drawOval(cx, cy, radius, radius, strokeWidth, color);
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

    void Canvas::drawCircle(const RectF& rect, float strokeWidth, const Color& color) {
        float cx = rect.left + rect.width() / 2;
        float cy = rect.top + rect.height() / 2;
        float radius = std::min(rect.width() / 2, rect.height() / 2);

        drawOval(cx, cy, radius, radius, strokeWidth, color);
    }

    void Canvas::fillCircle(const RectF& rect, const Color& color) {
        float cx = rect.left + rect.width() / 2;
        float cy = rect.top + rect.height() / 2;
        float radius = std::min(rect.width() / 2, rect.height() / 2);

        fillOval(cx, cy, radius, radius, color);
    }


    void Canvas::drawOval(float cx, float cy, float radiusX, float radiusY, const Color& color) {
        D2D1_COLOR_F _color = {
            color.r, color.g, color.b, color.a, };
        solid_brush_->SetColor(_color);
        render_target_->DrawEllipse(
            D2D1::Ellipse(
                D2D1::Point2F(cx, cy),
                radiusX, radiusY),
            solid_brush_.get());
    }

    void Canvas::drawOval(float cx, float cy, float radiusX, float radiusY, float strokeWidth, const Color& color) {
        D2D1_COLOR_F _color = {
            color.r, color.g, color.b, color.a, };
        solid_brush_->SetColor(_color);
        render_target_->DrawEllipse(
            D2D1::Ellipse(
                D2D1::Point2F(cx, cy),
                radiusX, radiusY),
            solid_brush_.get(), strokeWidth);
    }

    void Canvas::fillOval(float cx, float cy, float radiusX, float radiusY, const Color& color) {
        D2D1_COLOR_F _color = {
            color.r, color.g, color.b, color.a, };
        solid_brush_->SetColor(_color);
        render_target_->FillEllipse(
            D2D1::Ellipse(D2D1::Point2F(cx, cy), radiusX, radiusY), solid_brush_.get());
    }


    void Canvas::fillGeometry(ID2D1Geometry* geo, ID2D1Brush* brush) {
        render_target_->FillGeometry(geo, brush);
    }


    void Canvas::drawBitmap(Bitmap* bitmap) {
        if (bitmap == nullptr) {
            return;
        }

        RectF srcRect(0.f, 0.f, bitmap->getWidth(), bitmap->getHeight());

        drawBitmap(srcRect, srcRect, 1.f, bitmap);
    }

    void Canvas::drawBitmap(float x, float y, Bitmap* bitmap) {
        if (bitmap == nullptr) {
            return;
        }

        auto width = bitmap->getWidth();
        auto height = bitmap->getHeight();
        RectF srcRect(0.f, 0.f, width, height);
        RectF dstRect(x, y, width + x, height + y);

        drawBitmap(srcRect, dstRect, 1.f, bitmap);
    }

    void Canvas::drawBitmap(const RectF& dst, float opacity, Bitmap* bitmap) {
        if (bitmap == nullptr) {
            return;
        }

        RectF srcRect(0.f, 0.f, bitmap->getWidth(), bitmap->getHeight());

        drawBitmap(srcRect, dst, opacity, bitmap);
    }

    void Canvas::drawBitmap(const RectF& src, const RectF& dst, float opacity, Bitmap* bitmap) {
        if (bitmap == nullptr) {
            return;
        }

        D2D1_RECT_F d2d_src_rect = {
            src.left, src.top, src.right, src.bottom };
        D2D1_RECT_F d2d_dst_rect = {
            dst.left, dst.top, dst.right, dst.bottom };

        render_target_->DrawBitmap(
            bitmap->getNative().get(), d2d_dst_rect, opacity,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            d2d_src_rect);
    }


    void Canvas::drawText(
        std::wstring text, IDWriteTextFormat* textFormat,
        const RectF& layoutRect, const Color& color) {

        if (textFormat == nullptr) {
            return;
        }

        D2D1_COLOR_F d2d_color = {
            color.r, color.g, color.b, color.a, };
        D2D1_RECT_F d2d_layout_rect = {
            layoutRect.left, layoutRect.top, layoutRect.right, layoutRect.bottom };

        solid_brush_->SetColor(d2d_color);
        render_target_->DrawTextW(
            text.c_str(), text.length(), textFormat, d2d_layout_rect, solid_brush_.get());
    }

    void Canvas::drawTextLayout(
        float x, float y,
        IDWriteTextLayout* textLayout, const Color& color) {

        if (textLayout == nullptr) {
            return;
        }

        D2D1_COLOR_F d2d_color = {
            color.r, color.g, color.b, color.a, };
        solid_brush_->SetColor(d2d_color);
        render_target_->DrawTextLayout(D2D1::Point2F(x, y), textLayout, solid_brush_.get());
    }

    void Canvas::drawTextLayoutWithEffect(
        View* v, float x, float y,
        IDWriteTextLayout* textLayout, const Color& color) {

        if (text_renderer_ == nullptr) {
            text_renderer_ = new TextRenderer(render_target_);
            text_renderer_->setOpacity(opacity_);
        }

        text_renderer_->setTextColor(color);
        textLayout->Draw(v, text_renderer_.get(), x, y);
    }

}