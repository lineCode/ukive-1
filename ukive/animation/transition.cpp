#include "transition.h"


namespace ukive {

    Transition::Transition()
    {
    }

    Transition::~Transition()
    {
    }


    ComPtr<IUIAnimationTransition> Transition::getTransition()
    {
        return mTransition;
    }


    std::shared_ptr<Transition> Transition::accDecTransition(
        double duration, double finalValue,
        double accRatio, double decRatio)
    {
        ComPtr<IUIAnimationTransition> aTransition;

        HRESULT hr = AnimationManager::sTransitionLibrary->
            CreateAccelerateDecelerateTransition(
                duration, finalValue, accRatio, decRatio, &aTransition);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<Transition> transition(new Transition());
            transition->mTransition = aTransition;
            return transition;
        }

        return 0;
    }

    std::shared_ptr<Transition> Transition::constanceTransition(double duration)
    {
        ComPtr<IUIAnimationTransition> aTransition;

        HRESULT hr = AnimationManager::sTransitionLibrary->
            CreateConstantTransition(duration, &aTransition);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<Transition> transition(new Transition());
            transition->mTransition = aTransition;
            return transition;
        }

        return 0;
    }

    std::shared_ptr<Transition> Transition::cubicTransition(
        double duration, double finalValue, double finalVelocity)
    {
        ComPtr<IUIAnimationTransition> aTransition;

        HRESULT hr = AnimationManager::sTransitionLibrary->
            CreateCubicTransition(duration, finalValue, finalVelocity, &aTransition);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<Transition> transition(new Transition());
            transition->mTransition = aTransition;
            return transition;
        }

        return 0;
    }

    std::shared_ptr<Transition> Transition::discreteTransition(
        double delay, double finalValue, double hold)
    {
        ComPtr<IUIAnimationTransition> aTransition;

        HRESULT hr = AnimationManager::sTransitionLibrary->
            CreateDiscreteTransition(delay, finalValue, hold, &aTransition);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<Transition> transition(new Transition());
            transition->mTransition = aTransition;
            return transition;
        }

        return 0;
    }

    std::shared_ptr<Transition> Transition::instantaneousTransition(double finalValue)
    {
        ComPtr<IUIAnimationTransition> aTransition;

        HRESULT hr = AnimationManager::sTransitionLibrary->
            CreateInstantaneousTransition(finalValue, &aTransition);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<Transition> transition(new Transition());
            transition->mTransition = aTransition;
            return transition;
        }

        return 0;
    }

    std::shared_ptr<Transition> Transition::linearTransition(double duration, double finalValue)
    {
        ComPtr<IUIAnimationTransition> aTransition;

        HRESULT hr = AnimationManager::sTransitionLibrary->
            CreateLinearTransition(duration, finalValue, &aTransition);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<Transition> transition(new Transition());
            transition->mTransition = aTransition;
            return transition;
        }

        return 0;
    }

    std::shared_ptr<Transition> Transition::linearTransitionFromSpeed(double speed, double finalValue)
    {
        ComPtr<IUIAnimationTransition> aTransition;

        HRESULT hr = AnimationManager::sTransitionLibrary->
            CreateLinearTransitionFromSpeed(speed, finalValue, &aTransition);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<Transition> transition(new Transition());
            transition->mTransition = aTransition;
            return transition;
        }

        return 0;
    }

    std::shared_ptr<Transition> Transition::parabolicTransitionFromAcc(
        double finalValue, double finalVelocity, double acceleration)
    {
        ComPtr<IUIAnimationTransition> aTransition;

        HRESULT hr = AnimationManager::sTransitionLibrary->
            CreateParabolicTransitionFromAcceleration(
                finalValue, finalVelocity, acceleration, &aTransition);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<Transition> transition(new Transition());
            transition->mTransition = aTransition;
            return transition;
        }

        return 0;
    }

    std::shared_ptr<Transition> Transition::reversalTransition(double duration)
    {
        ComPtr<IUIAnimationTransition> aTransition;

        HRESULT hr = AnimationManager::sTransitionLibrary->
            CreateReversalTransition(duration, &aTransition);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<Transition> transition(new Transition());
            transition->mTransition = aTransition;
            return transition;
        }

        return 0;
    }

    std::shared_ptr<Transition> Transition::sinusoidalTransitionFromRange(
        double duration, double minValue, double maxValue,
        double period, UTSlope slope)
    {
        ComPtr<IUIAnimationTransition> aTransition;

        UI_ANIMATION_SLOPE aSlop;
        if (slope == INCREASING)
            aSlop = UI_ANIMATION_SLOPE_INCREASING;
        else
            aSlop = UI_ANIMATION_SLOPE_DECREASING;

        HRESULT hr = AnimationManager::sTransitionLibrary->
            CreateSinusoidalTransitionFromRange(
                duration, minValue, maxValue, period, aSlop, &aTransition);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<Transition> transition(new Transition());
            transition->mTransition = aTransition;
            return transition;
        }

        return 0;
    }

    std::shared_ptr<Transition> Transition::sinusoidalTransitionFromVelocity(
        double duration, double period)
    {
        ComPtr<IUIAnimationTransition> aTransition;

        HRESULT hr = AnimationManager::sTransitionLibrary->
            CreateSinusoidalTransitionFromVelocity(duration, period, &aTransition);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<Transition> transition(new Transition());
            transition->mTransition = aTransition;
            return transition;
        }

        return 0;
    }

    std::shared_ptr<Transition> Transition::smoothStopTransition(double maxDuration, double finalValue)
    {
        ComPtr<IUIAnimationTransition> aTransition;

        HRESULT hr = AnimationManager::sTransitionLibrary->
            CreateSmoothStopTransition(maxDuration, finalValue, &aTransition);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<Transition> transition(new Transition());
            transition->mTransition = aTransition;
            return transition;
        }

        return 0;
    }


    std::shared_ptr<Transition> Transition::fromInterpolator(IUIAnimationInterpolator *interpolator)
    {
        ComPtr<IUIAnimationTransition> aTransition;

        HRESULT hr = AnimationManager::sTransitionFactory->
            CreateTransition(interpolator, &aTransition);
        if (SUCCEEDED(hr))
        {
            std::shared_ptr<Transition> transition(new Transition());
            transition->mTransition = aTransition;
            return transition;
        }

        return 0;
    }

}