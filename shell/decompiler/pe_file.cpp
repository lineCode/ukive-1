#include "pe_file.h"

#include <fstream>

#include "ukive/log.h"


namespace dpr {

    template <typename E>
    std::enable_if_t<std::is_enum<E>::value, E> operator|(E lhs, E rhs) {
        return E(std::underlying_type<E>::type(lhs) | std::underlying_type<E>::type(rhs));
    }

    template <typename E>
    std::enable_if_t<std::is_enum<E>::value, E> operator&(E lhs, E rhs) {
        return E(std::underlying_type<E>::type(lhs) & std::underlying_type<E>::type(rhs));
    }

    PEFile::PEFile()
        : is_pe32_plus_(false),
          ep_vir_addr_(0),
          ep_offset_(0),
          ep_sec_vir_addr_(0),
          ep_sec_vir_size_(0) { }

    bool PEFile::parse(const string16& file_name) {
        DCHECK(sizeof(CoffFileHeader) == kCoffFileHeaderSize);
        DCHECK(sizeof(OptionalHeader) == kOptionalHeaderSize);
        DCHECK(sizeof(OptionalHeaderPlus) == kOptionalHeaderPlusSize);
        DCHECK(sizeof(OptionalHeaderWin) == kOptionalHeaderWinSize);
        DCHECK(sizeof(OptionalHeaderWinPlus) == kOptionalHeaderWinPlusSize);
        DCHECK(sizeof(ImageDataDirectory) == kImageDataDirectorySize);
        DCHECK(sizeof(SectionHeader) == kSectionHeaderSize);

        std::ifstream pe_file(file_name, std::ios::binary);
        if (pe_file.fail()) {
            DCHECK(false);
            return false;
        }

        auto prev_pos = pe_file.tellg();
        pe_file.seekg(0, std::ios_base::end);
        auto file_size = pe_file.tellg();
        pe_file.seekg(prev_pos);

        std::unique_ptr<uint8_t[]> buf(new uint8_t[file_size]);
        pe_file.read(reinterpret_cast<char*>(buf.get()), file_size);
        if (pe_file.fail()) {
            DCHECK(false);
            return false;
        }

        uint32_t pe_sign_offset = *reinterpret_cast<uint32_t*>(buf.get() + kPESignOffsetPtr);
        if (*reinterpret_cast<uint32_t*>(buf.get() + pe_sign_offset) != kPESign) {
            DCHECK(false);
            return false;
        }

        auto optional_header = reinterpret_cast<OptionalHeader*>(buf.get() + pe_sign_offset + 4 + kCoffFileHeaderSize);
        is_pe32_plus_ = (optional_header->magic == OptionalHeaderMagic::PE32Plus);
        if (is_pe32_plus_) {
            return processPE32Plus(buf.get(), pe_sign_offset + 4);
        }

        return processPE32(buf.get(), pe_sign_offset + 4);
    }

    uint32_t PEFile::getEpVirtualAddr() const {
        return ep_vir_addr_;
    }

    uint32_t PEFile::getEpSecVirtualAddr() const {
        return ep_sec_vir_addr_;
    }

    uint32_t PEFile::getEpSecVirtualSize() const {
        return ep_sec_vir_size_;
    }

    bool PEFile::processPE32(const uint8_t* buf, uint32_t coff_offset) {
        auto coff_header = reinterpret_cast<const CoffFileHeader*>(buf + coff_offset);
        if (coff_header->machine != MachineType::I386) {
            DCHECK(false);
            return false;
        }

        if (coff_header->chrs != (CoffCharacteristics::EXECUTABLE_IMAGE | CoffCharacteristics::_32BIT_MACHINE)) {
            DCHECK(false);
            return false;
        }

        auto optional_header = reinterpret_cast<const OptionalHeader*>(buf + coff_offset + kCoffFileHeaderSize);
        ep_vir_addr_ = optional_header->ep_addr;

        for (uint16_t i = 0; i < coff_header->section_num; ++i) {
            auto section_header = reinterpret_cast<const SectionHeader*>(
                buf + coff_offset + kCoffFileHeaderSize + coff_header->optional_header_size + i * kSectionHeaderSize);

            if ((section_header->chrs & SectionFlags::MEM_EXECUTE) == SectionFlags::MEM_EXECUTE) {
                if (optional_header->ep_addr >= section_header->virtual_addr &&
                    optional_header->ep_addr < section_header->virtual_addr + section_header->virtual_size)
                {
                    ep_offset_ = optional_header->ep_addr - section_header->virtual_addr;
                    ep_sec_vir_addr_ = section_header->virtual_addr;
                    ep_sec_vir_size_ = section_header->virtual_size;
                }
            }
        }

        return true;
    }

    bool PEFile::processPE32Plus(const uint8_t* buf, uint32_t coff_offset) {
        auto coff_header = reinterpret_cast<const dpr::CoffFileHeader*>(buf + coff_offset);
        if (coff_header->machine != dpr::MachineType::I386) {
            DCHECK(false);
            return false;
        }

        if (coff_header->chrs != (CoffCharacteristics::EXECUTABLE_IMAGE | CoffCharacteristics::_32BIT_MACHINE)) {
            DCHECK(false);
            return false;
        }

        return false;
    }

}