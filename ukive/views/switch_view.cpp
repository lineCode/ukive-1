#include "switch_view.h"

#include "ukive/graphics/color.h"


namespace ukive {

    namespace {

        const int kDefaultTrackWidth = 60;
        const int kDefaultTrackHeight = 20;
        const int kDefaultTrackRadius = kDefaultTrackHeight / 2;
        const int kDefaultThumbRadius = static_cast<int>(kDefaultTrackRadius * 1.75f);

        const Color kNormalTrackColor = Color::ofRGB(0xe0e0e0);
        const Color kNormalThumbColor = Color::ofRGB(0xf5f5f5);
        const Color kCheckedTrackColor = Color::ofRGB(0xafbfff);
        const Color kCheckedThumbColor = Color::ofRGB(0x5677fc);

        const double kNormalShadowBlur = 2.0;
        const Color kNormalShadowColor = Color::ofRGB(0x9e9e9e);
        const int kNormalShadowPadding = 0;
        const int kNormalShadowYOffset = 1;

        const int kFeedbackRadius = static_cast<int>(kDefaultThumbRadius * 1.75f);
        const Color kFeedbackColor = Color::ofRGB(0xe0e0e0);

    }

    SwitchView::SwitchView(Window* w)
        : SwitchView(w, {}) {
    }

    SwitchView::SwitchView(Window* w, AttrsRef attrs)
        : View(w, attrs) {
    }

    SwitchView::~SwitchView() {
    }

    void SwitchView::onMeasure(int width, int height, int width_mode, int height_mode) {
        int final_w = kDefaultTrackWidth
            + (kFeedbackRadius - kDefaultTrackRadius + kNormalShadowPadding) * 2;
        int final_h = kDefaultTrackHeight
            + (kFeedbackRadius - kDefaultTrackRadius + kNormalShadowPadding) * 2;
        final_w += getPaddingLeft() + getPaddingRight();
        final_h += getPaddingTop() + getPaddingBottom();

        setMeasuredSize(final_w, final_h);
    }

    void SwitchView::onDraw(Canvas* canvas) {

    }

    bool SwitchView::onInputEvent(InputEvent* e) {
        return false;
    }
}