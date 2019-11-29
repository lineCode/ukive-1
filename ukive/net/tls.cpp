#include "ukive/net/tls.h"

#include <chrono>
#include <random>

#include "utils/stl_utils.h"
#include "utils/big_integer/big_integer.h"

#include "ukive/net/socket.h"
#include "ukive/security/crypto/ecdp.h"


namespace ukive {
namespace net {
namespace tls {

    TLS::TLS() {}

    TLS::~TLS() {}

    void TLS::parseFragment(const TLSRecordLayer::TLSPlaintext& text) {
        switch (text.type) {
        case ContentType::Alert:
        {
            if (text.fragment.length() < 2) {
                DCHECK(false);
                return;
            }

            auto level = AlertLevel(text.fragment[0]);
            auto descp = AlertDescription(text.fragment[1]);
            break;
        }

        case ContentType::Handshake:
        {
            if (text.fragment.length() < 4) {
                DCHECK(false);
                return;
            }

            auto hs_type = HandshakeType(text.fragment[0]);
            auto hs_length = (uint32_t(text.fragment[1]) << 16) | (uint32_t(text.fragment[2]) << 8) | text.fragment[3];
            stringu8 content = text.fragment.substr(4, hs_length);
            switch (hs_type) {
            case HandshakeType::ServerHello:
                parseServerHello(content);
                break;

            default:
                break;
            }

            break;
        }

        case ContentType::ChangeCipherSpec:
            // Do nothing
            break;

        case ContentType::ApplicationData:
        {
            break;
        }

        default:
            break;
        }
    }

    void TLS::parseServerHello(const stringu8& content) {
        ProtocolVersion ver;
        ver.major = content[0];
        ver.minor = content[1];

        stringu8 random = content.substr(2, 32);
        uint8_t length8 = content[34];
        stringu8 legacy_session_id_echo = content.substr(35, length8);
        uint8_t cs0 = content[35 + length8];
        uint8_t cs1 = content[35 + length8 + 1];
        uint8_t legacy_compression_method = content[35 + length8 + 2];
        uint16_t length16 = (uint16_t(content[35 + length8 + 3]) << 8) | content[35 + length8 + 4];

        stringu8 hello_req_rand({ 0xCF, 0x21, 0xAD, 0x74, 0xE5, 0x9A, 0x61, 0x11, 0xBE, 0x1D, 0x8C, 0x02, 0x1E, 0x65, 0xB8, 0x91,
            0xC2, 0xA2, 0x11, 0x16, 0x7A, 0xBB, 0x8C, 0x5E, 0x07, 0x9E, 0x09, 0xE2, 0xC8, 0xA8, 0x33, 0x9C });

        if (random == hello_req_rand) {
            DCHECK(false);
            return;
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
        auto exts_data = getSupportExtensions();
        out->append(getUInt16Bytes(UIntToUInt16(exts_data.size())));
        out->append(exts_data);
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
        std::vector<Extension> exts;

        // ServerName
        {
            string8 name_str = host_;

            Extension sn;
            sn.type = ExtensionType::ServerName;

            ServerName name;
            name.type = NameType::HostName;
            name.host_name.append(name_str.begin(), name_str.end());

            sn.data.append(getUInt16Bytes(1 + 2 + UIntToUInt16(name.host_name.size())));
            sn.data.push_back(uint8_t(name.type));
            sn.data.append(getUInt16Bytes(UIntToUInt16(name.host_name.size())));
            sn.data.append(name.host_name);

            exts.push_back(sn);
        }

        // SupportedVersions
        Extension sv;
        sv.type = ExtensionType::SupportedVersions;
        sv.data.push_back(2);
        sv.data.append({ 3, 4 });
        exts.push_back(sv);

        // SupportedGroups
        Extension sg;
        sg.type = ExtensionType::SupportedGroups;
        sg.data.append(getUInt16Bytes(2 * 3));
        sg.data.append(getUInt16Bytes(uint16_t(NamedGroup::X25519)));
        sg.data.append(getUInt16Bytes(uint16_t(NamedGroup::SECP384R1)));
        sg.data.append(getUInt16Bytes(uint16_t(NamedGroup::SECP256R1)));
        exts.push_back(sg);

        // SignatureAlgorithms
        Extension sa;
        sa.type = ExtensionType::SignatureAlgorithms;
        sa.data.append(getUInt16Bytes(4));
        sa.data.append(getUInt16Bytes(uint16_t(SignatureScheme::ECDSA_SECP256R1_SHA256)));
        sa.data.append(getUInt16Bytes(uint16_t(SignatureScheme::RSA_PKCS1_SHA256)));
        exts.push_back(sa);

        // KeyShare
        Extension ks;
        ks.type = ExtensionType::KeyShare;

        KeyShareClientHello key_share;

        {
            auto k = utl::BigInteger::fromRandom(32 * 8);
            k.setBit(255, 0);
            k.setBit(254, 1);
            k.setBit(2, 0);
            k.setBit(1, 0);
            k.setBit(0, 0);

            uint32_t A;
            uint8_t cofactor, Up;
            utl::BigInteger p, order, Vp, result;
            crypto::ECDP::curve25519(&p, &A, &order, &cofactor, &Up, &Vp);
            crypto::ECDP::X25519(p, k, utl::BigInteger::fromU32(Up), &result);

            auto r = result.getBytesLE();
            if (r.size() < 32) {
                r.insert(r.end(), 32 - r.size(), 0);
            }

            KeyShareEntry entry_x25519;
            entry_x25519.group = NamedGroup::X25519;
            entry_x25519.key_exchange.append(r);
            key_share.client_shares.push_back(std::move(entry_x25519));
        }

        uint8_t h;
        utl::BigInteger a, b, S, p, Gx, Gy, n;
        {
            crypto::ECDP::secp384r1(&p, &a, &b, &S, &Gx, &Gy, &n, &h);
            auto d = utl::BigInteger::fromRandom(utl::BigInteger::ONE, n - 1);
            crypto::ECDP::mulPoint(p, a, d, &Gx, &Gy);
            crypto::ECDP::verifyPoint(p, a, b, Gx, Gy);

            ECDHEParams p_secp384;
            p_secp384.X = Gx.getBytesBE();
            p_secp384.Y = Gy.getBytesBE();

            KeyShareEntry entry_s384;
            entry_s384.group = NamedGroup::SECP384R1;
            entry_s384.key_exchange = p_secp384.toBytes();
            key_share.client_shares.push_back(std::move(entry_s384));
        }

        {
            crypto::ECDP::secp256r1(&p, &a, &b, &S, &Gx, &Gy, &n, &h);
            auto d = utl::BigInteger::fromRandom(utl::BigInteger::ONE, n - 1);
            crypto::ECDP::mulPoint(p, a, d, &Gx, &Gy);
            crypto::ECDP::verifyPoint(p, a, b, Gx, Gy);

            ECDHEParams p_secp256;
            p_secp256.X = Gx.getBytesBE();
            p_secp256.Y = Gy.getBytesBE();

            KeyShareEntry entry_s256;
            entry_s256.group = NamedGroup::SECP256R1;
            entry_s256.key_exchange = p_secp256.toBytes();
            key_share.client_shares.push_back(std::move(entry_s256));
        }

        uint16_t entries_length = 0;
        for (const auto& entry : key_share.client_shares) {
            entries_length += 2 + 2 + UIntToUInt16(entry.key_exchange.size());
        }
        ks.data.append(getUInt16Bytes(entries_length));

        for (const auto& entry : key_share.client_shares) {
            ks.data.append(getUInt16Bytes(uint16_t(entry.group)));
            ks.data.append(getUInt16Bytes(UIntToUInt16(entry.key_exchange.size())));
            ks.data.append(entry.key_exchange);
        }
        exts.push_back(ks);

        // Extension 统计
        for (const auto& ext : exts) {
            ret.append(getUInt16Bytes(uint16_t(ext.type)));
            ret.append(getUInt16Bytes(UIntToUInt16(ext.data.size())));
            ret.append(ext.data);
        }

        return ret;
    }

    void TLS::testHandshake() {
        host_ = "tls13.crypto.mozilla.org";

        if (!record_layer_.connect(host_)) {
            DCHECK(false);
            return;
        }

        stringu8 client_hello;
        constructHandshake(HandshakeType::ClientHello, &client_hello);

        TLSRecordLayer::TLSPlaintext text;
        text.type = ContentType::Handshake;
        text.version.major = 3;
        text.version.minor = 1;
        text.length = uint16_t(client_hello.length());
        text.fragment = client_hello;

        if (!record_layer_.sendFragment(text)) {
            DCHECK(false);
            return;
        }

        TLSRecordLayer::TLSPlaintext out;
        if (!record_layer_.recvFragment(&out)) {
            DCHECK(false);
            return;
        }
        parseFragment(out);

        if (!record_layer_.recvFragment(&out)) {
            DCHECK(false);
            return;
        }
        parseFragment(out);

        record_layer_.disconnect();
    }

}
}
}