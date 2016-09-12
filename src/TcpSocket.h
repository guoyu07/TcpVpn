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
#define BUF_LEN 100000
class TcpSocket {
private:
    int sock;
    bool alive = true;

    char send_buf[100000];

    TcpSocket(int __sock);

    char sock_buf[BUF_LEN];
    int buffer_len = 0;
    int buffer_start = 0;
    //fetch a recv_buf, return NULL if there is not any
    char* fetch_buf();
    //allocate a buf for recv
    char* alloc_buf();
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
