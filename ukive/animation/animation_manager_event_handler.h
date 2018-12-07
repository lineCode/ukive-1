#ifndef UKIVE_ANIMATION_ANIMATION_MANAGER_EVENT_HANDLER_H_
#define UKIVE_ANIMATION_ANIMATION_MANAGER_EVENT_HANDLER_H_

#include "ukive/animation/animation_manager.h"


namespace ukive {

    class AnimationManagerEventHandler : public IUIAnimationManagerEventHandler {
    public:
        AnimationManagerEventHandler(
            AnimationManager::OnStateChangedListener* listener);
        ~AnimationManagerEventHandler();

        HRESULT STDMETHODCALLTYPE OnManagerStatusChanged(
            UI_ANIMATION_MANAGER_STATUS newStatus,
            UI_ANIMATION_MANAGER_STATUS previousStatus);

        IFACEMETHODIMP_(ULONG) AddRef();
        IFACEMETHODIMP_(ULONG) Release();
        IFACEMETHODIMP QueryInterface(REFIID riid, void** ppOutput);

    private:
        ULONG ref_count_;
        AnimationManager::OnStateChangedListener* listener_;
    };

}

#endif  // UKIVE_ANIMATION_ANIMATION_MANAGER_EVENT_HANDLER_H_