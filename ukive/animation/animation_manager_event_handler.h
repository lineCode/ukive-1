#ifndef UKIVE_ANIMATION_ANIMATION_MANAGER_EVENT_HANDLER_H_
#define UKIVE_ANIMATION_ANIMATION_MANAGER_EVENT_HANDLER_H_

#include "ukive/animation/animation_manager.h"


namespace ukive {

    class AnimationManagerEventHandler : public IUIAnimationManagerEventHandler {
    public:
        AnimationManagerEventHandler(
            AnimationManager::OnStateChangedListener *listener);
        ~AnimationManagerEventHandler();

        HRESULT STDMETHODCALLTYPE OnManagerStatusChanged(
            _In_  UI_ANIMATION_MANAGER_STATUS newStatus,
            _In_  UI_ANIMATION_MANAGER_STATUS previousStatus);

        IFACEMETHODIMP_(ULONG) AddRef();
        IFACEMETHODIMP_(ULONG) Release();
        IFACEMETHODIMP QueryInterface(_In_ REFIID riid, _Outptr_ void** ppOutput);

    private:
        ULONG ref_count_;
        AnimationManager::OnStateChangedListener *listener_;
    };

}

#endif  // UKIVE_ANIMATION_ANIMATION_MANAGER_EVENT_HANDLER_H_