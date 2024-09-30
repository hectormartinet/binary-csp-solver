#include "solver.h"
#include <iostream>

Solver::Solver(CSP _problem) : problem(_problem) {
    unsetVariables = problem.getVariables();
}

bool Solver::solve() {
    nbNodesExplored++;
    if (!feasible()) return false;
    if (unsetVariables.empty()) return true;
    int var = *unsetVariables.begin();
    unsetVariables.erase(var);
    setVariables.emplace(var,0);
    for (int value : problem.getDomain(var)) {
        setVariables.at(var) = value;
        if (solve()) return true;
    }
    setVariables.erase(var);
    unsetVariables.emplace(var);
    return false;
}

void Solver::displaySolution() const{
    std::cout << "SOLUTION" << std::endl;
    for (auto [var,value] : setVariables) {
        std::cout << var << ":" << value << std::endl;
    }
}