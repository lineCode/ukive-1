#include "view_animator.h"

#include "ukive/views/view.h"
#include "ukive/window/window.h"


namespace ukive {

    ViewAnimator::ViewAnimator(View *v)
    {
        duration_ = 0.2;
        owner_view_ = v;

        animator_ = new Animator(
            owner_view_->getWindow()->getAnimationManager());
    }

    ViewAnimator::~ViewAnimator()
    {
        delete animator_;
    }


    void ViewAnimator::start()
    {
        animator_->start();
    }

    void ViewAnimator::cancel()
    {
        animator_->stop();
    }

    ViewAnimator *ViewAnimator::setDuration(double duration)
    {
        duration_ = duration;
        return this;
    }


    ViewAnimator *ViewAnimator::x(double x)
    {
        typedef std::numeric_limits<double> lim;

        animator_->addVariable(
            VIEW_ANIM_X, owner_view_->getX(),
            -(lim::max)(), (lim::max)());
        animator_->addTransition(
            VIEW_ANIM_X, Transition::linearTransition(duration_, x));
        animator_->setOnValueChangedListener(VIEW_ANIM_X, this);

        return this;
    }

    ViewAnimator *ViewAnimator::y(double y)
    {
        typedef std::numeric_limits<double> lim;

        animator_->addVariable(
            VIEW_ANIM_Y, owner_view_->getY(),
            -(lim::max)(), (lim::max)());
        animator_->addTransition(
            VIEW_ANIM_Y, Transition::linearTransition(duration_, y));
        animator_->setOnValueChangedListener(VIEW_ANIM_Y, this);

        return this;
    }

    ViewAnimator *ViewAnimator::alpha(double value)
    {
        animator_->addVariable(
            VIEW_ANIM_ALPHA, owner_view_->getAlpha(), 0, 1);
        animator_->addTransition(
            VIEW_ANIM_ALPHA, Transition::linearTransition(duration_, value));
        animator_->setOnValueChangedListener(VIEW_ANIM_ALPHA, this);

        return this;
    }

    ViewAnimator *ViewAnimator::scaleX(double value)
    {
        typedef std::numeric_limits<double> lim;

        animator_->addVariable(
            VIEW_ANIM_SCALE_X, owner_view_->getScaleX(),
            -(lim::max)(), (lim::max)());
        animator_->addTransition(
            VIEW_ANIM_SCALE_X, Transition::linearTransition(duration_, value));
        animator_->setOnValueChangedListener(VIEW_ANIM_SCALE_X, this);

        return this;
    }

    ViewAnimator *ViewAnimator::scaleY(double value)
    {
        typedef std::numeric_limits<double> lim;

        animator_->addVariable(
            VIEW_ANIM_SCALE_Y, owner_view_->getScaleY(),
            -(lim::max)(), (lim::max)());
        animator_->addTransition(
            VIEW_ANIM_SCALE_Y, Transition::linearTransition(duration_, value));
        animator_->setOnValueChangedListener(VIEW_ANIM_SCALE_Y, this);

        return this;
    }

    ViewAnimator *ViewAnimator::translateX(double value)
    {
        typedef std::numeric_limits<double> lim;

        animator_->addVariable(
            VIEW_ANIM_TRANSLATE_X, owner_view_->getTranslateX(),
            -(lim::max)(), (lim::max)());
        animator_->addTransition(
            VIEW_ANIM_TRANSLATE_X, Transition::linearTransition(duration_, value));
        animator_->setOnValueChangedListener(VIEW_ANIM_TRANSLATE_X, this);

        return this;
    }

    ViewAnimator *ViewAnimator::translateY(double value)
    {
        typedef std::numeric_limits<double> lim;

        animator_->addVariable(
            VIEW_ANIM_TRANSLATE_Y, owner_view_->getTranslateY(),
            -(lim::max)(), (lim::max)());
        animator_->addTransition(
            VIEW_ANIM_TRANSLATE_Y, Transition::linearTransition(duration_, value));
        animator_->setOnValueChangedListener(VIEW_ANIM_TRANSLATE_Y, this);

        return this;
    }

    ViewAnimator *ViewAnimator::setListener(Animator::OnAnimatorListener *l)
    {
        animator_->setOnStateChangedListener(l);
        return this;
    }


    Animator *ViewAnimator::createRectReveal(
        View *v, double centerX, double centerY,
        double startWidthRadius, double endWidthRadius,
        double startHeightRadius, double endHeightRadius)
    {
        typedef std::numeric_limits<double> dLimit;

        Animator *animator = new Animator(
            v->getWindow()->getAnimationManager());
        animator->addVariable(0, startWidthRadius, -(dLimit::max)(), (dLimit::max)());
        animator->addTransition(0, Transition::linearTransition(0.1, endWidthRadius));
        animator->addVariable(1, startHeightRadius, -(dLimit::max)(), (dLimit::max)());
        animator->addTransition(1, Transition::linearTransition(0.2, endHeightRadius));

        v->setHasReveal(true);
        v->setRevealType(REVEAL_RECT);
        v->setRevealCenterX(centerX);
        v->setRevealCenterY(centerY);
        v->setRevealWidthRadius(startWidthRadius);
        v->setRevealHeightRadius(startHeightRadius);

        class RevealValueListener
            : public Animator::OnValueChangedListener
        {
        public:
            RevealValueListener(View *w)
                :view_(w) {}

            void onValueChanged(
                unsigned int varIndex,
                IUIAnimationStoryboard *storyboard,
                IUIAnimationVariable *variable,
                double newValue, double previousValue)
            {
                if (varIndex == 0)
                    view_->setRevealWidthRadius(newValue);
                else if (varIndex == 1)
                    view_->setRevealHeightRadius(newValue);
            }
            void onIntegerValueChanged(
                unsigned int varIndex,
                IUIAnimationStoryboard *storyboard,
                IUIAnimationVariable *variable,
                int newValue, int previousValue) {}
        private:
            View * view_;
        }*value_listener = new RevealValueListener(v);

        class RevealStateListener
            : public Animator::OnAnimatorListener
        {
        public:
            RevealStateListener(View *w)
                :view_(w) {}

            void onAnimationStart(Animator *animator) {}
            void onAnimationEnd(Animator *animator) {
                view_->setHasReveal(false);
            }
            void onAnimationCancel(Animator *animator) {
                view_->setHasReveal(false);
            }
        private:
            View * view_;
        }*state_listener = new RevealStateListener(v);

        animator->setOnValueChangedListener(0, value_listener);
        animator->setOnValueChangedListener(1, value_listener);
        animator->setOnStateChangedListener(state_listener);

        return animator;
    }

    Animator *ViewAnimator::createCirculeReveal(
        View *v, double centerX, double centerY, double startRadius, double endRadius)
    {
        typedef std::numeric_limits<double> lim;

        Animator *animator = new Animator(
            v->getWindow()->getAnimationManager());
        animator->addVariable(0, startRadius, -(lim::max)(), (lim::max)());
        animator->addTransition(0, Transition::linearTransition(0.15, endRadius));

        v->setHasReveal(true);
        v->setRevealType(REVEAL_CIRCULE);
        v->setRevealCenterX(centerX);
        v->setRevealCenterY(centerY);
        v->setRevealRadius(startRadius);

        class RevealValueListener
            : public Animator::OnValueChangedListener
        {
        private:
            View *view_;
        public:
            RevealValueListener(View *w)
                :view_(w) {}

            void onValueChanged(
                unsigned int varIndex,
                IUIAnimationStoryboard *storyboard,
                IUIAnimationVariable *variable,
                double newValue, double previousValue)
            {
                view_->setRevealRadius(newValue);
            }
            void onIntegerValueChanged(
                unsigned int varIndex,
                IUIAnimationStoryboard *storyboard,
                IUIAnimationVariable *variable,
                int newValue, int previousValue) {}
        }*valueListener = new RevealValueListener(v);

        class RevealStateListener
            : public Animator::OnAnimatorListener
        {
        private:
            View *view_;
        public:
            RevealStateListener(View *w)
                :view_(w) {}

            void onAnimationStart(Animator *animator)
            {
            }
            void onAnimationEnd(Animator *animator)
            {
                view_->setHasReveal(false);
            }
            void onAnimationCancel(Animator *animator)
            {
                view_->setHasReveal(false);
            }
        }*stateListener = new RevealStateListener(v);

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
        case VIEW_ANIM_X:
            owner_view_->setX(newValue);
            break;
        case VIEW_ANIM_Y:
            owner_view_->setY(newValue);
            break;
        case VIEW_ANIM_ALPHA:
            owner_view_->setAlpha(newValue);
            break;
        case VIEW_ANIM_SCALE_X:
            owner_view_->setScaleX(newValue);
            break;
        case VIEW_ANIM_SCALE_Y:
            owner_view_->setScaleY(newValue);
            break;
        case VIEW_ANIM_TRANSLATE_X:
            owner_view_->setTranslateX(newValue);
            break;
        case VIEW_ANIM_TRANSLATE_Y:
            owner_view_->setTranslateY(newValue);
            break;
        case VIEW_ANIM_REVEAL:
            owner_view_->setRevealRadius(newValue);
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