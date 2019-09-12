#ifndef CYRONENO_TEXT_OPENTYPE_TT_INSTRUCTION_TTI_INTERPRETER_H_
#define CYRONENO_TEXT_OPENTYPE_TT_INSTRUCTION_TTI_INTERPRETER_H_

#include <cstdint>
#include <stack>
#include <vector>

#include "../tables/cvt.h"
#include "graphics_state.h"


namespace cyro {
namespace otf {

    class TTInterpreter {
    public:
        TTInterpreter(CVT::CVTTable& cvt, GraphicsState& gs);

        bool decode(const uint8_t* opcode, uint32_t length);

    private:
        bool decodeNext(const uint8_t* opcode, uint32_t length, uint32_t* op_length);

        std::stack<int32_t> stack_;
        std::vector<uint32_t> storage_;

        CVT::CVTTable& cvt_;
        GraphicsState& gs_;
    };

}
}

#endif  // CYRONENO_TEXT_OPENTYPE_TT_INSTRUCTION_TTI_INTERPRETER_H_