#ifndef UKIVE_ANIMATION_ANIMATION_VARIABLE_CHANGE_HANDLER_H_
#define UKIVE_ANIMATION_ANIMATION_VARIABLE_CHANGE_HANDLER_H_

#include "ukive/animation/animator.h"
#include "ukive/animation/animation_manager.h"


namespace ukive {

    class AnimationVariableChangeHandler
        : public IUIAnimationVariableChangeHandler,
        public IUIAnimationVariableIntegerChangeHandler {
    public:
        AnimationVariableChangeHandler(
            Animator::OnValueChangedListener* listener, unsigned int varIndex);
        ~AnimationVariableChangeHandler();

        HRESULT STDMETHODCALLTYPE OnValueChanged(
            IUIAnimationStoryboard* storyboard,
            IUIAnimationVariable* variable,
            DOUBLE newValue,
            DOUBLE previousValue);

        HRESULT STDMETHODCALLTYPE OnIntegerValueChanged(
            IUIAnimationStoryboard* storyboard,
            IUIAnimationVariable* variable,
            INT32 newValue,
            INT32 previousValue);

        IFACEMETHODIMP_(ULONG) AddRef();
        IFACEMETHODIMP_(ULONG) Release();
        IFACEMETHODIMP QueryInterface(REFIID riid, void** ppOutput);

    private:
        ULONG ref_count_;
        unsigned int var_index_;
        Animator::OnValueChangedListener* listener_;
    };

}

#endif  // UKIVE_ANIMATION_ANIMATION_VARIABLE_CHANGE_HANDLER_H_