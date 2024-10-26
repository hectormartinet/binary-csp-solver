#include <iostream>
#include <cassert>

#include "solver.h"

int main(int argc, char** argv) {
    if (argc < 7) {
        std::cout << "Usage: run filePath solveMethod variableChooser valueChooser verbosity timeLimit" << std::endl;
        exit(1);
    }
    const std::string modelPath = argv[1];
    const std::string _solveMethod = argv[2];
    const std::string _variableChooser = argv[3];
    const std::string _valueChooser = argv[4];
    const std::string _verbosity = argv[5];
    const std::string _timeLimit = argv[6];
    const std::string _randomSeed = argv[7];
    // const std::string checkFeasibility = argv[6];
    // const std::string solveInProcess = argv[7];

    const std::vector<std::string> parameters = {_solveMethod, _variableChooser, _valueChooser, _timeLimit, _randomSeed};

    CSP csp(modelPath);
    Solver solver(csp, parameters, stoi(_verbosity));

    solver.solve();
    // if (checkFeasibility == "1") solver.checkFeasibility(csp);
    // if (solveInProcess == "1") assert(solver.getNbNodesExplored() == 0); 
    return 0;
}