#ifndef UKIVE_VIEWS_SEEK_BAR_H_
#define UKIVE_VIEWS_SEEK_BAR_H_

#include "ukive/views/view.h"
#include "ukive/animation/animator.h"
#include "ukive/views/seek_value_changed_listener.h"


namespace ukive {

    class SeekBar : public View, public Animator::OnValueChangedListener
    {
    public:
        SeekBar(Window* w);
        ~SeekBar();

        void setMaximum(float maximum);
        void setProgress(float progress, bool notify = false);
        float getProgress();

        void setOnSeekValueChangedListener(OnSeekValueChangedListener* l);

        void onMeasure(
            int width, int height,
            int widthMode, int heightMode) override;

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

        bool isMouseInThumb(int mouseX, int mouseY);
        bool isMouseInTrack(int mouseX, int mouseY);
        void computePercent(int mouseX, int mouseY);

        void startZoomInAnimation();
        void startZoomOutAnimation();

        int mSeekTrackHeight;
        int mSeekThumbMinDiameter;
        int mSeekThumbMaxDiameter;
        float mSeekThumbCurDiameter;

        float mMaximum;
        float mSeekPercent;
        bool mIsMouseLeftKeyAvailable;

        Animator* mThumbInAnimator;
        Animator* mThumbOutAnimator;
        OnSeekValueChangedListener* listener_;
    };

}

#endif  // UKIVE_VIEWS_SEEK_BAR_H_