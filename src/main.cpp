#include <bot.hpp>
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main() {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    inet_aton("127.0.0.1", &addr.sin_addr);
    int res;
    auto b = Bot(&addr);

    std::cout << "INFO: Calling back...\n";
    res = b.callback();
    if (res == -1) {
        std::cout << "FAIL: Callback failed\n";
    } else {
        std::cout << "SUCCESS: Callback success\n";
    }

    return 0;
}