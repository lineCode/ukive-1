#include "scroller.h"

#include "ukive/animation/animator.h"
#include "ukive/window/window.h"


namespace ukive {

    Scroller::Scroller(Window *wnd) {
        animator_ = new Animator(wnd->getAnimationManager());
    }

    Scroller::~Scroller() {
        animator_->stop();
        delete animator_;
    }


    void Scroller::fling(int startX, int startY, float velocityX, float velocityY) {
        animator_->reset();
        animator_->addVariable(0, startX, INT32_MIN, INT32_MAX);
        animator_->addVariable(1, startY, INT32_MIN, INT32_MAX);
        //animator_->addTransition(0, UTransition::acc());
        //animator_->addTransition(1, UTransition::linearTransition(duration, startY + dy));
        animator_->start();
    }

    void Scroller::startScroll(int startX, int startY, int dx, int dy, double duration) {
        animator_->reset();
        animator_->addVariable(0, startX, INT32_MIN, INT32_MAX);
        animator_->addVariable(1, startY, INT32_MIN, INT32_MAX);
        animator_->addTransition(0, Transition::linearTransition(duration, startX + dx));
        animator_->addTransition(1, Transition::linearTransition(duration, startY + dy));
        animator_->start();
    }

}