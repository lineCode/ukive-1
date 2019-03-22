#ifndef UKIVE_VIEWS_SEEK_BAR_H_
#define UKIVE_VIEWS_SEEK_BAR_H_

#include "ukive/views/view.h"
#include "ukive/animation/animator.h"
#include "ukive/views/seek_value_changed_listener.h"


namespace ukive {

    class SeekBar : public View, public Animator::OnValueChangedListener {
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

        void onValueChanged(
            unsigned int varIndex,
            IUIAnimationStoryboard* storyboard,
            IUIAnimationVariable* variable,
            double newValue, double previousValue) override;
        void onIntegerValueChanged(
            unsigned int varIndex,
            IUIAnimationStoryboard* storyboard,
            IUIAnimationVariable* variable,
            int newValue, int previousValue) override;

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

        Animator* thumb_in_animator_;
        Animator* thumb_out_animator_;
        OnSeekValueChangedListener* listener_;
    };

}

#endif  // UKIVE_VIEWS_SEEK_BAR_H_