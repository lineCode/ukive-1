#ifndef UKIVE_VIEWS_SEEK_BAR_H_
#define UKIVE_VIEWS_SEEK_BAR_H_

#include "ukive/views/view.h"
#include "ukive/animation/animator.h"
#include "ukive/views/seek_value_changed_listener.h"


namespace ukive {

    class SeekBar : public View, public Animator::OnValueChangedListener
    {
    private:
        int mSeekTrackHeight;
        int mSeekThumbMinDiameter;
        int mSeekThumbMaxDiameter;
        float mSeekThumbCurDiameter;

        float mMaximum;
        float mSeekPercent;

        Animator *mThumbInAnimator;
        Animator *mThumbOutAnimator;
        OnSeekValueChangedListener *mListener;

        void initSeekBar();

    private:
        bool mIsMouseLeftKeyAvailable;

        bool isMouseInThumb(int mouseX, int mouseY);
        bool isMouseInTrack(int mouseX, int mouseY);
        void computePercent(int mouseX, int mouseY);

        void startZoomInAnimation();
        void startZoomOutAnimation();

    public:
        SeekBar(Window *window);
        SeekBar(Window *window, int id);
        ~SeekBar();

        void setMaximum(float maximum);
        void setProgress(float progress, bool notify = false);
        float getProgress();

        void setOnSeekValueChangedListener(OnSeekValueChangedListener *l);

        virtual void onMeasure(
            int width, int height,
            int widthMode, int heightMode) override;

        virtual void onDraw(Canvas *canvas) override;

        virtual bool onInputEvent(InputEvent *e) override;

        void onValueChanged(
            unsigned int varIndex,
            IUIAnimationStoryboard *storyboard,
            IUIAnimationVariable *variable,
            double newValue, double previousValue) override;
        void onIntegerValueChanged(
            unsigned int varIndex,
            IUIAnimationStoryboard *storyboard,
            IUIAnimationVariable *variable,
            int newValue, int previousValue) override;
    };

}

#endif  // UKIVE_VIEWS_SEEK_BAR_H_