#include "animation_manager.h"

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
            IID_PPV_ARGS(&mAnimationManager));
        if (FAILED(hr)) {
            return hr;
        }

        hr = ::CoCreateInstance(
            CLSID_UIAnimationTimer,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&mAnimationTimer));
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
        mAnimationManager->Pause();
    }

    void AnimationManager::resume() {
        mAnimationManager->Resume();
    }

    void AnimationManager::finish(double second) {
        mAnimationManager->FinishAllStoryboards(second);
    }

    void AnimationManager::abandon() {
        mAnimationManager->AbandonAllStoryboards();
    }

    bool AnimationManager::isBusy() {
        UI_ANIMATION_MANAGER_STATUS status;
        HRESULT hr = mAnimationManager->GetStatus(&status);

        return (status == UI_ANIMATION_MANAGER_BUSY);
    }

    bool AnimationManager::update() {
        UI_ANIMATION_SECONDS secondsNow;
        HRESULT hr = mAnimationTimer->GetTime(&secondsNow);

        if (SUCCEEDED(hr)) {
            hr = mAnimationManager->Update(secondsNow);
        }

        return SUCCEEDED(hr);
    }


    void AnimationManager::connectTimer(bool enable) {
        HRESULT hr;
        if (enable) {
            IUIAnimationTimerUpdateHandler *pTimerUpdateHandler;
            hr = mAnimationManager->QueryInterface(IID_PPV_ARGS(&pTimerUpdateHandler));
            if (SUCCEEDED(hr)) {
                hr = mAnimationTimer->SetTimerUpdateHandler(
                    pTimerUpdateHandler,
                    UI_ANIMATION_IDLE_BEHAVIOR_DISABLE);
                pTimerUpdateHandler->Release();
            }
        }
        else {
            hr = mAnimationTimer->SetTimerUpdateHandler(
                0, UI_ANIMATION_IDLE_BEHAVIOR_DISABLE);
        }
    }


    void AnimationManager::setTimerEventListener(OnTimerEventListener *l) {
        if (l == nullptr) {
            mAnimationTimer->SetTimerEventHandler(0);
        }
        else {
            AnimationTimerHandler *handler = new AnimationTimerHandler(l);
            mAnimationTimer->SetTimerEventHandler(handler);
            handler->Release();
        }
    }

    void AnimationManager::setOnStateChangedListener(OnStateChangedListener *l) {
        if (l == nullptr) {
            mAnimationManager->SetManagerEventHandler(0);
        }
        else {
            AnimationManagerEventHandler *handler = new AnimationManagerEventHandler(l);
            mAnimationManager->SetManagerEventHandler(handler);
            handler->Release();
        }
    }


    IUIAnimationManager *AnimationManager::getAnimationManager() {
        return mAnimationManager.get();
    }

    IUIAnimationTimer *AnimationManager::getAnimationTimer() {
        return mAnimationTimer.get();
    }

    IUIAnimationTransitionLibrary *AnimationManager::getTransitionLibrary() {
        return sTransitionLibrary;
    }

}