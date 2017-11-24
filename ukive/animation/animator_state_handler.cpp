#include "animator_state_handler.h"


namespace ukive {

    AnimatorStateHandler::AnimatorStateHandler(Animator *animator)
    {
        mRefCount = 1;
        mListener = nullptr;
        mAnimator = animator;
    }

    AnimatorStateHandler::~AnimatorStateHandler()
    {
    }


    void AnimatorStateHandler::setOnAnimatorListener(Animator::OnAnimatorListener *l)
    {
        mListener = l;
    }


    HRESULT STDMETHODCALLTYPE AnimatorStateHandler::OnStoryboardStatusChanged(
        IUIAnimationStoryboard *storyboard,
        UI_ANIMATION_STORYBOARD_STATUS newStatus,
        UI_ANIMATION_STORYBOARD_STATUS previousStatus)
    {
        if (mListener)
        {
            switch (newStatus)
            {
                //���ڴ�����
            case UI_ANIMATION_STORYBOARD_BUILDING:
                break;
                //׼�����š�
            case UI_ANIMATION_STORYBOARD_SCHEDULED:
                break;
                //��ȡ����
            case UI_ANIMATION_STORYBOARD_CANCELLED:
                mListener->onAnimationCancel(mAnimator);
                break;
                //���ڲ��š�
            case UI_ANIMATION_STORYBOARD_PLAYING:
                mListener->onAnimationStart(mAnimator);
                break;
                //�������ض̡�
            case UI_ANIMATION_STORYBOARD_TRUNCATED:
                break;
                //������ϡ�
            case UI_ANIMATION_STORYBOARD_FINISHED:
                mListener->onAnimationEnd(mAnimator);
                break;
                //������ϣ����Բ��š�
            case UI_ANIMATION_STORYBOARD_READY:
                break;
                //Ȩ�޲��㡣
            case UI_ANIMATION_STORYBOARD_INSUFFICIENT_PRIORITY:
                break;
            }
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AnimatorStateHandler::OnStoryboardUpdated(
        IUIAnimationStoryboard *storyboard)
    {
        return S_OK;
    }


    IFACEMETHODIMP_(ULONG) AnimatorStateHandler::AddRef()
    {
        return InterlockedIncrement(&mRefCount);
    }

    IFACEMETHODIMP_(ULONG) AnimatorStateHandler::Release()
    {
        unsigned long newCount = InterlockedDecrement(&mRefCount);

        if (newCount == 0)
        {
            delete this;
            return 0;
        }

        return newCount;
    }

    IFACEMETHODIMP AnimatorStateHandler::QueryInterface(
        _In_ REFIID riid, _Outptr_ void** ppOutput)
    {
        if (__uuidof(IUIAnimationStoryboardEventHandler) == riid)
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