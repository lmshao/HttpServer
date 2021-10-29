//
// Created by lmshao on 2021/10/26.
//

#ifndef _HTTP_ROUTER_H
#define _HTTP_ROUTER_H

#include <functional>
#include <mutex>
#include <string>

#include "http_service.h"

class HttpRouter;

typedef bool (HttpRouter::*handler_t)(HttpRequest &, HttpResponse &);

class HttpRouter {
  public:
    bool doService(HttpRequest &req, HttpResponse &res);

    const std::map<std::string, handler_t> *getHandlers() const {
        return handlers_;
    }

    static HttpRouter *getInstance();

    static void deleteInstance();

  protected:
    HttpRouter() {
        HandlerStruct hs[] = {{kGET, "/hello", &HttpRouter::f1},
                              {kPOST, "/hello", &HttpRouter::f1},
                              {kPUT, "/hello", &HttpRouter::f1},
                              {kPATCH, "/hello", &HttpRouter::f1},
                              {kDELETE, "/hello", &HttpRouter::f1}};

        // register
        for (auto &h : hs) {
            handlers_[h.method][h.router] = h.handler;
        }
    }

  private:
    bool f1(HttpRequest &req, HttpResponse &res);

    struct HandlerStruct {
        HttpMethod method;
        std::string router;
        handler_t handler;
    };

    std::map<std::string, handler_t> handlers_[kOTHERS];

    static HttpRouter *instance_;
    static std::mutex mutex_;
};

#endif  //_HTTP_ROUTER_H
