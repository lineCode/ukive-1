#ifndef UKIVE_ANIMATION_ANIMATOR_STATE_HANDLER_H_
#define UKIVE_ANIMATION_ANIMATOR_STATE_HANDLER_H_

#include "ukive/animation/animator.h"
#include "ukive/animation/animation_manager.h"


namespace ukive {

    class AnimatorStateHandler
        : public IUIAnimationStoryboardEventHandler
    {
    private:
        ULONG mRefCount;
        Animator *mAnimator;
        Animator::OnAnimatorListener *mListener;

    public:
        AnimatorStateHandler(Animator *animator);
        ~AnimatorStateHandler();

        void setOnAnimatorListener(Animator::OnAnimatorListener *l);

        HRESULT STDMETHODCALLTYPE OnStoryboardStatusChanged(
            IUIAnimationStoryboard *storyboard,
            UI_ANIMATION_STORYBOARD_STATUS newStatus,
            UI_ANIMATION_STORYBOARD_STATUS previousStatus);

        HRESULT STDMETHODCALLTYPE OnStoryboardUpdated(
            IUIAnimationStoryboard *storyboard);

        IFACEMETHODIMP_(ULONG) AddRef();
        IFACEMETHODIMP_(ULONG) Release();
        IFACEMETHODIMP QueryInterface(_In_ REFIID riid, _Outptr_ void** ppOutput);
    };

}

#endif  // UKIVE_ANIMATION_ANIMATOR_STATE_HANDLER_H_