#include "Admission.h"

int main(int argc, char *argv[]) {
    // phase 1
    int admissionSockfd;   // admission office
    int departmentSockfd;  // department
    int studentSockfd;     // student
    
    ssize_t len;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    struct sockaddr_in admissionAddr;                 // server
    struct sockaddr_in remoteAddr;                    // client
    char buf[BUFSIZ];
    memset(&admissionAddr, 0, sizeof(admissionAddr)); // data initialization
    
    // set the properties of struct admisssionAddr
    admissionAddr.sin_family      = AF_INET;                     // use IP
    admissionAddr.sin_addr.s_addr = inet_addr("127.0.0.1");      // server's IP
    admissionAddr.sin_port        = htons(3300 + STUDENTID);     // server's port number
    
    // create server's socket
    if ((admissionSockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        return 1;
    }

    // bind the socket to admissionAddr
    if (bind(admissionSockfd, (struct sockaddr *)&admissionAddr, sizeof(struct sockaddr)) < 0) {
        perror("Admission socket binds to address failed!");
        return 1;
    }

    // print prompt message 1
    socklen_t admissionAddrLen = sizeof(admissionAddr);
    printf("The admission office has TCP port %d and IP address %s\n",
           ntohs(admissionAddr.sin_port), inet_ntoa(admissionAddr.sin_addr));
    
    // listen for the connecting requests, the message list is of length of 5
    if (listen(admissionSockfd, 5) < 0) {
        perror("listen error");
        return 1;
    };
    
    // 3 departments, record their required gpa
    int n = 3;
    float departments[3][3] = { -1 };
    while (n--) {
        // wait for the requirement
        if ((departmentSockfd = accept(admissionSockfd,
            (struct sockaddr *)&remoteAddr, &sin_size)) < 0) {
            perror("accept error");
            return 1;
        }
        
        // receive the departments's information
        while ((len = recv(departmentSockfd, buf, BUFSIZ, 0)) > 0) {
            char _gpa[BUFSIZ];
            strcpy(_gpa, buf + 3);
            float gpa = atof(_gpa);
            departments[buf[0] - 'A'][buf[1] - '1'] = gpa;
        }

        // print prompt message 2
        printf("Received the program list from Department%c\n", buf[0]);
        
        // close TCP connection to department
        close(departmentSockfd);
    }
    
    // print prompt message 3
    printf("End of Phase 1 for the admission office\n");
    
    // print prompt message 4
    printf("The admission office has TCP port %d and IP address %s\n",
           ntohs(admissionAddr.sin_port), inet_ntoa(admissionAddr.sin_addr));
    
    // receive 5 students' application
    n = 5;
    int  index;
    bool found = false;
    float studentGPA[5]      = { -1 };
    int   interests[5][3][2] = { -1 };
    while (n--) {
        // wait for the student's application
        if((studentSockfd = accept(admissionSockfd, (struct sockaddr *)&remoteAddr, &sin_size)) < 0) {
            perror("accept error");
            return 1;
        }

        /* 
         * receive the student's application and
         * judge if he/she can be admitted,
         * send back a result
         */
        index = 0;
        int currentStudentNumber = 0;
        while ((len = recv(studentSockfd, buf, BUFSIZ, 0)) > 0) {
            if (buf[0] == '\n') {
                break;
            }
            if (buf[0] == 's' && buf[1] == 't') {
                currentStudentNumber = buf[7] - '1';
            }
            if (buf[0] == 'G' && buf[1] == 'P' && buf[2] == 'A') {
                char _gpa[BUFSIZ];
                strcpy(_gpa, buf + 4);
                float gpa = atof(_gpa);
                studentGPA[currentStudentNumber] = gpa;
            }
            if (buf[0] == 'I' && buf[1] == 'n') {
                int t1 = buf[10] - 'A';
                int t2 = buf[11] - '1';
                interests[currentStudentNumber][index][0] = t1;
                interests[currentStudentNumber][index][1] = t2;
                found = departments[t1][t2] == -1 ? false : true;
            }
            memset(buf, 0, BUFSIZ);
        }

        // print prompt message 5
        printf("Admission office receive the application from Student%d\n", currentStudentNumber + 1);

        // send message back to student
        if (found) {
            strcpy(buf, "valid");
        } else {
            strcpy(buf, "0");
        }
        int flags = fcntl(studentSockfd, F_GETFL, 0);
        fcntl(studentSockfd, F_SETFL, flags & ~O_NONBLOCK);
        send(studentSockfd, buf, strlen(buf) + 1, 0);
        // close TCP connection to student
        close(studentSockfd);
    }
    // close admissionSockfd
    close(admissionSockfd);

    // phase 2 act as a client
    // create a UDP socket
    int admissionUDPSockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (admissionSockfd < 0) {
        perror("Admission creaets UDP socket fail");
        return 1;
    }

    // set properties of struct studentUDPAddr, departmentUDPAddr
    struct sockaddr_in admissionUDPAddr, studentUDPAddr, departmentUDPAddr;
    studentUDPAddr.sin_family         = PF_INET;
    studentUDPAddr.sin_addr.s_addr    = INADDR_ANY;
    departmentUDPAddr.sin_family      = PF_INET;
    departmentUDPAddr.sin_addr.s_addr = INADDR_ANY;
    socklen_t admissionUDPAddrLen = sizeof(admissionUDPAddr);

    // check if the application meets the requirement
    bool matched = false;
    index        = 0;
    n            = 0;
    bool isPrinted = false;
    for (int n = 0; n < 5; n++) {
        matched = false;
        index = 0;
        // match the corresponding program and judge if the student meets the requirement
        while (!matched && index < 3 && interests[n][index][0] != -1) {
            int t1 = interests[n][index][0];
            int t2 = interests[n][index][1];
            if (t1 >= 0 && t1 < 3 && t2 >= 0 && t2 < 3 &&
                departments[t1][t2] != -1 && studentGPA[n] >= departments[t1][t2]) {
                matched = true;
                break;
            }
            index++;
        }

        // send message to student
        studentUDPAddr.sin_port = htons(21400 + n * 100 + STUDENTID);
        char t1[2];
        if (!matched) {
            char rejectbuf[] = "Reject";
            sendto(admissionUDPSockfd, rejectbuf, strlen(rejectbuf), 0,
                  (struct sockaddr*)&studentUDPAddr, sizeof(studentUDPAddr));
        } else {
            t1[0] = interests[n][index][0] + 'A';
            t1[1] = '\0';
            char t2[2]; t2[0] = interests[n][index][1] + '1';
            t2[1] = '\0';
            char acceptbuf[BUFSIZ];
            strcat(acceptbuf, "Accept#");
            strcat(acceptbuf, t1);
            strcat(acceptbuf, t2);
            strcat(acceptbuf, "#department");
            strcat(acceptbuf, t1);
            sendto(admissionUDPSockfd, acceptbuf, strlen(acceptbuf), 0,
                  (struct sockaddr*)&studentUDPAddr, sizeof(studentUDPAddr));
            
            // send message to the corresponding department
            departmentUDPAddr.sin_port = htons(21100 + (t1[0] - 'A') * 100 + STUDENTID);
            char sendbuf[BUFSIZ] = {'\0'};
            char t3[2] = {'\0'}; t3[0] = n + '1';
            t3[1] = '\0';
            std::stringstream ss;
            ss << studentGPA[n];
            std::string str = ss.str();
            char _gpa[BUFSIZ] = {'\0'};
            strcat(_gpa, str.c_str());
            strcat(sendbuf, "Student");
            strcat(sendbuf, t3);
            strcat(sendbuf, "#");
            strcat(sendbuf, _gpa);
            strcat(sendbuf, "#");
            strcat(sendbuf, t1);
            strcat(sendbuf, t2);
            sendto(admissionUDPSockfd, sendbuf, strlen(sendbuf), 0,
                  (struct sockaddr*)&departmentUDPAddr, sizeof(departmentUDPAddr));
            memset(sendbuf, 0, BUFSIZ);
        }

        if (getsockname(admissionUDPSockfd, (struct sockaddr*)&admissionUDPAddr, &admissionUDPAddrLen) < 0) {
            perror("admission office get UDP socket port number fail");
            return 1;
        }
        // print prompt message 6
        if (!isPrinted) {
            printf("The admission office has UDP port %d and IP address %s for Phase 2\n",
            ntohs(admissionUDPAddr.sin_port), inet_ntoa(admissionUDPAddr.sin_addr));
            isPrinted = true;
        }

        if (matched) {
            // print prompt message 7
            printf("The admission office has sent the application result to Student%d\n", n + 1);
            // print prompt message 8
            printf("The admission office has sent one admitted student to Department%c\n", t1[0]);
        }
    }

    for (int i = 0; i < 3; i++) {
        departmentUDPAddr.sin_port = htons(21100 + i * 100 + STUDENTID);
        sendto(admissionUDPSockfd, "\n", strlen("\n"), 0,
                (struct sockaddr*)&departmentUDPAddr, sizeof(departmentUDPAddr));
    }
    // print prompt message 9
    printf("End of Phase 2 for the admission office\n");
    
    return 0;
}
