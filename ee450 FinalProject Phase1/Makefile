Student: Department
	cd Student; \
	g++ -o Student Student.cpp -lnsl -lresolv;\
	cd ..

Department: Admission
	cd Department; \
	g++ -o Department Department.cpp -lnsl -lresolv; \
	cd ..

Admission: clean
	cd Admission; \
	g++ -o Admission Admission.cpp -lnsl -lresolv; \
	cd ..

clean:
	rm -f Admission/Admission
	rm -f Department/Department
	rm -f Student/Student
