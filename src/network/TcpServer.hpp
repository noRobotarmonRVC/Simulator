#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <string>

/// Single-threaded TCP server that accepts one client at a time.
///
/// After accepting, it hands the socket to a Session which processes
/// the request/response loop synchronously.  While a client is connected
/// the server is not accepting new connections.
class TcpServer {
public:
    using RequestHandler = std::function<std::string(const std::string&)>;

    /// @param port    TCP port to listen on.
    /// @param handler Passed to each Session to handle individual commands.
    TcpServer(uint16_t port, RequestHandler handler);
    ~TcpServer();

    TcpServer(const TcpServer&)            = delete;
    TcpServer& operator=(const TcpServer&) = delete;

    /// Block and accept connections until stop() is called.
    void start();

    /// Signal the accept loop to exit (safe to call from another thread).
    void stop();

private:
    uint16_t       m_port;
    RequestHandler m_handler;
    int            m_listenFd{-1};
    std::atomic<bool> m_running{false};

    void createListenSocket();
};
