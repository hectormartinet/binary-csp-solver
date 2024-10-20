CC=g++
CFLAGS=-I.

run: main.cpp solver.cpp constraint.cpp problemreader.cpp csp.cpp
	$(CC) -o run main.cpp solver.cpp constraint.cpp problemreader.cpp csp.cpp $(CFLAGS)
 