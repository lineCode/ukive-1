#include "ukive/net/http_client.h"

#include <winsock2.h>
#include <WS2tcpip.h>

#include "ukive/log.h"

#pragma comment(lib, "Ws2_32.lib")


namespace ukive {
namespace net {

    bool HttpClient::is_initialized_ = false;

    HttpClient::HttpClient() {}

    void HttpClient::initialize() {
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

    void HttpClient::unInitialize() {
        WSACleanup();
        is_initialized_ = false;
    }

    bool HttpClient::connect(const std::string& url) {
        if (!is_initialized_) {
            LOG(Log::ERR) << "Winsock.dll is not initialized!";
            return false;
        }

        auto s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == INVALID_SOCKET) {
            LOG(Log::ERR) << "Failed to create socket: " << WSAGetLastError();
            return false;
        }

        std::shared_ptr<SOCKET> s_guard(&s, [](SOCKET* s) {
            closesocket(*s);
        });

        ADDRINFOA addr_info = { 0 };
        addr_info.ai_family = AF_INET;
        addr_info.ai_socktype = SOCK_STREAM;
        addr_info.ai_protocol = IPPROTO_TCP;

        std::string host = "www.baidu.com";

        PADDRINFOA addr_ret = nullptr;
        auto result = ::getaddrinfo(host.c_str(), "80", &addr_info, &addr_ret);
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

        if (::connect(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
            LOG(Log::ERR) << "Failed to connect: " << WSAGetLastError();
            return false;
        }

        std::string ua = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.131 Safari/537.36";

        std::string buf;
        buf.append("GET / HTTP/1.1").append("\r\n");
        buf.append("Host: ").append(host).append("\r\n");
        buf.append("User-Agent: ").append(ua).append("\r\n");
        buf.append("\r\n");

        auto bytes_sent = ::send(s, buf.data(), buf.length(), 0);
        if (bytes_sent == SOCKET_ERROR) {
            LOG(Log::ERR) << "Failed to send: " << WSAGetLastError();
            return false;
        }

        if (::shutdown(s, SD_SEND) == SOCKET_ERROR) {
            LOG(Log::ERR) << "Failed to shutdown: " << WSAGetLastError();
            return false;
        }

        int bytes_revd;
        char recv_buf[512];
        std::string response;

        do {
            bytes_revd = ::recv(s, recv_buf, 512, 0);
            if (bytes_revd > 0) {
                response.append(recv_buf, bytes_revd);
            } else if (bytes_revd == 0) {
                LOG(Log::INFO) << "Connection closed";
            } else {
                LOG(Log::ERR) << "Failed to recv: " << WSAGetLastError();
            }
        } while (bytes_revd > 0);

        return true;
    }

}
}