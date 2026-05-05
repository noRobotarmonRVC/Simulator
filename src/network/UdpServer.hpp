#pragma once
#include <atomic>
#include <thread>
#include "command/CommandDispatcher.hpp"

class UdpServer {
public:
    UdpServer(CommandDispatcher& dispatcher, int port);
    ~UdpServer();

    void start();
    void stop();

private:
    void runLoop();

    CommandDispatcher& m_dispatcher;
    int                m_port;
    int                m_sockfd = -1;
    std::atomic<bool>  m_running{false};
    std::thread        m_thread;
};
