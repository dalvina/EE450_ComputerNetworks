

1.   We have finished the code of phase 1 and 2 and run the code on the school server viterbi-scf2.usc.edu.
2.   Code files are “Department.h”, “Department.cpp”, “Admission.h”, “Admission.cpp”, “Student.h”, “Student.cpp”
     a) “Department.h”: defines the student ID and includes all needed file.
     b) “Department.cpp”: forks other department processes, reads the input file and store them in memory, sets a TCP connection to admission office and sends the information of input file to admission office.
     c) “Admission.h”: defines the student ID and includes all needed file.
     d) “Admission.cpp”: listens the port providing service and receives information from departments and students. After doing so, judge which student should be admitted and send message to correspond port.
     e) “Student.h” defines the student ID and includes all needed file.
     f) “Student.cpp”: forks other department processes, reads the input file and store them in memory, sets a TCP connection to admission office and sends the information of input file to admission office.
3.  Compile and excutation:
        Firstly, compile the “Admission” and run the “Admission” target. Then, compile the “Department” and run the “Department” target. Finally, compile the “Student” and run the “Student” target. After doing so, we can see the message printed on the terminal.
4.    We have the message exchanged following the given format.
5.    If you find that the error “Address already been used”, please wait a moment and try again. If you know how to see and kill process with command line, you can also try with it.
