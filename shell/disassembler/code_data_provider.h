#ifndef DISASSEMBLER_CODE_DATA_PROVIDER_H_
#define DISASSEMBLER_CODE_DATA_PROVIDER_H_

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "shell/disassembler/intel_instruction_params.h"


namespace dpr {

    class CodeDataProvider {
    public:
        virtual ~CodeDataProvider() = default;

        virtual uint8_t get8(uint32_t off) const = 0;
        virtual uint16_t get16(uint32_t off) const = 0;
        virtual uint32_t get32(uint32_t off) const = 0;
        virtual uint64_t get64(uint64_t off) const = 0;
    };


    class StaticCodeDataProvider : public CodeDataProvider {
    public:
        explicit StaticCodeDataProvider(const uint8_t* buf);

        uint8_t get8(uint32_t off) const override;
        uint16_t get16(uint32_t off) const override;
        uint32_t get32(uint32_t off) const override;
        uint64_t get64(uint64_t off) const override;

    private:
        const uint8_t* buf_;
    };


    class DynamicCodeDataProvider : public CodeDataProvider {
    public:
        DynamicCodeDataProvider(uint32_t base, HANDLE process);

        uint8_t get8(uint32_t off) const override;
        uint16_t get16(uint32_t off) const override;
        uint32_t get32(uint32_t off) const override;
        uint64_t get64(uint64_t off) const override;

    private:
        uint32_t base_;
        HANDLE process_;
    };

}

#endif  // DISASSEMBLER_CODE_DATA_PROVIDER_H_