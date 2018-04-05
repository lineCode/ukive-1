#include "animation_variable_change_handler.h"


namespace ukive {

    AnimationVariableChangeHandler::AnimationVariableChangeHandler(
        Animator::OnValueChangedListener *listener, unsigned int varIndex)
    {
        ref_count_ = 1;
        mVarIndex = varIndex;
        listener_ = listener;
    }


    AnimationVariableChangeHandler::~AnimationVariableChangeHandler()
    {
    }


    HRESULT STDMETHODCALLTYPE AnimationVariableChangeHandler::OnValueChanged(
        _In_  IUIAnimationStoryboard *storyboard,
        _In_  IUIAnimationVariable *variable,
        _In_  DOUBLE newValue,
        _In_  DOUBLE previousValue)
    {
        if (listener_)
            listener_->onValueChanged(mVarIndex, storyboard, variable, newValue, previousValue);

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AnimationVariableChangeHandler::OnIntegerValueChanged(
        _In_  IUIAnimationStoryboard *storyboard,
        _In_  IUIAnimationVariable *variable,
        _In_  INT32 newValue,
        _In_  INT32 previousValue)
    {
        if (listener_)
            listener_->onIntegerValueChanged(mVarIndex, storyboard, variable, newValue, previousValue);

        return S_OK;
    }


    IFACEMETHODIMP_(ULONG) AnimationVariableChangeHandler::AddRef()
    {
        return InterlockedIncrement(&ref_count_);
    }

    IFACEMETHODIMP_(ULONG) AnimationVariableChangeHandler::Release()
    {
        unsigned long newCount = InterlockedDecrement(&ref_count_);

        if (newCount == 0)
        {
            delete this;
            return 0;
        }

        return newCount;
    }

    IFACEMETHODIMP AnimationVariableChangeHandler::QueryInterface(
        _In_ REFIID riid, _Outptr_ void** ppOutput)
    {
        if (__uuidof(IUIAnimationVariableChangeHandler) == riid)
        {
            *ppOutput = this;
        }
        if (__uuidof(IUIAnimationVariableIntegerChangeHandler) == riid)
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