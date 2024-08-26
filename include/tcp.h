#pragma once


#include <functional>

namespace tcp {

    typedef std::function<void(int, int, const char*, uint16_t)> listen_callback;

    bool listen(int& sockfd, uint16_t port, const listen_callback& cb);
    bool open(int& sockfd, const std::string& ip, uint16_t port, int = 1000);
    bool close(int& sockfd);
    int send(int sockfd, const void* src, int len);
    int receive(int sockfd, void* dst, int len, int to_sec = 600);
    int set_blocking(int sockfd, bool val);
}

