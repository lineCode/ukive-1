﻿#include "animation_timer_handler.h"


namespace ukive {

    AnimationTimerHandler::AnimationTimerHandler(
        AnimationManager::OnTimerEventListener *listener)
        :ref_count_(1),
        listener_(listener) {}

    AnimationTimerHandler::~AnimationTimerHandler() {}


    HRESULT STDMETHODCALLTYPE AnimationTimerHandler::OnPreUpdate()
    {
        if (listener_) {
            listener_->onPreUpdate();
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AnimationTimerHandler::OnPostUpdate()
    {
        if (listener_) {
            listener_->onPostUpdate();
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AnimationTimerHandler::OnRenderingTooSlow(
        UINT32 framesPerSecond)
    {
        if (listener_) {
            listener_->onRenderingTooSlow(framesPerSecond);
        }

        return S_OK;
    }


    IFACEMETHODIMP_(ULONG) AnimationTimerHandler::AddRef()
    {
        return InterlockedIncrement(&ref_count_);
    }

    IFACEMETHODIMP_(ULONG) AnimationTimerHandler::Release()
    {
        ULONG newCount = InterlockedDecrement(&ref_count_);

        if (newCount == 0) {
            delete this;
            return 0;
        }

        return newCount;
    }

    IFACEMETHODIMP AnimationTimerHandler::QueryInterface(
        _In_ REFIID riid, _Outptr_ void** ppOutput)
    {
        if (__uuidof(IUIAnimationTimerEventHandler) == riid) {
            *ppOutput = this;
        }
        else if (__uuidof(IUnknown) == riid) {
            *ppOutput = this;
        }
        else {
            *ppOutput = nullptr;
            return E_FAIL;
        }

        AddRef();

        return S_OK;
    }

}