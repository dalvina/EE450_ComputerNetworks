#include "Admission.h"

int main(int argc, char *argv[]) {
    int admissionSockfd;   // admission office
    int departmentSockfd;  // department
    int studentSockfd;     // student
    
    ssize_t len;
    socklen_t sin_size;
    struct sockaddr_in admissionAddr;                 // server
    struct sockaddr_in remoteAddr;                    // client
    char buf[BUFSIZ];
    memset(&admissionAddr, 0, sizeof(admissionAddr)); // data initialization
    
    admissionAddr.sin_family      = AF_INET;                     // IP
    admissionAddr.sin_addr.s_addr = inet_addr("127.0.0.1");      // server's IP
    admissionAddr.sin_port        = htons(3300 + STUDENTID);     // server's port number
    
    // create server's socket
    if ((admissionSockfd = socket(PF_INET,SOCK_STREAM,0)) < 0) {
        perror("socket error");
        return 1;
    }
    
    // bind the socket to admissionAddr
    if (bind(admissionSockfd, (struct sockaddr *)&admissionAddr, sizeof(struct sockaddr)) < 0) {
        perror("Admission office binds to network failed!");
        return 1;
    }
    
    
    // print prompt message
    socklen_t admissionAddrLen = sizeof(admissionAddr);
    if (getsockname(admissionSockfd,
                    (struct sockaddr*)&admissionAddr,
                    &admissionAddrLen) < 0) {
        perror("get local port number error!");
    }
    printf("The admission office has TCP port %d and IP address %s\n",
           ntohs(admissionAddr.sin_port), inet_ntoa(admissionAddr.sin_addr));
    
    
    // listen for the linking requests, the message list is of length of 5
    if (listen(admissionSockfd, 5) < 0) {
        perror("listen error");
        return 1;
    };
    
    sin_size = sizeof(struct sockaddr_in);
    
    /*
     * write departments' requirements
     */
    FILE * file;
    file = fopen("requirement.txt", "w");
    
    int n = 3; // 3 departments
    while (n--) {
        // wait for the requirement
        if((departmentSockfd = accept(admissionSockfd,
           (struct sockaddr *)&remoteAddr, &sin_size)) < 0) {
            perror("accept error");
            return 1;
        }
        
        
        // receive the client's information
        while ((len = recv(departmentSockfd, buf, BUFSIZ, 0)) > 0) {
            buf[len] = (char)'\n';
            fwrite(buf, sizeof(char), len, file);
        }
        fwrite("\n", 1, 1, file);
        printf("Received the program list from Department%c\n", buf[0]);
        

        // close TCP connection to department
        close(departmentSockfd);
    }
    
    fclose(file);
    printf("End of Phase 1 for the admission office\n");
    
    printf("The admission office has TCP port %d and IP address %s\n",
           ntohs(admissionAddr.sin_port), inet_ntoa(admissionAddr.sin_addr));
    
    
    // // receive students' application
    // n = 5; // 5 students
    // while (n--) {
    //     // wait for the student's application
    //     if((studentSockfd = accept(admissionSockfd, (struct sockaddr *)&remoteAddr, &sin_size)) < 0) {
    //         perror("accept error");
    //         return 1;
    //     }
        
    //     float gpa = 0;
    //     char interest1[2];
    //     char interest2[2];
    //     char interest3[2];
    //     /* 
    //      * receive the student's application and
    //      * judge if he/she can be admitted,
    //      * send back a prompt message
    //      */
    //     while ((len = recv(studentSockfd, buf, BUFSIZ, 0)) > 0) {
    //         if (buf[0] == 'G' && buf[1] == 'P' && buf[2] == 'A') {
    //             char _gpa[3];
    //             _gpa[0] = buf[4];
    //             _gpa[1] = buf[5];
    //             _gpa[2] = buf[6];
    //             gpa = atof(_gpa);
    //         }
    //         if (buf[0] == 'I' && buf[1] == 'n' && buf[8] == '1') {
    //             interest1[0] = buf[10];
    //             interest1[1] = buf[11];
    //         }
    //         if (buf[0] == 'I' && buf[1] == 'n' && buf[8] == '2') {
    //             interest2[0] = buf[10];
    //             interest2[1] = buf[11];
    //         }
    //         if (buf[0] == 'I' && buf[1] == 'n' && buf[8] == '3') {
    //             interest3[0] = buf[10];
    //             interest3[1] = buf[11];
    //         }
    //     }
        
    //     file = fopen("requirement.txt", "r");
    //     if (file == NULL) {
    //         printf("cannot open file %s!", "requirement.txt");
    //     }
    //     while (!feof(file)) {
    //         fgets(buf, BUFSIZ, file);
    //         if (interest1[0] == buf[0] && interest1[1] == buf[1]) {
    //             char _gpa[3];
    //             _gpa[0] = buf[3];
    //             _gpa[1] = buf[4];
    //             _gpa[2] = buf[5];
    //             float req_gpa = atof(_gpa);
    //             if (gpa > req_gpa) {
    //                 char dep[1];
    //                 dep[0] = interest1[0];
    //                 char * accept = nullptr;
    //                 accept = strcat(accept, "Accept#");
    //                 accept = strcat(accept, interest1);
    //                 accept = strcat(accept, "#department");
    //                 accept = strcat(accept, dep);
    //                 printf("%s", accept);
    //                 send(studentSockfd, accept, strlen(accept), 0);
    //             }
    //         }
    //     }
    //     fclose(file);
        
    //     // close TCP connection to studentDepartment
    //     close(studentSockfd);
    // }
    
    // close admissionSockfd
    close(admissionSockfd);
    
    return 0;
}

