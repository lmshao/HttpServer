//
// Created by lmshao on 2021/10/26.
//

#include "http_router.h"

#include <sstream>

HttpRouter *HttpRouter::instance_ = nullptr;
std::mutex HttpRouter::mutex_;

HttpRouter *HttpRouter::getInstance() {
    if (!instance_) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!instance_) {
            instance_ = new HttpRouter();
        }
    }
    return instance_;
}

void HttpRouter::deleteInstance() {
    delete instance_;
    instance_ = nullptr;
}

bool HttpRouter::f1(HttpRequest &req, HttpResponse &res) {
    std::stringstream ss;
    ss << req.getMethod() << " -- " << req.getPath() << " -- "
       << "Path Not Found";
    return res.send(ss.str());
}

bool HttpRouter::doService(HttpRequest &req, HttpResponse &res) {
    if (req.getMethod() < kGET || req.getMethod() >= kOTHERS) {
        printf("invalid type=%d", req.getMethod());
        return false;
    }

    res.setKeepAlive(req.isKeepAlive());
    bool keep = req.isKeepAlive();

    std::string path = req.getPath();
    if (path.empty()) {
        res.setStatus(400);
        return res.send("GUN");
    }

    auto it = handlers_[req.getMethod()].find(path);
    if (it != handlers_[req.getMethod()].end()) {
        handler_t handler = it->second;
        return (this->*handler)(req, res);
    }

    res.setStatus(400);
    return res.send("Method Not Found");
}
