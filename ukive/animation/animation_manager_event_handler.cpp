#include "animation_manager_event_handler.h"


namespace ukive {

    AnimationManagerEventHandler::AnimationManagerEventHandler(
        AnimationManager::OnStateChangedListener *listener)
    {
        ref_count_ = 1;
        listener_ = listener;
    }


    AnimationManagerEventHandler::~AnimationManagerEventHandler()
    {
    }


    HRESULT STDMETHODCALLTYPE AnimationManagerEventHandler::OnManagerStatusChanged(
        _In_  UI_ANIMATION_MANAGER_STATUS newStatus,
        _In_  UI_ANIMATION_MANAGER_STATUS previousStatus)
    {
        if (listener_)
            listener_->onStateChanged(newStatus, previousStatus);

        return S_OK;
    }


    IFACEMETHODIMP_(ULONG) AnimationManagerEventHandler::AddRef()
    {
        return InterlockedIncrement(&ref_count_);
    }

    IFACEMETHODIMP_(ULONG) AnimationManagerEventHandler::Release()
    {
        unsigned long newCount = InterlockedDecrement(&ref_count_);

        if (newCount == 0)
        {
            delete this;
            return 0;
        }

        return newCount;
    }

    IFACEMETHODIMP AnimationManagerEventHandler::QueryInterface(
        _In_ REFIID riid, _Outptr_ void** ppOutput)
    {
        if (__uuidof(IUIAnimationManagerEventHandler) == riid)
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