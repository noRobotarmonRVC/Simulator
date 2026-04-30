#pragma once

#include <functional>
#include <string>

/// Manages a single client TCP connection.
///
/// Reads newline-terminated lines from the socket, passes each line to the
/// request handler, and writes the response + '\n' back.  Blocks until the
/// client disconnects or a read error occurs.
class Session {
public:
    using RequestHandler = std::function<std::string(const std::string&)>;

    /// @param fd      Connected socket file descriptor (Session takes ownership).
    /// @param handler Called with each trimmed request line; returns response.
    Session(int fd, RequestHandler handler);
    ~Session();

    Session(const Session&)            = delete;
    Session& operator=(const Session&) = delete;

    /// Block and process requests until the client disconnects.
    void run();

private:
    int            m_fd;
    RequestHandler m_handler;
};
