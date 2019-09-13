#ifndef UKIVE_NET_TLS_COMMON_HPP_
#define UKIVE_NET_TLS_COMMON_HPP_

#include <cstdint>

#include "ukive/log.h"
#include "ukive/utils/string_utils.h"


namespace ukive {
namespace net {
namespace tls {

    enum class ContentType : uint8_t {
        ChangeCipherSpec = 20,
        Alert = 21,
        Handshake = 22,
        ApplicationData = 23,
    };

    struct ProtocolVersion {
        uint8_t major;
        uint8_t minor;
    };

    template <typename T>
    uint16_t UIntToUInt16(T val) {
        static_assert(std::is_unsigned<T>::value, "T must be unsigned");
        DCHECK(static_cast<T>(std::numeric_limits<uint16_t>::max()) >= val);
        return static_cast<uint16_t>(val);
    }

    template <typename T>
    uint32_t UIntToUInt24(T val) {
        static_assert(std::is_unsigned<T>::value, "T must be unsigned");
        DCHECK(static_cast<T>(std::pow(2U, 24U) - 1U) >= val);
        return static_cast<uint32_t>(val);
    }

    template <typename E>
    std::underlying_type_t<E> enum_cast(E e) {
        return static_cast<std::underlying_type_t<E>>(e);
    }

    stringu8 getUInt16Bytes(uint16_t val);
    stringu8 getUInt24Bytes(uint32_t val);
    stringu8 getUInt32Bytes(uint32_t val);

}
}
}

#endif  // UKIVE_NET_TLS_COMMON_HPP_