#include <iostream>
#include <cassert>

#include "solver.h"

int main(int argc, char** argv) {
    if (argc < 11) {
        std::cout << "Usage: run filePath solveMethod variableChooser valueChooser verbosity timeLimit" << std::endl;
        exit(1);
    }
    const std::string _modelPath = argv[1];
    const std::string _solveMethod = argv[2];
    const std::string _variableChooser = argv[3];
    const std::string _valueChooser = argv[4];
    const std::string _verbosity = argv[5];
    const std::string _timeLimit = argv[6];
    const std::string _randomSeed = argv[7];
    const std::string _nbSolutions = argv[8];
    const std::string _checkIfFoundSolution = argv[9];
    const std::string _checkSolveAtRoot = argv[10];

    const std::vector<std::string> parameters = {_solveMethod, _variableChooser, _valueChooser, _timeLimit, _randomSeed, _nbSolutions};

    CSP csp(_modelPath);
    Solver solver(csp, parameters, stoi(_verbosity));

    solver.solve();
    
    solver.checkFeasibility(csp);
    if (solver.hasFoundSolution()) solver.checkFeasibility(csp);
    if (stoi(_checkIfFoundSolution)) assert(solver.hasFoundSolution());
    if (stoi(_checkSolveAtRoot)) assert(solver.getNbNodesExplored() == 0);
    // if (solver.hasFoundSolution()) solver.displaySolution();
    return 0;
}