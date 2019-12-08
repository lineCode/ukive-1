#include "utils/xml/xml_pedometer.h"


namespace utl {

    XMLPedometer::XMLPedometer()
        : cur_row_(1),
          cur_col_(1),
          is_r_prefix_(false) {}

    void XMLPedometer::step(uint32_t count) {
        cur_col_ += count;
        is_r_prefix_ = false;
    }

    void XMLPedometer::space(uint32_t ch) {
        if (ch == 0x20 || ch == 0x9) { // 空格和制表符
            ++cur_col_;
            is_r_prefix_ = false;
        } else if (ch == 0xD) {        // \r
            is_r_prefix_ = true;
            ++cur_row_;
            cur_col_ = 1;
        } else if (ch == 0xA) {        // \n
            if (!is_r_prefix_) {
                ++cur_row_;
                cur_col_ = 1;
            } else {
                is_r_prefix_ = false;
            }
        } else {
            is_r_prefix_ = false;
        }
    }

    void XMLPedometer::newline() {
        is_r_prefix_ = false;
        ++cur_row_;
        cur_col_ = 1;
    }

    void XMLPedometer::reset() {
        cur_row_ = 1;
        cur_col_ = 1;
        is_r_prefix_ = false;
    }

    void XMLPedometer::save() {
        history_.push({ cur_row_, cur_col_ });
    }

    void XMLPedometer::restore() {
        if (history_.empty()) {
            return;
        }

        cur_row_ = history_.top().first;
        cur_col_ = history_.top().second;
        history_.pop();
    }

    void XMLPedometer::discard() {
        if (!history_.empty()) {
            history_.pop();
        }
    }

    uint32_t XMLPedometer::getCurRow() const {
        return cur_row_;
    }

    uint32_t XMLPedometer::getCurCol() const {
        return cur_col_;
    }

}