#include "ukive/net/socket.h"

#include <WS2tcpip.h>

#include "ukive/log.h"
#include "ukive/utils/stl_utils.h"

#pragma comment(lib, "Ws2_32.lib")


namespace ukive {
namespace net {

    bool is_initialized_ = false;

    void initializeSocket() {
        if (is_initialized_) {
            LOG(Log::ERR) << "Winsock.dll is already initialized!";
            return;
        }

        WORD ver_req = MAKEWORD(2, 2);
        WSADATA wsa_data;

        auto ret = WSAStartup(ver_req, &wsa_data);
        if (ret != 0) {
            LOG(Log::ERR) << "Failed to init Winsock.dll: " << ret;
        }

        if (LOBYTE(wsa_data.wVersion) != 2 || HIBYTE(wsa_data.wVersion) != 2) {
            LOG(Log::ERR) << "Could not find a usable version of Winsock.dll";
            WSACleanup();
        }

        is_initialized_ = true;
    }

    void unInitializeSocket() {
        WSACleanup();
        is_initialized_ = false;
    }


    // SocketClient
    SocketClient::SocketClient()
        : socket_(INVALID_SOCKET) {
    }

    SocketClient::~SocketClient() {
        if (socket_ != INVALID_SOCKET) {
            closesocket(socket_);
        }
    }

    bool SocketClient::connect(const std::string& ip, unsigned short port) {
        if (!is_initialized_) {
            LOG(Log::ERR) << "Winsock.dll is not initialized!";
            return false;
        }

        if (socket_ != INVALID_SOCKET) {
            LOG(Log::ERR) << "This client is already connected!";
            return false;
        }

        socket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socket_ == INVALID_SOCKET) {
            LOG(Log::ERR) << "Failed to create socket: " << WSAGetLastError();
            return false;
        }

        ULONG addr_bin = 0;
        if (InetPtonA(AF_INET, ip.c_str(), &addr_bin) != 1) {
            LOG(Log::ERR) << "Failed to convert ip addr: " << WSAGetLastError();
        }

        sockaddr_in addr = { 0 };
        addr.sin_family = AF_INET;
        addr.sin_addr.S_un.S_addr = addr_bin;
        addr.sin_port = htons(port);

        if (::connect(socket_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
            LOG(Log::ERR) << "Failed to connect: " << WSAGetLastError();
            return false;
        }

        return true;
    }

    bool SocketClient::connectByHost(const std::string& host, unsigned short port) {
        if (!is_initialized_) {
            LOG(Log::ERR) << "Winsock.dll is not initialized!";
            return false;
        }

        if (socket_ != INVALID_SOCKET) {
            LOG(Log::ERR) << "This client is already connected!";
            return false;
        }

        socket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socket_ == INVALID_SOCKET) {
            LOG(Log::ERR) << "Failed to create socket: " << WSAGetLastError();
            return false;
        }

        ADDRINFOA addr_info = { 0 };
        addr_info.ai_family = AF_INET;
        addr_info.ai_socktype = SOCK_STREAM;
        addr_info.ai_protocol = IPPROTO_TCP;

        PADDRINFOA addr_ret = nullptr;
        auto result = ::getaddrinfo(host.c_str(), std::to_string(port).c_str(), &addr_info, &addr_ret);
        if (result != 0) {
            LOG(Log::ERR) << "Failed to get addr info: " << result;
            return false;
        }
        if (!addr_ret) {
            LOG(Log::ERR) << "No addr info";
            return false;
        }

        sockaddr_in addr = { 0 };
        memcpy(&addr, addr_ret->ai_addr, addr_ret->ai_addrlen);
        freeaddrinfo(addr_ret);

        if (::connect(socket_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
            LOG(Log::ERR) << "Failed to connect: " << WSAGetLastError();
            return false;
        }

        return true;
    }

    bool SocketClient::send(const std::string& buf) {
        if (socket_ == INVALID_SOCKET) {
            LOG(Log::ERR) << "Invalid socket.";
            return false;
        }

        auto bytes_sent = ::send(socket_, buf.data(), STLCInt(buf.length()), 0);
        if (bytes_sent == SOCKET_ERROR) {
            LOG(Log::ERR) << "Failed to send: " << WSAGetLastError();
            return false;
        }
        return true;
    }

    bool SocketClient::recv(std::string* buf) {
        if (socket_ == INVALID_SOCKET) {
            LOG(Log::ERR) << "Invalid socket.";
            return false;
        }

        int bytes_revd;
        char recv_buf[512];
        std::string response;

        do {
            bytes_revd = ::recv(socket_, recv_buf, 512, 0);
            if (bytes_revd > 0) {
                response.append(recv_buf, bytes_revd);
            } else if (bytes_revd == 0) {
                LOG(Log::INFO) << "Connection closed";
            } else {
                LOG(Log::ERR) << "Failed to recv: " << WSAGetLastError();
                return false;
            }
        } while (bytes_revd > 0);

        *buf = std::move(response);
        return true;
    }

    bool SocketClient::shutdownSend() {
        if (socket_ != INVALID_SOCKET) {
            if (::shutdown(socket_, SD_SEND) == SOCKET_ERROR) {
                LOG(Log::ERR) << "Failed to shutdown: " << WSAGetLastError();
                return false;
            }
        }
        return true;
    }

    void SocketClient::close() {
        if (socket_ != INVALID_SOCKET) {
            closesocket(socket_);
            socket_ = INVALID_SOCKET;
        }
    }


    // SocketServer
    SocketServer::SocketServer()
        : server_socket_(INVALID_SOCKET),
          accept_socket_(INVALID_SOCKET) {
    }

    SocketServer::~SocketServer() {
        if (server_socket_ != INVALID_SOCKET) {
            closesocket(server_socket_);
        }
        if (accept_socket_ != INVALID_SOCKET) {
            closesocket(accept_socket_);
        }
    }

    bool SocketServer::wait(const std::string& ip, unsigned short port) {
        if (!is_initialized_) {
            LOG(Log::ERR) << "Winsock.dll is not initialized!";
            return false;
        }

        if (server_socket_ != INVALID_SOCKET) {
            LOG(Log::ERR) << "This server socket is already created!";
            return false;
        }

        server_socket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (server_socket_ == INVALID_SOCKET) {
            LOG(Log::ERR) << "Failed to create socket: " << WSAGetLastError();
            return false;
        }

        ULONG addr_bin = 0;
        if (InetPtonA(AF_INET, ip.c_str(), &addr_bin) != 1) {
            LOG(Log::ERR) << "Failed to convert ip addr: " << WSAGetLastError();
        }

        sockaddr_in addr = { 0 };
        addr.sin_family = AF_INET;
        addr.sin_addr.S_un.S_addr = addr_bin;
        addr.sin_port = htons(port);

        if (::bind(server_socket_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
            LOG(Log::ERR) << "Failed to bind: " << WSAGetLastError();
            return false;
        }

        if (::listen(server_socket_, SOMAXCONN) == SOCKET_ERROR) {
            LOG(Log::ERR) << "Failed to listen: " << WSAGetLastError();
            return false;
        }

        accept_socket_ = ::accept(server_socket_, nullptr, nullptr);
        if (accept_socket_ == INVALID_SOCKET) {
            LOG(Log::ERR) << "Failed to accept: " << WSAGetLastError();
            return false;
        }
        return true;
    }

    bool SocketServer::recv(std::string* buf) {
        if (accept_socket_ == INVALID_SOCKET) {
            LOG(Log::ERR) << "Invalid accept socket!";
            return false;
        }

        char recv_buf[512];
        std::string response;

        int bytes_revd = ::recv(accept_socket_, recv_buf, 512, 0);
        if (bytes_revd > 0) {
            response.append(recv_buf, bytes_revd);
        } else if (bytes_revd == 0) {
            LOG(Log::INFO) << "Connection closed";
            return false;
        } else {
            LOG(Log::ERR) << "Failed to recv: " << WSAGetLastError();
            return false;
        }

        *buf = std::move(response);
        return true;
    }

}
}