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
    const char* verbosity = argv[2];
    // const char* time = argv[2];
    // std::string solveMethod = argv[2];
    // std::string valueChooser = argv[3];
    // std::string variableChooser = argv[4];
    // const char* verbosity = argc > 5 ? argv[(5)] : "1";
    

    CSP csp(modelPath);
    Solver solver(csp, atoi(verbosity));

    solver.solve();

    if (solver.hasFoundSolution()) {
        auto sol = solver.retrieveSolution();
        assert(csp.feasible(sol));
    }


    return 0;
}