#include "animation_timer_handler.h"


namespace ukive {

    AnimationTimerHandler::AnimationTimerHandler(
        AnimationManager::OnTimerEventListener *listener)
        :ref_count_(1),
        listener_(listener) {}

    AnimationTimerHandler::~AnimationTimerHandler() {}


    HRESULT STDMETHODCALLTYPE AnimationTimerHandler::OnPreUpdate() {
        if (listener_) {
            listener_->onPreUpdate();
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AnimationTimerHandler::OnPostUpdate() {
        if (listener_) {
            listener_->onPostUpdate();
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AnimationTimerHandler::OnRenderingTooSlow(
        UINT32 framesPerSecond) {

        if (listener_) {
            listener_->onRenderingTooSlow(framesPerSecond);
        }

        return S_OK;
    }


    IFACEMETHODIMP_(ULONG) AnimationTimerHandler::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }

    IFACEMETHODIMP_(ULONG) AnimationTimerHandler::Release() {
        auto nc = InterlockedDecrement(&ref_count_);
        if (nc == 0) {
            delete this;
        }

        return nc;
    }

    IFACEMETHODIMP AnimationTimerHandler::QueryInterface(
        REFIID riid, void** ppOutput) {

        if (ppOutput == nullptr) {
            return E_POINTER;
        }

        if (__uuidof(IUIAnimationTimerEventHandler) == riid) {
            *ppOutput = this;
        } else if (__uuidof(IUnknown) == riid) {
            *ppOutput = this;
        } else {
            *ppOutput = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

}