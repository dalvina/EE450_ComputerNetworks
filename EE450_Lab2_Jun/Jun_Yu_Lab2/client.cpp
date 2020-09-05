//
//  client.cpp
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

int main(int argc, char *argv[]) {
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    
    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    
    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("client: connect");
            close(sockfd);
            continue;
        }
        
        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);
    
    freeaddrinfo(servinfo); 
    
    //Discover phase
    string id;
    id = idtoString();
    strcpy(buf, id.c_str());
    if ((numbytes = send(sockfd, buf, MAXDATASIZE-1, 0)) > 0) {
        cout << "(Client Discover Phase:trying to get IP Address from server), Transaction ID: " << buf << endl;
    }
    
    //Request phase
    string IPAddrNum;
    strcpy(buf, id.c_str());
    if ((numbytes = send(sockfd, buf, MAXDATASIZE-1, 0)) > 0) {
        cout << "(Client Request Phase)offer the IP Address: " << IPAddrNum << ", transaction ID: " << buf << endl;
    }
    
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    
    buf[numbytes] = '\0';
    close(sockfd);
    
    return 0;
}
