#include "network/Session.hpp"

#include <sys/socket.h>
#include <unistd.h>

Session::Session(int fd, RequestHandler handler)
    : m_fd{fd}
    , m_handler{std::move(handler)}
{}

Session::~Session() {
    if (m_fd >= 0) {
        ::close(m_fd);
        m_fd = -1;
    }
}

void Session::run() {
    std::string buffer;
    char ch = '\0';

    while (true) {
        const ssize_t n = ::recv(m_fd, &ch, 1, 0);
        if (n <= 0) break;  // disconnect or error

        if (ch == '\n') {
            // Strip trailing '\r' (Windows line endings)
            if (!buffer.empty() && buffer.back() == '\r') {
                buffer.pop_back();
            }
            if (!buffer.empty()) {
                const std::string response = m_handler(buffer) + "\n";
                ::send(m_fd, response.c_str(), response.size(), 0);
            }
            buffer.clear();
        } else {
            buffer += ch;
        }
    }
}
