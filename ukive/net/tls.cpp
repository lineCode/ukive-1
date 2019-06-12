#include "ukive/net/tls.h"

#include <chrono>
#include <random>

#include "ukive/utils/stl_utils.h"


namespace {

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

}

namespace ukive {
namespace net {
namespace tls {

    void constructFragment(ContentType type, const stringu8& plain_text, stringu8* out) {
        DCHECK(out && plain_text.size() <= std::pow(2U, 14U));

        // type
        out->push_back(enum_cast(type));

        // version
        out->push_back(3); // major
        out->push_back(3); // minor

        // length
        out->append(getUInt16Bytes(UIntToUInt16(plain_text.size())));  // 16bit
        out->append(plain_text);
    }

    void constructHandshake(HandshakeType type, stringu8* out) {
        DCHECK(out);

        out->push_back(enum_cast(type));

        stringu8 message;
        switch (type) {
        case HandshakeType::ClientHello:
            constructClientHello(&message);
            break;
        default:
            break;
        }

        out->append(getUInt24Bytes(UIntToUInt24(message.size()))); // 24bit
        out->append(message);
    }

    void constructClientHello(stringu8* out) {
        DCHECK(out);

        // client_version
        out->push_back(3); // major
        out->push_back(3); // minor

        // random
        out->append(getTimestampFromUInt32()); // gmt_unix_time
        out->append(getRandomBytes(28));       // random_bytes

        // session_id
        out->push_back(0);

        // cipher_suites
        out->append(getSupportCipherSuiteBytes({}));

        // compression_methods
        out->append(getSupportCompressionMethods());

        // extensions
        // nothing
    }

    stringu8 getTimestampFromUInt32() {
        auto ts = std::chrono::steady_clock::now().time_since_epoch();
        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(ts).count();
        DCHECK(timestamp <= std::numeric_limits<uint32_t>::max());
        auto ts_uint32 = static_cast<uint32_t>(timestamp);

        return getUInt32Bytes(ts_uint32);
    }

    stringu8 getRandomBytes(uint32_t size) {
        std::random_device rd;
        std::default_random_engine en(rd());
        std::uniform_int_distribution<int> user_dist(0, 255);

        stringu8 ret;
        for (uint32_t i = 0; i < size; ++i) {
            ret.push_back(user_dist(en));
        }
        return ret;
    }

    stringu8 getSupportCipherSuiteBytes(const std::vector<CipherSuite>& suites) {
        stringu8 ret;
        ret.append(getUInt16Bytes(UIntToUInt16(std::max(suites.size() * 2, size_t(2))))); // 16bit
        for (const auto& suite : suites) {
            switch (suite) {
            case CipherSuite::TLS_NULL_WITH_NULL_NULL:             ret.append({ 0x00, 0x00 }); break;
            case CipherSuite::TLS_RSA_WITH_NULL_MD5:               ret.append({ 0x00, 0x01 }); break;
            case CipherSuite::TLS_RSA_WITH_NULL_SHA:               ret.append({ 0x00, 0x02 }); break;
            case CipherSuite::TLS_RSA_WITH_NULL_SHA256:            ret.append({ 0x00, 0x3B }); break;
            case CipherSuite::TLS_RSA_WITH_RC4_128_MD5:            ret.append({ 0x00, 0x04 }); break;
            case CipherSuite::TLS_RSA_WITH_RC4_128_SHA:            ret.append({ 0x00, 0x05 }); break;
            case CipherSuite::TLS_RSA_WITH_3DES_EDE_CBC_SHA:       ret.append({ 0x00, 0x0A }); break;
            case CipherSuite::TLS_RSA_WITH_AES_128_CBC_SHA:        ret.append({ 0x00, 0x2F }); break;
            case CipherSuite::TLS_RSA_WITH_AES_256_CBC_SHA:        ret.append({ 0x00, 0x35 }); break;
            case CipherSuite::TLS_RSA_WITH_AES_128_CBC_SHA256:     ret.append({ 0x00, 0x3C }); break;
            case CipherSuite::TLS_RSA_WITH_AES_256_CBC_SHA256:     ret.append({ 0x00, 0x3D }); break;
            case CipherSuite::TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA:    ret.append({ 0x00, 0x0D }); break;
            case CipherSuite::TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA:    ret.append({ 0x00, 0x10 }); break;
            case CipherSuite::TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA:   ret.append({ 0x00, 0x13 }); break;
            case CipherSuite::TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA:   ret.append({ 0x00, 0x16 }); break;
            case CipherSuite::TLS_DH_DSS_WITH_AES_128_CBC_SHA:     ret.append({ 0x00, 0x30 }); break;
            case CipherSuite::TLS_DH_RSA_WITH_AES_128_CBC_SHA:     ret.append({ 0x00, 0x31 }); break;
            case CipherSuite::TLS_DHE_DSS_WITH_AES_128_CBC_SHA:    ret.append({ 0x00, 0x32 }); break;
            case CipherSuite::TLS_DHE_RSA_WITH_AES_128_CBC_SHA:    ret.append({ 0x00, 0x33 }); break;
            case CipherSuite::TLS_DH_DSS_WITH_AES_256_CBC_SHA:     ret.append({ 0x00, 0x36 }); break;
            case CipherSuite::TLS_DH_RSA_WITH_AES_256_CBC_SHA:     ret.append({ 0x00, 0x37 }); break;
            case CipherSuite::TLS_DHE_DSS_WITH_AES_256_CBC_SHA:    ret.append({ 0x00, 0x38 }); break;
            case CipherSuite::TLS_DHE_RSA_WITH_AES_256_CBC_SHA:    ret.append({ 0x00, 0x39 }); break;
            case CipherSuite::TLS_DH_DSS_WITH_AES_128_CBC_SHA256:  ret.append({ 0x00, 0x3E }); break;
            case CipherSuite::TLS_DH_RSA_WITH_AES_128_CBC_SHA256:  ret.append({ 0x00, 0x3F }); break;
            case CipherSuite::TLS_DHE_DSS_WITH_AES_128_CBC_SHA256: ret.append({ 0x00, 0x40 }); break;
            case CipherSuite::TLS_DHE_RSA_WITH_AES_128_CBC_SHA256: ret.append({ 0x00, 0x67 }); break;
            case CipherSuite::TLS_DH_DSS_WITH_AES_256_CBC_SHA256:  ret.append({ 0x00, 0x68 }); break;
            case CipherSuite::TLS_DH_RSA_WITH_AES_256_CBC_SHA256:  ret.append({ 0x00, 0x69 }); break;
            case CipherSuite::TLS_DHE_DSS_WITH_AES_256_CBC_SHA256: ret.append({ 0x00, 0x6A }); break;
            case CipherSuite::TLS_DHE_RSA_WITH_AES_256_CBC_SHA256: ret.append({ 0x00, 0x6B }); break;
            case CipherSuite::TLS_DH_anon_WITH_RC4_128_MD5:        ret.append({ 0x00, 0x18 }); break;
            case CipherSuite::TLS_DH_anon_WITH_3DES_EDE_CBC_SHA:   ret.append({ 0x00, 0x1B }); break;
            case CipherSuite::TLS_DH_anon_WITH_AES_128_CBC_SHA:    ret.append({ 0x00, 0x34 }); break;
            case CipherSuite::TLS_DH_anon_WITH_AES_256_CBC_SHA:    ret.append({ 0x00, 0x3A }); break;
            case CipherSuite::TLS_DH_anon_WITH_AES_128_CBC_SHA256: ret.append({ 0x00, 0x6C }); break;
            case CipherSuite::TLS_DH_anon_WITH_AES_256_CBC_SHA256: ret.append({ 0x00, 0x6D }); break;
            }
        }

        if (suites.empty()) {
            ret.append({ 0x00, 0x00 });
        }

        return ret;
    }

    stringu8 getSupportCompressionMethods() {
        stringu8 ret;
        ret.append(getUInt16Bytes(1)); // 8bit
        ret.push_back(0);

        return ret;
    }

    stringu8 getUInt16Bytes(uint16_t val) {
        stringu8 ret;
        ret.push_back((val >> 8) & 0xFFFF);
        ret.push_back(val & 0x00FF);
        return ret;
    }

    stringu8 getUInt24Bytes(uint32_t val) {
        stringu8 ret;
        ret.push_back((val >> 16) & 0x00FFFFFF);
        ret.push_back((val >> 8) & 0x0000FFFF);
        ret.push_back(val & 0x000000FF);
        return ret;
    }

    stringu8 getUInt32Bytes(uint32_t val) {
        stringu8 ret;
        ret.push_back(val >> 24);
        ret.push_back((val >> 16) & 0x00FFFFFF);
        ret.push_back((val >> 8) & 0x0000FFFF);
        ret.push_back(val & 0x000000FF);
        return ret;
    }

}
}
}