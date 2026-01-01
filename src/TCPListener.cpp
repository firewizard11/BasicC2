#include "TCPListener.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <queue>
#include <string>
#include <sys/socket.h>
#include <system_error>
#include <thread>
#include <unistd.h>
#include <mutex>

using namespace std;

TCPListener::TCPListener(string ip, int port, int backlog) {
    m_ip = ip;
    m_port = port;
    m_backlog = backlog;
    m_sockfd = -1;
    m_accepting = false;
}

TCPListener::~TCPListener() {
    stop();
}

string TCPListener::get_ip() {
    return m_ip;
}

int TCPListener::get_port() {
    return m_port;
}

bool TCPListener::start() {
    bool res;
    res = open_listener();
    if (!res) {
        return false;
    }

    res = start_accepting();
    if (!res) {
        return false;
    }

    return true;
}

bool TCPListener::stop() {
    m_accepting = false;
    close_listener();
    if (m_thread.joinable()) {
        m_thread.join();
    }

    return true;
}

bool TCPListener::has_connection() {
    bool res;
    lock_guard<mutex> guard(m_conns_mutex);
    res = m_conns.empty();
    return res;
}

Connection TCPListener::get_connection() {
    Connection c;
    if (!has_connection()) {
        c.fd = -1;
        return c;
    }

    lock_guard<mutex> guard(m_conns_mutex);
    c = m_conns.front();
    m_conns.pop();
    return c;
}

bool TCPListener::is_listening() {
    return m_sockfd > -1;
}

bool TCPListener::open_listener() {
    if (is_listening()) {
        return false;
    }
    int res;
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    inet_aton(m_ip.c_str(), &addr.sin_addr);
    res = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (res == -1) {
        return false;
    }
    m_sockfd = res;

    res = bind(m_sockfd, (const sockaddr *)&addr, (socklen_t)sizeof(addr));
    if (res == -1) {
        close_listener();
        return false;
    }

    res = listen(m_sockfd, m_backlog);
    if (res == -1) {
        close_listener();
        return false;
    }
    return true;
}

bool TCPListener::close_listener() {
    if (!is_listening()) {
        return false;
    }
    close(m_sockfd);
    m_sockfd = -1;
    return true;
}

bool TCPListener::start_accepting() {
    try {
        m_accepting = true;
        m_thread = thread(&TCPListener::accept_loop, this);
    } catch (system_error &e) {
        m_accepting = false;
        close_listener();
        return false;
    }

    return true;
}

void TCPListener::accept_loop() {
    int res;
    sockaddr_in addr;
    socklen_t len;
    Connection c;

    while (m_accepting) {
        res = accept(m_sockfd, (sockaddr *)&addr, &len);
        if (res == -1) {
            continue;
        }

        c.fd = res;
        c.addr = addr;
        add_connection(c);
    }
}

bool TCPListener::add_connection(Connection conn) {
    lock_guard<mutex> guard(m_conns_mutex);
    m_conns.push(conn);
    return true;
}