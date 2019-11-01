#include "tti_interpreter.h"

#define NEXT_BYTE()  \
    ++idx;           \
    if (idx >= length) { return false; }


namespace cyro {
namespace otf {

    TTInterpreter::TTInterpreter(CVT::CVTTable& cvt, GraphicsState& gs)
        : cvt_(cvt), gs_(gs) {
    }

    bool TTInterpreter::decode(const uint8_t* opcode, uint32_t length) {
        if (length == 0) {
            return true;
        }

        for (uint32_t i = 0; i < length;) {
            uint32_t cur_op_length = 0;
            if (!decodeNext(opcode, length - i, &cur_op_length)) {
                return false;
            }
            i += cur_op_length;
        }

        return true;
    }

    bool TTInterpreter::decodeNext(const uint8_t* opcode, uint32_t length, uint32_t* op_length) {
        if (length == 0) {
            return false;
        }

        uint32_t idx = 0;
        uint8_t cur_op = opcode[0];
        if (cur_op == 0x40) {
            // NPUSHB
            NEXT_BYTE();
            uint8_t n = opcode[idx];
            for (uint8_t i = 0; i < n; ++i) {
                NEXT_BYTE();
                stack_.push(uint32_t(opcode[idx]));
            }
            *op_length = 1 + 1 + n;
        } else if (cur_op == 0x41) {
            // NPUSHW
            NEXT_BYTE();
            uint8_t n = opcode[idx];
            for (uint8_t i = 0; i < n; ++i) {
                NEXT_BYTE();
                uint16_t tmp = uint16_t(opcode[idx]) << 8;
                NEXT_BYTE();
                tmp |= opcode[idx];
                stack_.push(int32_t(tmp));
            }
            *op_length = 1 + 1 + n * 2;
        } else if (cur_op >= 0xB0 && cur_op <= 0xB7) {
            // PUSHB
            uint8_t n = cur_op - 0xB0 + 1;
            for (uint8_t i = 0; i < n; ++i) {
                NEXT_BYTE();
                stack_.push(uint32_t(opcode[idx]));
            }
            *op_length = 1 + n;
        } else if (cur_op >= 0xB8 && cur_op <= 0xBF) {
            // PUSHW
            uint8_t n = cur_op - 0xB8 + 1;
            for (uint8_t i = 0; i < n; ++i) {
                NEXT_BYTE();
                uint16_t tmp = uint16_t(opcode[idx]) << 8;
                NEXT_BYTE();
                tmp |= opcode[idx];
                stack_.push(int32_t(tmp));
            }
            *op_length = 1 + n * 2;
        } else if (cur_op == 0x43) {
            // RS
            auto loc = uint32_t(stack_.top());
            stack_.pop();
            stack_.push(uint32_t(storage_[loc]));
            *op_length = 1;
        } else if (cur_op == 0x42) {
            // WS
            auto val = uint32_t(stack_.top());
            stack_.pop();
            auto loc = uint32_t(stack_.top());
            stack_.pop();
            storage_[loc] = val;
            *op_length = 1;
        } else if (cur_op == 0x44) {
            // WCVTP
            auto val = stack_.top();
            stack_.pop();
            auto loc = uint32_t(stack_.top());
            stack_.pop();
            cvt_.cvt[loc] = val;
            *op_length = 1;
        } else if (cur_op == 0x70) {
            // WCVTF
            auto val = uint32_t(stack_.top());
            stack_.pop();
            auto loc = uint32_t(stack_.top());
            stack_.pop();
            cvt_.cvt[loc] = val;
            *op_length = 1;
        } else if (cur_op == 0x45) {
            // RCVT
            auto loc = uint32_t(stack_.top());
            stack_.pop();
            stack_.push(cvt_.cvt[loc]);
            *op_length = 1;
        } else if (cur_op >= 0x00 && cur_op <= 0x01) {
            // SVTCA
            auto a = cur_op - 0x00;
            uint16_t val = uint16_t(1) << 14;
            if (a == 0x00) {
                gs_.freedom_vector = { 0, val };
                gs_.proj_vector = { 0, val };
            } else {
                gs_.freedom_vector = { val, 0 };
                gs_.proj_vector = { val, 0 };
            }
            *op_length = 1;
        } else if (cur_op >= 0x02 && cur_op <= 0x03) {
            // SPVTCA
            auto a = cur_op - 0x02;
            uint16_t val = uint16_t(1) << 14;
            if (a == 0x00) {
                gs_.proj_vector = { 0, val };
            } else {
                gs_.proj_vector = { val, 0 };
            }
            *op_length = 1;
        } else if (cur_op >= 0x04 && cur_op <= 0x05) {
            // SFVTCA
            auto a = cur_op - 0x04;
            uint16_t val = uint16_t(1) << 14;
            if (a == 0x00) {
                gs_.freedom_vector = { 0, val };
            } else {
                gs_.freedom_vector = { val, 0 };
            }
            *op_length = 1;
        } else if (cur_op >= 0x06 && cur_op <= 0x07) {
            // SPVTL
            auto a = cur_op - 0x06;
            auto p1 = uint32_t(stack_.top());
            stack_.pop();
            auto p2 = uint32_t(stack_.top());
            stack_.pop();

            if (gs_.gep1 == 0) {

            } else {

            }

            if (a == 0x00) {

            } else {

            }
        }

        return true;
    }

}
}