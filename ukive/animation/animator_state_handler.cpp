#include "animator_state_handler.h"


namespace ukive {

    AnimatorStateHandler::AnimatorStateHandler(Animator* animator)
        :ref_count_(1),
        listener_(nullptr),
        animator_(animator) {
    }

    AnimatorStateHandler::~AnimatorStateHandler() {
    }


    void AnimatorStateHandler::setOnAnimatorListener(Animator::OnAnimatorListener* l) {
        listener_ = l;
    }


    HRESULT STDMETHODCALLTYPE AnimatorStateHandler::OnStoryboardStatusChanged(
        IUIAnimationStoryboard* storyboard,
        UI_ANIMATION_STORYBOARD_STATUS newStatus,
        UI_ANIMATION_STORYBOARD_STATUS previousStatus)
    {
        if (listener_) {
            switch (newStatus) {
                //正在创建。
            case UI_ANIMATION_STORYBOARD_BUILDING:
                break;
                //准备播放。
            case UI_ANIMATION_STORYBOARD_SCHEDULED:
                break;
                //被取消。
            case UI_ANIMATION_STORYBOARD_CANCELLED:
                listener_->onAnimationCancel(animator_);
                break;
                //正在播放。
            case UI_ANIMATION_STORYBOARD_PLAYING:
                listener_->onAnimationStart(animator_);
                break;
                //动画被截短。
            case UI_ANIMATION_STORYBOARD_TRUNCATED:
                break;
                //播放完毕。
            case UI_ANIMATION_STORYBOARD_FINISHED:
                listener_->onAnimationEnd(animator_);
                break;
                //创建完毕，可以播放。
            case UI_ANIMATION_STORYBOARD_READY:
                break;
                //权限不足。
            case UI_ANIMATION_STORYBOARD_INSUFFICIENT_PRIORITY:
                break;
            }
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AnimatorStateHandler::OnStoryboardUpdated(
        IUIAnimationStoryboard* storyboard) {
        return S_OK;
    }


    IFACEMETHODIMP_(ULONG) AnimatorStateHandler::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }

    IFACEMETHODIMP_(ULONG) AnimatorStateHandler::Release() {
        auto nc = InterlockedDecrement(&ref_count_);
        if (nc == 0) {
            delete this;
        }

        return nc;
    }

    IFACEMETHODIMP AnimatorStateHandler::QueryInterface(
        REFIID riid, void** ppOutput) {

        if (ppOutput == nullptr) {
            return E_POINTER;
        }

        if (__uuidof(IUIAnimationStoryboardEventHandler) == riid) {
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