all: coordinate_descent

coordinate_descent: learning.o DataManagement.o
	g++ learning.o DataManagement.o -o coordinate_descent

learning.o: Learning.cpp DataManagement.h
	g++ -c Learning.cpp

DataManagement.o: DataManagement.h
	g++ -c DataManagement.cpp

clean:
	rm -f *.o coordinate_descent *~ #*
