all: DB

DB: learning.o DataManagement.o techniques.o linear_models.o
	g++ learning.o DataManagement.o techniques.o linear_models.o -o DB

learning.o: Learning.cpp DataManagement.h techniques.h
	g++ -c Learning.cpp

DataManagement.o: DataManagement.cpp DataManagement.h
	g++ -c DataManagement.cpp

techniques.o: techniques.cpp techniques.h DataManagement.h linear_models.h 
	g++ -c techniques.cpp

linear_models.o: linear_models.cpp linear_models.h
	g++ -c linear_models.cpp

clean:
	rm -f *.o DB *~ #*
