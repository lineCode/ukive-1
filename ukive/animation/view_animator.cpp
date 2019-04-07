#include "view_animator.h"

#include "ukive/views/view.h"
#include "ukive/window/window.h"

#include "ukive/animation/interpolator.h"


namespace ukive {

    ViewAnimator::ViewAnimator(View* v)
        : duration_(0.2),
          owner_view_(v),
          listener_(nullptr)
    {
        director_.setListener(this);
    }

    ViewAnimator::~ViewAnimator() {
    }

    void ViewAnimator::start() {
        director_.start();
        owner_view_->invalidate();
    }

    void ViewAnimator::cancel() {
        director_.stop();
    }

    ViewAnimator* ViewAnimator::setDuration(uint64_t duration) {
        duration_ = duration;
        return this;
    }

    ViewAnimator* ViewAnimator::x(double x) {
        director_.addAnimator(VIEW_ANIM_X);
        director_.setInitValue(VIEW_ANIM_X, owner_view_->getX());
        director_.setDuration(VIEW_ANIM_X, duration_);
        director_.setInterpolator(VIEW_ANIM_X, new LinearInterpolator(x));
        return this;
    }

    ViewAnimator* ViewAnimator::y(double y) {
        director_.addAnimator(VIEW_ANIM_Y);
        director_.setInitValue(VIEW_ANIM_Y, owner_view_->getY());
        director_.setDuration(VIEW_ANIM_Y, duration_);
        director_.setInterpolator(VIEW_ANIM_Y, new LinearInterpolator(y));
        return this;
    }

    ViewAnimator* ViewAnimator::alpha(double value) {
        director_.addAnimator(VIEW_ANIM_ALPHA);
        director_.setInitValue(VIEW_ANIM_ALPHA, owner_view_->getAlpha());
        director_.setDuration(VIEW_ANIM_ALPHA, duration_);
        director_.setInterpolator(VIEW_ANIM_ALPHA, new LinearInterpolator(value));
        return this;
    }

    ViewAnimator* ViewAnimator::scaleX(double value) {
        director_.addAnimator(VIEW_ANIM_SCALE_X);
        director_.setInitValue(VIEW_ANIM_SCALE_X, owner_view_->getScaleX());
        director_.setDuration(VIEW_ANIM_SCALE_X, duration_);
        director_.setInterpolator(VIEW_ANIM_SCALE_X, new LinearInterpolator(value));
        return this;
    }

    ViewAnimator* ViewAnimator::scaleY(double value) {
        director_.addAnimator(VIEW_ANIM_SCALE_Y);
        director_.setInitValue(VIEW_ANIM_SCALE_Y, owner_view_->getScaleY());
        director_.setDuration(VIEW_ANIM_SCALE_Y, duration_);
        director_.setInterpolator(VIEW_ANIM_SCALE_Y, new LinearInterpolator(value));
        return this;
    }

    ViewAnimator* ViewAnimator::translateX(double value) {
        director_.addAnimator(VIEW_ANIM_TRANSLATE_X);
        director_.setInitValue(VIEW_ANIM_TRANSLATE_X, owner_view_->getTranslateX());
        director_.setDuration(VIEW_ANIM_TRANSLATE_X, duration_);
        director_.setInterpolator(VIEW_ANIM_TRANSLATE_X, new LinearInterpolator(value));
        return this;
    }

    ViewAnimator* ViewAnimator::translateY(double value) {
        director_.addAnimator(VIEW_ANIM_TRANSLATE_Y);
        director_.setInitValue(VIEW_ANIM_TRANSLATE_Y, owner_view_->getTranslateY());
        director_.setDuration(VIEW_ANIM_TRANSLATE_Y, duration_);
        director_.setInterpolator(VIEW_ANIM_TRANSLATE_Y, new LinearInterpolator(value));
        return this;
    }

    ViewAnimator* ViewAnimator::rectReveal(
        double center_x, double center_y,
        double start_hori_radius, double end_hori_radius,
        double start_vert_radius, double end_vert_radius)
    {
        director_.addAnimator(VIEW_ANIM_RECT_REVEAL_R_X);
        director_.setInitValue(VIEW_ANIM_RECT_REVEAL_R_X, start_hori_radius);
        director_.setDuration(VIEW_ANIM_RECT_REVEAL_R_X, 100);
        director_.setInterpolator(VIEW_ANIM_RECT_REVEAL_R_X, new LinearInterpolator(end_hori_radius));

        director_.addAnimator(VIEW_ANIM_RECT_REVEAL_R_Y);
        director_.addAnimator(VIEW_ANIM_RECT_REVEAL_R_Y);
        director_.setInitValue(VIEW_ANIM_RECT_REVEAL_R_Y, start_vert_radius);
        director_.setDuration(VIEW_ANIM_RECT_REVEAL_R_Y, 200);
        director_.setInterpolator(VIEW_ANIM_RECT_REVEAL_R_Y, new LinearInterpolator(end_vert_radius));

        owner_view_->setHasReveal(true);
        owner_view_->setRevealType(REVEAL_RECT);
        owner_view_->setRevealCenterX(center_x);
        owner_view_->setRevealCenterY(center_y);
        owner_view_->setRevealWidthRadius(start_hori_radius);
        owner_view_->setRevealHeightRadius(start_vert_radius);

        return this;
    }

    ViewAnimator* ViewAnimator::circleReveal(
        double center_x, double center_y, double start_radius, double end_radius)
    {
        director_.addAnimator(VIEW_ANIM_CIRCLE_REVEAL_R);
        director_.setInitValue(VIEW_ANIM_CIRCLE_REVEAL_R, start_radius);
        director_.setDuration(VIEW_ANIM_CIRCLE_REVEAL_R, 150);
        director_.setInterpolator(VIEW_ANIM_CIRCLE_REVEAL_R, new LinearInterpolator(end_radius));

        owner_view_->setHasReveal(true);
        owner_view_->setRevealType(REVEAL_CIRCULE);
        owner_view_->setRevealCenterX(center_x);
        owner_view_->setRevealCenterY(center_y);
        owner_view_->setRevealRadius(start_radius);

        return this;
    }

    ViewAnimator* ViewAnimator::setListener(AnimationDirectorListener* l) {
        listener_ = l;
        return this;
    }

    ViewAnimator* ViewAnimator::setFinishedHandler(const FinishedHandler& h) {
        finished_handler_ = h;
        return this;
    }

    void ViewAnimator::onPreViewDraw() {
        director_.update();
    }

    void ViewAnimator::onPostViewDraw() {
        if (director_.isRunning()) {
            owner_view_->invalidate();
        }
    }

    void ViewAnimator::onDirectorStarted(AnimationDirector* director, const Animator2* animator) {
        if (listener_) {
            listener_->onDirectorStarted(director, animator);
        }
    }

    void ViewAnimator::onDirectorProgress(AnimationDirector* director, const Animator2* animator) {
        double new_value = animator->getCurValue();

        switch (animator->getId()) {
        case VIEW_ANIM_X:
            owner_view_->setX(new_value);
            break;
        case VIEW_ANIM_Y:
            owner_view_->setY(new_value);
            break;
        case VIEW_ANIM_ALPHA:
            owner_view_->setAlpha(new_value);
            break;
        case VIEW_ANIM_SCALE_X:
            owner_view_->setScaleX(new_value);
            break;
        case VIEW_ANIM_SCALE_Y:
            owner_view_->setScaleY(new_value);
            break;
        case VIEW_ANIM_TRANSLATE_X:
            owner_view_->setTranslateX(new_value);
            break;
        case VIEW_ANIM_TRANSLATE_Y:
            owner_view_->setTranslateY(new_value);
            break;
        case VIEW_ANIM_RECT_REVEAL_R_X:
            owner_view_->setRevealWidthRadius(new_value);
            break;
        case VIEW_ANIM_RECT_REVEAL_R_Y:
            owner_view_->setRevealHeightRadius(new_value);
            break;
        case VIEW_ANIM_CIRCLE_REVEAL_R:
            owner_view_->setRevealRadius(new_value);
            break;
        default:
            break;
        }

        if (listener_) {
            listener_->onDirectorProgress(director, animator);
        }
    }

    void ViewAnimator::onDirectorStopped(AnimationDirector* director, const Animator2* animator) {
        if (listener_) {
            listener_->onDirectorStopped(director, animator);
        }
    }

    void ViewAnimator::onDirectorFinished(AnimationDirector* director, const Animator2* animator) {
        if (animator) {
            // TODO:
            if (animator->getId() == VIEW_ANIM_RECT_REVEAL_R_Y ||
                animator->getId() == VIEW_ANIM_CIRCLE_REVEAL_R)
            {
                owner_view_->setHasReveal(false);
            }
        }

        if (listener_) {
            listener_->onDirectorFinished(director, animator);
        }

        if (!animator && finished_handler_) {
            finished_handler_(director);
        }
    }

    void ViewAnimator::onDirectorReset(AnimationDirector* director, const Animator2* animator) {
        if (listener_) {
            listener_->onDirectorReset(director, animator);
        }
    }

}