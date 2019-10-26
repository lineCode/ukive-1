#ifndef UKIVE_ANIMATION_SCROLLER_H_
#define UKIVE_ANIMATION_SCROLLER_H_

#include <cstdint>


namespace ukive {

    class Window;

    class Scroller {
    public:
        explicit Scroller(Window* w);
        ~Scroller();

        void inertia(int start_x, int start_y, float velocity_x, float velocity_y);
        void startScroll(int start_x, int start_y, int dx, int dy, uint64_t duration);

        bool compute();
        void finish();

        bool isFinished() const;
        int getDeltaX() const;
        int getDeltaY() const;
        int getCurScrollX() const;
        int getCurScrollY() const;

    private:
        enum Type {
            INERTIA,
            LINEAR,
        };

        int cur_x_ = 0;
        int cur_y_ = 0;
        int prev_x_ = 0;
        int prev_y_ = 0;
        int delta_x_ = 0;
        int delta_y_ = 0;
        bool is_finished_ = true;

        int start_x_ = 0;
        int start_y_ = 0;
        uint64_t start_time_ = 0;
        Type type_ = LINEAR;

        // Linear
        int distance_x_ = 0;
        int distance_y_ = 0;
        uint64_t duration_ = 0;

        // inertia
        uint64_t prev_time_ = 0;
        float cur_velocity_x_ = 0;
        float cur_velocity_y_ = 0;
        float decelerate_x_ = 0;
        float decelerate_y_ = 0;
    };

}

#endif  // UKIVE_ANIMATION_SCROLLER_H_