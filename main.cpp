#include <getopt.h>
#include <signal.h>
#include <stdio.h>

#include <memory>

#include "master_service.h"
#include "utils.h"

MasterService *service;

void signalHandler(int signum) {
    LOG("Process terminating with signal (%d)\n", signum);
    service->stop();
    exit(signum);
}

void showUsage(char *cmd) {
    LOG("Usage : %s\n", cmd);
    LOG("\t-h\tlistening ip; default 0.0.0.0\n");
    LOG("\t-p\tlistening port, default 8080\n");
    LOG("\t-s\tlistening ip:port, such as -s 127.0.0.1:8080\n");
}

int main(int argc, char *argv[]) {
    LOG("Build Time: %s %s\n", __DATE__, __TIME__);
    int ret;
    std::string ip("0.0.0.0");
    int port = 8080;
    std::string ip_port;

    while ((ret = getopt(argc, argv, ":h:p:s:")) != -1) {
        switch (ret) {
            case ('h'):
                ip = std::string(optarg);
                break;
            case ('p'):
                port = ::atoi(optarg);
                break;
            case ('s'):
                ip_port = std::string(optarg);
                break;
            case ':':
                LOG("option [-%c] requires an argument\n", (char)optopt);
                showUsage(argv[0]);
                exit(1);
            case '?':
                LOG("unknown option: %c\n", (char)optopt);
                showUsage(argv[0]);
                exit(1);
            default:
                break;
        }
    }

    if (!ip_port.empty()) {
        auto colon = ip_port.find(':');
        if (colon != std::string::npos) {
            ip = ip_port.substr(0, colon);
            port = ::atoi(ip_port.substr(colon + 1).c_str());
        } else {
            showUsage(argv[0]);
            exit(1);
        }
    }

    if (port > 65535 || port <= 0) {
        showUsage(argv[0]);
        exit(1);
    }

    signal(SIGINT, signalHandler);

    std::shared_ptr<MasterService> masterService(new MasterService);
    service = masterService.get();
    masterService->setHost(ip);
    masterService->setPort(port);
    masterService->run();
    return 0;
}
