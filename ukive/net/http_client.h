#ifndef UKIVE_NET_HTTP_CLIENT_H_
#define UKIVE_NET_HTTP_CLIENT_H_

#include "utils/string_utils.h"

// ���� RFC 7230 ʵ�ֵ� HTTP �ͻ���

namespace ukive {
namespace net {

    class HttpClient {
    public:
        HttpClient();

        bool connect(const std::string& url);

    private:
        struct URLInfo {
            std::string scheme;
            std::string host;
            std::string path;
            unsigned short port = 0;
        };

        bool getURLInfo(const std::string& url, URLInfo* info) const;
    };

}
}

#endif  // UKIVE_NET_HTTP_CLIENT_H_