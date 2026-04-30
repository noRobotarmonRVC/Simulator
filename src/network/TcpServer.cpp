#include "network/TcpServer.hpp"
#include "network/Session.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>

TcpServer::TcpServer(uint16_t port, RequestHandler handler)
    : m_port{port}
    , m_handler{std::move(handler)}
{}

TcpServer::~TcpServer() {
    stop();
    if (m_listenFd >= 0) {
        ::close(m_listenFd);
        m_listenFd = -1;
    }
}

void TcpServer::createListenSocket() {
    m_listenFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_listenFd < 0) {
        throw std::runtime_error("socket() failed");
    }

    const int opt = 1;
    ::setsockopt(m_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(m_port);

    if (::bind(m_listenFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        ::close(m_listenFd);
        m_listenFd = -1;
        throw std::runtime_error("bind() failed on port " + std::to_string(m_port));
    }

    if (::listen(m_listenFd, 1) < 0) {
        ::close(m_listenFd);
        m_listenFd = -1;
        throw std::runtime_error("listen() failed");
    }
}

void TcpServer::start() {
    createListenSocket();
    m_running = true;

    while (m_running) {
        // Use select() with a 1-second timeout so stop() can interrupt.
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(m_listenFd, &readfds);
        timeval tv{1, 0};

        const int ret = ::select(m_listenFd + 1, &readfds, nullptr, nullptr, &tv);
        if (ret <= 0) continue;  // timeout or EINTR

        sockaddr_in clientAddr{};
        socklen_t   addrLen = sizeof(clientAddr);
        const int clientFd  = ::accept(
            m_listenFd,
            reinterpret_cast<sockaddr*>(&clientAddr),
            &addrLen);

        if (clientFd < 0) continue;

        Session session{clientFd, m_handler};
        session.run();  // blocks until client disconnects
    }
}

void TcpServer::stop() {
    m_running = false;
}
