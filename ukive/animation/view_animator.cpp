#include "view_animator.h"

#include "ukive/views/view.h"
#include "ukive/window/window.h"


namespace ukive {

    ViewAnimator::ViewAnimator(View *widget)
    {
        mOwningWidget = widget;
        mDuration = 0.2;

        mAnimator = new Animator(
            mOwningWidget->getWindow()->getAnimationManager());
    }

    ViewAnimator::~ViewAnimator()
    {
        delete mAnimator;
    }


    void ViewAnimator::start()
    {
        mAnimator->start();
    }

    void ViewAnimator::cancel()
    {
        mAnimator->stop();
    }

    ViewAnimator *ViewAnimator::setDuration(double duration)
    {
        mDuration = duration;

        return this;
    }


    ViewAnimator *ViewAnimator::x(double x)
    {
        typedef std::numeric_limits<double> dLimit;

        mAnimator->addVariable(
            UWIDGET_ANIM_X, mOwningWidget->getX(),
            -(dLimit::max)(), (dLimit::max)());
        mAnimator->addTransition(
            UWIDGET_ANIM_X, Transition::linearTransition(mDuration, x));
        mAnimator->setOnValueChangedListener(UWIDGET_ANIM_X, this);

        return this;
    }

    ViewAnimator *ViewAnimator::y(double y)
    {
        typedef std::numeric_limits<double> dLimit;

        mAnimator->addVariable(
            UWIDGET_ANIM_Y, mOwningWidget->getY(),
            -(dLimit::max)(), (dLimit::max)());
        mAnimator->addTransition(
            UWIDGET_ANIM_Y, Transition::linearTransition(mDuration, y));
        mAnimator->setOnValueChangedListener(UWIDGET_ANIM_Y, this);

        return this;
    }

    ViewAnimator *ViewAnimator::alpha(double value)
    {
        mAnimator->addVariable(
            UWIDGET_ANIM_ALPHA, mOwningWidget->getAlpha(), 0, 1);
        mAnimator->addTransition(
            UWIDGET_ANIM_ALPHA, Transition::linearTransition(mDuration, value));
        mAnimator->setOnValueChangedListener(UWIDGET_ANIM_ALPHA, this);

        return this;
    }

    ViewAnimator *ViewAnimator::scaleX(double value)
    {
        typedef std::numeric_limits<double> dLimit;

        mAnimator->addVariable(
            UWIDGET_ANIM_SCALE_X, mOwningWidget->getScaleX(),
            -(dLimit::max)(), (dLimit::max)());
        mAnimator->addTransition(
            UWIDGET_ANIM_SCALE_X, Transition::linearTransition(mDuration, value));
        mAnimator->setOnValueChangedListener(UWIDGET_ANIM_SCALE_X, this);

        return this;
    }

    ViewAnimator *ViewAnimator::scaleY(double value)
    {
        typedef std::numeric_limits<double> dLimit;

        mAnimator->addVariable(
            UWIDGET_ANIM_SCALE_Y, mOwningWidget->getScaleY(),
            -(dLimit::max)(), (dLimit::max)());
        mAnimator->addTransition(
            UWIDGET_ANIM_SCALE_Y, Transition::linearTransition(mDuration, value));
        mAnimator->setOnValueChangedListener(UWIDGET_ANIM_SCALE_Y, this);

        return this;
    }

    ViewAnimator *ViewAnimator::translateX(double value)
    {
        typedef std::numeric_limits<double> dLimit;

        mAnimator->addVariable(
            UWIDGET_ANIM_TRANSLATE_X, mOwningWidget->getTranslateX(),
            -(dLimit::max)(), (dLimit::max)());
        mAnimator->addTransition(
            UWIDGET_ANIM_TRANSLATE_X, Transition::linearTransition(mDuration, value));
        mAnimator->setOnValueChangedListener(UWIDGET_ANIM_TRANSLATE_X, this);

        return this;
    }

    ViewAnimator *ViewAnimator::translateY(double value)
    {
        typedef std::numeric_limits<double> dLimit;

        mAnimator->addVariable(
            UWIDGET_ANIM_TRANSLATE_Y, mOwningWidget->getTranslateY(),
            -(dLimit::max)(), (dLimit::max)());
        mAnimator->addTransition(
            UWIDGET_ANIM_TRANSLATE_Y, Transition::linearTransition(mDuration, value));
        mAnimator->setOnValueChangedListener(UWIDGET_ANIM_TRANSLATE_Y, this);

        return this;
    }

    ViewAnimator *ViewAnimator::setListener(Animator::OnAnimatorListener *l)
    {
        mAnimator->setOnStateChangedListener(l);

        return this;
    }


    Animator *ViewAnimator::createRectReveal(
        View *widget, double centerX, double centerY,
        double startWidthRadius, double endWidthRadius,
        double startHeightRadius, double endHeightRadius)
    {
        typedef std::numeric_limits<double> dLimit;

        Animator *animator = new Animator(
            widget->getWindow()->getAnimationManager());
        animator->addVariable(0, startWidthRadius, -(dLimit::max)(), (dLimit::max)());
        animator->addTransition(0, Transition::linearTransition(0.1, endWidthRadius));
        animator->addVariable(1, startHeightRadius, -(dLimit::max)(), (dLimit::max)());
        animator->addTransition(1, Transition::linearTransition(0.2, endHeightRadius));

        widget->setHasReveal(true);
        widget->setRevealType(REVEAL_RECT);
        widget->setRevealCenterX(centerX);
        widget->setRevealCenterY(centerY);
        widget->setRevealWidthRadius(startWidthRadius);
        widget->setRevealHeightRadius(startHeightRadius);

        class RevealValueListener
            : public Animator::OnValueChangedListener
        {
        private:
            View *mWidget;
        public:
            RevealValueListener(View *w)
            {
                mWidget = w;
            }
            void onValueChanged(
                unsigned int varIndex,
                IUIAnimationStoryboard *storyboard,
                IUIAnimationVariable *variable,
                double newValue, double previousValue)
            {
                if (varIndex == 0)
                    mWidget->setRevealWidthRadius(newValue);
                else if (varIndex == 1)
                    mWidget->setRevealHeightRadius(newValue);
            }
            void onIntegerValueChanged(
                unsigned int varIndex,
                IUIAnimationStoryboard *storyboard,
                IUIAnimationVariable *variable,
                int newValue, int previousValue) {}
        }*valueListener = new RevealValueListener(widget);

        class RevealStateListener
            : public Animator::OnAnimatorListener
        {
        private:
            View *mWidget;
        public:
            RevealStateListener(View *w)
            {
                mWidget = w;
            }

            void onAnimationStart(Animator *animator)
            {
            }
            void onAnimationEnd(Animator *animator)
            {
                mWidget->setHasReveal(false);
            }
            void onAnimationCancel(Animator *animator)
            {
                mWidget->setHasReveal(false);
            }
        }*stateListener = new RevealStateListener(widget);

        animator->setOnValueChangedListener(0, valueListener);
        animator->setOnValueChangedListener(1, valueListener);
        animator->setOnStateChangedListener(stateListener);

        return animator;
    }

    Animator *ViewAnimator::createCirculeReveal(
        View *widget, double centerX, double centerY, double startRadius, double endRadius)
    {
        typedef std::numeric_limits<double> dLimit;

        Animator *animator = new Animator(
            widget->getWindow()->getAnimationManager());
        animator->addVariable(0, startRadius, -(dLimit::max)(), (dLimit::max)());
        animator->addTransition(0, Transition::linearTransition(0.15, endRadius));

        widget->setHasReveal(true);
        widget->setRevealType(REVEAL_CIRCULE);
        widget->setRevealCenterX(centerX);
        widget->setRevealCenterY(centerY);
        widget->setRevealRadius(startRadius);

        class RevealValueListener
            : public Animator::OnValueChangedListener
        {
        private:
            View *mWidget;
        public:
            RevealValueListener(View *w)
            {
                mWidget = w;
            }
            void onValueChanged(
                unsigned int varIndex,
                IUIAnimationStoryboard *storyboard,
                IUIAnimationVariable *variable,
                double newValue, double previousValue)
            {
                mWidget->setRevealRadius(newValue);
            }
            void onIntegerValueChanged(
                unsigned int varIndex,
                IUIAnimationStoryboard *storyboard,
                IUIAnimationVariable *variable,
                int newValue, int previousValue) {}
        }*valueListener = new RevealValueListener(widget);

        class RevealStateListener
            : public Animator::OnAnimatorListener
        {
        private:
            View *mWidget;
        public:
            RevealStateListener(View *w)
            {
                mWidget = w;
            }

            void onAnimationStart(Animator *animator)
            {
            }
            void onAnimationEnd(Animator *animator)
            {
                mWidget->setHasReveal(false);
                //mWidget->invalidate();
            }
            void onAnimationCancel(Animator *animator)
            {
                mWidget->setHasReveal(false);
                //mWidget->invalidate();
            }
        }*stateListener = new RevealStateListener(widget);

        animator->setOnValueChangedListener(0, valueListener);
        animator->setOnStateChangedListener(stateListener);

        return animator;
    }


    void ViewAnimator::onValueChanged(
        unsigned int varIndex,
        IUIAnimationStoryboard *storyboard,
        IUIAnimationVariable *variable,
        double newValue, double previousValue)
    {
        switch (varIndex)
        {
        case UWIDGET_ANIM_X:
            mOwningWidget->setX(newValue);
            break;
        case UWIDGET_ANIM_Y:
            mOwningWidget->setY(newValue);
            break;
        case UWIDGET_ANIM_ALPHA:
            mOwningWidget->setAlpha(newValue);
            break;
        case UWIDGET_ANIM_SCALE_X:
            mOwningWidget->setScaleX(newValue);
            break;
        case UWIDGET_ANIM_SCALE_Y:
            mOwningWidget->setScaleY(newValue);
            break;
        case UWIDGET_ANIM_TRANSLATE_X:
            mOwningWidget->setTranslateX(newValue);
            break;
        case UWIDGET_ANIM_TRANSLATE_Y:
            mOwningWidget->setTranslateY(newValue);
            break;
        case UWIDGET_ANIM_REVEAL:
            mOwningWidget->setRevealRadius(newValue);
            break;
        }
    }

    void ViewAnimator::onIntegerValueChanged(
        unsigned int varIndex,
        IUIAnimationStoryboard *storyboard,
        IUIAnimationVariable *variable,
        int newValue, int previousValue)
    {
    }

}