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

    enum class AlertLevel : uint8_t {
        Warning = 1,
        Fatal = 2,
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
        InappropriateFallback = 86,
        UserCanceled = 90,
        NoRenegotiation = 100,
        MissingExtension = 109,
        UnsupportedExtension = 110,
        UnrecognizedName = 112,
        BadCertificateStatusResponse = 113,
        UnknownPSKIdentity = 115,
        CertificateRequired = 116,
        NoApplicationProtocol = 120,
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

        TLS_AES_128_GCM_SHA256,
        TLS_AES_256_GCM_SHA384,
        TLS_CHACHA20_POLY1305_SHA256,
        TLS_AES_128_CCM_SHA256,
        TLS_AES_128_CCM_8_SHA256,
    };

    enum class NamedGroup : uint16_t {
        // ECDHE
        SECP256R1 = 0x0017,
        SECP384R1 = 0x0018,
        SECP512R1 = 0x0019,
        X25519 = 0x001D,
        X448 = 0x001E,

        // DHE
        FFDHE2048 = 0x0100,
        FFDHE3072 = 0x0100,
        FFDHE4096 = 0x0100,
        FFDHE6144 = 0x0100,
        FFDHE8192 = 0x0100,

        // Reserved
        FFDHE_PRIVATE_USE_START = 0x01FC,
        FFDHE_PRIVATE_USE_END = 0x01FF,
        ECDHE_PRIVATE_USE_START = 0xFE00,
        ECDHE_PRIVATE_USE_END = 0xFEFF,
    };

    // 4.2.3 Signature Algorithms
    enum class SignatureScheme : uint16_t {
        // RSASSA-PKCS1-v1.5
        RSA_PKCS1_SHA256 = 0x0401,
        RSA_PKCS1_SHA384 = 0x0501,
        RSA_PKCS1_SHA512 = 0x0601,

        // ECDSA
        ECDSA_SECP256R1_SHA256 = 0x0403,
        ECDSA_SECP384R1_SHA384 = 0x0503,
        ECDSA_SECP512R1_SHA512 = 0x0603,

        // RSASSA-PSS with OID rsaEncryption
        RSA_PSS_RSAE_SHA256 = 0x0804,
        RSA_PSS_RSAE_SHA384 = 0x0805,
        RSA_PSS_RSAE_SHA512 = 0x0806,

        // EDDSA
        ED25519 = 0x0807,
        ED448 = 0x0808,

        // RSASSA-PSS with OID RSASSA-PSS
        RSA_PSS_PSS_SHA256 = 0x0809,
        RSA_PSS_PSS_SHA384 = 0x080a,
        RSA_PSS_PSS_SHA512 = 0x080b,

        // Legacy
        RSA_PKCS1_SHA1 = 0x0201,
        ECDSA_SHA1 = 0x0203,

        // Reserved
        PRIVATE_USE_START = 0xFE00,
        PRIVATE_USE_END = 0xFFFF,
    };

    enum class ExtensionType : uint16_t {
        ServerName = 0,
        MaxFragmentLength = 1,
        StatusRequest = 5,
        SupportedGroups = 10,
        SignatureAlgorithms = 13,
        UseSrtp = 14,
        Heartbeat = 15,
        ApplicationLayerProtocolNegotiation = 16,
        SignedCertificateTimestamp = 18,
        ClientCertificateType = 19,
        ServerCertificateType = 20,
        Padding = 21,
        PreSharedKey = 41,
        EarlyData = 42,
        SupportedVersions = 43,
        Cookie = 44,
        PSKKeyExchangeModes = 45,
        CertificateAuthorities = 47,
        OidFilters = 48,
        PostHandshakeAuth = 49,
        SignatureAlgorithmsCert = 50,
        KeyShare = 51,
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


    class TLS {
    public:
        TLS() = default;

        void testHandshake();

    private:
        void makeFragment(ContentType type, const stringu8& plain_text, stringu8* out);
        void parseFragment(const stringu8& raw);

        void constructHandshake(HandshakeType type, stringu8* out);

        void constructClientHello(stringu8* out);

        stringu8 getTimestampFromUInt32();
        stringu8 getRandomBytes(uint32_t size);
        stringu8 getSupportCipherSuiteBytes(const std::vector<CipherSuite>& suites);
        stringu8 getSupportCompressionMethods();
        stringu8 getSupportExtensions();

        stringu8 getUInt16Bytes(uint16_t val);
        stringu8 getUInt24Bytes(uint32_t val);
        stringu8 getUInt32Bytes(uint32_t val);
    };


}
}
}

#endif  // UKIVE_NET_TLS_H_