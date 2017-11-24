#ifndef UKIVE_ANIMATION_SCROLLER_H_
#define UKIVE_ANIMATION_SCROLLER_H_


namespace ukive {

    class Window;
    class Animator;

    class Scroller
    {
    private:
        Animator *mScrollAnimator;

    public:
        Scroller(Window *wnd);
        ~Scroller();

        void fling(int startX, int startY, float velocityX, float velocityY);
        void startScroll(int startX, int startY, int dx, int dy, double duration);
    };

}

#endif  // UKIVE_ANIMATION_SCROLLER_H_