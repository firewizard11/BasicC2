#pragma once
#include <netinet/in.h>
#include <queue>
#include <string>
#include <thread>
#include <mutex>
using namespace std;

struct Connection {
    int fd;
    sockaddr_in addr;
};

class TCPListener {
  public:
    TCPListener(string ip, int port, int backlog);
    ~TCPListener();

    string get_ip();
    int get_port();

    bool start();
    bool stop();
    bool has_connection();
    Connection get_connection();

  private:
    bool is_listening();
    bool open_listener();
    bool close_listener();
    bool start_accepting();
    void accept_loop();
    bool add_connection(Connection conn);

    int m_sockfd;
    thread m_thread;
    bool m_accepting;
    queue<Connection> m_conns;
    mutex m_conns_mutex;
    string m_ip;
    int m_port;
    int m_backlog;
};