#include "animator.h"

#include "ukive/log.h"
#include "ukive/animation/animator_state_handler.h"
#include "ukive/animation/animation_variable_change_handler.h"


namespace ukive {

    Animator::Animator(AnimationManager* mgr) {
        anim_mgr_ = mgr;
        mAnimatorStateListener = new AnimatorStateHandler(this);

        HRESULT hr = anim_mgr_->getAnimationManager()
            ->CreateStoryboard(&mStoryboard);
        if (FAILED(hr)) {
            DLOG(Log::FATAL) << "create storyboard failed.";
        }
        mStoryboard->SetStoryboardEventHandler(mAnimatorStateListener);
    }


    Animator::~Animator() {
        stop();
        mAnimatorStateListener->Release();
    }


    void Animator::start() {
        UI_ANIMATION_SECONDS time;
        anim_mgr_->getAnimationTimer()->GetTime(&time);
        mStoryboard->Schedule(time);
    }

    void Animator::stop() {
        mStoryboard->Abandon();
    }

    void Animator::finish(double second) {
        mStoryboard->Finish(second);
    }

    void Animator::reset() {
        UI_ANIMATION_STORYBOARD_STATUS status;
        mStoryboard->GetStatus(&status);
        if (status == UI_ANIMATION_STORYBOARD_BUILDING) {
            return;
        }

        mVariableList.clear();

        mStoryboard->Abandon();
        mStoryboard.reset();
        HRESULT hr = anim_mgr_->getAnimationManager()
            ->CreateStoryboard(&mStoryboard);
        if (FAILED(hr)) {
            DLOG(Log::FATAL) << "create storyboard failed.";
            return;
        }
        mStoryboard->SetStoryboardEventHandler(mAnimatorStateListener);
    }

    void Animator::startTransition(unsigned int varIndex, std::shared_ptr<Transition> transition) {
        auto it = mVariableList.find(varIndex);
        if (it == mVariableList.end()) {
            return;
        }

        UI_ANIMATION_SECONDS time;
        anim_mgr_->getAnimationTimer()->GetTime(&time);

        anim_mgr_->getAnimationManager()->
            ScheduleTransition(it->second.get(), transition->getTransition().get(), time);
    }


    void Animator::setOnStateChangedListener(OnAnimatorListener* l) {
        mAnimatorStateListener->setOnAnimatorListener(l);
    }

    void Animator::setOnValueChangedListener(
        unsigned int varIndex, OnValueChangedListener* l) {

        auto it = mVariableList.find(varIndex);
        if (it == mVariableList.end()) {
            return;
        }

        if (l == nullptr) {
            it->second->SetVariableChangeHandler(nullptr);
            it->second->SetVariableIntegerChangeHandler(nullptr);
        } else {
            AnimationVariableChangeHandler* handler
                = new AnimationVariableChangeHandler(l, varIndex);
            it->second->SetVariableChangeHandler(handler);
            it->second->SetVariableIntegerChangeHandler(handler);
            handler->Release();
        }
    }


    double Animator::getValue(unsigned int varIndex) {
        auto it = mVariableList.find(varIndex);
        if (it == mVariableList.end()) {
            return 0;
        }

        double value = 0;
        it->second->GetValue(&value);

        return value;
    }

    int Animator::getIntValue(unsigned int varIndex) {
        auto it = mVariableList.find(varIndex);
        if (it == mVariableList.end()) {
            return 0;
        }

        int value = 0;
        it->second->GetIntegerValue(&value);

        return value;
    }

    double Animator::getPrevValue(unsigned int varIndex) {
        auto it = mVariableList.find(varIndex);
        if (it == mVariableList.end()) {
            return 0;
        }

        double value = 0;
        it->second->GetPreviousValue(&value);

        return value;
    }

    int Animator::getPrevIntValue(unsigned int varIndex) {
        auto it = mVariableList.find(varIndex);
        if (it == mVariableList.end()) {
            return 0;
        }

        int value = 0;
        it->second->GetPreviousIntegerValue(&value);

        return value;
    }

    double Animator::getFinalValue(unsigned int varIndex) {
        auto it = mVariableList.find(varIndex);
        if (it == mVariableList.end()) {
            return 0;
        }

        double value = 0;
        it->second->GetFinalValue(&value);

        return value;
    }

    int Animator::getFinalIntValue(unsigned int varIndex) {
        auto it = mVariableList.find(varIndex);
        if (it == mVariableList.end()) {
            return 0;
        }

        int value = 0;
        it->second->GetFinalIntegerValue(&value);

        return value;
    }

    double Animator::getElapsedTime() {
        double time = 0;
        mStoryboard->GetElapsedTime(&time);
        return time;
    }


    bool Animator::addVariable(
        unsigned int varIndex, double initValue,
        double lower, double upper) {

        auto it = mVariableList.find(varIndex);
        if (it != mVariableList.end()) {
            mVariableList.erase(varIndex);
        }

        ComPtr<IUIAnimationVariable> aVariable;

        HRESULT hr = anim_mgr_->getAnimationManager()->
            CreateAnimationVariable(initValue, &aVariable);
        if (FAILED(hr)) {
            return false;
        }

        aVariable->SetLowerBound(lower);
        aVariable->SetUpperBound(upper);

        mVariableList.insert(
            std::pair<unsigned int, ComPtr<IUIAnimationVariable>>(varIndex, aVariable));

        return true;
    }

    bool Animator::addTransition(unsigned int varIndex, std::shared_ptr<Transition> transition) {
        auto it = mVariableList.find(varIndex);
        if (it == mVariableList.end()) {
            return false;
        }

        HRESULT hr = mStoryboard->AddTransition(
            it->second.get(), transition->getTransition().get());
        if (FAILED(hr)) {
            return false;
        }

        return true;
    }

    bool Animator::addTransition(
        unsigned int varIndex, std::shared_ptr<Transition> transition,
        UI_ANIMATION_KEYFRAME key) {

        auto it = mVariableList.find(varIndex);
        if (it == mVariableList.end()) {
            return false;
        }

        HRESULT hr = mStoryboard->AddTransitionAtKeyframe(
            it->second.get(), transition->getTransition().get(), key);
        if (FAILED(hr)) {
            return false;
        }

        return true;
    }

    bool Animator::addTransition(
        unsigned int varIndex, std::shared_ptr<Transition> transition,
        UI_ANIMATION_KEYFRAME startKey, UI_ANIMATION_KEYFRAME endKey) {

        auto it = mVariableList.find(varIndex);
        if (it == mVariableList.end()) {
            return false;
        }

        HRESULT hr = mStoryboard->AddTransitionBetweenKeyframes(
            it->second.get(), transition->getTransition().get(), startKey, endKey);
        if (FAILED(hr)) {
            return false;
        }

        return true;
    }

    bool Animator::addKey(
        UI_ANIMATION_KEYFRAME existed, double offset, UI_ANIMATION_KEYFRAME* newKey) {

        HRESULT hr = mStoryboard->AddKeyframeAtOffset(existed, offset, newKey);
        if (FAILED(hr)) {
            return false;
        }

        return true;
    }

    bool Animator::addKey(
        std::shared_ptr<Transition> transition, UI_ANIMATION_KEYFRAME* newKey) {

        HRESULT hr = mStoryboard->AddKeyframeAfterTransition(transition->getTransition().get(), newKey);
        if (FAILED(hr)) {
            return false;
        }

        return true;
    }

    bool Animator::hasVariable(unsigned int varIndex) {
        auto it = mVariableList.find(varIndex);
        return (it != mVariableList.end());
    }

    bool Animator::removeVariable(unsigned int varIndex) {
        auto it = mVariableList.find(varIndex);
        if (it == mVariableList.end()) {
            return false;
        }

        mVariableList.erase(varIndex);
        return true;
    }

}