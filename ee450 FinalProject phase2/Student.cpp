#include "Student.h"

int main(int argc, const char * argv[]) {
    // phase 1
    
    ssize_t len;
    struct sockaddr_in studentAddr;
    struct sockaddr_in addmissionAddr;
    char buf[BUFSIZ];
    memset(&addmissionAddr, 0, sizeof(addmissionAddr));
    memset(&studentAddr, 0, sizeof(studentAddr));

    // set the addr reusable
    // int on = 1;
    // setsockopt(studentSockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    
    // Server
    addmissionAddr.sin_family      = AF_INET;
    addmissionAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addmissionAddr.sin_port        = htons(3300 + STUDENTID);
    
    // process marker
    char id = '1';
    
    // fork
    pid_t pid;
    for (int i = 0; i < 4; i++) {
        // sleep(1);
        pid = fork();
        if (pid < 0) {
            perror("fork error!\n");
        } else if (pid == 0) {
            id = '2' + i;
            break;
        }
    }

    // set the process name
    char pname[BUFSIZ] = "Student";
    pname[strlen(pname)] = id;
    /**********send message to admission**********/
    // client's port number is dynamically distributed
    int studentSockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (studentSockfd < 0) {
        perror("Student creates socket failed!");
        return 1;
    }
    // bind the socket to the server's addr
    if(connect(studentSockfd, (struct sockaddr *)&addmissionAddr, sizeof(struct sockaddr)) < 0) {
        perror("Student connects to Addmission Office failed!");
        return 1;
    }
    
    // print prompt message 1
    socklen_t studentAddrLen = sizeof(studentAddr);
    if (getsockname(studentSockfd,
        (struct sockaddr*)&studentAddr, &studentAddrLen) < 0) {
        perror("get local port number error!");
    }
    printf("%s has TCP port %d and IP address %s\n", pname,
           ntohs(studentAddr.sin_port), inet_ntoa(studentAddr.sin_addr));
    
    // read file
    char fname[BUFSIZ] = "student";
    fname[strlen(fname)] = id;
    strcat(fname, ".txt");
    FILE * file;
    file = fopen(fname, "r");
    if (file == NULL) {
        printf("cannot open file %s!", fname);
    }
    strcpy(buf, fname);

    // int flags = fcntl(studentSockfd, F_GETFL, 0);
    // fcntl(studentSockfd, F_SETFL, flags & ~O_NONBLOCK);
    len = send(studentSockfd, buf, strlen(buf), 0);
    while (!feof(file)) {
        fgets(buf, BUFSIZ, file);
        len = send(studentSockfd, buf, strlen(buf), 0);
        printf("");
    }
    memset(buf, 0, BUFSIZ);
    fclose(file);
    len = send(studentSockfd, "\n", strlen("\n"), 0);

    // print prompt message 2
    printf("Completed sending application for %s\n", pname);

    // print prompt message 3
    len = recv(studentSockfd, buf, BUFSIZ, 0);
    printf("%s has received the reply from the admission office\n", pname);
    // close socket
    close(studentSockfd);
    

    /***************listen to udp port first***************/
    // phase 2 act as a UDP server
    // create a UDP socket
    int studentUDPSockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (studentUDPSockfd < 0) {
        perror("Student creates UDP socket failed!");
        return 1;
    }
    // set properties of struct studentUDPAddr
    struct sockaddr_in addmissionAddrUDP, studentUDPAddr;
    studentUDPAddr.sin_family = PF_INET;
    studentUDPAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    studentUDPAddr.sin_port = htons(21400 + (id - '1') * 100 + STUDENTID);

    // bind the UDP socket to the addr struct
    socklen_t admissionAddrLen = sizeof(addmissionAddrUDP);
    if (bind(studentUDPSockfd, (const struct sockaddr*)&studentUDPAddr, admissionAddrLen) < 0) {
        perror("Student socket binds to address failed!");
        return 1;
    }

    // print the prompt message 4
    printf("%s has UDP port %d and IP address %s for phase 2\n", pname,
            ntohs(studentUDPAddr.sin_port), inet_ntoa(studentUDPAddr.sin_addr));
    // receive result from admission office
    char recvbuf[BUFSIZ];
    if (buf[0] != '0') {
        recvfrom(studentUDPSockfd, recvbuf, BUFSIZ, 0, (struct sockaddr*)&addmissionAddrUDP, &admissionAddrLen);
        // print the promt message 5
        printf("%s has received the application result\n", pname);
    }
    // print the promt message 6
    printf("End of Phase 2 for %s\n", pname);
    return 0;
}
