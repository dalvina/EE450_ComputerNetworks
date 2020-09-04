Student: Department
	g++ -g -o Student Student.cpp -lresolv;

Department: Admission
	g++ -g -o Department Department.cpp -lresolv;

Admission: clean
	g++ -g -o Admission Admission.cpp -lresolv;

clean:
	rm -f Admission
	rm -f Department
	rm -f Student
