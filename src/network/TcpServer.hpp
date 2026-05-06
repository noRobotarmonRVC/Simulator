#pragma once
#include <atomic>
#include <thread>
#include "command/CommandDispatcher.hpp"

class TcpServer {
public:
    TcpServer(CommandDispatcher& dispatcher, int port);
    ~TcpServer();

    void start();
    void stop();

private:
    void runLoop();
    void serveClient(int connfd);

    CommandDispatcher& m_dispatcher;
    int                m_port;
    int                m_listenfd = -1;
    std::atomic<bool>  m_running{false};
    std::thread        m_thread;
};
