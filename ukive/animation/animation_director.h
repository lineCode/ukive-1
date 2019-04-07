#ifndef UKIVE_ANIMATION_ANIMATION_DIRECTOR_H_
#define UKIVE_ANIMATION_ANIMATION_DIRECTOR_H_

#include <map>
#include <memory>

#include "ukive/animation/animator2.h"


namespace ukive {

    class Animator2;
    class AnimationDirector;

    class AnimationDirectorListener {
    public:
        virtual ~AnimationDirectorListener() = default;

        virtual void onDirectorStarted(AnimationDirector* director, const Animator2* animator) {}
        virtual void onDirectorProgress(AnimationDirector* director, const Animator2* animator) {}
        virtual void onDirectorStopped(AnimationDirector* director, const Animator2* animator) {}
        virtual void onDirectorFinished(AnimationDirector* director, const Animator2* animator) {}
        virtual void onDirectorReset(AnimationDirector* director, const Animator2* animator) {}
    };

    class AnimationDirector : public AnimationListener {
    public:
        AnimationDirector();

        void addAnimator(int id);
        void removeAnimator(int id);
        bool hasAnimator(int id);

        void start();
        void stop();
        void finish();
        void reset();
        void update();

        void setDuration(int id, uint64_t duration);
        void setInitValue(int id, double init_val);
        void setInterpolator(int id, Interpolator* ipr);
        void setListener(AnimationDirectorListener* l);

        bool isRunning() const;
        bool isFinished() const;

        uint64_t getDuration(int id) const;
        double getCurValue(int id) const;
        double getInitValue(int id) const;
        Interpolator* getInterpolator(int id) const;

    private:
        // AnimationListener
        void onAnimationStarted(Animator2* animator) override;
        void onAnimationProgress(Animator2* animator) override;
        void onAnimationStopped(Animator2* animator) override;
        void onAnimationFinished(Animator2* animator) override;
        void onAnimationReset(Animator2* animator) override;

        AnimationDirectorListener* listener_;
        std::map<int, std::unique_ptr<Animator2>> animators_;
    };

}

#endif  // UKIVE_ANIMATION_ANIMATION_DIRECTOR_H_