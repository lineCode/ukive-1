#ifndef CYRONENO_TEXT_OPENTYPE_TT_INSTRUCTION_GRAPHICS_STATE_H_
#define CYRONENO_TEXT_OPENTYPE_TT_INSTRUCTION_GRAPHICS_STATE_H_

#include <cstdint>


namespace cyro {
namespace otf {

    struct Vec {
        uint16_t x;
        uint16_t y;
    };

    struct GraphicsState {
        bool auto_flip;
        uint32_t ctrl_val_cut_in;
        uint16_t delta_base;
        uint16_t delta_shift;
        Vec dual_proj_vectors;
        Vec freedom_vector;
        uint16_t gep0;
        uint16_t gep1;
        uint16_t gep2;
        uint16_t inst_ctrl;
        uint16_t loop;
        uint16_t min_distance;
        Vec proj_vector;
        uint16_t round_state;
        uint16_t rp0;
        uint16_t rp1;
        uint16_t rp2;
        bool scan_ctrl;
        uint16_t single_width_cut_in;
        uint16_t single_width_val;

        void reset() {
            auto_flip = true;
            ctrl_val_cut_in = (17 << 24) | (16);
            delta_base = 9;
            delta_shift = 3;
            freedom_vector = { 1, 0 };
            gep0 = 1;
            gep1 = 1;
            gep2 = 1;
            inst_ctrl = 0;
            loop = 1;
            min_distance = 1;
            proj_vector = { 1, 0 };
            round_state = 1;
            rp0 = 0;
            rp1 = 0;
            rp2 = 0;
            scan_ctrl = false;
            single_width_cut_in = 0;
            single_width_val = 0;
        }
    };

}
}

#endif  // CYRONENO_TEXT_OPENTYPE_TT_INSTRUCTION_GRAPHICS_STATE_H_