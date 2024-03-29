#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <netinet/in.h>
#include <rdma/rsocket.h>


const int TIME_OUT = 1000;

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = rsocket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (rbind(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)) < 0) 
            error("ERROR on binding");
    rlisten(sockfd,5);

    // initialize the rpoll
    struct pollfd poll_tools[MAX_NUM];
    int pos = 0;
    poll_tools[pos].fd = sockfd;
    poll_tools[pos].events = POLLIN;    // maybe need to change?
    ++pos;

    int nfds = 0, n = 0;
    clilen = sizeof(cli_addr);
    
    int prev_fd = 0;
    while(true) {
        nfds = rpoll(poll_tools, pos, TIME_OUT);
        if(nfds != prev_fd) {
            std::cout << "nfd: " << nfds << std::endl;
            prev_fd = nfds;
        }
        

        if(nfds == -1) {
            std::cerr << "epoll wait error" << std::endl;
            exit(-7);
        }

        if (poll_tools[0].revents & POLLIN) {
            newsockfd = raccept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
            printf("the sockfd: %d, the newsocketfd: %d\n", sockfd, newsockfd);
            poll_tools[pos].fd = newsockfd;
            poll_tools[pos].events = POLLIN; 
            ++pos; 
        } 
        for(n = 1; n < pos; ++n) {
            if(poll_tools[n].revents & POLLIN) {
                newsockfd = poll_tools[n].fd;
                int m = rread(newsockfd,buffer,255);
                if (m < 0) error("ERROR reading from socket");
                printf("Here is the message: %s\n",buffer);

                std::string message;
                std::cout << "server want to send: ";
                std::cin >> message;
                m = rwrite(newsockfd, message.c_str() ,18);
                if (m < 0) error("ERROR writing to socket");
            }
        }
    }
   
    return 0; 
}