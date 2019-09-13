#ifndef UKIVE_NET_SOCKET_H_
#define UKIVE_NET_SOCKET_H_

#include <winsock2.h>

#include "ukive/utils/string_utils.h"


namespace ukive {
namespace net {

    class SocketClient {
    public:
        SocketClient();
        ~SocketClient();

        bool connect(const std::string& ip, unsigned short port);
        bool connectByHost(const std::string& host, unsigned short port);
        bool send(const std::string& buf);
        bool recv(int length, std::string* buf);
        bool recvAll(std::string* buf);

        bool shutdownSend();
        void close();

    private:
        enum class WaitType {
            SEND,
            RECV,
        };

        bool wait(WaitType type, int timeout_sec = -1);

        SOCKET socket_;
    };

    class SocketServer {
    public:
        SocketServer();
        ~SocketServer();

        bool wait(const std::string& ip, unsigned short port);
        bool recv(std::string* buf);

    private:
        SOCKET server_socket_;
        SOCKET accept_socket_;
    };

    void initializeSocket();
    void unInitializeSocket();

}
}

#endif  // UKIVE_NET_SOCKET_H_