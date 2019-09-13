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

        TLSRecordLayer();
        ~TLSRecordLayer();

        bool connect(const string8& host);
        void disconnect();

        bool sendFragment(const TLSPlaintext& text);
        bool recvFragment(TLSPlaintext* text);

    private:
        void OnBackgroundWorker();

        std::thread worker_;
        SocketClient socket_client_;
    };

}
}
}

#endif  // UKIVE_NET_TLS_RECORD_LAYER_HPP_