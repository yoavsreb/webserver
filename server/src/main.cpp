#include "Server.hpp"
#include "Logger.hpp"
#include "ThreadPool.hpp"
#include "RequestExecutor.hpp"

#include <cstdlib>
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

struct ServerOptions {
    std::string logfile;
};

po::options_description createOptions() {
    po::options_description desc{"Allowed Options"};
    desc.add_options()
    ("help", "produce help message")
    ("logfile", po::value<std::string>(), "Path to logfile")
        ;
    return desc;
}

ServerOptions loadOptions(int argc, char** argv, po::options_description&& desc) {
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help") || argc == 1) {
        std::cout << desc;
        exit(0);
    } 
    return ServerOptions{vm["logfile"].as<std::string>()};
}




int main(int argc, char** argv) {
    auto serverOptions = loadOptions(argc, argv, createOptions()); 
    Logger logger{serverOptions.logfile};
    requesthandler::RequestHandlerFactory factory{logger};
     
    ThreadPool thrPool;
    info(logger, "About to start server at localhost:8080");
    Server<requesthandler::RequestHandlerFactory, requesthandler::RequestHandler, ThreadPool> s{factory, thrPool, 8080};
    s.run();
	return 0;
}

/**
 * You can create a simple feedback server using :
 *
 *
#include <errno.h>
#include <iostream>

...

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
    SameThreadThreadPool thrPool;
    Server<SimpleFeedbackExecutorFactory, SimpleFeedbackExecutor, SameThreadThreadPool> s{factory, thrPool, 8080};
 */
