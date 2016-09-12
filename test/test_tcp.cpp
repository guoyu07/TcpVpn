//
// Created by jianyu on 9/10/16.
//

#include <iostream>
#include "../src/TcpSocket.h"

int main() {
    char buf[1500];
    TcpSocket socket = TcpSocket();
    socket.connect_addr("127.0.0.1", 1800);
    long i = 0, r = 0;
    while (true) {
        r = socket.recv_package((unsigned char *) &i);
        if (r > 0)
            printf("recv %d\n",i);
    }

}
