#ifndef DISASSEMBLER_INTEL_INSTRUCTION_PARAMS_H_
#define DISASSEMBLER_INTEL_INSTRUCTION_PARAMS_H_

#include "ukive/utils/string_utils.h"


namespace dpr {

    enum class CPUMode {
        _16Bit,
        _32Bit,
        _64Bit,
    };

    struct Prefix {
        uint8_t g1;
        uint8_t g2;
        uint8_t g3;
        uint8_t g4;
        uint8_t mand;
        uint8_t rex;
        uint8_t vex[3];
        uint32_t vex_length;

        Prefix()
            : g1(0), g2(0), g3(0), g4(0), mand(0), rex(0),
              vex(), vex_length(0) {
        }

        uint8_t rexW() const {
            return (rex & 0x8);
        }

        uint8_t rexR() const {
            return (rex & 0x4);
        }

        uint8_t rexX() const {
            return (rex & 0x2);
        }

        uint8_t rexB() const {
            return (rex & 0x1);
        }

        // Only used in 3-bit
        uint8_t vexW() const {
            return (vex[2] & 0x80);
        }

        uint8_t vexR() const {
            return (vex[1] & 0x80);
        }

        // Only used in 3-bit
        uint8_t vexX() const {
            return (vex[1] & 0x40);
        }

        // Only used in 3-bit
        uint8_t vexB() const {
            return (vex[1] & 0x20);
        }

        uint8_t vexL() const {
            if (vex_length == 2) {
                return (vex[1] & 0x4);
            }
            if (vex_length == 3) {
                return (vex[2] & 0x4);
            }
            return 0;
        }

        // Only used in 3-bit
        uint8_t vexM() const {
            return (vex[1] & 0x1F);
        }

        uint8_t vexV() const {
            if (vex_length == 2) {
                return (vex[1] & 0x78);
            }
            if (vex_length == 3) {
                return (vex[2] & 0x78);
            }
            return 0;
        }

        uint8_t vexP() const {
            if (vex_length == 2) {
                return (vex[1] & 0x3);
            }
            if (vex_length == 3) {
                return (vex[2] & 0x73);
            }
            return 0;
        }

        bool hasRex() const {
            return rex != 0;
        }

        bool hasVex() const {
            return vex_length > 0;
        }

        uint32_t length() const {
            uint32_t l = 0;
            if (g1 != 0) { ++l; }
            if (g2 != 0) { ++l; }
            if (g3 != 0) { ++l; }
            if (g4 != 0) { ++l; }
            if (mand != 0) { ++l; }
            if (rex != 0) { ++l; }
            l += vex_length;
            return l;
        }

        void reset() {
            g1 = 0;
            g2 = 0;
            g3 = 0;
            g4 = 0;
            mand = 0;
            rex = 0;
            vex_length = 0;
        }
    };

    struct ModRMMemMode {
        string8 idx1_reg;
        string8 idx2_reg;
        uint32_t disp_length = 0;
        uint32_t disp = 0;
        string8 seg_reg;
        bool has_sib = false;
    };

    struct ModRMRegMode {
        string8 b_reg;
        string8 w_reg;
        string8 dw_reg;
        string8 mm_reg;
        string8 xmm_reg;
    };

    struct SIBBaseMode {
        string8 reg;
        uint32_t disp_length = 0;
        uint32_t disp = 0;
        string8 seg_reg;
    };

    struct SIBScaleMode {
        string8 reg;
        uint32_t scale = 0;
    };

    struct ModRMField {
        bool is_reg = false;
        ModRMMemMode mrm_mem;
        ModRMRegMode mrm_reg;
        string8 selected_reg;
    };

    struct SIBField {
        SIBBaseMode sib_base;
        SIBScaleMode sib_scale;
    };

    struct Operand {
        string8 reg;

        bool use_modrm;
        ModRMField modrm_field;
        SIBField sib_field;

        uint64_t disp;
        uint64_t imme;
        uint32_t disp_length;
        uint32_t imme_length;
        // 内存寻址时，访问的内存大小
        uint32_t operand_size;

        bool is_digit;
        uint32_t digit;

        // 立即数指针，16:16 16:32 16:64 这样子的
        bool is_pointer;
        bool is_mem_pointer;
        uint32_t pointer_length;
        uint16_t pointer_seg;
        uint64_t pointer_addr;

        Operand()
            : use_modrm(false),
              disp(0),
              imme(0),
              disp_length(0),
              imme_length(0),
              operand_size(0),
              is_digit(false),
              digit(0),
              is_pointer(false),
              is_mem_pointer(false),
              pointer_length(0),
              pointer_seg(0),
              pointer_addr(0) {
        }

        static Operand ofReg(const string8& reg) {
            Operand op;
            op.reg = reg;
            return op;
        }
        static Operand ofModRM(const ModRMField& mf, const SIBField& sf) {
            Operand op;
            op.use_modrm = true;
            op.modrm_field = mf;
            op.sib_field = sf;
            return op;
        }
        static Operand ofDisp(uint64_t disp, uint32_t len) {
            Operand op;
            op.disp = disp;
            op.disp_length = len;
            return op;
        }
        static Operand ofImme(uint64_t imme, uint32_t len) {
            Operand op;
            op.imme = imme;
            op.imme_length = len;
            return op;
        }
        static Operand ofDigit(uint32_t digit) {
            Operand op;
            op.is_digit = true;
            op.digit = digit;
            return op;
        }
        static Operand ofPointer(uint16_t seg, uint64_t addr, uint32_t len, bool mem) {
            Operand op;
            op.is_pointer = true;
            op.is_mem_pointer = mem;
            op.pointer_length = len;
            op.pointer_seg = seg;
            op.pointer_addr = addr;
            return op;
        }
    };

    struct SelConfig {
        bool use_explicit = false;
        bool use_fwait = false;
        bool use_nop = true;
        bool use_sal = false;
        uint8_t e_z = 0;
        uint8_t b_nae_c = 0;
        uint8_t nb_ae_nc = 0;
        uint8_t nz_ne = 0;
        uint8_t be_na = 0;
        uint8_t nbe_a = 0;
        uint8_t p_pe = 0;
        uint8_t np_po = 0;
        uint8_t l_nge = 0;
        uint8_t nl_ge = 0;
        uint8_t le_ng = 0;
        uint8_t nle_g = 0;
        uint8_t shl_sal = 0;
        uint8_t repne_nz = 0;
        uint8_t rep_e_z = 0;
    };

    uint32_t selectOperandSize(bool d, const Prefix& p);
    uint32_t selectOperandSize64(const string8& ss, const Prefix& p);
    uint32_t selectAddressSize(bool d, const Prefix& p);
    uint32_t selectAddressSize64(const Prefix& p);

}

#endif  // DISASSEMBLER_INTEL_INSTRUCTION_PARAMS_H_