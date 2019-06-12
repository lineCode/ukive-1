#ifndef UKIVE_NET_TLS_H_
#define UKIVE_NET_TLS_H_

#include <cstdint>
#include <string>
#include <vector>

#include "ukive/utils/string_utils.h"


// 根据 RFC 8446 实现的 TLS 1.3 客户端
// https://tools.ietf.org/html/rfc8446

namespace ukive {
namespace net {
namespace tls {

    enum class ContentType : uint8_t {
        ChangeCipherSpec = 20,
        Alert = 21,
        Handshake = 22,
        ApplicationData = 23,
    };

    enum class AlertDescription : uint8_t {
        CloseNotify = 0,
        UnexpectedMessage = 10,
        BadRecordMac = 20,
        DecryptionFailedRESERVED = 21,
        RecordOverflow = 22,
        DecompressionFailure = 30,
        HandshakeFailure = 40,
        NoCertificateRESERVED = 41,
        BadCertificate = 42,
        UnsupportedCertificate = 43,
        CertificateRevoked = 44,
        CertificateExpired = 45,
        CertificateUnknown = 46,
        IllegalParameter = 47,
        UnknownCA = 48,
        AccessDenied = 49,
        DecodeError = 50,
        DecryptError = 51,
        ExportRestrictionRESERVED = 60,
        ProtocolVersion = 70,
        InsufficientSecurity = 71,
        InternalError = 80,
        UserCanceled = 90,
        NoRenegotiation = 100,
        UnsupportedExtension = 110,
    };

    enum class HandshakeType : uint8_t {
        HelloRequest = 0,
        ClientHello = 1,
        ServerHello = 2,
        Certificate = 11,
        ServerKeyExchange = 12,
        CertificateRequest = 13,
        ServerHelloDone = 14,
        CertificateVerify = 15,
        ClientKeyExchange = 16,
        Finished = 20,
    };

    enum class CipherSuite {
        TLS_NULL_WITH_NULL_NULL,

        TLS_RSA_WITH_NULL_MD5,
        TLS_RSA_WITH_NULL_SHA,
        TLS_RSA_WITH_NULL_SHA256,
        TLS_RSA_WITH_RC4_128_MD5,
        TLS_RSA_WITH_RC4_128_SHA,
        TLS_RSA_WITH_3DES_EDE_CBC_SHA,
        TLS_RSA_WITH_AES_128_CBC_SHA,
        TLS_RSA_WITH_AES_256_CBC_SHA,
        TLS_RSA_WITH_AES_128_CBC_SHA256,
        TLS_RSA_WITH_AES_256_CBC_SHA256,
        TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA,
        TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA,
        TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA,
        TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA,
        TLS_DH_DSS_WITH_AES_128_CBC_SHA,
        TLS_DH_RSA_WITH_AES_128_CBC_SHA,
        TLS_DHE_DSS_WITH_AES_128_CBC_SHA,
        TLS_DHE_RSA_WITH_AES_128_CBC_SHA,
        TLS_DH_DSS_WITH_AES_256_CBC_SHA,
        TLS_DH_RSA_WITH_AES_256_CBC_SHA,
        TLS_DHE_DSS_WITH_AES_256_CBC_SHA,
        TLS_DHE_RSA_WITH_AES_256_CBC_SHA,
        TLS_DH_DSS_WITH_AES_128_CBC_SHA256,
        TLS_DH_RSA_WITH_AES_128_CBC_SHA256,
        TLS_DHE_DSS_WITH_AES_128_CBC_SHA256,
        TLS_DHE_RSA_WITH_AES_128_CBC_SHA256,
        TLS_DH_DSS_WITH_AES_256_CBC_SHA256,
        TLS_DH_RSA_WITH_AES_256_CBC_SHA256,
        TLS_DHE_DSS_WITH_AES_256_CBC_SHA256,
        TLS_DHE_RSA_WITH_AES_256_CBC_SHA256,

        TLS_DH_anon_WITH_RC4_128_MD5,
        TLS_DH_anon_WITH_3DES_EDE_CBC_SHA,
        TLS_DH_anon_WITH_AES_128_CBC_SHA,
        TLS_DH_anon_WITH_AES_256_CBC_SHA,
        TLS_DH_anon_WITH_AES_128_CBC_SHA256,
        TLS_DH_anon_WITH_AES_256_CBC_SHA256,
    };

    struct ProtocolVersion {
        uint8_t major;
        uint8_t minor;
    };

    struct TLSPlaintext {
        ContentType type;
        ProtocolVersion version;
        uint16_t length;
        stringu8 fragment;
    };

    struct TLSCompressed {
        ContentType type;
        ProtocolVersion version;
        uint16_t length;
        stringu8 fragment;
    };

    void constructFragment(ContentType type, const stringu8& plain_text, stringu8* out);

    void constructHandshake(HandshakeType type, stringu8* out);

    void constructClientHello(stringu8* out);

    stringu8 getTimestampFromUInt32();
    stringu8 getRandomBytes(uint32_t size);
    stringu8 getSupportCipherSuiteBytes(const std::vector<CipherSuite>& suites);
    stringu8 getSupportCompressionMethods();

    stringu8 getUInt16Bytes(uint16_t val);
    stringu8 getUInt24Bytes(uint32_t val);
    stringu8 getUInt32Bytes(uint32_t val);

}
}
}

#endif  // UKIVE_NET_TLS_H_