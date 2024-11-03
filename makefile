CC=g++
CFLAGS=-I. -Ofast

CONF := debug
NAME = run

ifeq ($(CONF),debug)
    CXXFLAGS += -g -Wall -Wextra -Wpedantic
endif
ifeq ($(CONF),release)
	CXXFLAGS += -Ofast -DNDEBUG
endif

SRC = main.cpp solver.cpp constraint.cpp problemreader.cpp csp.cpp instances.cpp alldifferentfamily.cpp

run: $(SRC)
	$(CXX) $(CXXFLAGS) -o run $(SRC)