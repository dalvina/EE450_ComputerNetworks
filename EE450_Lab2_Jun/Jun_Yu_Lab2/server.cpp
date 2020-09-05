//
//  server.cpp
//  Jun_Yu_Lab2
//
//  Created by 余珺 on 6/10/19.
//  Copyright © 2019 Jun. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include<cstdlib>
#include <iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<cstdlib>
#define MAXDATASIZE 100 // max number of bytes
#define PORT "3536"  // the port users will be connecting to
#define BACKLOG 10     // how many pending connections queue will hold

using namespace std;
void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int myPow(double x, int n) {
    if (n == 0) return 1;
    if (n == 1) return int(x);
    if (n < 0) {
        x = 1/x;
        if(n % 2 == 0){
            return myPow(x*x, -(n/2));
        }else{
            return int(x) * myPow(x*x, -((n+1)/2));
        }
    }
    if(n % 2 == 0){
        return myPow(x*x, n/2);
    }else{
        return int(x) * myPow(x*x, (n-1)/2);
    }
}

std::string toString (int randNum) {
    stringstream ss;
    ss << randNum;
    return ss.str();
}

std ::string addrtoString (string idNum) {
    srand((unsigned) time(0));
    string addrNum = "";
    int iter = 0;
    for (int i = 0; i < 24; i++) {
        int tmp = rand() % 2;
        iter += (tmp * myPow(2, (i % 8)));
        if (i % 8 == 7) {
            addrNum = toString(iter) + "." + addrNum;
            iter = 0;
        }
    }
    return idNum + addrNum;
}

std ::string idtoString () {
    srand((unsigned) time(0));
    int randNum = 0;
    for (int i = 0; i < 8; i++) {
        int tmp = rand() % 2;
        randNum += ( tmp * myPow(2, i));
    }
    return toString(randNum);
}



int main(void)
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    int numBytes;
    char buf[MAXDATASIZE];
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    
    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        
        break;
    }
    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }
    
    freeaddrinfo(servinfo); // all done with this structure
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    printf("server: waiting for connections...\n");
    
    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }
        
        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("(Server) got connection from %s\n", s);
        
        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
            
            string IPAddrNum;
            string idNum;
            //recieve the request from client and print out the message
            if ((numBytes = recv(new_fd, buf, MAXDATASIZE-1, 0))>0) {
                printf("(Server) receive transaction number： '%s'\n",buf);
                sleep(3);
            }
            
            //offer
            string recvID = string(buf);
            IPAddrNum = addrtoString(recvID);
            idNum = idtoString();
            string msg = IPAddrNum + "#" + idNum;
            strcpy(buf, msg.c_str());
            if ((numBytes = send(new_fd, buf, MAXDATASIZE-1, 0)) > 0) {
                cout << "(Server Offer Phase) offer IP address " << IPAddrNum << ", Transaction ID: " << idNum << endl;
            }
            
            //acknowledge
            idNum= idtoString();
            strcpy(buf, idNum.c_str());
            if ((numBytes = send(new_fd, buf, MAXDATASIZE-1, 0)) > 0) {
                cout << "(Server Acknowledge Phase) IP Address: " << IPAddrNum << ", Transaction ID: " << buf << endl;
            }
            close(new_fd);
            exit(0);
            
        }
        close(new_fd);  // parent doesn't need this
    }
    return 0;
    
}
    
        
