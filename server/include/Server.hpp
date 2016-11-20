#ifndef SERVER_HPP
#define SERVER_HPP

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <stdexcept>

template <typename ExecutorFactory, typename Executer, typename ThrPool >
class Server {
public:
    Server(ExecutorFactory& t, ThrPool& thP, uint16_t portNum) 
        : executorFactory(t), threadPool(thP), portNumber(portNum)  {}

    void run() {
        int sockfd, newsockfd;
        struct sockaddr_in serv_addr, cli_addr;
        socklen_t clilen;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
              throw std::runtime_error("ERROR opening socket");
        }
        ::bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portNumber);
        if (bind(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)) < 0) {
            throw std::runtime_error("ERROR on binding");
        }
        listen(sockfd,5);
        clilen = sizeof(cli_addr);
        while (1) {
            newsockfd = accept(sockfd, 
                           (struct sockaddr *) &cli_addr, &clilen);
            if (newsockfd < 0) {
                throw std::runtime_error("ERROR on accept");
            }
            
            Executer executor = executorFactory.buildExecuter(newsockfd);
            threadPool.add(std::move(executor), newsockfd);


            //executor(newsockfd);
        }
    }

private:
    ExecutorFactory& executorFactory;
    ThrPool& threadPool;
    uint16_t portNumber;
};


#endif
