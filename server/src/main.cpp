#include <iostream>
#include "MyApi.hpp"

#include <vector>
#include "Server.hpp"

struct MyExecutor {
    void operator()(int socket) {
        std::vector<uint8_t> buffer(256);
        int n;
                 
        n = read(socket, buffer.data(), 255);
        if (n < 0) {
            throw std::runtime_error("ERROR reading from socket");
        }
        buffer.resize(n);    
        printf("Here is the message: %s\n",buffer.data());
        n = write(socket,"I got your message",18);
        if (n < 0) throw std::runtime_error("ERROR writing to socket");
        close(socket);
    }
};

int main(int argc, char** argv) {
	std::cout << "Starting server" << std::endl;
	ServerApi api;
	api.helloWorld();
    
    int i;
    MyExecutor exec;
    Server<MyExecutor, int> s(exec, i, 8080);
    s.run();
	return 0;
}

