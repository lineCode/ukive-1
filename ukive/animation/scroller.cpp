#include "scroller.h"

#include <algorithm>

#include "ukive/system/time_utils.h"
#include "ukive/window/window.h"
#include "ukive/log.h"


namespace ukive {

    Scroller::Scroller(Window *w) {
        int dpi_x, dpi_y;
        w->getDpi(&dpi_x, &dpi_y);

        float mu = 0.02f;
        init_dec_x_ = mu * (9.78f * 100 / 2.54f * dpi_x);
        init_dec_y_ = mu * (9.78f * 100 / 2.54f * dpi_y);
    }

    Scroller::~Scroller() {
    }

    void Scroller::inertia(int start_x, int start_y, float velocity_x, float velocity_y) {
        if (velocity_x == 0 && velocity_y == 0) {
            return;
        }

        delta_x_ = 0;
        delta_y_ = 0;
        prev_x_ = cur_x_ = start_x_ = start_x;
        prev_y_ = cur_y_ = start_y_ = start_y;
        prev_time_ = start_time_ = TimeUtils::upTimeMillisPrecise();
        type_ = INERTIA;
        is_finished_ = false;

        if ((cur_velocity_x_ < 0 && velocity_x > 0) ||
            (cur_velocity_x_ > 0 && velocity_x < 0))
        {
            cur_velocity_x_ = velocity_x;
        } else {
            cur_velocity_x_ += velocity_x;
        }

        if ((cur_velocity_y_ < 0 && velocity_y > 0) ||
            (cur_velocity_y_ > 0 && velocity_y < 0))
        {
            cur_velocity_y_ = velocity_y;
        } else {
            cur_velocity_y_ += velocity_y;
        }

        float mu = 0.05f;
        decelerate_x_ = init_dec_x_;
        if (velocity_x >= 0) {
            decelerate_x_ *= -1;
        }

        decelerate_y_ = init_dec_y_;
        if (velocity_y >= 0) {
            decelerate_y_ *= -1;
        }
    }

    void Scroller::startScroll(int start_x, int start_y, int dx, int dy, uint64_t duration) {
        if (dx == 0 && dy == 0) {
            return;
        }

        delta_x_ = 0;
        delta_y_ = 0;
        prev_x_ = cur_x_ = start_x_ = start_x;
        prev_y_ = cur_y_ = start_y_ = start_y;
        start_time_ = TimeUtils::upTimeMillisPrecise();
        type_ = LINEAR;
        is_finished_ = false;

        distance_x_ = dx;
        distance_y_ = dy;
        duration_ = duration;
    }

    bool Scroller::compute() {
        if (is_finished_) {
            return false;
        }

        auto cur_time = TimeUtils::upTimeMillisPrecise();

        if (type_ == LINEAR) {
            auto elapsed = cur_time - start_time_;
            if (elapsed >= duration_) {
                cur_x_ = start_x_ + distance_x_;
                cur_y_ = start_y_ + distance_y_;
                is_finished_ = true;
            } else {
                double percent = std::min(elapsed / double(duration_), 1.0);
                cur_x_ = start_x_ + distance_x_ * percent;
                cur_y_ = start_y_ + distance_y_ * percent;
            }
        } else if (type_ == INERTIA) {
            auto elapsed = (cur_time - prev_time_) / 1000.f;

            if (cur_velocity_x_ != 0) {
                float vx1 = cur_velocity_x_ + decelerate_x_ * elapsed;
                if (vx1 != 0) {
                    if ((vx1 > 0 && cur_velocity_x_ < 0) ||
                        (vx1 < 0 && cur_velocity_x_ > 0))
                    {
                        elapsed = -cur_velocity_x_ / decelerate_x_;
                        vx1 = 0;
                    }
                }
                cur_x_ += std::ceil((cur_velocity_x_ + 0.5f * decelerate_x_ * elapsed) * elapsed);
                cur_velocity_x_ = vx1;
            }

            if (cur_velocity_y_ != 0) {
                float vy1 = cur_velocity_y_ + decelerate_y_ * elapsed;
                if (vy1 != 0) {
                    if ((vy1 > 0 && cur_velocity_y_ < 0) ||
                        (vy1 < 0 && cur_velocity_y_ > 0))
                    {
                        elapsed = -cur_velocity_y_ / decelerate_y_;
                        vy1 = 0;
                    }
                }
                cur_y_ += std::ceil((cur_velocity_y_ + 0.5f * decelerate_y_ * elapsed) * elapsed);
                cur_velocity_y_ = vy1;
            }

            if (cur_velocity_x_ == 0 && cur_velocity_y_ == 0) {
                is_finished_ = true;
            }

            /*DLOG(Log::INFO) << "compute## elapsed:" << elapsed
                << " velocity:" << cur_velocity_y_
                << " dy:" << (cur_y_ - prev_y_);*/
        } else {
            is_finished_ = true;
        }

        delta_x_ = cur_x_ - prev_x_;
        delta_y_ = cur_y_ - prev_y_;
        prev_x_ = cur_x_;
        prev_y_ = cur_y_;
        prev_time_ = cur_time;

        return true;
    }

    void Scroller::finish() {
        is_finished_ = true;
        cur_velocity_y_ = 0;
        cur_velocity_x_ = 0;
    }

    bool Scroller::isFinished() const {
        return is_finished_;
    }

    int Scroller::getDeltaX() const {
        return delta_x_;
    }

    int Scroller::getDeltaY() const {
        return delta_y_;
    }

    int Scroller::getCurScrollX() const {
        return cur_x_;
    }

    int Scroller::getCurScrollY() const {
        return cur_y_;
    }

}
