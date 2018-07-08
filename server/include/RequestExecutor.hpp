#ifndef _REQUESTEXECUTOR_HPP
#define _REQUESTEXECUTOR_HPP
#include "RequestResponse.capnp.h"
#include "Logger.hpp"
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <iostream>
#include <vector>
#include <unistd.h>
namespace requesthandler {

template<typename T1, typename T2>
T2* ptrCast(T1* ptr) {
    void* raw = static_cast<void*>(ptr);
    return static_cast<T2*>(raw);
}
        
struct RequestHandler {
    void operator()() {
        std::vector<uint64_t> buffer(2048);
        int n;
                 
        n = read(socketId, buffer.data(), 2048*sizeof(uint64_t));
        if (n < 0) {
            std::cerr << "Tried to read socket:" << socketId << " errno:" << errno << std::endl;
            throw std::runtime_error("ERROR reading from socket");
        }
        buffer.resize(n);    
        ::kj::ArrayPtr<::capnp::word> kjPtr(ptrCast<uint64_t, ::capnp::word>(buffer.data()),
                static_cast<size_t>(n) / sizeof(uint64_t) + 1);
        ::capnp::FlatArrayMessageReader msgReader(kjPtr);
        auto request = msgReader.getRoot<::webserver::Request>();
        std::cerr << "Request path: " << request.getPath().cStr() << std::endl;
        std::cerr << "Payload size: " << n << std::endl;
        std::string msg("I got your message");

        n = write(socketId, msg.c_str(), msg.length());
        if (n < 0) throw std::runtime_error("ERROR writing to socket");
        close(socketId);
        socketId = -1;
    }

    void run() {
        (*this)();
    }
    RequestHandler(int i, Logger& log) : socketId(i), logger(log) {}

    ~RequestHandler() {
        if (socketId > 0) {
            close(socketId);
        }
    }

    RequestHandler(RequestHandler&& other) : socketId(other.socketId), logger(other.logger) {
        other.socketId = -1;

    }
private:
    int socketId;
    Logger& logger;
};

struct RequestHandlerFactory {
    RequestHandlerFactory(Logger& l) : logger(l) {}

    RequestHandler buildExecuter(int i) {
        return RequestHandler(i, logger);
    }

    Logger& logger;
};
}

#endif
