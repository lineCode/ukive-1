#include "ukive/net/tls.h"

#include <chrono>
#include <random>

#include "utils/stl_utils.h"
#include "utils/big_integer/big_integer.h"

#include "ukive/net/socket.h"
#include "ukive/security/crypto/ecdp.h"
#include "ukive/security/digest/sha.h"


namespace ukive {
namespace net {
namespace tls {

    TLS::TLS() {}

    TLS::~TLS() {}

    void TLS::parseFragment(const TLSRecordLayer::TLSPlaintext& text) {
        stringu8::size_type idx = 0;
        for (; idx < text.fragment.size();) {
            switch (text.type) {
            case ContentType::Alert:
            {
                if (text.fragment.length() < 2 + idx) {
                    DCHECK(false);
                    return;
                }

                auto level = AlertLevel(text.fragment[idx++]);
                auto descp = AlertDescription(text.fragment[idx++]);
                break;
            }

            case ContentType::Handshake:
            {
                if (text.fragment.length() < 4 + idx) {
                    DCHECK(false);
                    return;
                }

                auto hs_type = HandshakeType(text.fragment[idx++]);
                auto hs_length = (uint32_t(text.fragment[idx++]) << 16) 
                    | (uint32_t(text.fragment[idx++]) << 8) 
                    | text.fragment[idx++];
                stringu8 content = text.fragment.substr(idx, hs_length);
                idx += hs_length;

                switch (hs_type) {
                case HandshakeType::ServerHello:
                    server_hello_data_ = text.fragment;
                    parseServerHello(content);
                    break;

                case HandshakeType::EncryptedExtensions:
                    parseEncryptedExtensions(content);
                    break;

                case HandshakeType::Certificate:
                    parseCertificate(content);
                    break;

                case HandshakeType::CertificateVerify:
                    break;

                case HandshakeType::Finished:
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
        DCHECK(cs0 == 0x13 && cs1 == 0x01);
        uint8_t legacy_compression_method = content[35 + length8 + 2];
        uint16_t length16 = (uint16_t(content[35 + length8 + 3]) << 8) | content[35 + length8 + 4];

        stringu8 hello_req_rand({ 0xCF, 0x21, 0xAD, 0x74, 0xE5, 0x9A, 0x61, 0x11, 0xBE, 0x1D, 0x8C, 0x02, 0x1E, 0x65, 0xB8, 0x91,
            0xC2, 0xA2, 0x11, 0x16, 0x7A, 0xBB, 0x8C, 0x5E, 0x07, 0x9E, 0x09, 0xE2, 0xC8, 0xA8, 0x33, 0x9C });

        if (random == hello_req_rand) {
            DCHECK(false);
            return;
        }

        stringu8 ext_data = content.substr(35 + length8 + 5, length16);
        parsePlainExtensions(ext_data);

        // Section 7.1
        // 生成 server_handshake_traffic_secret
        uint8_t salt[32];
        uint8_t psk[32];
        std::memset(salt, 0, 32);
        std::memset(psk, 0, 32);

        uint8_t early_secret[64];
        digest::HKDF::hkdfExtract(
            digest::SHAVersion::SHA256,
            salt, 32, psk, 32, early_secret);

        stringu8 out;
        deriveSecret(stringu8(early_secret, 32), "derived", {}, &out);

        stringu8 ecdhe = share_K_;

        uint8_t handshake_secret[64];
        digest::HKDF::hkdfExtract(
            digest::SHAVersion::SHA256,
            out.data(), out.size(), ecdhe.data(), ecdhe.size(), handshake_secret);

        stringu8 sht_secret;
        stringu8 message = client_hello_data_ + server_hello_data_;
        deriveSecret(
            stringu8(handshake_secret, 32), "s hs traffic",
            message, &sht_secret);

        // Section 7.3
        // 生成 server_write_iv
        stringu8 sw_iv;
        // iv 的长度根据 RFC 5116
        // https://tools.ietf.org/html/rfc5116
        HKDFExpandLabel(sht_secret, "iv", {}, 12, &sw_iv);

        // 生成 server_write_key
        stringu8 sw_key;
        HKDFExpandLabel(sht_secret, "key", {}, 16, &sw_key);

        record_layer_.setServerWriteKey(sw_key, sw_iv);
    }

    void TLS::parsePlainExtensions(const stringu8& data) {
        size_t i = 0;
        std::vector<Extension> exts;
        for (; i < data.length();) {
            Extension ext;
            ext.type = ExtensionType((uint16_t(data[i++]) << 8) | data[i++]);
            uint16_t length = (uint16_t(data[i++]) << 8) | data[i++];
            ext.data = data.substr(i, length);
            i += length;
            exts.push_back(ext);
        }

        for (auto& ext : exts) {
            if (ext.type == ExtensionType::SupportedVersions) {
                ProtocolVersion sel;
                sel.major = ext.data[0];
                sel.minor = ext.data[1];
                DCHECK(sel.major == 0x03 && sel.minor == 0x04);
            } else if (ext.type == ExtensionType::KeyShare) {
                KeyShareEntry entry;
                entry.group = NamedGroup((uint16_t(ext.data[0]) << 8) | ext.data[1]);
                uint16_t length = (uint16_t(ext.data[2]) << 8) | ext.data[3];
                entry.key_exchange = ext.data.substr(4, length);
                DCHECK(entry.group == NamedGroup::X25519);

                BigInteger share_K;
                auto U = BigInteger::fromBytesLE(entry.key_exchange);
                U.setBit(255, 0);
                crypto::ECDP::X25519(x25519_P_, x25519_K_, U, &share_K);

                share_K_ = share_K.getBytesLE();
                if (share_K_.size() < 32) {
                    share_K_.insert(share_K_.end(), 32 - share_K_.size(), 0);
                }
            }
        }
    }

    void TLS::parseEncryptedExtensions(const stringu8& data) {
        uint16_t total_length = (uint16_t(data[0]) << 8) | (data[1]);

        std::vector<Extension> exts;
        for (size_t i = 2; i < total_length + 2U;) {
            Extension ext;
            ext.type = ExtensionType((uint16_t(data[i++]) << 8) | data[i++]);
            uint16_t length = (uint16_t(data[i++]) << 8) | data[i++];
            ext.data = data.substr(i, length);
            i += length;
            exts.push_back(ext);
        }

        for (auto& ext : exts) {
            if (ext.type == ExtensionType::ServerName) {
                // data 可能是空的
                // RFC 6066 Section 3
                // https://tools.ietf.org/html/rfc6066
            } else if (ext.type == ExtensionType::SupportedGroups) {
                std::vector<NamedGroup> grps;
                uint16_t length = (uint16_t(ext.data[0]) << 8) | (ext.data[1]);
                for (uint16_t i = 2; i < length + 2;) {
                    grps.push_back(NamedGroup((uint16_t(ext.data[i++]) << 8) | (ext.data[i++])));
                }
            }
        }
    }

    void TLS::parseCertificate(const stringu8& data) {
        Certificate cert_info;

        size_t i = 0;
        uint8_t crc_length = data[i++];
        cert_info.certificate_request_context = data.substr(i, crc_length);

        uint32_t cl_length = (uint32_t(data[i++]) << 16) | (uint32_t(data[i++]) << 8) | data[i++];
        for (; i < data.length();) {
            CertificateEntry entry;
            uint32_t d_len = (uint32_t(data[i++]) << 16) | (uint32_t(data[i++]) << 8) | data[i++];
            entry.cert_data = data.substr(i, d_len);
            i += d_len;

            uint16_t e_len = (uint16_t(data[i++]) << 8) | data[i++];
            for (size_t j = i; j < e_len + i;) {
                Extension ext;
                ext.type = ExtensionType((uint16_t(data[j++]) << 8) | data[j++]);
                uint16_t len = (uint16_t(data[j++]) << 8) | data[j++];
                ext.data = data.substr(j, len);
            }
            i += e_len;
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

        client_hello_data_ = *out;
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

            x25519_K_ = k;
            x25519_P_ = p;

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

        key_share_ = key_share;

        return ret;
    }

    bool TLS::deriveSecret(
        const stringu8& secret, const string8& label, const stringu8& message,
        stringu8* out)
    {
        uint8_t hash_result[32];

        digest::SHA256 sha256;
        sha256.init();
        int ret = sha256.update(message.data(), message.size());
        if (ret != digest::shaSuccess) {
            return false;
        }
        ret = sha256.result(hash_result);
        if (ret != digest::shaSuccess) {
            return false;
        }

        return HKDFExpandLabel(secret, label, stringu8(hash_result, 32), 32, out);
    }

    bool TLS::HKDFExpandLabel(
        const stringu8& secret, const string8& label, const stringu8& context,
        uint32_t length, stringu8* out)
    {
        string8 tmp = "tls13 ";

        HKDFLabel hkdf_label;
        hkdf_label.length = length;
        hkdf_label.label.append(tmp.begin(), tmp.end()).append(stringu8(label.begin(), label.end()));
        hkdf_label.context.append(context);

        stringu8 input;
        input.append(getUInt16Bytes(hkdf_label.length));
        input.push_back(uint8_t(hkdf_label.label.size()));
        input.append(hkdf_label.label);
        input.push_back(uint8_t(hkdf_label.context.size()));
        input.append(hkdf_label.context);

        uint8_t* okm = new uint8_t[length];
        int result = digest::HKDF::hkdfExpand(
            digest::SHAVersion::SHA256,
            secret.data(), secret.size(),
            input.data(), input.size(),
            okm, length);
        if (result != digest::shaSuccess) {
            delete[] okm;
            return false;
        }

        *out = stringu8(okm, length);
        delete[] okm;
        return true;
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
        if (out.type != ContentType::ChangeCipherSpec) {
            parseFragment(out);
        }

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