#ifndef UKIVE_ANIMATION_ANIMATION_TIMER_HANDLER_H_
#define UKIVE_ANIMATION_ANIMATION_TIMER_HANDLER_H_

#include "ukive/animation/animation_manager.h"


namespace ukive {

    class AnimationTimerHandler : public IUIAnimationTimerEventHandler {
    public:
        AnimationTimerHandler(
            AnimationManager::OnTimerEventListener *listener);
        ~AnimationTimerHandler();

        HRESULT STDMETHODCALLTYPE OnPreUpdate();
        HRESULT STDMETHODCALLTYPE OnPostUpdate();
        HRESULT STDMETHODCALLTYPE OnRenderingTooSlow(
            UINT32 framesPerSecond);

        IFACEMETHODIMP_(ULONG) AddRef();
        IFACEMETHODIMP_(ULONG) Release();
        IFACEMETHODIMP QueryInterface(_In_ REFIID riid, _Outptr_ void** ppOutput);

    private:
        ULONG ref_count_;
        AnimationManager::OnTimerEventListener *listener_;
    };

}

#endif  // UKIVE_ANIMATION_ANIMATION_TIMER_HANDLER_H_