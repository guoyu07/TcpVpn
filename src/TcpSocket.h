//
// Created by jianyu on 9/10/16.
//

#ifndef SHARPVPN_TCPSOCKET_H
#define SHARPVPN_TCPSOCKET_H


#include <string>
#include <queue>

typedef struct TcpPackageHeader {
    u_int16_t len;
} TcpPackageHeader;

#define EOP '*'
#define PACKAGE_HEADER_SIZE sizeof(TcpPackageHeader)
#define MAX_BACKLOG 30
class TcpSocket {
private:
    int sock;
    char buf[1500];
    char backup_a[100000];
    char backup_b[100000];
    char send_buf[150000];
    char *sock_buf = backup_a;
    int pos = 0;
    bool alive = true;
    TcpSocket(int __sock);
public:
    TcpSocket();
    int bind_addr(std::string host, int port);
    int connect_addr(std::string host, int port);
    int recv_package(unsigned char *recv_buf);
    int send_package(unsigned char *buf, int size);
    TcpSocket* accept_connection();
    bool is_alive();
    void close_addr();
    void listen_addr();
};


#endif //SHARPVPN_TCPSOCKET_H
