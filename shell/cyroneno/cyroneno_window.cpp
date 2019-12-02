#include "shell/cyroneno/cyroneno_window.h"

#include <algorithm>
#include <random>

#include "utils/log.h"

#include "ukive/application.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/bitmap_factory.h"
#include "ukive/graphics/point.h"
#include "ukive/views/title_bar/title_bar.h"
#include "ukive/graphics/bitmap.h"
#include "ukive/views/image_view.h"
#include "ukive/graphics/color.h"
#include "ukive/drawable/color_drawable.h"

#include "cyroneno/ray_tracer/ray_tracer.h"
#include "cyroneno/rasterizer/rasterizer.h"
#include "cyroneno/pipeline/pipeline.h"
#include "cyroneno/text/opentype/opentype_font.h"
#include "cyroneno/text/opentype/opentype_rasterizer.h"

#include "shell/resources/oigka_resources_id.h"


namespace {
    enum class Examples {
        RAY_TRACER,
        PIPELINE,
        BASIC_RASTERIZER,
        TEXT_RASTERIZER,
        OTHER,
    };

    constexpr auto IMAGE_WIDTH = 800;
    constexpr auto IMAGE_HEIGHT = 800;
}

namespace shell {

    CyronenoWindow::CyronenoWindow() {
    }

    void CyronenoWindow::onCreate() {
        Window::onCreate();

        showTitleBar();
        setBackgroundColor(ukive::Color::Transparent);
        setContentView(Res::Layout::cyroneno_layout_xml);

        getContentView()->setBackground(new ukive::ColorDrawable(ukive::Color(0.5f, 0.5f, 0.5f)));

        img_view_ = findViewById<ukive::ImageView>(Res::Id::iv_cyroneno_img);

        Examples examples = Examples::BASIC_RASTERIZER;

        switch (examples) {
        case Examples::RAY_TRACER:
        {
            cyro::ImagePng image(IMAGE_WIDTH, IMAGE_HEIGHT);
            cyro::RayTracer ray_tracer;
            ray_tracer.rayTracer(cyro::ProjectionType::ORTHO, IMAGE_WIDTH, IMAGE_HEIGHT, &image);
            auto img_data_ptr = reinterpret_cast<unsigned char*>(image.data_);

            // Save to file
            /*ukive::Application::getWICManager()->saveToPngFile(
                IMAGE_WIDTH, IMAGE_HEIGHT, img_data_ptr, L"test.png");*/

            bmp_ = ukive::BitmapFactory::create(this, IMAGE_WIDTH, IMAGE_HEIGHT, img_data_ptr);
            break;
        }

        case Examples::PIPELINE:
        {
            cyro::Pipeline pipeline(IMAGE_WIDTH, IMAGE_HEIGHT);
            pipeline.launch();
            auto image = pipeline.getOutput();
            auto img_data_ptr = reinterpret_cast<unsigned char*>(image->data_);

            // Save to file
            /*ukive::Application::getWICManager()->saveToPngFile(
                IMAGE_WIDTH, IMAGE_HEIGHT, img_data_ptr, L"test.png");*/

            bmp_ = ukive::BitmapFactory::create(this, IMAGE_WIDTH, IMAGE_HEIGHT, img_data_ptr);
            break;
        }

        case Examples::BASIC_RASTERIZER:
        {
            cyro::Rasterizer rasterizer(IMAGE_WIDTH, IMAGE_HEIGHT);

            //TEST_QUAD_BEZIERS(rasterizer);

            rasterizer.drawCubicBezier(
                cyro::Point2(200, 200),
                cyro::Point2(600, 900),
                cyro::Point2(10, -50),
                cyro::Point2(400, 400),
                cyro::Color(0, 0, 0, 0.5f));

            //TEST_CUBIC_BEZIERS(rasterizer);

            /*rasterizer.drawTriangle(
                {50, 50}, {70, 200}, {300, 50},
                cyro::Color(1, 0, 0, 1),
                cyro::Color(0, 1, 0, 1),
                cyro::Color(0, 0, 1, 1));*/
            auto image = rasterizer.getOutput();
            auto img_data_ptr = reinterpret_cast<unsigned char*>(image->data_);

            // Save to file
            /*ukive::Application::getWICManager()->saveToPngFile(
                IMAGE_WIDTH, IMAGE_HEIGHT, img_data_ptr, L"test.png");*/

            bmp_ = ukive::BitmapFactory::create(this, IMAGE_WIDTH, IMAGE_HEIGHT, img_data_ptr);
            break;
        }

        case Examples::TEXT_RASTERIZER:
        {
            cyro::otf::OpenTypeFont font;
            if (font.parse(L"D:\\msyh.ttc")) {
                auto& head = font.GetHead();
                auto& maxp = font.GetProfile();

                uint16_t glyph_id = 0;
                if (!font.parseGlyphIdForChar(L'赢', &glyph_id)) {
                    return;
                }

                cyro::otf::OpenTypeFont::GlyphInfo glyph;
                if (!font.parseGlyph(glyph_id, &glyph)) {
                    return;
                }

                int dpi_x, dpi_y;
                getDpi(&dpi_x, &dpi_y);

                int font_size = 32;
                float scale = (font_size * dpi_x) / (96.f * head.unit_per_em);

                int width = std::ceil((glyph.glyph_header.max_x - glyph.glyph_header.min_x + 1) * scale);
                int height = std::ceil((glyph.glyph_header.max_y - glyph.glyph_header.min_y + 1) * scale);

                int off_x = 0 - glyph.glyph_header.min_x;
                int off_y = 0 - glyph.glyph_header.min_y;

                cyro::otf::OpenTypeRasterizer rasterizer(width, height);
                rasterizer.drawGlyph(glyph, off_x, off_y, scale, cyro::Color(0, 0, 0, 0.5f));

                auto image = rasterizer.getOutput();
                auto img_data_ptr = reinterpret_cast<unsigned char*>(image->data_);
                bmp_ = ukive::BitmapFactory::create(this, width, height, img_data_ptr);
            }
            break;
        }

        default:
        {
            cyro::ImagePng image(100, 100, cyro::ColorBGRAInt(0, 0, 0, 0));

            std::random_device rd;
            std::default_random_engine en(rd());
            std::uniform_int_distribution<int> user_dist(64, 255);
            std::normal_distribution<float> norm_dist;

            for (int h = 0; h < 100; ++h) {
                for (int w = 0; w < 100; ++w) {
                    int alpha = user_dist(en);
                    //int alpha = norm_dist(en) * 255;
                    image.setColor(w, h, cyro::ColorBGRAInt(0, 0, 0, alpha));
                }
            }
            auto img_data_ptr = reinterpret_cast<unsigned char*>(image.data_);
            bmp_ = ukive::BitmapFactory::create(this, 100, 100, img_data_ptr);

            // Save to file
            ukive::Application::getWICManager()->saveToPngFile(
                100, 100, img_data_ptr, L"test.png");
            break;
        }
        }

        img_view_->setImageBitmap(bmp_);
    }

    void CyronenoWindow::TEST_LINES(cyro::Rasterizer& rz) {
        // 1L, 3L
        rz.drawLine({ 100, 100 }, { 200, 150 }, cyro::Color(0, 0, 0, 0.5));
        rz.drawLine({ 206, 150 }, { 106, 100 }, cyro::Color(0, 0, 0, 0.5));

        // 1H, 3H
        rz.drawLine({ 100, 100 }, { 150, 300 }, cyro::Color(0, 0, 0, 1));
        rz.drawLine({ 156, 300 }, { 106, 100 }, cyro::Color(0, 0, 0, 1));

        // 2L, 4L
        rz.drawLine({ 200, 150 }, { 300, 100 }, cyro::Color(0, 0, 0, 1));
        rz.drawLine({ 306, 100 }, { 206, 150 }, cyro::Color(0, 0, 0, 1));

        // 2H, 4H
        rz.drawLine({ 100, 300 }, { 150, 100 }, cyro::Color(0, 0, 0, 1));
        rz.drawLine({ 156, 100 }, { 106, 300 }, cyro::Color(0, 0, 0, 1));
    }

    void CyronenoWindow::TEST_ELLIPSES(cyro::Rasterizer& rz) {
        rz.drawCircle({ 400, 400 }, 4, cyro::Color(0, 0, 0, 0.5));
        rz.drawEllipse({ 400, 400 }, 4, 3, cyro::Color(0, 0, 0, 0.5));
    }

    void CyronenoWindow::TEST_QUAD_BEZIERS(cyro::Rasterizer& rz) {
        cyro::Point2 p1(200, 400);
        cyro::Point2 p3(400, 360);
        for (int i = 0; i < 360; i += 2) {
            double rad = (3.14159265359 / 180) * i;
            double x = 400 * std::sin(rad);
            double y = 400 * std::cos(rad);
            double x2 = std::round((p1.x + p3.x) / 2 + x);
            double y2 = std::round((p1.y + p3.y) / 2 + y);

            rz.drawQuadBezier(p1, { x2, y2 }, p3, cyro::Color(0, 0, 0, 0.5f));
        }

        if (false)
        {
            // 斜率较大
            rz.drawQuadBezier({ 400, -40 + 400 }, { 400, 400 }, { 10 + 400, 18 + 400 }, cyro::Color(0, 0, 0, 0.5f));
            rz.drawQuadBezier({ 10 + 410, 18 + 400 }, { 410, 400 }, { 410, -40 + 400 }, cyro::Color(0, 0, 0, 0.5f));

            rz.drawQuadBezier({ 450, -40 + 400 }, { 450, 400 }, { -10 + 450, 18 + 400 }, cyro::Color(0, 0, 0, 0.5f));
            rz.drawQuadBezier({ -10 + 460, 18 + 400 }, { 460, 400 }, { 460, -40 + 400 }, cyro::Color(0, 0, 0, 0.5f));

            rz.drawQuadBezier({ 400, 40 + 450 }, { 400, 450 }, { 10 + 400, -18 + 450 }, cyro::Color(0, 0, 0, 0.5f));
            rz.drawQuadBezier({ 10 + 410, -18 + 450 }, { 410, 450 }, { 410, 40 + 450 }, cyro::Color(0, 0, 0, 0.5f));

            rz.drawQuadBezier({ 450, 40 + 450 }, { 450, 450 }, { -10 + 450, -18 + 450 }, cyro::Color(0, 0, 0, 0.5f));
            rz.drawQuadBezier({ -10 + 460, -18 + 450 }, { 460, 450 }, { 460, 40 + 450 }, cyro::Color(0, 0, 0, 0.5f));

            // 斜率较小
            rz.drawQuadBezier({ -40 + 400, 400 }, { 400, 400 }, { 18 + 400, 10 + 400 }, cyro::Color(0, 0, 0, 0.5f));
            rz.drawQuadBezier({ 18 + 400, 10 + 410 }, { 400, 410 }, { -40 + 400, 410 }, cyro::Color(0, 0, 0, 0.5f));

            rz.drawQuadBezier({ -40 + 400, 450 }, { 400, 450 }, { 18 + 400, -10 + 450 }, cyro::Color(0, 0, 0, 0.5f));
            rz.drawQuadBezier({ 18 + 400, -10 + 460 }, { 400, 460 }, { -40 + 400, 460 }, cyro::Color(0, 0, 0, 0.5f));

            rz.drawQuadBezier({ 40 + 450, 400 }, { 450, 400 }, { -18 + 450, 10 + 400 }, cyro::Color(0, 0, 0, 0.5f));
            rz.drawQuadBezier({ -18 + 450, 10 + 410 }, { 450, 410 }, { 40 + 450, 410 }, cyro::Color(0, 0, 0, 0.5f));

            rz.drawQuadBezier({ 40 + 450, 450 }, { 450, 450 }, { -18 + 450, -10 + 450 }, cyro::Color(0, 0, 0, 0.5f));
            rz.drawQuadBezier({ -18 + 450, -10 + 460 }, { 450, 460 }, { 40 + 450, 460 }, cyro::Color(0, 0, 0, 0.5f));

            //rz.drawBezier({ -20 + 400, 15 + 400 }, { 400, 400 }, { 10 + 400, -20 + 400 }, cyro::Color(0, 0, 0, 0.5f));
            //rz.drawBezier({ 420, 418 }, { 400, 400 }, { 410, 360 }, cyro::Color(0, 0, 0, 0.5f));
        }

        rz.drawQuadBezier(
            { 400, 400 }, { 200, 450 }, { 400, 500 },
            0.5, 0.01, 0.5,
            cyro::Color(0, 0, 0, 0.5f));

        if (false)
        {
            rz.drawQuadBezier({ 400, 400 }, { 200, 450 }, { 400, 500 }, cyro::Color(0, 0, 0, 0.5f));
            rz.drawQuadBezier({ 400, 500 }, { 600, 550 }, { 400, 600 }, cyro::Color(0, 0, 0, 0.5f));

            rz.drawQuadBezier({ 400, 300 }, { 350, 500 }, { 300, 300 }, cyro::Color(0, 0, 0, 0.5f));
            rz.drawQuadBezier({ 300, 300 }, { 350, 500 }, { 400, 300 }, cyro::Color(0, 0, 0, 0.5f));

            rz.drawQuadBezier({ 500, 300 }, { 450, 100 }, { 400, 300 }, cyro::Color(0, 0, 0, 0.5f));
            rz.drawQuadBezier({ 400, 300 }, { 450, 100 }, { 500, 300 }, cyro::Color(0, 0, 0, 0.5f));
        }
    }

    void CyronenoWindow::TEST_CUBIC_BEZIERS(cyro::Rasterizer& rz) {
        if (false)
        {
            rz.drawCubicBezier(
                cyro::Point2(200, 200),
                cyro::Point2(200, 250),
                cyro::Point2(250, 300),
                cyro::Point2(300, 300),
                cyro::Color(0, 0, 0, 0.5f));

            rz.drawCubicBezier(
                cyro::Point2(200, 200),
                cyro::Point2(200, 300),
                cyro::Point2(300, 250),
                cyro::Point2(400, 500),
                cyro::Color(0, 0, 0, 0.5f));

            rz.drawCubicBezier(
                cyro::Point2(200, 200),
                cyro::Point2(500, 500),
                cyro::Point2(200, 500),
                cyro::Point2(500, 200),
                cyro::Color(0, 0, 0, 0.5f));

            rz.drawCubicBezier(
                cyro::Point2(200, 200),
                cyro::Point2(550, 550),
                cyro::Point2(150, 550),
                cyro::Point2(500, 200),
                cyro::Color(0, 0, 0, 0.5f));

            rz.drawCubicBezier(
                cyro::Point2(200, 200),
                cyro::Point2(600, 900),
                cyro::Point2(-50, 50),
                cyro::Point2(400, 400),
                cyro::Color(0, 0, 0, 0.5f));
        }

        if (false)
        {
            double t1 = 0.2;
            double t2 = 0.5;

            cyro::Point2 p0(200, 200);
            cyro::Point2 p1(250, 400);
            cyro::Point2 p2(400, 450);
            cyro::Point2 p3(700, 350);

            rz.drawLine(p0, p1, cyro::Color(0, 0, 0, 1));
            rz.drawLine(p1, p2, cyro::Color(0, 0, 0, 1));
            rz.drawLine(p2, p3, cyro::Color(0, 0, 0, 1));

            {
                double t21 = (t2 - t1) / (1 - t1);

                auto pt01 = p0 + (p1 - p0)*t1;
                auto p11 = p1 + (p2 - p1)*t1;
                auto p21 = p2 + (p3 - p2)*t1;
                auto pt02 = pt01 + (p11 - pt01)*t1;
                auto pt04 = p11 + (p21 - p11)*t1;
                auto pt03 = pt02 + (pt04 - pt02)*t1;

                rz.drawLine(pt01, p11, cyro::Color(0, 0, 0, 1));
                rz.drawLine(p11, p21, cyro::Color(0, 0, 0, 1));
                rz.drawLine(pt02, pt04, cyro::Color(0, 0, 0, 1));
                rz.drawCircle(pt03, 2, cyro::Color(0, 0, 0, 1));

                auto pt11 = pt03 + (pt04 - pt03)*t21;
                auto pt14 = pt04 + (p21 - pt04)*t21;
                auto pt16 = p21 + (p3 - p21)*t21;
                auto pt15 = pt14 + (pt16 - pt14)*t21;
                auto pt12 = pt11 + (pt14 - pt11)*t21;
                auto pt13 = pt12 + (pt15 - pt12)*t21;

                rz.drawLine(pt11, pt14, cyro::Color(0, 0, 1, 1));
                rz.drawLine(pt14, pt16, cyro::Color(0, 0, 1, 1));
                rz.drawLine(pt12, pt15, cyro::Color(0, 0, 1, 1));
                rz.drawCircle(pt13, 2, cyro::Color(0, 0, 1, 1));
            }

            /*{
                auto pt01 = p0 + (p1 - p0)*t2;
                auto p11 = p1 + (p2 - p1)*t2;
                auto p21 = p2 + (p3 - p2)*t2;
                auto pt02 = pt01 + (p11 - pt01)*t2;
                auto pt04 = p11 + (p21 - p11)*t2;
                auto pt03 = pt02 + (pt04 - pt02)*t2;

                rz.drawLine(pt01, p11, cyro::Color(0.8f, 0.6f, 0, 1));
                rz.drawLine(p11, p21, cyro::Color(0.8f, 0.6f, 0, 1));
                rz.drawLine(pt02, pt04, cyro::Color(0.8f, 0.6f, 0, 1));
                rz.drawCircle(pt03, 2, cyro::Color(0.8f, 0.6f, 0, 1));
            }*/
        }
    }

}