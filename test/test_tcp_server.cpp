//
// Created by jianyu on 9/10/16.
//

#include <iostream>
#include "../src/TcpSocket.h"

int main() {
    TcpSocket socket = TcpSocket();
    int s = socket.bind_addr("127.0.0.1", 1800);
    if (s < 0) {
        return 0;
    }
    socket.listen_addr();
    TcpSocket *so = socket.accept_connection();
    long i = 5;
    int size;
    for (i = 0;i < 100;i++) {
        size = so->send_package((unsigned char *) &i, sizeof(long));
    }
}