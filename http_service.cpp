//
// Created by lmshao on 2021/10/23.
//

#include "http_service.h"

#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <memory>

#include "http_router.h"
#include "thread_pool.h"
#include "utils.h"

HttpService::HttpService() : length_(4096) {
    buffer_ = new char[length_];
};

HttpService::HttpService(ClientInfo *client)
    : socket_(client->socket), client_(client), req_(nullptr), res_(nullptr), length_(4096) {
    buffer_ = new char[length_];
}

HttpService::~HttpService() {
    delete req_;
    delete res_;
    delete[] buffer_;
}

void HttpService::reset() {
    reset(0);
}

void HttpService::reset(int socket) {
    delete req_;
    delete res_;
    socket_ = socket;
    client_ = nullptr;
    memset(buffer_, 0, length_);
}

void HttpService::handle(ClientInfo *clientInfo) {
    ThreadPool::getInstance()->addTask(run, (void *)clientInfo);
}

void HttpService::run(void *client) {
    ClientInfo *clientInfo = (ClientInfo *)client;

    std::shared_ptr<HttpService> http_service(new HttpService(clientInfo));
    http_service->req_ = new HttpRequest(clientInfo);
    http_service->res_ = new HttpResponse(clientInfo->socket);
    bool res = http_service->receive();
    if (!res)
        return;
    http_service->req_->parseHttpData(http_service->getBuffer(), http_service->getBufferLength());
    http_service->doService();
}

void HttpService::stop() {
    ThreadPool::deleteInstance();
}

bool HttpService::receive() {
    ssize_t bytes = recv(socket_, buffer_, length_, 0);
    LOGD("Recv %s %zd", getClient().c_str(), bytes);
    if (bytes == 0) {
        close(socket_);
    }
    return bytes > 0;
}

bool HttpService::doService() {
    if (req_ && res_)
        return HttpRouter::getInstance()->doService(*req_, *res_);

    return false;
}
