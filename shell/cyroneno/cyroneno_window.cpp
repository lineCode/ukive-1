#include "shell/cyroneno/cyroneno_window.h"

#include <algorithm>

#include "ukive/application.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/bitmap_factory.h"
#include "ukive/graphics/point.h"
#include "ukive/log.h"
#include "ukive/views/title_bar/title_bar.h"
#include "ukive/graphics/bitmap.h"
#include "ukive/views/image_view.h"
#include "ukive/graphics/color.h"
#include "ukive/drawable/color_drawable.h"

#include "shell/cyroneno/ray_tracer.h"
#include "shell/cyroneno/pipeline/rasterizer.h"
#include "shell/cyroneno/pipeline/pipeline.h"
#include "shell/cyroneno/text/opentype/opentype_font.h"
#include "shell/cyroneno/text/opentype/opentype_rasterizer.h"
#include "shell/resources/oigka_resources_id.h"


namespace {
    enum class Examples {
        RAY_TRACER,
        OBJ_RASTERIZER,
        TEXT_RASTERIZER,
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

        Examples examples = Examples::TEXT_RASTERIZER;
        if (examples == Examples::RAY_TRACER) {
            cyro::RayTracer ray_tracer;
            auto image = ray_tracer.rayTracer(cyro::ProjectionType::ORTHO, IMAGE_WIDTH, IMAGE_HEIGHT);
            auto img_data_ptr = reinterpret_cast<unsigned char*>(image.data_.data());

            // Save to file
            /*ukive::Application::getWICManager()->saveToPngFile(
                IMAGE_WIDTH, IMAGE_HEIGHT, img_data_ptr, L"test.png");*/

            bmp_ = ukive::BitmapFactory::create(this, IMAGE_WIDTH, IMAGE_HEIGHT, img_data_ptr);
        } else if (examples == Examples::OBJ_RASTERIZER) {
            /*cyro::Rasterizer rasterizer(IMAGE_WIDTH, IMAGE_HEIGHT);
            rasterizer.drawLine({100, 100}, {200, 99}, cyro::Color(0, 0, 0, 1));
            rasterizer.drawTriangle(
                {50, 50}, {70, 200}, {300, 50},
                cyro::Color(1, 0, 0, 1),
                cyro::Color(0, 1, 0, 1),
                cyro::Color(0, 0, 1, 1));
            auto image = rasterizer.getOutput();*/

            cyro::Pipeline pipeline(IMAGE_WIDTH, IMAGE_HEIGHT);
            pipeline.launch();
            auto image = pipeline.getOutput();

            auto img_data_ptr = reinterpret_cast<unsigned char*>(image.data_.data());

            // Save to file
            /*ukive::Application::getWICManager()->saveToPngFile(
                IMAGE_WIDTH, IMAGE_HEIGHT, img_data_ptr, L"test.png");*/

            bmp_ = ukive::BitmapFactory::create(this, IMAGE_WIDTH, IMAGE_HEIGHT, img_data_ptr);
        } else if (examples == Examples::TEXT_RASTERIZER) {
            cyro::otf::OpenTypeFont font;
            if (font.parse(L"D:\\msyh.ttc")) {
                auto& head = font.GetHead();
                auto& maxp = font.GetProfile();

                uint16_t glyph_id = 0;
                if (!font.parseGlyphIdForChar(L'Œ“', &glyph_id)) {
                    return;
                }

                cyro::otf::OpenTypeFont::GlyphInfo glyph;
                if (!font.parseGlyph(glyph_id, &glyph)) {
                    return;
                }

                int dpi = 144;
                int font_size = 128;
                float scale = (font_size * dpi) / (96.f * head.unit_per_em);

                int width = std::ceil((glyph.glyph_header.max_x - glyph.glyph_header.min_x) * scale);
                int height = std::ceil((glyph.glyph_header.max_y - glyph.glyph_header.min_y) * scale);

                int off_x = 0 - glyph.glyph_header.min_x;
                int off_y = 0 - glyph.glyph_header.min_y;

                cyro::otf::OpenTypeRasterizer rasterizer(width, height);
                rasterizer.drawGlyph(glyph, off_x, off_y, scale);

                auto image = rasterizer.getOutput();
                auto img_data_ptr = reinterpret_cast<unsigned char*>(image.data_.data());
                bmp_ = ukive::BitmapFactory::create(this, width, height, img_data_ptr);
            }
        }

        img_view_->setImageBitmap(bmp_);
    }

}