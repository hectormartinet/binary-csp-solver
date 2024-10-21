#include <iostream>
#include <cassert>

#include "solver.h"

int main(int argc, char** argv) {
    srand((unsigned int)(time(NULL)));
    if (argc < 6) {
        std::cout << "Usage: run filePath solveMethod variableChooser valueChooser verbosity" << std::endl;
        exit(1);
    }
    const std::string modelPath = argv[1];
    const std::string _solveMethod = argv[2];
    const std::string _variableChooser = argv[3];
    const std::string _valueChooser = argv[4];
    const std::string verbosity = argv[5];

    const std::vector<std::string> parameters = {_solveMethod, _variableChooser, _valueChooser};

    CSP csp(modelPath);
    Solver solver(csp, parameters, stoi(verbosity));

    solver.solve();
    solver.checkFeasibility(csp);

    return 0;
}