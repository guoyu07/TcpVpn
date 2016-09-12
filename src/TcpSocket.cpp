//
// Created by jianyu on 9/10/16.
//

#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <glog/logging.h>
#include <iostream>
#include "TcpSocket.h"

int get_ipinfo(std::string host_s, int port, sockaddr *addr, socklen_t *addrlen) {
    struct addrinfo hints;
    struct addrinfo *res;
    int r;
    const char *host = host_s.c_str();
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_family = AF_INET;
    if (0 != (r = getaddrinfo(host, NULL, &hints, &res))) {
        LOG(ERROR) << "SOCKET INITIALIZATION:getaddrinfo, error";
        return -1;
    }

    if (res->ai_family == AF_INET)
        ((struct sockaddr_in *)res->ai_addr)->sin_port = htons(port);
    else if (res->ai_family == AF_INET6)
        ((struct sockaddr_in6 *)res->ai_addr)->sin6_port = htons(port);
    else {
        LOG(ERROR) << "SOCKET INITIALIZATION:unknown ai_family " << res->ai_family;
        freeaddrinfo(res);
        return -1;
    }
    memcpy(addr, res->ai_addr, res->ai_addrlen);
    *addrlen = res->ai_addrlen;
    return 0;
}


TcpSocket::TcpSocket() {
    int flag = 1;
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int r = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (unsigned char*)&flag, sizeof(int));
    if (r) {
        LOG(INFO) << "set TCP_NODELAY fail";
    }
}

int TcpSocket::bind_addr(std::string host, int port) {
    struct sockaddr addr;
    socklen_t len;
    get_ipinfo(host, port, &addr, &len);
    int r = bind(sock, &addr, len);
    if (r < 0)
        return -1;
    return 0;
}

int TcpSocket::connect_addr(std::string host, int port) {
    struct sockaddr addr;
    socklen_t len;
    get_ipinfo(host, port, &addr, &len);
    int r= connect(sock, &addr, len);
    if (r < 0) {
        return -1;
    }
    return 0;
}

int TcpSocket::recv_package(unsigned char *recv_buf) {
    int more = recv(sock, buf, 1500, 0);
    LOG(INFO) << "recv " << more;
    if (more <= 0) {
        alive = false;
        return -1;
    }
    memcpy(sock_buf + pos, buf, more);
    pos += more;
    if (pos < PACKAGE_HEADER_SIZE) {
        LOG(INFO) << "need " << pos - PACKAGE_HEADER_SIZE;
        return 0;
    }
    TcpPackageHeader *header = (TcpPackageHeader*)sock_buf;
    int package_len = header->len;
    if (package_len > pos + PACKAGE_HEADER_SIZE) {
        LOG(INFO) << "need " << pos - package_len;
        return 0;
    }
    memcpy(recv_buf, sock_buf + PACKAGE_HEADER_SIZE, package_len);
    if (pos == package_len + PACKAGE_HEADER_SIZE) {
        pos = 0;
        sock_buf = backup_a;
    } else {
        if (sock_buf == backup_a) {
            memcpy(backup_b, sock_buf + package_len + PACKAGE_HEADER_SIZE,
            pos - package_len - PACKAGE_HEADER_SIZE);
            sock_buf = backup_b;
            pos -= package_len + PACKAGE_HEADER_SIZE;
        } else if (sock_buf == backup_b) {
            memcpy(backup_a, sock_buf + package_len + PACKAGE_HEADER_SIZE,
                   pos - package_len - PACKAGE_HEADER_SIZE);
            sock_buf = backup_a;
            pos -= package_len + PACKAGE_HEADER_SIZE;
        }
    }
    return package_len;
}

int TcpSocket::send_package(unsigned char *_buf, int size) {
    memcpy(send_buf + PACKAGE_HEADER_SIZE, _buf, size);
    TcpPackageHeader *header = (TcpPackageHeader*)send_buf;
    header->len = size;
    return send(sock, send_buf, size + PACKAGE_HEADER_SIZE, 0);
}

TcpSocket* TcpSocket::accept_connection() {
    int fd = accept(sock, NULL, NULL);
    if (fd <= 0) {
        return NULL;
    }
    return new TcpSocket(fd);
}

TcpSocket::TcpSocket(int __sock) {
    sock = __sock;
}

bool TcpSocket::is_alive() {
    return alive;
}

void TcpSocket::close_addr() {
    close(sock);
}

void TcpSocket::listen_addr() {
    listen(sock, MAX_BACKLOG);
}