#include <arpa/inet.h>
#include <bot.hpp>
#include <iostream>
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

    std::cout << "INFO: Starting Listener\n";
    res = b.start_listener();
    if (res == -1) {
        std::cout << "FAIL: Failed to start listener\n";
        return -1;
    } else {
        std::cout << "SUCCESS: Listener Started\n";
    }

    b.stop_listener();
    return 0;
}