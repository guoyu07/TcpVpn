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
    char *recv_buffer = NULL;
    int more = 0;
    while (recv_buffer == NULL) {
        recv_buffer = fetch_buf();
        if (recv_buffer == NULL) {
            more = recv(sock, alloc_buf(), 1500, 0);
            if (more >= 0) {
                buffer_len += more;
                continue;
            } else {
                return -1;
            }
        }
    }
    TcpPackageHeader *header = (TcpPackageHeader*)recv_buffer;
    memcpy(recv_buf, recv_buffer + PACKAGE_HEADER_SIZE, header->len);
    return  header->len;
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

char* TcpSocket::alloc_buf() {
    if (buffer_len == 0) {
        buffer_start = 0;
        return sock_buf;
    }
    if (BUF_LEN - buffer_start - buffer_len < 1500) {
        memcpy(sock_buf, sock_buf + buffer_start, buffer_len);
    }
    return sock_buf + buffer_start + buffer_len;
}

char* TcpSocket::fetch_buf() {
    if (buffer_len < PACKAGE_HEADER_SIZE) {
        return NULL;
    }
    TcpPackageHeader *header = (TcpPackageHeader*)(sock_buf + buffer_start);
    if (buffer_len < header->len + PACKAGE_HEADER_SIZE) {
        return NULL;
    }
    char* return_buf = sock_buf + buffer_start;
    buffer_start += header->len + PACKAGE_HEADER_SIZE;
    buffer_len -= header->len + PACKAGE_HEADER_SIZE;
    return return_buf;
}