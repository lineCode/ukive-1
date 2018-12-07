#include "animation_variable_change_handler.h"


namespace ukive {

    AnimationVariableChangeHandler::AnimationVariableChangeHandler(
        Animator::OnValueChangedListener* listener, unsigned int varIndex)
        :ref_count_(1),
        var_index_(varIndex),
        listener_(listener) {
    }


    AnimationVariableChangeHandler::~AnimationVariableChangeHandler() {
    }


    HRESULT STDMETHODCALLTYPE AnimationVariableChangeHandler::OnValueChanged(
        IUIAnimationStoryboard* storyboard,
        IUIAnimationVariable* variable,
        DOUBLE newValue,
        DOUBLE previousValue) {

        if (listener_) {
            listener_->onValueChanged(var_index_, storyboard, variable, newValue, previousValue);
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AnimationVariableChangeHandler::OnIntegerValueChanged(
        IUIAnimationStoryboard* storyboard,
        IUIAnimationVariable* variable,
        INT32 newValue,
        INT32 previousValue) {

        if (listener_) {
            listener_->onIntegerValueChanged(var_index_, storyboard, variable, newValue, previousValue);
        }

        return S_OK;
    }


    IFACEMETHODIMP_(ULONG) AnimationVariableChangeHandler::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }

    IFACEMETHODIMP_(ULONG) AnimationVariableChangeHandler::Release() {
        auto nc = InterlockedDecrement(&ref_count_);
        if (nc == 0) {
            delete this;
        }

        return nc;
    }

    IFACEMETHODIMP AnimationVariableChangeHandler::QueryInterface(
        REFIID riid, void** ppOutput) {

        if (ppOutput == nullptr) {
            return E_POINTER;
        }

        if (__uuidof(IUIAnimationVariableChangeHandler) == riid) {
            *ppOutput = this;
        }
        if (__uuidof(IUIAnimationVariableIntegerChangeHandler) == riid) {
            *ppOutput = this;
        } else if (__uuidof(IUnknown) == riid) {
            *ppOutput = this;
        } else {
            *ppOutput = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

}