#include "ukive/event/velocity_calculator.h"

#include <algorithm>

#include "ukive/event/input_event.h"
#include "ukive/system/time_utils.h"


namespace {

    const float kMaxVelocity = 10000;

}

namespace ukive {

    void VelocityCalculator::onInputEvent(const InputEvent* e) {
        switch (e->getEvent()) {
        case InputEvent::EVT_DOWN:
            is_touch_down_ = true;
            start_x_ = prev_x_ = e->getX();
            start_y_ = prev_y_ = e->getX();
            prev_time_ = TimeUtils::upTimeMicros();
            break;

        case InputEvent::EVT_MOVE:
        {
            int cur_x = e->getX();
            int cur_y = e->getY();
            int dx = cur_x - start_x_;
            int dy = cur_y - start_y_;
            if (dx * dx + dy * dy > 12 * 12) {
                calculate(cur_x, cur_y);

                prev_x_ = cur_x;
                prev_y_ = cur_y;
                prev_time_ = TimeUtils::upTimeMicros();
            } else {
                velocity_x_ = 0;
                velocity_y_ = 0;
            }
            break;
        }

        case InputEvent::EVT_UP:
        {
            int cur_x = e->getX();
            int cur_y = e->getY();
            int dx = cur_x - start_x_;
            int dy = cur_y - start_y_;
            if (dx * dx + dy * dy > 12 * 12) {
                if (cur_x != prev_x_ || cur_y != prev_y_) {
                    calculate(cur_x, cur_y);
                }
            } else {
                velocity_x_ = 0;
                velocity_y_ = 0;
            }
            break;
        }

        default:
            break;
        }
    }

    void VelocityCalculator::calculate(int cur_x, int cur_y) {
        auto duration = float(TimeUtils::upTimeMicros() - prev_time_) / 1000000;
        if (duration == 0) {
            if (cur_x == prev_x_) {
                velocity_x_ = 0;
            } else {
                velocity_x_ = kMaxVelocity;
            }

            if (cur_y == prev_y_) {
                velocity_y_ = 0;
            } else {
                velocity_y_ = kMaxVelocity;
            }
        } else {
            velocity_x_ = float(cur_x - prev_x_) / duration;
            if (velocity_x_ > kMaxVelocity) {
                velocity_x_ = kMaxVelocity;
            }
            if (velocity_x_ < -kMaxVelocity) {
                velocity_x_ = -kMaxVelocity;
            }

            velocity_y_ = float(cur_y - prev_y_) / duration;
            if (velocity_y_ > kMaxVelocity) {
                velocity_y_ = kMaxVelocity;
            }
            if (velocity_y_ < -kMaxVelocity) {
                velocity_y_ = -kMaxVelocity;
            }
        }
    }

    float VelocityCalculator::getVelocityX() const {
        return velocity_x_;
    }

    float VelocityCalculator::getVelocityY() const {
        return velocity_y_;
    }

}