#include "scroller.h"

#include "ukive/animation/animator.h"
#include "ukive/window/window.h"


namespace ukive {

    Scroller::Scroller(Window *wnd) {
        mScrollAnimator = new Animator(wnd->getAnimationManager());
    }

    Scroller::~Scroller() {
        mScrollAnimator->stop();
        delete mScrollAnimator;
    }


    void Scroller::fling(int startX, int startY, float velocityX, float velocityY)
    {
        mScrollAnimator->reset();
        mScrollAnimator->addVariable(0, startX, INT32_MIN, INT32_MAX);
        mScrollAnimator->addVariable(1, startY, INT32_MIN, INT32_MAX);
        //mScrollAnimator->addTransition(0, UTransition::acc());
        //mScrollAnimator->addTransition(1, UTransition::linearTransition(duration, startY + dy));
        mScrollAnimator->start();
    }

    void Scroller::startScroll(int startX, int startY, int dx, int dy, double duration)
    {
        mScrollAnimator->reset();
        mScrollAnimator->addVariable(0, startX, INT32_MIN, INT32_MAX);
        mScrollAnimator->addVariable(1, startY, INT32_MIN, INT32_MAX);
        mScrollAnimator->addTransition(0, Transition::linearTransition(duration, startX + dx));
        mScrollAnimator->addTransition(1, Transition::linearTransition(duration, startY + dy));
        mScrollAnimator->start();
    }

}