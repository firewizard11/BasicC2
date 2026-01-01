#include <TCPListener.hpp>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
using namespace std;

int main() {
    auto t = TCPListener("0.0.0.0", 8000, 5);
    Connection c;

    t.start();
    cout << "INFO: Listening on " << t.get_ip() << ":" << t.get_port() << "\n";
    while (true) {
        c = t.get_connection();
        if (c.fd == -1) {
            cout << "INFO: Waiting for connection...\n";
            sleep(10);
            continue;
        }
        cout << "CONNECTION: " << inet_ntoa(c.addr.sin_addr) << ":" << ntohs(c.addr.sin_port) << "\n";
    } 
    return 0;
}