CC=g++
CFLAGS=-I. -O3

run: main.cpp solver.cpp constraint.cpp problemreader.cpp csp.cpp instances.cpp
	$(CC) -o run main.cpp solver.cpp constraint.cpp problemreader.cpp csp.cpp instances.cpp $(CFLAGS)
 