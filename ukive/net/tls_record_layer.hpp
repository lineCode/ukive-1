#ifndef UKIVE_NET_TLS_RECORD_LAYER_HPP_
#define UKIVE_NET_TLS_RECORD_LAYER_HPP_

#include <thread>

#include "ukive/net/socket.h"
#include "ukive/net/tls_common.hpp"


namespace ukive {
namespace net {
namespace tls {

    class TLSRecordLayer {
    public:
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

        struct TLSInnerPlaintext {
            stringu8 content;
            ContentType type;
            stringu8 zeros;
        };

        struct TLSCiphertext {
            ContentType opaque_type;
            ProtocolVersion legacy_record_version;
            uint16_t length;
            stringu8 encrypted_record;
        };

        TLSRecordLayer();
        ~TLSRecordLayer();

        bool connect(const string8& host);
        void disconnect();

        bool sendFragment(const TLSPlaintext& text);
        bool recvFragment(TLSPlaintext* text);

        void setServerWriteKey(const stringu8& key, const stringu8& iv);

    private:
        void OnBackgroundWorker();

        std::thread worker_;
        SocketClient socket_client_;

        bool is_encrypt_enabled_ = false;
        uint64_t sequence_num_w_ = 0;
        uint64_t sequence_num_r_ = 0;
        stringu8 sw_key_;
        stringu8 sw_iv_;
    };

}
}
}

#endif  // UKIVE_NET_TLS_RECORD_LAYER_HPP_