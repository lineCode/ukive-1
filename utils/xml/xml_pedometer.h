#ifndef UTILS_XML_PEDOMETER_H_
#define UTILS_XML_PEDOMETER_H_

#include <cstdint>
#include <stack>


namespace utl {

    class XMLPedometer {
    public:
        XMLPedometer();

        void step(uint32_t count);
        void space(uint32_t ch);
        void newline();
        void reset();

        void save();
        void restore();
        void discard();

        uint32_t getCurRow() const;
        uint32_t getCurCol() const;

    private:
        uint32_t cur_row_;
        uint32_t cur_col_;
        bool is_r_prefix_;
        std::stack<std::pair<uint32_t, uint32_t>> history_;
    };

}

#endif  // UTILS_XML_PEDOMETER_H_