#include "animation_timer_handler.h"


namespace ukive {

    AnimationTimerHandler::AnimationTimerHandler(
        AnimationManager::OnTimerEventListener *listener)
        :mRefCount(1),
        mListener(listener) {}

    AnimationTimerHandler::~AnimationTimerHandler() {}


    HRESULT STDMETHODCALLTYPE AnimationTimerHandler::OnPreUpdate()
    {
        if (mListener) {
            mListener->onPreUpdate();
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AnimationTimerHandler::OnPostUpdate()
    {
        if (mListener) {
            mListener->onPostUpdate();
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AnimationTimerHandler::OnRenderingTooSlow(
        UINT32 framesPerSecond)
    {
        if (mListener) {
            mListener->onRenderingTooSlow(framesPerSecond);
        }

        return S_OK;
    }


    IFACEMETHODIMP_(ULONG) AnimationTimerHandler::AddRef()
    {
        return InterlockedIncrement(&mRefCount);
    }

    IFACEMETHODIMP_(ULONG) AnimationTimerHandler::Release()
    {
        ULONG newCount = InterlockedDecrement(&mRefCount);

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