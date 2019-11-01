#ifndef UKIVE_NET_TLS_H_
#define UKIVE_NET_TLS_H_

#include <cstdint>
#include <string>
#include <vector>

#include "ukive/net/tls_common.hpp"
#include "ukive/utils/string_utils.h"
#include "ukive/net/tls_record_layer.hpp"


// 根据 RFC 8446 实现的 TLS 1.3 客户端
// https://tools.ietf.org/html/rfc8446

namespace ukive {
namespace net {
namespace tls {

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
        ClientHello = 1,
        ServerHello = 2,
        NewSessionTicket = 4,
        EndOfEarlyData = 5,
        EncryptedExtensions = 8,
        Certificate = 11,
        CertificateRequest = 13,
        CertificateVerify = 15,
        Finished = 20,
        KeyUpdate = 24,
        MessageHash = 254
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

    enum class NameType : uint8_t {
        HostName = 0,
    };

    struct ServerName {
        NameType type;
        stringu8 host_name;
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

    struct Extension {
        ExtensionType type;
        // 长度 0~2^16-1
        // 具体定义和 type 有关
        stringu8 data;
    };

    struct ECDHEParams {
        uint8_t legacy_form = 4;
        // 固定长度
        stringu8 X;
        // 固定长度
        stringu8 Y;

        stringu8 toBytes() const {
            stringu8 r;
            r.push_back(legacy_form);
            r.append(X).append(Y);
            return r;
        }
    };

    struct KeyShareEntry {
        NamedGroup group;
        // 长度 1~2^16-1
        // 具体定义和 group 有关
        stringu8 key_exchange;
    };

    struct KeyShareClientHello {
        // 长度 0~2^16-1
        std::vector<KeyShareEntry> client_shares;
    };


    class TLS {
    public:
        TLS();
        ~TLS();

        void testHandshake();

    private:
        void parseFragment(const TLSRecordLayer::TLSPlaintext& text);
        void parseServerHello(const stringu8& content);

        void constructHandshake(HandshakeType type, stringu8* out);

        void constructClientHello(stringu8* out);

        stringu8 getTimestampFromUInt32();
        stringu8 getRandomBytes(uint32_t size);
        stringu8 getSupportCipherSuiteBytes(const std::vector<CipherSuite>& suites);
        stringu8 getSupportCompressionMethods();
        stringu8 getSupportExtensions();

        string8 host_;
        TLSRecordLayer record_layer_;
    };


}
}
}

#endif  // UKIVE_NET_TLS_H_