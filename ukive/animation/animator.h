#ifndef UKIVE_ANIMATION_ANIMATOR_H_
#define UKIVE_ANIMATION_ANIMATOR_H_

#include <map>

#include "ukive/animation/transition.h"
#include "ukive/utils/com_ptr.h"


namespace ukive {

    class AnimationManager;
    class AnimatorStateHandler;

    class Animator
    {
    public:
        class OnValueChangedListener {
        public:
            virtual void onValueChanged(
                unsigned int varIndex,
                IUIAnimationStoryboard* storyboard,
                IUIAnimationVariable* variable,
                double newValue, double previousValue) = 0;
            virtual void onIntegerValueChanged(
                unsigned int varIndex,
                IUIAnimationStoryboard* storyboard,
                IUIAnimationVariable* variable,
                int newValue, int previousValue) = 0;
        };

        class OnAnimatorListener {
        public:
            virtual void onAnimationStart(Animator* animator) = 0;
            virtual void onAnimationEnd(Animator* animator) = 0;
            virtual void onAnimationCancel(Animator* animator) = 0;
        };

    public:
        Animator(AnimationManager* mgr);
        ~Animator();

        /// <summary>
        /// 执行定义在Storyboard上的动画。
        /// 如果该Storyboard之前已经执行动画，则调用无效。
        /// 此时需要先调用reset()方法重新创建Storyboard。
        /// 若要执行简单的单变量动画，请使用startTransition()方法。
        /// </summary>
        void start();

        /// <summary>
        /// 立即停止Storyboard动画。
        /// 若当前没有动画，则调用无效。
        /// 动画停止后，动画值将保留。
        /// </summary>
        void stop();

        /// <summary>
        /// 在指定时间内播放完Storyboard动画。
        /// 若当前没有动画，则调用无效。
        /// </summary>
        void finish(double second);

        /// <summary>
        /// 创建一个新的Storyboard实例。
        /// 旧的Storyboard实例将被删除，之前添加的变量也将被删除。
        /// 若当前的Storyboard从没执行，则调用无效。
        /// 若当前的Storyboard正在动画，则动画将停止。
        /// </summary>
        void reset();

        /// <summary>
        /// 开始一个Transition动画。
        /// 该动画是独立的，无法取消，只能等待动画结束。
        /// 适用于简单的单变量，且无需额外控制的动画。
        /// </summary>
        void startTransition(unsigned int varIndex, std::shared_ptr<Transition> transition);

        void setOnStateChangedListener(OnAnimatorListener* l);
        void setOnValueChangedListener(
            unsigned int varIndex, OnValueChangedListener* l);

        double getValue(unsigned int varIndex);
        int getIntValue(unsigned int varIndex);

        double getPrevValue(unsigned int varIndex);
        int getPrevIntValue(unsigned int varIndex);

        double getFinalValue(unsigned int varIndex);
        int getFinalIntValue(unsigned int varIndex);

        double getElapsedTime();

        bool addVariable(
            unsigned int varIndex, double initValue,
            double lower, double upper);
        bool addTransition(
            unsigned int varIndex, std::shared_ptr<Transition> transition);
        bool addTransition(
            unsigned int varIndex, std::shared_ptr<Transition> transition,
            UI_ANIMATION_KEYFRAME key);
        bool addTransition(
            unsigned int varIndex, std::shared_ptr<Transition> transition,
            UI_ANIMATION_KEYFRAME startKey, UI_ANIMATION_KEYFRAME endKey);
        bool addKey(
            UI_ANIMATION_KEYFRAME existed, double offset, UI_ANIMATION_KEYFRAME* newKey);
        bool addKey(
            std::shared_ptr<Transition> transition, UI_ANIMATION_KEYFRAME* newKey);

        bool hasVariable(unsigned int varIndex);
        bool removeVariable(unsigned int varIndex);

    private:
        AnimationManager* anim_mgr_;
        ComPtr<IUIAnimationStoryboard> mStoryboard;
        AnimatorStateHandler* mAnimatorStateListener;

        std::map<unsigned int, ComPtr<IUIAnimationVariable>> mVariableList;
    };

}

#endif  // UKIVE_ANIMATION_ANIMATOR_H_