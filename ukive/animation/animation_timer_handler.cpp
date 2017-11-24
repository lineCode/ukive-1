#include "animation_timer_handler.h"


namespace ukive {

    AnimationTimerHandler::AnimationTimerHandler(
        AnimationManager::OnTimerEventListener *listener)
    {
        mRefCount = 1;
        mListener = listener;
    }


    AnimationTimerHandler::~AnimationTimerHandler()
    {
    }


    HRESULT STDMETHODCALLTYPE AnimationTimerHandler::OnPreUpdate()
    {
        if (mListener)
            mListener->OnPreUpdate();

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AnimationTimerHandler::OnPostUpdate()
    {
        if (mListener)
            mListener->OnPostUpdate();

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AnimationTimerHandler::OnRenderingTooSlow(
        UINT32 framesPerSecond)
    {
        if (mListener)
            mListener->OnRenderingTooSlow(framesPerSecond);

        return S_OK;
    }


    IFACEMETHODIMP_(ULONG) AnimationTimerHandler::AddRef()
    {
        return InterlockedIncrement(&mRefCount);
    }

    IFACEMETHODIMP_(ULONG) AnimationTimerHandler::Release()
    {
        unsigned long newCount = InterlockedDecrement(&mRefCount);

        if (newCount == 0)
        {
            delete this;
            return 0;
        }

        return newCount;
    }

    IFACEMETHODIMP AnimationTimerHandler::QueryInterface(
        _In_ REFIID riid, _Outptr_ void** ppOutput)
    {
        if (__uuidof(IUIAnimationTimerEventHandler) == riid)
        {
            *ppOutput = this;
        }
        else if (__uuidof(IUnknown) == riid)
        {
            *ppOutput = this;
        }
        else
        {
            *ppOutput = 0;
            return E_FAIL;
        }

        AddRef();

        return S_OK;
    }

}