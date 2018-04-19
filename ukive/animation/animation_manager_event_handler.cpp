#include "animation_manager_event_handler.h"


namespace ukive {

    AnimationManagerEventHandler::AnimationManagerEventHandler(
        AnimationManager::OnStateChangedListener* listener)
        :ref_count_(1),
        listener_(listener) {
    }


    AnimationManagerEventHandler::~AnimationManagerEventHandler() {
    }


    HRESULT STDMETHODCALLTYPE AnimationManagerEventHandler::OnManagerStatusChanged(
        UI_ANIMATION_MANAGER_STATUS newStatus,
        UI_ANIMATION_MANAGER_STATUS previousStatus) {

        if (listener_) {
            listener_->onStateChanged(newStatus, previousStatus);
        }

        return S_OK;
    }


    IFACEMETHODIMP_(ULONG) AnimationManagerEventHandler::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }

    IFACEMETHODIMP_(ULONG) AnimationManagerEventHandler::Release() {
        auto nc = InterlockedDecrement(&ref_count_);
        if (nc == 0) {
            delete this;
        }

        return nc;
    }

    IFACEMETHODIMP AnimationManagerEventHandler::QueryInterface(
        REFIID riid, void** ppOutput) {

        if (ppOutput == nullptr) {
            return E_POINTER;
        }

        if (__uuidof(IUIAnimationManagerEventHandler) == riid) {
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