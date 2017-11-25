#include "canvas.h"

#include "ukive/text/text_renderer.h"


namespace ukive {

    Canvas::Canvas(ComPtr<ID2D1RenderTarget> renderTarget)
    {
        opacity_ = 1.f;
        layer_counter_ = 0;
        mTextRenderer = nullptr;
        matrix_ = D2D1::Matrix3x2F::Identity();

        render_target_ = renderTarget;
        render_target_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &solid_brush_);
        render_target_->CreateBitmapBrush(nullptr, &bitmap_brush_);
    }


    Canvas::~Canvas()
    {
        if (mTextRenderer)
            delete mTextRenderer;
    }


    void Canvas::setOpacity(float opacity)
    {
        if (opacity == opacity_)
            return;

        opacity_ = opacity;

        if (mTextRenderer)
            mTextRenderer->setOpacity(opacity_);
    }

    float Canvas::getOpacity()
    {
        return opacity_;
    }


    void Canvas::beginDraw()
    {
        render_target_->BeginDraw();
    }

    void Canvas::endDraw()
    {
        render_target_->EndDraw();
    }


    void Canvas::popClip()
    {
        render_target_->PopAxisAlignedClip();
    }

    void Canvas::pushClip(D2D1_RECT_F &rect)
    {
        render_target_->PushAxisAlignedClip(
            rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    }


    void Canvas::pushLayer(ID2D1Geometry *clipGeometry)
    {
        if (layer_counter_ > 0)
        {
            ++layer_counter_;
            return;
        }

        ComPtr<ID2D1DeviceContext> d2dDC
            = render_target_.cast<ID2D1DeviceContext>();
        if (d2dDC == nullptr)
        {
            if (layer_ == nullptr)
            {
                HRESULT hr = render_target_->CreateLayer(&layer_);
                if (FAILED(hr))
                    throw std::runtime_error("Canvas-Constructor(): Create layer failed.");
            }

            render_target_->PushLayer(
                D2D1::LayerParameters(D2D1::InfiniteRect(), clipGeometry),
                layer_.get());
        }
        else
        {
            d2dDC->PushLayer(
                D2D1::LayerParameters1(D2D1::InfiniteRect(), clipGeometry),
                nullptr);
        }

        ++layer_counter_;
    }

    void Canvas::pushLayer(D2D1_RECT_F &contentBound, ID2D1Geometry *clipGeometry)
    {
        if (layer_counter_ > 0)
        {
            ++layer_counter_;
            return;
        }

        ComPtr<ID2D1DeviceContext> d2dDC
            = render_target_.cast<ID2D1DeviceContext>();
        if (d2dDC == nullptr)
        {
            if (layer_ == nullptr)
            {
                HRESULT hr = render_target_->CreateLayer(&layer_);
                if (FAILED(hr))
                    throw std::runtime_error("Canvas-Constructor(): Create layer failed.");
            }

            render_target_->PushLayer(
                D2D1::LayerParameters(contentBound, clipGeometry),
                layer_.get());
        }
        else
        {
            d2dDC->PushLayer(
                D2D1::LayerParameters1(contentBound, clipGeometry),
                nullptr);
        }

        ++layer_counter_;
    }

    void Canvas::popLayer()
    {
        if (layer_counter_ > 1)
        {
            --layer_counter_;
            return;
        }

        render_target_->PopLayer();

        --layer_counter_;
    }


    void Canvas::save()
    {
        ID2D1Factory *factory = nullptr;
        render_target_->GetFactory(&factory);

        ID2D1DrawingStateBlock *drawingStateBlock;
        factory->CreateDrawingStateBlock(&drawingStateBlock);
        render_target_->SaveDrawingState(drawingStateBlock);

        opacity_stack_.push(opacity_);
        drawing_state_stack_.push(drawingStateBlock);
    }

    void Canvas::restore()
    {
        if (drawing_state_stack_.empty())
            return;

        opacity_ = opacity_stack_.top();

        ID2D1DrawingStateBlock *drawingStateBlock
            = drawing_state_stack_.top().get();

        D2D1_DRAWING_STATE_DESCRIPTION desc;
        drawingStateBlock->GetDescription(&desc);

        D2D1_MATRIX_3X2_F matrix = desc.transform;

        matrix_._11 = matrix._11;
        matrix_._12 = matrix._12;
        matrix_._21 = matrix._21;
        matrix_._22 = matrix._22;
        matrix_._31 = matrix._31;
        matrix_._32 = matrix._32;

        render_target_->RestoreDrawingState(drawingStateBlock);

        opacity_stack_.pop();
        drawing_state_stack_.pop();

        if (mTextRenderer)
            mTextRenderer->setOpacity(opacity_);
    }

    ID2D1RenderTarget *Canvas::getRT()
    {
        return render_target_.get();
    }


    void Canvas::scale(float sx, float sy)
    {
        scale(sx, sy, 0.f, 0.f);
    }

    void Canvas::scale(float sx, float sy, float cx, float cy)
    {
        D2D1::Matrix3x2F tmp = D2D1::Matrix3x2F::Scale(D2D1::SizeF(sx, sy), D2D1::Point2F(cx, cy));
        matrix_ = matrix_*tmp;
        render_target_->SetTransform(matrix_);
    }

    void Canvas::rotate(float angle)
    {
        rotate(angle, 0.f, 0.f);
    }

    void Canvas::rotate(float angle, float cx, float cy)
    {
        D2D1::Matrix3x2F tmp = D2D1::Matrix3x2F::Rotation(angle, D2D1::Point2F(cx, cy));
        matrix_ = matrix_*tmp;
        render_target_->SetTransform(matrix_);
    }

    void Canvas::translate(int dx, int dy)
    {
        this->translate(
            static_cast<float>(dx),
            static_cast<float>(dy));
    }

    void Canvas::translate(float dx, float dy)
    {
        D2D1::Matrix3x2F tmp = D2D1::Matrix3x2F::Translation(D2D1::SizeF(dx, dy));
        matrix_ = matrix_*tmp;
        render_target_->SetTransform(matrix_);
    }


    void Canvas::setMatrix(D2D1::Matrix3x2F matrix)
    {
        matrix_ = matrix;
        render_target_->SetTransform(matrix_);
    }

    D2D1::Matrix3x2F Canvas::getMatrix()
    {
        return matrix_;
    }


    void Canvas::fillOpacityMask(
        float width, float height,
        ID2D1Bitmap *mask, ID2D1Bitmap *content)
    {
        bitmap_brush_->SetBitmap(content);

        D2D1_RECT_F rect = D2D1::RectF(0, 0, width, height);

        render_target_->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
        render_target_->FillOpacityMask(
            mask, bitmap_brush_.get(), D2D1_OPACITY_MASK_CONTENT_GRAPHICS, rect, rect);
        render_target_->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    }


    void Canvas::drawRect(D2D1_RECT_F &rect, Color &color)
    {
        D2D1_COLOR_F _color = {
            color.r,
            color.g,
            color.b,
            color.a, };
        solid_brush_->SetColor(_color);
        render_target_->DrawRectangle(rect, solid_brush_.get());
    }

    void Canvas::drawRect(D2D1_RECT_F &rect, float strokeWidth, Color &color)
    {
        D2D1_COLOR_F _color = {
            color.r,
            color.g,
            color.b,
            color.a, };
        solid_brush_->SetColor(_color);
        render_target_->DrawRectangle(rect, solid_brush_.get(), strokeWidth);
    }


    void Canvas::fillRect(D2D1_RECT_F &rect, Color &color)
    {
        D2D1_COLOR_F _color = {
            color.r,
            color.g,
            color.b,
            color.a, };
        solid_brush_->SetColor(_color);
        render_target_->FillRectangle(rect, solid_brush_.get());
    }


    void Canvas::drawRoundRect(
        D2D1_RECT_F &rect, float radius, Color &color)
    {
        D2D1_COLOR_F _color = {
            color.r,
            color.g,
            color.b,
            color.a, };
        solid_brush_->SetColor(_color);
        render_target_->DrawRoundedRectangle(
            D2D1::RoundedRect(rect, radius, radius), solid_brush_.get());
    }

    void Canvas::drawRoundRect(
        D2D1_RECT_F &rect, float strokeWidth,
        float radius, Color &color)
    {
        D2D1_COLOR_F _color = {
            color.r,
            color.g,
            color.b,
            color.a, };
        solid_brush_->SetColor(_color);
        render_target_->DrawRoundedRectangle(
            D2D1::RoundedRect(rect, radius, radius), solid_brush_.get(), strokeWidth);
    }

    void Canvas::fillRoundRect(
        D2D1_RECT_F &rect, float radius, Color &color)
    {
        D2D1_COLOR_F _color = {
            color.r,
            color.g,
            color.b,
            color.a, };
        solid_brush_->SetColor(_color);
        render_target_->FillRoundedRectangle(
            D2D1::RoundedRect(rect, radius, radius), solid_brush_.get());
    }


    void Canvas::drawCircle(float cx, float cy, float radius, Color &color)
    {
        drawOval(cx, cy, radius, radius, color);
    }

    void Canvas::drawCircle(float cx, float cy, float radius, float strokeWidth, Color &color)
    {
        drawOval(cx, cy, radius, radius, strokeWidth, color);
    }

    void Canvas::fillCircle(float cx, float cy, float radius, Color &color)
    {
        fillOval(cx, cy, radius, radius, color);
    }


    void Canvas::drawOval(float cx, float cy, float radiusX, float radiusY, Color &color)
    {
        D2D1_COLOR_F _color = {
            color.r,
            color.g,
            color.b,
            color.a, };
        solid_brush_->SetColor(_color);
        render_target_->DrawEllipse(
            D2D1::Ellipse(
                D2D1::Point2F(cx, cy),
                radiusX, radiusY),
            solid_brush_.get());
    }

    void Canvas::drawOval(float cx, float cy, float radiusX, float radiusY, float strokeWidth, Color &color)
    {
        D2D1_COLOR_F _color = {
            color.r,
            color.g,
            color.b,
            color.a, };
        solid_brush_->SetColor(_color);
        render_target_->DrawEllipse(
            D2D1::Ellipse(
                D2D1::Point2F(cx, cy),
                radiusX, radiusY),
            solid_brush_.get(), strokeWidth);
    }

    void Canvas::fillOval(float cx, float cy, float radiusX, float radiusY, Color &color)
    {
        D2D1_COLOR_F _color = {
            color.r,
            color.g,
            color.b,
            color.a, };
        solid_brush_->SetColor(_color);
        render_target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), radiusX, radiusY), solid_brush_.get());
    }


    void Canvas::drawBitmap(ID2D1Bitmap *bitmap)
    {
        if (bitmap == nullptr)
            return;

        D2D1_SIZE_F size = bitmap->GetSize();
        D2D1_RECT_F srcRect = D2D1::RectF(0.f, 0.f, size.width, size.height);

        drawBitmap(srcRect, srcRect, 1.f, bitmap);
    }

    void Canvas::drawBitmap(float x, float y, ID2D1Bitmap *bitmap)
    {
        if (bitmap == nullptr)
            return;

        D2D1_SIZE_F size = bitmap->GetSize();
        D2D1_RECT_F srcRect = D2D1::RectF(0.f, 0.f, size.width, size.height);
        D2D1_RECT_F dstRect = D2D1::RectF(x, y, size.width + x, size.height + y);

        drawBitmap(srcRect, dstRect, 1.f, bitmap);
    }

    void Canvas::drawBitmap(D2D1_RECT_F &dst, float opacity, ID2D1Bitmap *bitmap)
    {
        if (bitmap == nullptr)
            return;

        D2D1_SIZE_F size = bitmap->GetSize();
        D2D1_RECT_F srcRect = D2D1::RectF(0.f, 0.f, size.width, size.height);

        drawBitmap(srcRect, dst, opacity, bitmap);
    }

    void Canvas::drawBitmap(D2D1_RECT_F &src, D2D1_RECT_F &dst, float opacity, ID2D1Bitmap *bitmap)
    {
        if (bitmap == nullptr)
            return;

        render_target_->DrawBitmap(
            bitmap, dst, opacity,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            src);
    }


    void Canvas::drawText(
        std::wstring text, IDWriteTextFormat *textFormat,
        D2D1_RECT_F &layoutRect, Color &color)
    {
        if (textFormat == nullptr)
            return;

        D2D1_COLOR_F _color = {
            color.r,
            color.g,
            color.b,
            color.a, };
        solid_brush_->SetColor(_color);
        render_target_->DrawTextW(text.c_str(), text.length(), textFormat, layoutRect, solid_brush_.get());
    }

    void Canvas::drawTextLayout(
        float x, float y,
        IDWriteTextLayout *textLayout, Color &color)
    {
        if (textLayout == nullptr)
            return;

        D2D1_COLOR_F _color = {
            color.r,
            color.g,
            color.b,
            color.a, };
        solid_brush_->SetColor(_color);
        render_target_->DrawTextLayout(D2D1::Point2F(x, y), textLayout, solid_brush_.get());
    }

    void Canvas::drawTextLayoutWithEffect(
        View *widget,
        float x, float y,
        IDWriteTextLayout *textLayout, Color &color)
    {
        if (mTextRenderer == nullptr)
        {
            mTextRenderer = new TextRenderer(render_target_);
            mTextRenderer->setOpacity(opacity_);
        }

        mTextRenderer->setTextColor(color);
        textLayout->Draw(widget, mTextRenderer, x, y);
    }

}