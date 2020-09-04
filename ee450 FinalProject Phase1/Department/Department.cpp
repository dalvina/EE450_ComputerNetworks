#include "Department.h"

int main(int argc, const char * argv[]) {
    int departmentSockfd;
    ssize_t len;
    struct sockaddr_in departmentAddr;                      // department
    struct sockaddr_in addmissionAddr;                      // server
    char buf[BUFSIZ];                                       // data buffer
    memset(&addmissionAddr, 0, sizeof(addmissionAddr));
    memset(&departmentAddr, 0, sizeof(departmentAddr));
    
    
    /*
     * server
     */
    addmissionAddr.sin_family      = AF_INET;                     // IP
    addmissionAddr.sin_addr.s_addr = inet_addr("127.0.0.1");      // IP address
    addmissionAddr.sin_port        = htons(3300 + STUDENTID);     // server's port number
    
    
    /*
     * process marker
     */
    char id = 'A';
    
    
    // fork()
    pid_t pid;
    for (int i = 0; i < 2; i++) {
        pid = fork();
        
        if (pid < 0) {
            perror("fork error!\n");
        } else if (pid == 0) {
            id = 'B' + i;
            // sleep(2);
            break;
        }
    }
    // create department socket, the port number is dynamically distributed
    departmentSockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (departmentSockfd < 0) {
        perror("Department creates socket failed!");
        return 1;
    }
    
    // connect to the server
    if(connect(departmentSockfd, (struct sockaddr *)&addmissionAddr, sizeof(struct sockaddr)) < 0) {
        perror("Department connects to Addmission Office failed!");
        return 1;
    }
    // print prompt message
    char pname[BUFSIZ] = "Department";
    pname[strlen(pname)] = id;
    socklen_t departmentAddrLen = sizeof(departmentAddr);
    if (getsockname(departmentSockfd,
                    (struct sockaddr*)&departmentAddr,
                    &departmentAddrLen) < 0) {
        perror("get local port number error!");
    }
    printf("%s has TCP port %d and IP address %s for Phase1\n", pname,
           ntohs(departmentAddr.sin_port), inet_ntoa(departmentAddr.sin_addr));
    printf("%s is now connected to the admission office\n", pname);
    
    
    // read file
    char fname[BUFSIZ] = "./Department/department";
    fname[strlen(fname)] = id;
    strcat(fname, ".txt");
    FILE * file = NULL;
    file = fopen(fname, "r");
    if (file == NULL) {
        printf("cannot open file %s!", fname);
        return 1;
    }
    while (!feof(file)) {
        fgets(buf, BUFSIZ, file);
        len = send(departmentSockfd, buf, strlen(buf), 0);
        // len = recv(departmentSockfd, buf, BUFSIZ, 0);
        if (buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }
        printf("%s has sent %s to the admission office\n", pname, buf);
    }
    fclose(file);
    
    
    // close departmentSockfd
    close(departmentSockfd);
    
    printf("Updating the admission office is done for %s\n", pname);
    printf("End of Phase 1 for %s\n", pname);
    
    return 0;
}
