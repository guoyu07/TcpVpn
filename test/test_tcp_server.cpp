//
// Created by jianyu on 9/10/16.
//

#include <iostream>
#include "../src/TcpSocket.h"

int main() {
    TcpSocket socket = TcpSocket();
    socket.bind_addr("127.0.0.1", 1800);
    socket.listen_addr();
    TcpSocket *so = socket.accept_connection();
    long i = 5;
    int size = so->send_package((unsigned char *)&i, sizeof(long));
}