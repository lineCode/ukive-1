#include "ukive/net/tls_record_layer.hpp"

#include "ukive/security/crypto/aead.hpp"
#include "ukive/utils/big_integer/byte_string.hpp"


namespace ukive {
namespace net {
namespace tls {

    TLSRecordLayer::TLSRecordLayer() {
    }

    TLSRecordLayer::~TLSRecordLayer() {
    }

    bool TLSRecordLayer::connect(const string8& host) {
        if (!socket_client_.connectByHost(host, 443)) {
            return false;
        }

        //worker_ = std::thread(&TLSRecordLayer::OnBackgroundWorker, this);
        return true;
    }

    void TLSRecordLayer::disconnect() {
        //worker_.join();
        socket_client_.close();
    }

    bool TLSRecordLayer::sendFragment(const TLSPlaintext& text) {
        stringu8 out;
        out.push_back(uint8_t(text.type));
        out.push_back(text.version.major);
        out.push_back(text.version.minor);

        // length, 16bit
        out.append(getUInt16Bytes(UIntToUInt16(text.fragment.size())));
        out.append(text.fragment);

        ++sequence_num_w_;

        return socket_client_.send(string8(out.begin(), out.end()));
    }

    bool TLSRecordLayer::recvFragment(TLSPlaintext* text) {
        string8 rec_header;
        if (!socket_client_.recv(5, &rec_header)) {
            return false;
        }
        if (rec_header.length() < 5) {
            return false;
        }

        text->type = ContentType(rec_header[0]);
        text->version.major = rec_header[1];
        text->version.minor = rec_header[2];
        text->length = (uint16_t(rec_header[3]) << 8) | uint16_t(rec_header[4]);

        string8 out;
        if (!socket_client_.recv(text->length, &out)) {
            return false;
        }
        if (out.length() < text->length) {
            return false;
        }

        if (text->type == ContentType::ChangeCipherSpec) {
            return true;
        }

        stringu8 result;
        if (is_encrypt_enabled_ && text->type == ContentType::ApplicationData) {
            stringu8 C(out.begin(), out.end());
            auto tag = C.substr(C.size() - 16);
            C.erase(C.size() - 16, 16);
            stringu8 A(rec_header.begin(), rec_header.end());

            // Section 5.3
            stringu8 padded_seq_num(sw_iv_.size() - 8, 0);
            padded_seq_num.append(getUInt64Bytes(sequence_num_r_));

            stringu8 nonce;
            ByteString::xor(padded_seq_num, sw_iv_, &nonce);

            if (!crypto::GCM::GCM_AD(sw_key_, nonce, C, A, 16, tag, &result)) {
                return false;
            }
            ++sequence_num_r_;

            uint8_t ch = 0;
            auto idx = result.find_last_not_of(ch);
            if (idx == stringu8::npos) {
                return false;
            }

            text->type = ContentType(result[idx]);
            result = result.substr(0, idx);
        } else {
            result = stringu8(out.begin(), out.end());
        }

        text->fragment = result;
        return true;
    }

    void TLSRecordLayer::setServerWriteKey(const stringu8& key, const stringu8& iv) {
        sw_iv_ = iv;
        sw_key_ = key;
        is_encrypt_enabled_ = true;
    }

    void TLSRecordLayer::OnBackgroundWorker() {
    }

}
}
}