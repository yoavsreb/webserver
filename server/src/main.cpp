#include <iostream>
#include "MyApi.hpp"
#include <errno.h>
#include <vector>
#include "Server.hpp"

#define SERVER_MT
#ifdef SERVER_MT
#include "ThreadPool.hpp"
#endif

struct SimpleFeedbackExecutor {
    void operator()() {
        std::vector<uint8_t> buffer(256);
        int n;
                 
        n = read(socketId, buffer.data(), 255);
        if (n < 0) {
            std::cout << "Tried to read socket:" << socketId << " errno:" << errno << std::endl;
            throw std::runtime_error("ERROR reading from socket");
        }
        buffer.resize(n);    
        printf("Here is the message: %s\n",buffer.data());
        std::string msg("I got your message");
        n = write(socketId, msg.c_str(), msg.length());
        if (n < 0) throw std::runtime_error("ERROR writing to socket");
        close(socketId);
        socketId = -1;
    }

    void run() {
        (*this)();
    }
    SimpleFeedbackExecutor(int i) : socketId(i) {}

    ~SimpleFeedbackExecutor() {
        if (socketId > 0) {
            close(socketId);
        }
    }

    SimpleFeedbackExecutor(SimpleFeedbackExecutor&& other) : socketId(other.socketId) {
        other.socketId = -1;
    }
private:
    int socketId;
};


struct SimpleFeedbackExecutorFactory {
    SimpleFeedbackExecutor buildExecuter(int i) {
        return SimpleFeedbackExecutor{i};
    }
};

struct SameThreadThreadPool {
    template<typename Functor>
    void add( Functor&& functor, int taskId) {
        std::cout << "MyThreadPool::add #" << taskId << std::endl;
        functor();
    }
};

int main(int argc, char** argv) {
	std::cout << "Starting server" << std::endl;
	ServerApi api;
	api.helloWorld();
    
    int i;
    SimpleFeedbackExecutorFactory factory;
#ifndef SERVER_MT
    SameThreadThreadPool thrPool;
    Server<SimpleFeedbackExecutorFactory, SimpleFeedbackExecutor, SameThreadThreadPool> s{factory, thrPool, 8080};
#else
    ThreadPool thrPool;
    Server<SimpleFeedbackExecutorFactory, SimpleFeedbackExecutor, ThreadPool> s{factory, thrPool, 8080};
#endif
    s.run();
	return 0;
}

