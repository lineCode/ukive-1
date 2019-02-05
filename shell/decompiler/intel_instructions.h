#ifndef DECOMPILER_INTEL_INSTRUCTIONS_H_
#define DECOMPILER_INTEL_INSTRUCTIONS_H_

#include <cstdint>

#include "ukive/utils/string_utils.h"

#include "intel_instruction_params.h"
#include "code_data_provider.h"


namespace dpr {

    struct ExtraInfo {
        Prefix prefix;

        string8 opcode;
        uint8_t opcode_bytes[3];
        uint32_t opcode_length;
        std::vector<Operand> operands;

        bool has_modrm;
        uint8_t modrm;
        bool has_sib;
        uint8_t sib;

        ExtraInfo()
            : opcode_bytes{},
              opcode_length(0),
              has_modrm(false),
              modrm(0),
              has_sib(false),
              sib(0) {}

        void reset() {
            prefix.reset();
            opcode.clear();
            opcode_length = 0;
            operands.clear();
            has_modrm = false;
            modrm = 0;
            has_sib = false;
            sib = 0;
        }

        uint32_t length() const {
            uint32_t off = prefix.length();
            off += opcode_length;
            if (has_modrm) { ++off; }
            if (has_sib) { ++off; }

            for (const auto& op : operands) {
                off += op.disp_length + op.imme_length + op.pointer_length
                    + ((op.use_modrm && !op.modrm_field.is_reg) ? op.modrm_field.mrm_mem.disp_length : 0)
                    + ((op.use_modrm && op.modrm_field.mrm_mem.has_sib) ? op.sib_field.sib_base.disp_length : 0);
            }

            return off;
        }

        string8 toString() const {
            string8 result = opcode;
            if (prefix.g1) {
                if (prefix.g1 == 0xF0) result.append("LOCK ");
                if (prefix.g1 == 0xF2) result.append("REPNE ");
                if (prefix.g1 == 0xF3) result.append("REP ");
            }

            if (operands.empty()) {
                return result;
            }

            bool is_first = true;
            string8 cur_operand;
            for (const auto& op : operands) {
                if (op.is_digit) {
                    cur_operand = ukive::toString8Hex(op.digit);
                } else if (op.use_modrm) {
                    if (op.modrm_field.is_reg) {
                        cur_operand = op.modrm_field.selected_reg;
                    } else {
                        if (op.modrm_field.mrm_mem.has_sib) {
                            bool has_prefix = false;
                            auto& sib_base = op.sib_field.sib_base;
                            auto& sib_scale = op.sib_field.sib_scale;

                            if (op.operand_size != 0) {
                                cur_operand.append(getMemPtrInfo(op.operand_size));
                                cur_operand.append(" ");
                            }

                            if (!sib_base.seg_reg.empty()) {
                                cur_operand.append(getSegmentReg(sib_base.seg_reg));
                                cur_operand.append(":");
                            }

                            cur_operand.append("[");
                            if (!sib_base.reg.empty()) {
                                cur_operand.append(sib_base.reg);
                                has_prefix = true;
                            }
                            if (!sib_scale.reg.empty()) {
                                if (has_prefix) {
                                    cur_operand.append("+");
                                }
                                cur_operand.append(sib_scale.reg)
                                    .append("*").append(ukive::toString8Hex(sib_scale.scale));
                                has_prefix = true;
                            }
                            if (sib_base.disp_length > 0) {
                                auto signed_disp = getSignedDisp(sib_base.disp, sib_base.disp_length, has_prefix);
                                cur_operand.append(signed_disp);
                            }
                            cur_operand.append("]");
                        } else {
                            bool has_prefix = false;
                            auto& mrm_mem = op.modrm_field.mrm_mem;

                            if (op.operand_size != 0) {
                                cur_operand.append(getMemPtrInfo(op.operand_size));
                                cur_operand.append(" ");
                            }

                            if (!mrm_mem.seg_reg.empty()) {
                                cur_operand.append(getSegmentReg(mrm_mem.seg_reg));
                                cur_operand.append(":");
                            }

                            cur_operand.append("[");
                            if (!mrm_mem.idx1_reg.empty()) {
                                cur_operand.append(mrm_mem.idx1_reg);
                                has_prefix = true;
                            }
                            if (!mrm_mem.idx2_reg.empty()) {
                                if (has_prefix) {
                                    cur_operand.append("+");
                                }
                                cur_operand.append(mrm_mem.idx2_reg);
                                has_prefix = true;
                            }
                            if (mrm_mem.disp_length > 0) {
                                auto signed_disp = getSignedDisp(mrm_mem.disp, mrm_mem.disp_length, has_prefix);
                                cur_operand.append(signed_disp);
                            }
                            cur_operand.append("]");
                        }
                    }
                } else if (!op.reg.empty()) {
                    cur_operand = op.reg;
                } else if (op.imme_length > 0) {
                    cur_operand = getSignedDisp(op.imme, op.imme_length, false);
                } else if (op.disp_length > 0) {
                    if (op.operand_size != 0) {
                        cur_operand.append(getMemPtrInfo(op.operand_size));
                        cur_operand.append(" ");
                    }
                    cur_operand.append(getSegmentReg("DS"));
                    cur_operand.append(":");
                    cur_operand.append("[");
                    cur_operand.append(ukive::toString8Hex(op.disp));
                    cur_operand.append("]");
                } else if (op.is_pointer) {
                    if (op.is_mem_pointer) {
                        cur_operand.append(ukive::toString8Hex(op.pointer_seg));
                        cur_operand.append(":");
                        cur_operand.append("[");
                        cur_operand.append(ukive::toString8Hex(op.pointer_addr));
                        cur_operand.append("]");
                    } else {
                        cur_operand.append(ukive::toString8Hex(op.pointer_seg));
                        cur_operand.append(":");
                        cur_operand.append(ukive::toString8Hex(op.pointer_addr));
                    }
                }

                if (cur_operand.empty()) {
                    cur_operand = "ERR";
                }
                if (is_first) {
                    is_first = false;
                    result += " ";
                } else {
                    result += ", ";
                }
                result += cur_operand;
                cur_operand.clear();
            }

            return result;
        }

    private:
        string8 getSignedDisp(uint64_t disp, uint32_t length, bool has_prefix) const {
            string8 result;
            if (length == 1) {
                if (!(disp & 0x80)) {
                    if (has_prefix) result.append("+");
                    result.append(ukive::toString8Hex(disp));
                } else {
                    result.append("-");
                    result.append(ukive::toString8Hex(0xFFU - disp + 1U));
                }
                return result;
            }
            if (length == 2) {
                if (!(disp & 0x8000)) {
                    if (has_prefix) result.append("+");
                    result.append(ukive::toString8Hex(disp));
                } else {
                    result.append("-");
                    result.append(ukive::toString8Hex(0xFFFFU - disp + 1U));
                }
                return result;
            }
            if (length == 4) {
                if (!(disp & 0x80000000)) {
                    if (has_prefix) result.append("+");
                    result.append(ukive::toString8Hex(disp));
                } else {
                    result.append("-");
                    result.append(ukive::toString8Hex(0xFFFFFFFFU - disp + 1U));
                }
                return result;
            }
            if (length == 8) {
                if (!(disp & 0x8000000000000000)) {
                    if (has_prefix) result.append("+");
                    result.append(ukive::toString8Hex(disp));
                } else {
                    result.append("-");
                    result.append(ukive::toString8Hex(0xFFFFFFFFFFFFFFFFU - disp + 1U));
                }
                return result;
            }
            return "ERR";
        }

        string8 getMemPtrInfo(uint32_t operand_size) const {
            if (operand_size == 0) {
                return "";
            }
            if (operand_size == 1) {
                return "byte ptr";
            }
            if (operand_size == 2) {
                return "word ptr";
            }
            if (operand_size == 4) {
                return "dword ptr";
            }
            if (operand_size == 8) {
                return "qword ptr";
            }
            return "ERR";
        }

        string8 getSegmentReg(const string8& def_reg) const {
            // TODO: Jcc
            if (prefix.g2) {
                if (prefix.g2 == 0x2E) return "CS";
                if (prefix.g2 == 0x36) return "SS";
                if (prefix.g2 == 0x3E) return "DS";
                if (prefix.g2 == 0x26) return "ES";
                if (prefix.g2 == 0x64) return "FS";
                if (prefix.g2 == 0x65) return "GS";
            }
            return def_reg;
        }
    };

    struct CodeSegInfo {
        uint32_t cur;
        uint32_t size;
        CPUMode cpu_mode;
        CodeDataProvider* provider;

        CodeSegInfo operator+(uint32_t off) const {
            CodeSegInfo tmp = *this;
            tmp.cur += off;
            return tmp;
        }

        CodeSegInfo& operator+=(uint32_t off) {
            cur += off;
            return *this;
        }

        uint8_t get8(uint32_t off) const {
            return provider->get8(off);
        }

        uint16_t get16(uint32_t off) const {
            return provider->get16(off);
        }

        uint32_t get32(uint32_t off) const {
            return provider->get32(off);
        }

        uint64_t get64(uint32_t off) const {
            return provider->get64(off);
        }

        uint8_t getCur8() const {
            return provider->get8(cur);
        }
    };


    bool parseInstruction(CodeSegInfo csi, ExtraInfo* info);

    bool walkOnOneByteOpcodeMap(CodeSegInfo csi, ExtraInfo* info);
    bool walkOnTwoByteOpcodeMap(CodeSegInfo csi, ExtraInfo* info);
    bool walkOnThreeByteOpcodeMap(CodeSegInfo csi, ExtraInfo* info);
    bool walkOnCoprocessOpcodeMap(CodeSegInfo csi, ExtraInfo* info);

    bool parseOneByte(CodeSegInfo csi, Prefix prefix, uint8_t byte1, ExtraInfo* info);
    bool parseTwoByte(CodeSegInfo csi, Prefix prefix, uint8_t byte2, ExtraInfo* info);
    bool parseThreeByte(CodeSegInfo csi, Prefix prefix, uint8_t byte2, uint8_t byte3, ExtraInfo* info);

    bool parseAbbrs(CodeSegInfo csi, Prefix prefix, const string8& abbrs, const string8& ss, ExtraInfo* info);
    bool parseAbbr(CodeSegInfo csi, Prefix prefix, const string8& abbr, const string8& ss, ExtraInfo* info);
    bool parseModRMField(CodeSegInfo csi, Prefix prefix, uint8_t modrm, bool use_reg, ModRMField* field);
    bool parseSIBField(CodeSegInfo csi, Prefix prefix, uint8_t modrm, uint8_t sib, SIBField* field);

    uint32_t findImmeOffset(ExtraInfo* info);

    struct ParsedItem {
        string8 name;
        string8 ss;
        string8 decorate;
        string8 operands;
    };

    bool parseTableItem(const string8& item, ParsedItem* out);
}

#endif  // DECOMPILER_INTEL_INSTRUCTIONS_H_