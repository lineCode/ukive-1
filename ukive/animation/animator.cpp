#include "animator.h"

#include "ukive/log.h"
#include "ukive/animation/animator_state_handler.h"
#include "ukive/animation/animation_variable_change_handler.h"


namespace ukive {

    Animator::Animator(AnimationManager* mgr) {
        anim_mgr_ = mgr;
        animator_state_listener_ = new AnimatorStateHandler(this);

        HRESULT hr = anim_mgr_->getAnimationManager()
            ->CreateStoryboard(&story_board_);
        if (FAILED(hr)) {
            DLOG(Log::FATAL) << "create storyboard failed.";
        }
        story_board_->SetStoryboardEventHandler(animator_state_listener_);
    }

    Animator::~Animator() {
        stop();
        animator_state_listener_->Release();
    }


    void Animator::start() {
        UI_ANIMATION_SECONDS time;
        anim_mgr_->getAnimationTimer()->GetTime(&time);
        story_board_->Schedule(time);
    }

    void Animator::stop() {
        story_board_->Abandon();
    }

    void Animator::finish(double second) {
        story_board_->Finish(second);
    }

    void Animator::reset() {
        UI_ANIMATION_STORYBOARD_STATUS status;
        story_board_->GetStatus(&status);
        if (status == UI_ANIMATION_STORYBOARD_BUILDING) {
            return;
        }

        vars_.clear();

        story_board_->Abandon();
        story_board_.reset();

        HRESULT hr = anim_mgr_->getAnimationManager()
            ->CreateStoryboard(&story_board_);
        if (FAILED(hr)) {
            DLOG(Log::FATAL) << "create storyboard failed.";
            return;
        }
        story_board_->SetStoryboardEventHandler(animator_state_listener_);
    }

    void Animator::startTransition(unsigned int varIndex, std::shared_ptr<Transition> transition) {
        auto it = vars_.find(varIndex);
        if (it == vars_.end()) {
            return;
        }

        UI_ANIMATION_SECONDS time;
        anim_mgr_->getAnimationTimer()->GetTime(&time);

        anim_mgr_->getAnimationManager()->
            ScheduleTransition(it->second.get(), transition->getTransition().get(), time);
    }

    void Animator::setOnStateChangedListener(OnAnimatorListener* l) {
        animator_state_listener_->setOnAnimatorListener(l);
    }

    void Animator::setOnValueChangedListener(
        unsigned int varIndex, OnValueChangedListener* l) {

        auto it = vars_.find(varIndex);
        if (it == vars_.end()) {
            return;
        }

        if (!l) {
            it->second->SetVariableChangeHandler(nullptr);
            it->second->SetVariableIntegerChangeHandler(nullptr);
        } else {
            auto handler = new AnimationVariableChangeHandler(l, varIndex);
            it->second->SetVariableChangeHandler(handler);
            it->second->SetVariableIntegerChangeHandler(handler);
            handler->Release();
        }
    }

    double Animator::getValue(unsigned int varIndex) {
        auto it = vars_.find(varIndex);
        if (it == vars_.end()) {
            return 0;
        }

        double value = 0;
        it->second->GetValue(&value);

        return value;
    }

    int Animator::getIntValue(unsigned int varIndex) {
        auto it = vars_.find(varIndex);
        if (it == vars_.end()) {
            return 0;
        }

        int value = 0;
        it->second->GetIntegerValue(&value);

        return value;
    }

    double Animator::getPrevValue(unsigned int varIndex) {
        auto it = vars_.find(varIndex);
        if (it == vars_.end()) {
            return 0;
        }

        double value = 0;
        it->second->GetPreviousValue(&value);

        return value;
    }

    int Animator::getPrevIntValue(unsigned int varIndex) {
        auto it = vars_.find(varIndex);
        if (it == vars_.end()) {
            return 0;
        }

        int value = 0;
        it->second->GetPreviousIntegerValue(&value);

        return value;
    }

    double Animator::getFinalValue(unsigned int varIndex) {
        auto it = vars_.find(varIndex);
        if (it == vars_.end()) {
            return 0;
        }

        double value = 0;
        it->second->GetFinalValue(&value);

        return value;
    }

    int Animator::getFinalIntValue(unsigned int varIndex) {
        auto it = vars_.find(varIndex);
        if (it == vars_.end()) {
            return 0;
        }

        int value = 0;
        it->second->GetFinalIntegerValue(&value);

        return value;
    }

    double Animator::getElapsedTime() {
        double time = 0;
        story_board_->GetElapsedTime(&time);
        return time;
    }

    bool Animator::addVariable(
        unsigned int varIndex, double initValue,
        double lower, double upper) {

        auto it = vars_.find(varIndex);
        if (it != vars_.end()) {
            vars_.erase(varIndex);
        }

        ComPtr<IUIAnimationVariable> var;

        HRESULT hr = anim_mgr_->getAnimationManager()->
            CreateAnimationVariable(initValue, &var);
        if (FAILED(hr)) {
            return false;
        }

        var->SetLowerBound(lower);
        var->SetUpperBound(upper);

        vars_.insert({ varIndex, var });

        return true;
    }

    bool Animator::addTransition(unsigned int varIndex, std::shared_ptr<Transition> transition) {
        auto it = vars_.find(varIndex);
        if (it == vars_.end()) {
            return false;
        }

        HRESULT hr = story_board_->AddTransition(
            it->second.get(), transition->getTransition().get());
        if (FAILED(hr)) {
            return false;
        }

        return true;
    }

    bool Animator::addTransition(
        unsigned int varIndex, std::shared_ptr<Transition> transition,
        UI_ANIMATION_KEYFRAME key) {

        auto it = vars_.find(varIndex);
        if (it == vars_.end()) {
            return false;
        }

        HRESULT hr = story_board_->AddTransitionAtKeyframe(
            it->second.get(), transition->getTransition().get(), key);
        if (FAILED(hr)) {
            return false;
        }

        return true;
    }

    bool Animator::addTransition(
        unsigned int varIndex, std::shared_ptr<Transition> transition,
        UI_ANIMATION_KEYFRAME startKey, UI_ANIMATION_KEYFRAME endKey) {

        auto it = vars_.find(varIndex);
        if (it == vars_.end()) {
            return false;
        }

        HRESULT hr = story_board_->AddTransitionBetweenKeyframes(
            it->second.get(), transition->getTransition().get(), startKey, endKey);
        if (FAILED(hr)) {
            return false;
        }

        return true;
    }

    bool Animator::addKey(
        UI_ANIMATION_KEYFRAME existed, double offset, UI_ANIMATION_KEYFRAME* newKey) {

        HRESULT hr = story_board_->AddKeyframeAtOffset(existed, offset, newKey);
        if (FAILED(hr)) {
            return false;
        }

        return true;
    }

    bool Animator::addKey(
        std::shared_ptr<Transition> transition, UI_ANIMATION_KEYFRAME* newKey) {

        HRESULT hr = story_board_->AddKeyframeAfterTransition(transition->getTransition().get(), newKey);
        if (FAILED(hr)) {
            return false;
        }

        return true;
    }

    bool Animator::hasVariable(unsigned int varIndex) {
        auto it = vars_.find(varIndex);
        return (it != vars_.end());
    }

    bool Animator::removeVariable(unsigned int varIndex) {
        if (!hasVariable(varIndex)) {
            return false;
        }

        vars_.erase(varIndex);
        return true;
    }

}