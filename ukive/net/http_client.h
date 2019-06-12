#ifndef UKIVE_NET_HTTP_CLIENT_H_
#define UKIVE_NET_HTTP_CLIENT_H_

#include "ukive/utils/string_utils.h"

// 根据 RFC 7230 实现的 HTTP 客户端

namespace ukive {
namespace net {

    class HttpClient {
    public:
        HttpClient();

        static void initialize();
        static void unInitialize();

        bool connect(const std::string& url);

    private:
        static bool is_initialized_;
    };

}
}

#endif  // UKIVE_NET_HTTP_CLIENT_H_