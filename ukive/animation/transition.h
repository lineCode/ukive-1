#ifndef UKIVE_ANIMATION_TRANSITION_H_
#define UKIVE_ANIMATION_TRANSITION_H_

#include <memory>

#include "ukive/animation/animation_manager.h"
#include "ukive/utils/com_ptr.h"


namespace ukive {

    class Transition {
    public:
        ~Transition();

        enum UTSlope {
            INCREASING = 0,
            DECREASING = 1
        };

        ComPtr<IUIAnimationTransition> getTransition();

        static std::shared_ptr<Transition> accDecTransition(
            double duration, double finalValue,
            double accRatio, double decRatio);

        static std::shared_ptr<Transition> constanceTransition(double duration);

        static std::shared_ptr<Transition> cubicTransition(
            double duration, double finalValue, double finalVelocity);

        static std::shared_ptr<Transition> discreteTransition(
            double delay, double finalValue, double hold);

        static std::shared_ptr<Transition> instantaneousTransition(double finalValue);

        static std::shared_ptr<Transition> linearTransition(double duration, double finalValue);

        static std::shared_ptr<Transition> linearTransitionFromSpeed(double speed, double finalValue);

        static std::shared_ptr<Transition> parabolicTransitionFromAcc(
            double finalValue, double finalVelocity, double acceleration);

        static std::shared_ptr<Transition> reversalTransition(double duration);

        static std::shared_ptr<Transition> sinusoidalTransitionFromRange(
            double duration, double minValue, double maxValue,
            double period, UTSlope slope);

        static std::shared_ptr<Transition> sinusoidalTransitionFromVelocity(
            double duration, double period);

        static std::shared_ptr<Transition> smoothStopTransition(double maxDuration, double finalValue);

        static std::shared_ptr<Transition> fromInterpolator(IUIAnimationInterpolator *interpolator);

    private:
        Transition();

        ComPtr<IUIAnimationTransition> transition_;
    };

}

#endif  // UKIVE_ANIMATION_TRANSITION_H_