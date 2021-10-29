//
// Created by lmshao on 2021/10/28.
//

#include "http_utils.h"

#include <sys/socket.h>

#include <sstream>

#include "utils.h"

const std::map<std::string, HttpMethod> methodMap = {
    {"GET", kGET}, {"POST", kPOST}, {"PUT", kPUT}, {"PATCH", kPATCH}, {"DELETE", kDELETE}};

bool HttpRequest::parseHttpData(const char *data, int length) {
    const char *p = data, *pre = data;

    // parse method
    while (*pre != ' ') pre++;
    std::string method(p, pre);
    if (methodMap.find(method) != methodMap.end()) {
        method_ = methodMap.at(method);
    }

    LOG("%s ", method.c_str());
    p = ++pre;

    if (p > data + length)
        return false;

    // parse Path
    while (*pre != ' ') pre++;

    path_ = std::string(p, pre);
    LOG("%s\n", path_.c_str());
    if (path_.find('?')) {
        const char *s = path_.c_str() + path_.find('?') + 1;
        const char *e;

        std::string key, value;
        for (e = s; e < path_.c_str() + path_.length(); e++) {
            if (*e == '=') {
                key = std::string(s, e);
                s = e + 1;
            } else if (*e == '&') {
                value = std::string(s, e);
                params_.insert({key, value});
                s = e + 1;
                key.clear();
                value.clear();
            }
        }

        if (!key.empty()) {
            value = std::string(s, e);
            params_.insert({key, value});
        }

        path_ = path_.substr(0, path_.find('?'));
    }

    p = ++pre;
    if (p > data + length)
        return false;

    // parse HTTP protocol
    while (*pre != '\r' && *(pre + 1) != '\n') pre++;

    std::string protocol(p, pre);  // HTTP/1.1

    pre = pre + 2;
    p = pre;

    // parse headers & body
    while (p < data + length) {
        const char *colon = nullptr;
        while (*pre != '\r' && *(pre + 1) != '\n') {
            if (*pre == ':')
                colon = pre;
            pre++;
        }

        if (colon > p && colon < pre) {
            std::string key = std::string(p, colon);
            for (char &i : key) {
                i = ::tolower(i);
            }

            headers_.insert({key, std::string(colon + 1, pre)});
        }

        pre = pre + 2;
        p = pre;
        if (*p == '\r' && *(p + 1) == '\n') {
            requestBody_ = std::string(p + 2, data + length - p - 2);
            break;
        }
    }

    auto it = headers_.find("Connection");
    if (it != headers_.end()) {
        if (it->second == "keep-alive")
            isKeepAlive_ = true;
    }

    return true;
}

bool HttpResponse::send(const std::string &content) {
    responseBody_ = content;
    std::string response = getResponse();

    ssize_t bytes = ::send(socket_, response.c_str(), response.length(), 0);
    if (bytes != response.length()) {
        printf("send error %zd/%lu\n", bytes, response.length());
        return false;
    }

    return true;
}

std::string HttpResponse::getResponse() {
    static char buff[30] = {0};
    time_t t = time(nullptr);
    strftime(buff, sizeof(buff), "%a, %d %b %Y %H:%M:%S GMT\n", gmtime(&t));

    std::stringstream ss;
    ss << protocol_ << " " << status_ << " " << description_ << "\r\n";
    ss << "Date: " << std::string(buff) << "\r\n";
    ss << "Content-Length: " << responseBody_.length() << "\r\n";
    ss << "Content-Type: " << content_type_ << "\r\n";
    if (isKeepAlive_)
        ss << "Connection: keep-alive"
           << "\r\n";
    ss << "Server: my-http-server\r\n";
    ss << "\r\n" << responseBody_;

    return ss.str();
}
