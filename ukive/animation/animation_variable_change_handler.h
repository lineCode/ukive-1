#ifndef UKIVE_ANIMATION_ANIMATION_VARIABLE_CHANGE_HANDLER_H_
#define UKIVE_ANIMATION_ANIMATION_VARIABLE_CHANGE_HANDLER_H_

#include "ukive/animation/animator.h"
#include "ukive/animation/animation_manager.h"


namespace ukive {

    class AnimationVariableChangeHandler
        : public IUIAnimationVariableChangeHandler,
        public IUIAnimationVariableIntegerChangeHandler
    {
    private:
        ULONG ref_count_;
        unsigned int mVarIndex;
        Animator::OnValueChangedListener *listener_;

    public:
        AnimationVariableChangeHandler(
            Animator::OnValueChangedListener *listener, unsigned int varIndex);
        ~AnimationVariableChangeHandler();

        HRESULT STDMETHODCALLTYPE OnValueChanged(
            _In_  IUIAnimationStoryboard *storyboard,
            _In_  IUIAnimationVariable *variable,
            _In_  DOUBLE newValue,
            _In_  DOUBLE previousValue);

        HRESULT STDMETHODCALLTYPE OnIntegerValueChanged(
            _In_  IUIAnimationStoryboard *storyboard,
            _In_  IUIAnimationVariable *variable,
            _In_  INT32 newValue,
            _In_  INT32 previousValue);

        IFACEMETHODIMP_(ULONG) AddRef();
        IFACEMETHODIMP_(ULONG) Release();
        IFACEMETHODIMP QueryInterface(_In_ REFIID riid, _Outptr_ void** ppOutput);
    };

}

#endif  // UKIVE_ANIMATION_ANIMATION_VARIABLE_CHANGE_HANDLER_H_