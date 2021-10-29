//
// Created by lmshao on 2021/10/23.
//

#include "master_service.h"

#include <arpa/inet.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "http_service.h"
#include "utils.h"

MasterService::MasterService() : host_("0.0.0.0"), port_(80), stop_(false) {}

MasterService::MasterService(const std::string &host, int port) : host_(host), port_(port), stop_(false) {}

MasterService::~MasterService() {}

void MasterService::run() {
    int srv_socket, cli_socket;
    socklen_t addr_len;
    struct sockaddr_in srv_addr, cli_addr;

    struct epoll_event ev, readyEvents[1024];
    int nfds, epollfd;

    if ((srv_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        LOGE("error socket: %s", strerror(errno));
        exit(1);
    }

    int optval = 1;
    if (setsockopt(srv_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        LOGE("error setsockopt: %s", strerror(errno));
        exit(0);
    }

    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port_);
    srv_addr.sin_addr.s_addr = inet_addr(host_.c_str());  // htonl(INADDR_ANY);

    if (bind(srv_socket, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0) {
        LOGE("error bind %s:%d : %s", host_.c_str(), port_, strerror(errno));
        exit(1);
    }

    if (listen(srv_socket, 1024) < 0) {
        LOGE("error listen %s:%d : %s", host_.c_str(), port_, strerror(errno));
        exit(1);
    }

    LOG("Listen Port: %s:%d\nListening ...\n", host_.c_str(), port_);

    if ((epollfd = epoll_create1(0)) == -1) {
        LOGE("error epoll_create: %s", strerror(errno));
        exit(1);
    }

    ev.events = EPOLLIN;
    ev.data.fd = srv_socket;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, srv_socket, &ev) == -1) {
        LOGE("error epoll_ctl: %s", strerror(errno));
        exit(1);
    }

    while (!stop_) {
        nfds = epoll_wait(epollfd, readyEvents, 1024, -1);
        if (nfds == -1 || errno == EINTR) {
            LOGE("error epoll_wait: %s", strerror(errno));
            break;
        }

        for (int n = 0; n < nfds; n++) {
            if (readyEvents[n].data.fd == srv_socket) {
                cli_socket = accept(srv_socket, (struct sockaddr *)&cli_addr, &addr_len);
                if (cli_socket == -1) {
                    LOGE("error accept: %s", strerror(errno));
                    exit(1);
                }

                LOG("\nNew client connections client[%d] %s:%d\n", cli_socket, inet_ntoa(cli_addr.sin_addr),
                    ntohs(cli_addr.sin_port));

                auto *ci = new ClientInfo(cli_socket, inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
                this->clients_[cli_socket] = ci;

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = cli_socket;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, cli_socket, &ev) == -1) {
                    LOGE("error epoll_ctl: %s", strerror(errno));
                    exit(1);
                }
            } else {
                cli_socket = readyEvents[n].data.fd;
                HttpService::handle(clients_.at(cli_socket));
            }
        }
    }
}

void MasterService::setHost(const std::string &host) {
    host_ = host;
}

void MasterService::setPort(int port) {
    port_ = port;
}

void MasterService::stop() {
    stop_ = true;
    HttpService::stop();
}
