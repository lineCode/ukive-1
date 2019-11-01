#ifndef UKIVE_EVENT_VELOCITY_CALCULATOR_H_
#define UKIVE_EVENT_VELOCITY_CALCULATOR_H_

#include <cstdint>


namespace ukive {

    class InputEvent;

    class VelocityCalculator {
    public:
        VelocityCalculator() = default;

        void onInputEvent(const InputEvent* e);

        float getVelocityX() const;
        float getVelocityY() const;

    private:
        void calculate(int cur_x, int cur_y);

        int prev_x_ = 0;
        int prev_y_ = 0;
        int start_x_ = 0;
        int start_y_ = 0;
        bool is_touch_down_ = false;

        uint64_t prev_time_ = 0;
        float velocity_x_ = 0;
        float velocity_y_ = 0;
    };

}

#endif  // UKIVE_EVENT_VELOCITY_CALCULATOR_H_