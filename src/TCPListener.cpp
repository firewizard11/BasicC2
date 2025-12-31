#include "TCPListener.hpp"
#include <iostream>
#include <netinet/in.h>
#include <random>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

TCPListener::TCPListener() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(30000, 50000);

    m_listener = -1;
    m_listener_port = distrib(gen);
    m_running = false;
}

TCPListener::~TCPListener() {
    stop_listener();

    if (m_listener_thread.joinable()) {
        m_listener_thread.join();
    }
}

int TCPListener::get_port() {
    return m_listener_port;
}

int TCPListener::start_listener() {
    int res;

    std::cout << "INFO: Creating Listener on " << m_listener_port << "\n";
    res = create_listener();
    if (res == -1) {
        std::cout << "FAIL: Failed to create listener\n";
        stop_listener();
        return -1;
    }
    std::cout << "SUCCESS: Created listener\n";
    m_listener = res;

    std::cout << "INFO: Starting accept loop\n";
    res = background_listener();
    if (res == -1) {
        std::cout << "FAIL: Failed to background thread\n";
        stop_listener();
        return -1;
    }

    std::cout << "SUCCESS: Backgrounded thread\n";
    return 0;
}

int TCPListener::stop_listener() {

    if (is_listening()) {
        close(m_listener);
    }

    if (is_running()) {
        m_running = false;
    }

    return 0;
}

int TCPListener::background_listener() {
    try {
        m_listener_thread = std::thread(&TCPListener::accept_loop, this);
    } catch (const std::system_error &e) {
        std::cout << "EXCEPTION: " << e.what() << "\n";
        return -1;
    }

    return 0;
}

int TCPListener::create_listener() {
    int res;
    int lfd;
    sockaddr_in laddr;
    laddr.sin_family = AF_INET;
    laddr.sin_addr.s_addr = INADDR_ANY;
    laddr.sin_port = htons(m_listener_port);

    res = socket(AF_INET, SOCK_STREAM, 0);
    if (res == -1) {
        return -1;
    }
    lfd = res;

    res = bind(lfd, (sockaddr *)&laddr, (socklen_t)sizeof(laddr));
    if (res == -1) {
        close(lfd);
        return -1;
    }

    res = listen(lfd, 5);
    if (res == -1) {
        close(lfd);
        return -1;
    }

    return lfd;
}

bool TCPListener::is_listening() { return m_listener > -1; }

bool TCPListener::is_running() { return m_running; }

void TCPListener::accept_loop() {
    if (!is_listening()) {
        return;
    }
    int res;
    m_running = true;
    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    while (m_running) {
        res = accept(m_listener, (sockaddr *)&addr, &addr_len);
        if (res == -1) {
            continue;
        }
    }
}
