#include "Student.h"

int main(int argc, const char * argv[]) {
    int studentSockfd;
    ssize_t len;
    struct sockaddr_in studentAddr;
    struct sockaddr_in addmissionAddr;
    char buf[BUFSIZ];
    memset(&addmissionAddr, 0, sizeof(addmissionAddr));
    memset(&studentAddr, 0, sizeof(studentAddr));
    
    
    /*
     * Server
     */
    addmissionAddr.sin_family      = AF_INET;
    addmissionAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addmissionAddr.sin_port        = htons(3300 + STUDENTID);
    
    
    /*
     * process marker
     */
    char id = '1';
    
    
    /*
     * fork()
     */
    pid_t pid;
    for (int i = 0; i < 4; i++) {
        pid = fork();
        
        if (pid < 0) {
            perror("fork error!\r");
        } else if (pid == 0) {
            id = '2' + i;
            break;
        }
    }
    
    
    /*
     * client
     * port number is dynamically distributed
     */
    studentSockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (studentSockfd < 0) {
        perror("Department creates socket failed!");
        return 1;
    }
    
    
    /*
     * bind the socket to the server's addr
     */
    if(connect(studentSockfd, (struct sockaddr *)&addmissionAddr, sizeof(struct sockaddr)) < 0) {
        perror("Department connects to Addmission Office failed!");
        return 1;
    }
    
    
    /*
     * print prompt message
     */
    char pname[BUFSIZ] = "Student";
    pname[strlen(pname)] = id;
    socklen_t studentAddrLen = sizeof(studentAddr);
    if (getsockname(studentSockfd,
                    (struct sockaddr*)&studentAddr,
                    &studentAddrLen) < 0) {
        perror("get local port number error!");
    }
    printf("%s has TCP port %d and IP address %s\r", pname,
           ntohs(studentAddr.sin_port), inet_ntoa(studentAddr.sin_addr));
    
    
    /*
     * read file
     */
    char fname[BUFSIZ] = "student";
    fname[strlen(fname)] = id;
    strcat(fname, ".txt");
    FILE * file;
    file = fopen(fname, "r");
    if (file == NULL) {
        printf("cannot open file %s!", fname);
    }
    while (!feof(file)) {
        fgets(buf, BUFSIZ, file);
        len = send(studentSockfd, buf, strlen(buf), 0);
    }
    len = recv(studentSockfd, buf, BUFSIZ, 0);
    printf("%s", buf);
    
    //printf("Completed sending application for %s\r", pname);
    
    fclose(file);
    
    
    /*
     * close socket
     */
    close(studentSockfd);
    
    
    // printf("End of Phase 2 for %s\r", pname);
    
    return 0;
}

