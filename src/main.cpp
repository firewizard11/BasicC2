#include <arpa/inet.h>
#include <bot.hpp>
#include <netinet/in.h>
#include <sys/socket.h>

int main() {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    inet_aton("127.0.0.1", &addr.sin_addr);
    int res;
    auto b = Bot(&addr);
    
    res = b.callback();
    if (res == -1) {
        return -1;
    }

    b.start_listener();
    b.stop_listener();
    return 0;
}