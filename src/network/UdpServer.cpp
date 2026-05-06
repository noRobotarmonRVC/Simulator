#include "network/UdpServer.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <string>

UdpServer::UdpServer(CommandDispatcher& dispatcher, int port)
    : m_dispatcher(dispatcher), m_port(port)
{}

UdpServer::~UdpServer() {
    stop();
}

void UdpServer::start() {
    m_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_sockfd < 0)
        throw std::runtime_error("socket() failed");

    // 1-second receive timeout so stop() can interrupt cleanly
    struct timeval tv{1, 0};
    setsockopt(m_sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    struct sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(static_cast<uint16_t>(m_port));

    if (bind(m_sockfd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        close(m_sockfd);
        throw std::runtime_error("bind() failed");
    }

    m_running = true;
    m_thread  = std::thread(&UdpServer::runLoop, this);
}

void UdpServer::stop() {
    m_running = false;
    if (m_thread.joinable()) m_thread.join();
    if (m_sockfd >= 0) {
        close(m_sockfd);
        m_sockfd = -1;
    }
}

void UdpServer::runLoop() {
    char buf[1024];
    while (m_running) {
        struct sockaddr_in clientAddr{};
        socklen_t addrLen = sizeof(clientAddr);

        ssize_t n = recvfrom(m_sockfd, buf, sizeof(buf) - 1, 0,
                             reinterpret_cast<struct sockaddr*>(&clientAddr), &addrLen);
        if (n <= 0) continue;   // timeout (EAGAIN) or error — loop and check m_running

        buf[n] = '\0';
        std::string cmd(buf);

        std::string response = m_dispatcher.dispatch(cmd);
        response += '\n';

        sendto(m_sockfd, response.c_str(), response.size(), 0,
               reinterpret_cast<struct sockaddr*>(&clientAddr), addrLen);
    }
}
