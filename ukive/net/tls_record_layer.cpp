#include "ukive/net/tls_record_layer.hpp"


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

        return socket_client_.send(string8(out.begin(), out.end()));
    }

    bool TLSRecordLayer::recvFragment(TLSPlaintext* text) {
        string8 out;
        if (!socket_client_.recv(5, &out)) {
            return false;
        }
        if (out.length() < 5) {
            return false;
        }

        text->type = ContentType(out[0]);
        text->version.major = out[1];
        text->version.minor = out[2];
        text->length = (uint16_t(out[3]) << 8) | out[4];

        out.clear();
        if (!socket_client_.recv(text->length, &out)) {
            return false;
        }
        if (out.length() < text->length) {
            return false;
        }

        text->fragment = stringu8(out.begin(), out.end());
        return true;
    }

    void TLSRecordLayer::OnBackgroundWorker() {
    }

}
}
}