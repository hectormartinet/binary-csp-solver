#include "solver.h"
#include <iostream>

Solver::Solver(CSP _problem) : problem(_problem) {
    unsetVariables = problem.getVariables();
    constraints = problem.getConstraints();
}

bool Solver::forwardChecking(int x, int a) {
    std::vector<std::pair<int,int>> modifs;
    for (const auto& [y, ijConstraint] : constraints.at(x)) {
        if (!setVariables.count(y)) {
            unsigned int nbInfeasInDomain = 0;
            for (int b : problem.getDomain(y)) {
                if (!ijConstraint.feasible(a,b)) {
                    nbInfeasInDomain++;
                    modifs.push_back(std::make_pair(y, b));
                }
            }
            if (nbInfeasInDomain == problem.sizeDomain(y)) return false;
        }
    }
    for (std::pair<int,int> couple : modifs) {
        problem.removeVariableValue(couple.first, couple.second);
    }
    deltaDomains.push_back(modifs);
    return true;
}

void Solver::backOldDomains() {
    for (std::pair<int, int> couple : deltaDomains.back()) {
        problem.addVariableValue(couple.first, couple.second);
    }
    deltaDomains.pop_back();
}

bool Solver::solve() {
    nbNodesExplored++;
    if (unsetVariables.empty()) return true;
    int var = *unsetVariables.begin();
    unsetVariables.erase(var);

    for (int value : problem.getDomain(var)) {
        if (!forwardChecking(var, value)) continue;
        fixVarValue(var, value);
        if (solve()) return true;
        backOldDomains();
        unfixVarValue(var);
    }

    unsetVariables.emplace(var);
    return false;
}

void Solver::displaySolution() const{
    std::cout << "SOLUTION" << std::endl;
    for (auto [var,value] : setVariables) {
        std::cout << var << ":" << value << std::endl;
    }
}