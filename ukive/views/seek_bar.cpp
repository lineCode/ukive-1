#include "seek_bar.h"

#include <algorithm>

#include "ukive/application.h"
#include "ukive/event/input_event.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/window/window.h"
#include "ukive/graphics/color.h"


namespace ukive {

    SeekBar::SeekBar(Window *w)
        :View(w) {
        initSeekBar();
    }

    SeekBar::~SeekBar()
    {
        delete mThumbInAnimator;
        delete mThumbOutAnimator;
    }


    void SeekBar::initSeekBar()
    {
        mMaximum = 100.f;
        mSeekPercent = 0.f;
        mIsMouseLeftKeyAvailable = false;
        mListener = nullptr;

        mThumbInAnimator = new Animator(getWindow()->getAnimationManager());
        mThumbOutAnimator = new Animator(getWindow()->getAnimationManager());

        mSeekTrackHeight = 2;
        mSeekThumbMinDiameter = 10;
        mSeekThumbMaxDiameter = 14;

        mSeekThumbCurDiameter = mSeekThumbMinDiameter;
    }


    void SeekBar::setMaximum(float max)
    {
        if (mMaximum > 0)
            mMaximum = max;
    }

    void SeekBar::setProgress(float progress, bool notify)
    {
        float prog = std::min(mMaximum, progress);
        float percent = prog / mMaximum;
        if (percent != mSeekPercent)
        {
            mSeekPercent = percent;

            if (notify && mListener)
            {
                mListener->onSeekValueChanged(this, mSeekPercent*mMaximum);
                mListener->onSeekIntegerValueChanged(this, static_cast<int>(mSeekPercent*mMaximum));
            }

            invalidate();
        }
    }

    float SeekBar::getProgress()
    {
        return mMaximum*mSeekPercent;
    }

    void SeekBar::setOnSeekValueChangedListener(OnSeekValueChangedListener *l)
    {
        mListener = l;
    }


    bool SeekBar::isMouseInThumb(int mouseX, int mouseY)
    {
        float thumbRadius = mSeekThumbMaxDiameter / 2.f;
        int trackWidth = getWidth() - mSeekThumbMaxDiameter - getPaddingLeft() - getPaddingRight();

        float thumbCenterXInTrack = trackWidth*mSeekPercent + thumbRadius;
        float thumbCenterYInTrack = getPaddingTop() + thumbRadius;

        if (((mouseX - getPaddingLeft()) >= (thumbCenterXInTrack - thumbRadius))
            && ((mouseX - getPaddingLeft()) <= (thumbCenterXInTrack + thumbRadius)))
        {
            if ((mouseY - getPaddingTop()) >= (thumbCenterYInTrack - thumbRadius)
                && (mouseY - getPaddingTop()) <= (thumbCenterYInTrack + thumbRadius))
                return true;
        }

        return false;
    }

    bool SeekBar::isMouseInTrack(int mouseX, int mouseY)
    {
        float trackSpace = getWidth() - getPaddingLeft() - getPaddingRight();

        if ((mouseX - getPaddingLeft()) >= 0
            && (mouseX - getPaddingLeft()) <= trackSpace)
        {
            if ((mouseY - getPaddingTop()) >= 0
                && (mouseY - getPaddingTop()) <= mSeekThumbMaxDiameter)
                return true;
        }

        return false;
    }

    void SeekBar::computePercent(int mouseX, int mouseY)
    {
        float mouseInTrack = mouseX - getPaddingLeft() - mSeekThumbMaxDiameter / 2.f;
        float trackWidth = getWidth() - mSeekThumbMaxDiameter - getPaddingLeft() - getPaddingRight();
        mSeekPercent = std::max(0.f, mouseInTrack / trackWidth);
        mSeekPercent = std::min(1.f, mSeekPercent);

        if (mListener)
        {
            mListener->onSeekValueChanged(this, mSeekPercent*mMaximum);
            mListener->onSeekIntegerValueChanged(this, static_cast<int>(mSeekPercent*mMaximum));
        }
    }

    void SeekBar::startZoomInAnimation()
    {
        if (mSeekThumbCurDiameter < mSeekThumbMaxDiameter)
        {
            mThumbInAnimator->stop();
            mThumbOutAnimator->stop();

            mThumbInAnimator->addVariable(0,
                mSeekThumbCurDiameter,
                mSeekThumbMinDiameter,
                mSeekThumbMaxDiameter);
            mThumbInAnimator->setOnValueChangedListener(0, this);
            mThumbInAnimator->startTransition(
                0, Transition::linearTransition(0.1, mSeekThumbMaxDiameter));
        }
    }

    void SeekBar::startZoomOutAnimation()
    {
        if (mSeekThumbCurDiameter > mSeekThumbMinDiameter)
        {
            mThumbInAnimator->stop();
            mThumbOutAnimator->stop();

            mThumbOutAnimator->addVariable(0,
                mSeekThumbCurDiameter,
                mSeekThumbMinDiameter,
                mSeekThumbMaxDiameter);
            mThumbOutAnimator->setOnValueChangedListener(0, this);
            mThumbOutAnimator->startTransition(
                0, Transition::linearTransition(0.1, mSeekThumbMinDiameter));
        }
    }


    void SeekBar::onMeasure(
        int width, int height,
        int widthMode, int heightMode)
    {
        int finalWidth = 0;
        int finalHeight = 0;

        int hPadding = getPaddingLeft() + getPaddingRight();
        int vPadding = getPaddingTop() + getPaddingBottom();

        switch (widthMode)
        {
        case EXACTLY:
            finalWidth = width;
            break;

        case FIT:
            finalWidth = std::max(width, getMinimumWidth());
            break;

        case UNKNOWN:
            finalWidth = std::max(0, getMinimumWidth());
            break;
        }

        switch (heightMode)
        {
        case EXACTLY:
            finalHeight = height;
            break;

        case FIT:
            finalHeight = std::min(height, mSeekThumbMaxDiameter + vPadding);
            finalHeight = std::max(finalHeight, getMinimumHeight());
            break;

        case UNKNOWN:
            finalHeight = std::max(mSeekThumbMaxDiameter + vPadding, getMinimumHeight());
            break;
        }

        setMeasuredDimension(finalWidth, finalHeight);
    }

    void SeekBar::onDraw(Canvas *canvas)
    {
        float left = mSeekThumbMaxDiameter / 2.f;
        float top = mSeekThumbMaxDiameter / 2.f - mSeekTrackHeight / 2.f;
        float trackWidth = getWidth() - mSeekThumbMaxDiameter - getPaddingLeft() - getPaddingRight();

        float cur_pos = trackWidth * mSeekPercent;
        float centerX = left + cur_pos;
        float centerY = mSeekThumbMaxDiameter / 2.f;

        if (centerX < mSeekThumbMinDiameter)
            canvas->fillRect(RectF(left, top, trackWidth, mSeekTrackHeight), Color::Grey300);
        else
        {
            canvas->fillRect(RectF(left, top, cur_pos, mSeekTrackHeight), Color::Blue400);
            canvas->fillRect(RectF(centerX, top, trackWidth - cur_pos, mSeekTrackHeight), Color::Grey300);
        }

        canvas->fillCircle(centerX, centerY, mSeekThumbCurDiameter / 2.f, Color::Blue400);
    }

    bool SeekBar::onInputEvent(InputEvent *e)
    {
        View::onInputEvent(e);

        switch (e->getEvent())
        {
        case InputEvent::EVM_LEAVE_VIEW:
        {
            startZoomOutAnimation();
            break;
        }

        case InputEvent::EVM_DOWN:
        {
            if (e->getMouseKey() == InputEvent::MK_LEFT)
            {
                if (isMouseInThumb(e->getMouseX(), e->getMouseY()))
                {
                    mIsMouseLeftKeyAvailable = true;
                    computePercent(e->getMouseX(), e->getMouseY());
                    invalidate();
                }
                else if (isMouseInTrack(e->getMouseX(), e->getMouseY()))
                {
                    mIsMouseLeftKeyAvailable = true;
                    computePercent(e->getMouseX(), e->getMouseY());
                    startZoomInAnimation();
                }
            }

            break;
        }

        case InputEvent::EVM_MOVE:
        {
            if (mIsMouseLeftKeyAvailable)
            {
                computePercent(e->getMouseX(), e->getMouseY());
                invalidate();
            }
            else
            {
                if (isMouseInThumb(e->getMouseX(), e->getMouseY()))
                    startZoomInAnimation();
                else
                    startZoomOutAnimation();
            }
            break;
        }

        case InputEvent::EVM_UP:
        {
            if (e->getMouseKey() == InputEvent::MK_LEFT)
            {
                if (mIsMouseLeftKeyAvailable)
                {
                    mIsMouseLeftKeyAvailable = false;
                }
            }
            break;
        }
        }

        return true;
    }


    void SeekBar::onValueChanged(
        unsigned int varIndex,
        IUIAnimationStoryboard *storyboard,
        IUIAnimationVariable *variable,
        double newValue, double previousValue)
    {
        mSeekThumbCurDiameter = newValue;
    }

    void SeekBar::onIntegerValueChanged(
        unsigned int varIndex,
        IUIAnimationStoryboard *storyboard,
        IUIAnimationVariable *variable,
        int newValue, int previousValue)
    {
    }

}