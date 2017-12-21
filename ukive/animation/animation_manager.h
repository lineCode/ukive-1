#ifndef UKIVE_ANIMATION_ANIMATION_MANAGER_H_
#define UKIVE_ANIMATION_ANIMATION_MANAGER_H_

#include <uianimation.h>

#include "ukive/utils/com_ptr.h"


namespace ukive {

    class AnimationManager
    {
    private:
        ComPtr<IUIAnimationManager> mAnimationManager;
        ComPtr<IUIAnimationTimer> mAnimationTimer;

    public:
        static IUIAnimationTransitionLibrary *sTransitionLibrary;
        static IUIAnimationTransitionFactory *sTransitionFactory;

        class OnStateChangedListener
        {
        public:
            virtual void onStateChanged(
                UI_ANIMATION_MANAGER_STATUS newStatus,
                UI_ANIMATION_MANAGER_STATUS previousStatus) = 0;
        };

        class OnTimerEventListener
        {
        public:
            virtual void onPreUpdate() = 0;
            virtual void onPostUpdate() = 0;
            virtual void onRenderingTooSlow(unsigned int fps) = 0;
        };

    public:
        AnimationManager();
        ~AnimationManager();

        HRESULT init();
        void close();

        static HRESULT initGlobal();
        static void closeGlobal();

        void pause();
        void resume();
        void finish(double second);
        void abandon();
        bool isBusy();
        bool update();

        void connectTimer(bool enable);

        void setTimerEventListener(OnTimerEventListener *l);
        void setOnStateChangedListener(OnStateChangedListener *l);

        IUIAnimationManager *getAnimationManager();
        IUIAnimationTimer *getAnimationTimer();
        IUIAnimationTransitionLibrary *getTransitionLibrary();
    };

}

#endif  // UKIVE_ANIMATION_ANIMATION_MANAGER_H_