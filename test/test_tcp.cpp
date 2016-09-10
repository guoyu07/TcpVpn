//
// Created by jianyu on 9/10/16.
//

#include <iostream>
#include "../src/TcpSocket.h"

int main() {
    char buf[1500];
    TcpSocket socket = TcpSocket();
    socket.connect_addr("127.0.0.1", 1800);
    long i = 0;
    int r = socket.recv_package((unsigned char*)&i);
    std::cout << "recv " << r << "\n";

}
