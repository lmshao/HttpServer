//
// Created by lmshao on 2021/10/28.
//

#ifndef _HTTP_UTILS_H
#define _HTTP_UTILS_H

#include <map>
#include <string>
#include <unordered_map>

#include "master_service.h"

enum HttpMethod
{
    kGET = 0,
    kPOST,
    kPUT,
    kDELETE,
    kPATCH,
    kOTHERS
};

class HttpRequest {
  public:
    explicit HttpRequest(int socket) : socket_(socket), method_(kOTHERS), isKeepAlive_(false) {}

    explicit HttpRequest(ClientInfo *clientInfo) : socket_(clientInfo->socket), method_(kOTHERS), isKeepAlive_(false) {
        client_ = std::string(clientInfo->ip) + ':' + std::to_string(clientInfo->port);
    }

    const std::string &getPath() const {
        return path_;
    }

    bool isKeepAlive() const {
        return isKeepAlive_;
    }

    HttpMethod getMethod() const {
        return method_;
    }

    int getSocket() const {
        return socket_;
    }

    bool parseHttpData(const char *data, int length);

    const std::string &getRequestBody() const {
        return requestBody_;
    }

    const std::string &getClient() const {
        return client_;
    }

  protected:
    HttpMethod method_;
    std::string path_;
    std::unordered_map<std::string, std::string> params_;
    std::unordered_map<std::string, std::string> headers_;
    std::string requestBody_;
    bool isKeepAlive_;
    int socket_;
    std::string client_;
};

class HttpResponse {
  public:
    explicit HttpResponse(int socket = 0)
        : protocol_("HTTP/1.1"),
          status_(200),
          description_("OK"),
          content_type_("text/html"),
          isKeepAlive_(false),
          socket_(socket){};

    bool send(const std::string &content);

    std::string getResponse();

    HttpResponse &setKeepAlive(bool keepAlive) {
        isKeepAlive_ = keepAlive;
        return *this;
    }

    void setStatus(int status) {
        status_ = status;
        // auto fill description
        if (status == 400) {
            description_ = "Bad Request";
        }
    }

    void setDescription(const std::string &description) {
        description_ = description;
    }

    void setSocket(int socket) {
        socket_ = socket;
    }

  protected:
    std::string protocol_;
    int status_;
    std::string description_;
    std::string content_type_;
    std::string responseBody_;
    bool isKeepAlive_;
    int socket_;
};

#endif  // _HTTP_UTILS_H
