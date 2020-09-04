
## Introduction

I has finished the code of phase 1 and test the code on viterbi-scf2. In this project, I write codes for Department, Admission and Student, which is implemented in C++. Here are the main contents
- socket
- file operation
- string parsing

Code files are "Department.h", "Department.cpp", "Admission.h", "Admission.cpp", "Student.h", "Student.cpp"
- "Department.h": defines the student ID and includes all needed file
- "Department.cpp": forks other department processes, reads the input file and store them in memory, sets a TCP connection to admission office and sends the information of input file to admission office
- "Admission.h": defines the student ID and includes all needed file
- "Admission.cpp": listens the port providing service and receives information from departments
- "Student.h" defines the student ID and includes all needed file
- "Student.cpp": forks other department processes, reads the input file and store them in memory, sets a TCP connection to admission office and sends the information of input file to admission office

## How To Use?

1. Change current directory to the root of this project.
1. Add `.txt` files to each department's folder. For example, place `departmentA.txt` to `Department/`.
1. Compile the "Admission" and use `./Admission/Admission` to run the "Admission" target.
1. Then, compile the "Department" and use `./Department/Department` to run the "Department" target.
1. After doing so, we can see the message printed on the terminal.

## Questions

1. We have the message exchanged following the given format.
1. If the last time the "Admission" process didn’t terminate normally, we may couldn’t run "Admission" process correctly this time.
1. No Reused Code.

## Tips

- You can use `make Student` command to compile the source files for all of the department.
