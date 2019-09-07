#include "ukive/net/tls.h"

#include <chrono>
#include <random>

#include "ukive/utils/stl_utils.h"
#include "ukive/net/socket.h"


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

    void TLS::makeFragment(ContentType type, const stringu8& plain_text, stringu8* out) {
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

    void TLS::parseFragment(const stringu8& raw) {
        DCHECK(raw.size() >= 5);

        auto type = ContentType(raw[0]);
        auto major = raw[1];
        auto minor = raw[2];
        uint16_t length = (raw[3] << 8) | raw[4];

        switch (type) {
        case ContentType::Alert:
        {
            if (raw.size() - 5 < 2 || length != 2) {
                DCHECK(false);
                return;
            }

            auto level = AlertLevel(raw[5]);
            auto descp = AlertDescription(raw[6]);

            int i = 0;
            break;
        }

        default:
            break;
        }
    }

    void TLS::constructHandshake(HandshakeType type, stringu8* out) {
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

    void TLS::constructClientHello(stringu8* out) {
        DCHECK(out);

        // client_version
        out->push_back(3); // major
        out->push_back(3); // minor

        // random
        //out->append(getTimestampFromUInt32()); // gmt_unix_time
        out->append(getRandomBytes(32));       // random_bytes

        // session_id
        out->push_back(32);
        out->append(getRandomBytes(32));

        // cipher_suites
        // 在 9.1 节中规定了必须支持的加密套件
        out->append(getSupportCipherSuiteBytes({
            CipherSuite::TLS_AES_128_GCM_SHA256,
            CipherSuite::TLS_AES_256_GCM_SHA384,
            CipherSuite::TLS_CHACHA20_POLY1305_SHA256,
            CipherSuite::TLS_AES_128_CCM_SHA256,
            CipherSuite::TLS_AES_128_CCM_8_SHA256,
            CipherSuite::TLS_RSA_WITH_AES_256_CBC_SHA256
            }));

        // compression_methods
        out->append(getSupportCompressionMethods());

        // extensions
        out->append(getSupportExtensions());
    }

    stringu8 TLS::getTimestampFromUInt32() {
        auto ts = std::chrono::steady_clock::now().time_since_epoch();
        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(ts).count();
        DCHECK(timestamp <= std::numeric_limits<uint32_t>::max());
        auto ts_uint32 = static_cast<uint32_t>(timestamp);

        return getUInt32Bytes(ts_uint32);
    }

    stringu8 TLS::getRandomBytes(uint32_t size) {
        std::random_device rd;
        std::default_random_engine en(rd());
        std::uniform_int_distribution<int> user_dist(0, 255);

        stringu8 ret;
        for (uint32_t i = 0; i < size; ++i) {
            ret.push_back(user_dist(en));
        }
        return ret;
    }

    stringu8 TLS::getSupportCipherSuiteBytes(const std::vector<CipherSuite>& suites) {
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

            case CipherSuite::TLS_AES_128_GCM_SHA256:              ret.append({ 0x13, 0x01 }); break;
            case CipherSuite::TLS_AES_256_GCM_SHA384:              ret.append({ 0x13, 0x02 }); break;
            case CipherSuite::TLS_CHACHA20_POLY1305_SHA256:        ret.append({ 0x13, 0x03 }); break;
            case CipherSuite::TLS_AES_128_CCM_SHA256:              ret.append({ 0x13, 0x04 }); break;
            case CipherSuite::TLS_AES_128_CCM_8_SHA256:            ret.append({ 0x13, 0x05 }); break;
            }
        }

        if (suites.empty()) {
            ret.append({ 0x00, 0x00 });
        }

        return ret;
    }

    stringu8 TLS::getSupportCompressionMethods() {
        stringu8 ret;
        // 数组长度
        ret.push_back(1);
        ret.push_back(0);

        return ret;
    }

    stringu8 TLS::getSupportExtensions() {
        // 9.2 节中规定了必须支持的扩展
        stringu8 ret;
        ret.append(getUInt16Bytes(7 + 10 + 8));

        // SupportedVersions
        ret.append(getUInt16Bytes(uint16_t(ExtensionType::SupportedVersions)));
        ret.append(getUInt16Bytes(3));

        ret.push_back(2);
        ret.append({ 3, 4 });

        // SupportedGroups
        ret.append(getUInt16Bytes(uint16_t(ExtensionType::SupportedGroups)));
        ret.append(getUInt16Bytes(6));

        ret.append(getUInt16Bytes(4));
        ret.append(getUInt16Bytes(uint16_t(NamedGroup::X25519)));
        ret.append(getUInt16Bytes(uint16_t(NamedGroup::SECP256R1)));

        // SignatureAlgorithms
        ret.append(getUInt16Bytes(uint16_t(ExtensionType::SignatureAlgorithms)));
        ret.append(getUInt16Bytes(4));

        ret.append(getUInt16Bytes(2));
        ret.append(getUInt16Bytes(uint16_t(SignatureScheme::RSA_PKCS1_SHA256)));

        return ret;
    }

    stringu8 TLS::getUInt16Bytes(uint16_t val) {
        stringu8 ret;
        ret.push_back(val >> 8);
        ret.push_back(val & 0xFF);
        return ret;
    }

    stringu8 TLS::getUInt24Bytes(uint32_t val) {
        stringu8 ret;
        ret.push_back((val >> 16) & 0xFF);
        ret.push_back((val >> 8) & 0xFF);
        ret.push_back(val & 0xFF);
        return ret;
    }

    stringu8 TLS::getUInt32Bytes(uint32_t val) {
        stringu8 ret;
        ret.push_back(val >> 24);
        ret.push_back((val >> 16) & 0xFF);
        ret.push_back((val >> 8) & 0xFF);
        ret.push_back(val & 0xFF);
        return ret;
    }

    void TLS::testHandshake() {
        SocketClient client;
        if (!client.connectByHost("", 443)) {
            DCHECK(false);
            return;
        }

        stringu8 client_hello;
        constructHandshake(HandshakeType::ClientHello, &client_hello);

        stringu8 fragment;
        makeFragment(ContentType::Handshake, client_hello, &fragment);

        if (!client.send(string8(fragment.begin(), fragment.end()))) {
            DCHECK(false);
            return;
        }

        string8 resp;
        if (!client.recv(&resp)) {
            DCHECK(false);
            return;
        }

        parseFragment(stringu8(resp.begin(), resp.end()));

        client.close();
    }

}
}
}