#ifndef UKIVE_VIEWS_SEEK_BAR_H_
#define UKIVE_VIEWS_SEEK_BAR_H_

#include "ukive/views/view.h"
#include "ukive/animation/animator2.h"
#include "ukive/views/seek_value_changed_listener.h"


namespace ukive {

    class SeekBar : public View, public AnimationListener {
    public:
        explicit SeekBar(Window* w);
        SeekBar(Window* w, AttrsRef attrs);
        ~SeekBar();

        void setMaximum(float maximum);
        void setProgress(float progress, bool notify = false);
        float getProgress();

        void setOnSeekValueChangedListener(OnSeekValueChangedListener* l);

        void onMeasure(
            int width, int height,
            int width_mode, int height_mode) override;

        void onDraw(Canvas* canvas) override;

        bool onInputEvent(InputEvent* e) override;

        // AnimationListener
        void onAnimationProgress(Animator2* animator) override;

    private:
        void initSeekBar();

        bool isPointerInThumb(int x, int y);
        bool isPointerInTrack(int x, int y);
        void computePercent(int x, int y);

        void startZoomInAnimation();
        void startZoomOutAnimation();

        int track_height_;
        int thumb_min_diameter_;
        int thumb_max_diameter_;
        float thumb_cur_diameter_;

        float maximum_;
        float seek_percent_;
        bool is_pointer_left_key_available_;

        Animator2 thumb_in_animator_;
        Animator2 thumb_out_animator_;
        OnSeekValueChangedListener* listener_;
    };

}

#endif  // UKIVE_VIEWS_SEEK_BAR_H_