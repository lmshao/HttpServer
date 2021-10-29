//
// Created by lmshao on 2021/10/23.
//

#ifndef _MASTER_SERVICE_H
#define _MASTER_SERVICE_H

#include <string.h>

#include <string>
#include <unordered_map>

struct ClientInfo {
    int socket;
    char ip[16]{};
    int port;
    ClientInfo() : socket(0), port(0) {}

    ClientInfo(int socket, char *ip, int port) : socket(socket), port(port) {
        size_t length = strlen(ip);
        memcpy(this->ip, ip, length <= 15 ? length : 15);
    }
};

class MasterService {
  public:
    MasterService();
    MasterService(const std::string &host, int port);
    ~MasterService();

    void run();
    void stop();

    void setHost(const std::string &host);

    void setPort(int port);

  private:
    std::string host_;
    int port_;
    std::unordered_map<int, ClientInfo *> clients_;
    bool stop_;
};

#endif  //_MASTER_SERVICE_H
