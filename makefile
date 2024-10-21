CC=g++
CFLAGS=-I. -O3

CONF := debug
NAME = run

ifeq ($(CONF),debug)
    CXXFLAGS += -g -Wall -Wextra -Wpedantic
endif
ifeq ($(CONF),release)
	CXXFLAGS += -O3
endif

SRC = main.cpp solver.cpp constraint.cpp problemreader.cpp csp.cpp instances.cpp

run: $(SRC)
	$(CXX) $(CXXFLAGS) -o run $(SRC)