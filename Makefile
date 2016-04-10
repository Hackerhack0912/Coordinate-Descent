all: DB

DB: Learning.o DataManagement.o techniques.o
	g++ Learning.o DataManagement.o techniques.o -o DB

Learning.o: Learning.cpp DataManagement.h techniques.h
	g++ -c Learning.cpp

DataManagement.o: DataManagement.cpp DataManagement.h
	g++ -c DataManagement.cpp

techniques.o: techniques.cpp techniques.h DataManagement.h linear_models.h 
	g++ -c techniques.cpp

clean:
	rm -f *.o DB *~ #*
