#include "ukive/animation/animation_manager.h"

#include "ukive/animation/animation_manager_event_handler.h"
#include "ukive/animation/animation_timer_handler.h"


namespace ukive {

    IUIAnimationTransitionLibrary*
        AnimationManager::sTransitionLibrary = nullptr;
    IUIAnimationTransitionFactory*
        AnimationManager::sTransitionFactory = nullptr;


    AnimationManager::AnimationManager() {
    }


    AnimationManager::~AnimationManager() {
    }


    HRESULT AnimationManager::init() {
        HRESULT hr = ::CoCreateInstance(
            CLSID_UIAnimationManager,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&anim_mgr_));
        if (FAILED(hr)) {
            return hr;
        }

        hr = ::CoCreateInstance(
            CLSID_UIAnimationTimer,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&anim_timer_));
        if (FAILED(hr)) {
            return hr;
        }

        return S_OK;
    }

    void AnimationManager::close() {}

    HRESULT AnimationManager::initGlobal() {
        HRESULT hr = ::CoCreateInstance(
            CLSID_UIAnimationTransitionLibrary,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&sTransitionLibrary));
        if (FAILED(hr)) {
            return hr;
        }

        hr = ::CoCreateInstance(
            CLSID_UIAnimationTransitionFactory,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&sTransitionFactory));
        if (FAILED(hr)) {
            return hr;
        }

        return S_OK;
    }

    void AnimationManager::closeGlobal() {
        sTransitionLibrary->Release();
        sTransitionLibrary = nullptr;

        sTransitionFactory->Release();
        sTransitionFactory = nullptr;
    }


    void AnimationManager::pause() {
        anim_mgr_->Pause();
    }

    void AnimationManager::resume() {
        anim_mgr_->Resume();
    }

    void AnimationManager::finish(double second) {
        anim_mgr_->FinishAllStoryboards(second);
    }

    void AnimationManager::abandon() {
        anim_mgr_->AbandonAllStoryboards();
    }

    bool AnimationManager::isBusy() {
        UI_ANIMATION_MANAGER_STATUS status;
        HRESULT hr = anim_mgr_->GetStatus(&status);

        return (status == UI_ANIMATION_MANAGER_BUSY);
    }

    bool AnimationManager::update() {
        UI_ANIMATION_SECONDS secondsNow;
        HRESULT hr = anim_timer_->GetTime(&secondsNow);

        if (SUCCEEDED(hr)) {
            hr = anim_mgr_->Update(secondsNow);
        }

        return SUCCEEDED(hr);
    }


    void AnimationManager::connectTimer(bool enable) {
        HRESULT hr;
        if (enable) {
            IUIAnimationTimerUpdateHandler *pTimerUpdateHandler;
            hr = anim_mgr_->QueryInterface(IID_PPV_ARGS(&pTimerUpdateHandler));
            if (SUCCEEDED(hr)) {
                hr = anim_timer_->SetTimerUpdateHandler(
                    pTimerUpdateHandler,
                    UI_ANIMATION_IDLE_BEHAVIOR_DISABLE);
                anim_timer_->SetFrameRateThreshold(60);
                pTimerUpdateHandler->Release();
            }
        } else {
            hr = anim_timer_->SetTimerUpdateHandler(
                0, UI_ANIMATION_IDLE_BEHAVIOR_DISABLE);
        }
    }


    void AnimationManager::setTimerEventListener(OnTimerEventListener *l) {
        if (l == nullptr) {
            anim_timer_->SetTimerEventHandler(0);
        } else {
            AnimationTimerHandler *handler = new AnimationTimerHandler(l);
            anim_timer_->SetTimerEventHandler(handler);
            handler->Release();
        }
    }

    void AnimationManager::setOnStateChangedListener(OnStateChangedListener *l) {
        if (l == nullptr) {
            anim_mgr_->SetManagerEventHandler(0);
        } else {
            AnimationManagerEventHandler *handler = new AnimationManagerEventHandler(l);
            anim_mgr_->SetManagerEventHandler(handler);
            handler->Release();
        }
    }


    IUIAnimationManager *AnimationManager::getAnimationManager() {
        return anim_mgr_.get();
    }

    IUIAnimationTimer *AnimationManager::getAnimationTimer() {
        return anim_timer_.get();
    }

    IUIAnimationTransitionLibrary *AnimationManager::getTransitionLibrary() {
        return sTransitionLibrary;
    }

}
