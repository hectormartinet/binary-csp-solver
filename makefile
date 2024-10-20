CXX=g++
CXXFLAGS=-I.
CFLAGS_D=-g
CFLAGS_R=-O3

CONF := debug
NAME = run

ifeq ($(CONF),debug)
    CXXFLAGS += -g -Wall -Wextra -Wpedantic
endif
ifeq ($(CONF),release)
	CXXFLAGS += -O3
endif

SRC = main.cpp solver.cpp constraint.cpp problemreader.cpp csp.cpp

run: $(SRC)
	$(CXX) $(CXXFLAGS) -o run $(SRC)