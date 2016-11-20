#include <iostream>
#include "MyApi.hpp"

#include <vector>
#include "Server.hpp"

struct SimpleFeedbackExecutor {
    void operator()() {
        std::vector<uint8_t> buffer(256);
        int n;
                 
        n = read(socketId, buffer.data(), 255);
        if (n < 0) {
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

    SimpleFeedbackExecutor(int i) : socketId(i) {}

    ~SimpleFeedbackExecutor() {
        if (socketId > 0) {
            close(socketId);
        }
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
    SameThreadThreadPool thrPool;
    Server<SimpleFeedbackExecutorFactory, SimpleFeedbackExecutor, SameThreadThreadPool> s{factory, thrPool, 8080};
    s.run();
	return 0;
}

