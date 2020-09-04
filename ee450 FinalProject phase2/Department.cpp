#include "Department.h"

int main(int argc, const char * argv[]) {
    
    ssize_t len;
    struct sockaddr_in departmentAddr;                      // department
    struct sockaddr_in addmissionAddr;                      // server
    char buf[BUFSIZ];                                       // data buffer
    memset(&addmissionAddr, 0, sizeof(addmissionAddr));
    memset(&departmentAddr, 0, sizeof(departmentAddr));

    // set the addr reusable
    // int on = 1;
    // setsockopt(departmentSockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    
    // server
    addmissionAddr.sin_family      = AF_INET;                     // IP
    addmissionAddr.sin_addr.s_addr = inet_addr("127.0.0.1");      // IP address
    addmissionAddr.sin_port        = htons(3300 + STUDENTID);     // server's port number
    
    // process marker
    char id = 'A';
    
    // fork
    pid_t pid;
    for (int i = 0; i < 2; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork error!\n");
        } else if (pid == 0) {
            id = 'B' + i;
            break;
        }
    }
    // create department socket, the port number is dynamically distributed
    int departmentSockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (departmentSockfd < 0) {
        perror("Department creates socket failed\n");
        return 1;
    }
    // connect to the server
    if(connect(departmentSockfd, (struct sockaddr *)&addmissionAddr, sizeof(struct sockaddr)) < 0) {
        perror("Department connects to Addmission Office failed\n");
        return 1;
    }
    
    // print prompt message 1
    char pname[BUFSIZ] = "Department";
    pname[strlen(pname)] = id;
    socklen_t departmentAddrLen = sizeof(departmentAddr);
    if (getsockname(departmentSockfd,
                    (struct sockaddr*)&departmentAddr,
                    &departmentAddrLen) < 0) {
        perror("get local port number error\n");
    }
    printf("%s has TCP port %d and IP address %s for Phase1\n", pname,
           ntohs(departmentAddr.sin_port), inet_ntoa(departmentAddr.sin_addr));

    // print prompt message 2
    printf("%s is now connected to the admission office\n", pname);
    
    // read file
    char fname[BUFSIZ] = "department";
    fname[strlen(fname)] = id;
    strcat(fname, ".txt");
    FILE * file;
    file = fopen(fname, "r");
    if (file == NULL) {
        printf("cannot open file %s!", fname);
    }
    while (!feof(file)) {
        fgets(buf, BUFSIZ, file);
        len = send(departmentSockfd, buf, strlen(buf), 0);  
        // len = recv(departmentSockfd, buf, BUFSIZ, 0);
        if (buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }

        // print prompt message 3
        char program[3];
        program[0] = buf[0];
        program[1] = buf[1];
        program[2] = '\0';
        printf("%s has sent %s to the admission office\n", pname, program);
    }
    fclose(file);
    
    // close departmentSockfd
    close(departmentSockfd);
    
    // print prompt message 4
    printf("Updating the admission office is done for %s\n", pname);

    // print prompt message 5
    printf("End of Phase 1 for %s\n", pname);

    // phase 2 act as a server
    // create a UDP socket
    int departmentUDPSockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (departmentSockfd < 0) {
        perror("department creates UDP socket fail\n");
        return 1;
    }

    // set the addr reusable
    // on = 1;
    // setsockopt(departmentUDPSockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    // sleep(30);
    // set properties of struct departmentUDPAddr
    struct sockaddr_in departmentUDPAddr, addmissionUDPAddr;
    departmentUDPAddr.sin_family = PF_INET;
    departmentUDPAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    departmentUDPAddr.sin_port = htons(21100 + (id - 'A') * 100 + STUDENTID);

    // bind the UDP socket to the struct departmentUDPAddr
    socklen_t departmentUDPAddrLen = sizeof(departmentUDPAddr);
    socklen_t addmissionUDPAddrLen = sizeof(addmissionUDPAddr);
    if (bind(departmentUDPSockfd, (struct sockaddr*)&departmentUDPAddr, departmentAddrLen) < 0) {
        perror("department socket binds to address failed\n");
        return 1;
    }

    // print prompt message 6
    printf("The admission office has UDP port %d and IP address %s for Phase 2\n",
           ntohs(departmentUDPAddr.sin_port), inet_ntoa(departmentUDPAddr.sin_addr));

    // receive message from admission office
    char recvbuf[BUFSIZ];
    while (recvfrom(departmentUDPSockfd, recvbuf, BUFSIZ, 0,
                         (struct sockaddr*)&addmissionUDPAddr, &addmissionUDPAddrLen) > 0) {
        if (recvbuf[0] == '\n') {
            break;
        }
        // print prompt message 7
        printf("Student%c has been admitted to %s\n", recvbuf[7], pname);
    }

    // print prompt message 8
    printf("End of Phase 2 for %s\n", pname);

    return 0;
}
