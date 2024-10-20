#include <iostream>
#include <cassert>

#include "solver.h"

void test1(int a) {
    while (true) {
        std::cout << a << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: run filePath solveMethod valueChooser variableChooser [verbosity]" << std::endl;
        exit(1);
    }
    const char* modelPath = argv[1];
    const std::string _solveMethod = argv[2];
    const char* verbosity = argv[3];
    // const char* time = argv[2];
    // std::string valueChooser = argv[3];
    // std::string variableChooser = argv[4];
    // const char* verbosity = argc > 5 ? argv[(5)] : "1";
    SolveMethod solveMethod;
    if (_solveMethod == "AC4") solveMethod = SolveMethod::AC4;
    else if (_solveMethod == "FC") solveMethod = SolveMethod::ForwardChecking;
    else if (_solveMethod == "LP") solveMethod = SolveMethod::LazyPropagate;
    else throw std::logic_error("Wrong solve method");


    CSP csp(modelPath);
    Solver solver(csp, solveMethod, atoi(verbosity));

    solver.solve();

    if (solver.hasFoundSolution()) {
        auto sol = solver.retrieveSolution();
        assert(csp.feasible(sol));
    }


    return 0;
}