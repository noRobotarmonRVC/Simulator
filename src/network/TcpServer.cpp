#include "network/TcpServer.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

TcpServer::TcpServer(CommandDispatcher& dispatcher, int port)
    : m_dispatcher(dispatcher), m_port(port)
{}

TcpServer::~TcpServer() { stop(); }

void TcpServer::start() {
    m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listenfd < 0) throw std::runtime_error("socket() failed");

    int opt = 1;
    setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct timeval tv{1, 0};
    setsockopt(m_listenfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    struct sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(static_cast<uint16_t>(m_port));

    if (bind(m_listenfd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        close(m_listenfd);
        throw std::runtime_error("bind() failed");
    }
    if (listen(m_listenfd, 1) < 0) {
        close(m_listenfd);
        throw std::runtime_error("listen() failed");
    }

    m_running = true;
    m_thread  = std::thread(&TcpServer::runLoop, this);
}

void TcpServer::stop() {
    m_running = false;
    if (m_thread.joinable()) m_thread.join();
    if (m_listenfd >= 0) { close(m_listenfd); m_listenfd = -1; }
}

void TcpServer::runLoop() {
    while (m_running) {
        struct sockaddr_in clientAddr{};
        socklen_t addrLen = sizeof(clientAddr);
        int connfd = accept(m_listenfd,
                            reinterpret_cast<struct sockaddr*>(&clientAddr), &addrLen);
        if (connfd < 0) continue;

        serveClient(connfd);
        close(connfd);
    }
}

void TcpServer::serveClient(int connfd) {
    struct timeval tv{1, 0};
    setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    std::string pending;
    char buf[1024];

    while (m_running) {
        ssize_t n = recv(connfd, buf, sizeof(buf) - 1, 0);
        if (n == 0) break;
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
            break;
        }
        buf[n] = '\0';
        pending.append(buf, n);

        size_t pos;
        while ((pos = pending.find('\n')) != std::string::npos) {
            std::string cmd = pending.substr(0, pos);
            pending.erase(0, pos + 1);

            std::string response = m_dispatcher.dispatch(cmd);
            response += '\n';
            send(connfd, response.c_str(), response.size(), 0);
        }
    }
}
