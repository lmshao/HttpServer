//
// Created by lmshao on 2021/10/23.
//

#ifndef _HTTP_SERVICE_H
#define _HTTP_SERVICE_H

#include <string>

#include "http_utils.h"
#include "master_service.h"

class HttpService {
  public:
    HttpService();

    explicit HttpService(ClientInfo *client);

    virtual ~HttpService();

    void reset();

    void reset(int socket);

    static void handle(ClientInfo *ci);

    static void run(void *client);

    static void stop();

    bool receive();

    bool doService();

    char *getBuffer() const {
        return buffer_;
    }

    int getBufferLength() const {
        return length_;
    }

    std::string getClient() const {
        return std::string(client_->ip) + ':' + std::to_string(client_->port);
    }

  private:
    int socket_;
    ClientInfo *client_;
    HttpRequest *req_;
    HttpResponse *res_;
    char *buffer_;
    int length_;
};

#endif  //_HTTP_SERVICE_H
