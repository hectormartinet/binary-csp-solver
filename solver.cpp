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

void Solver::stepBack() {
    for (std::pair<int, int> val : deltaDomains.back()) {
        problem.addVariableValue(val.first, val.second);
    }
    deltaDomains.pop_back();
}

bool Solver::solve() {
    nbNodesExplored++;
    if (unsetVariables.empty()) return true;
    int x = *unsetVariables.begin();
    unsetVariables.erase(x);

    for (int a : problem.getDomain(x)) {
        if (!forwardChecking(x, a)) continue;
        fixVarValue(x, a);
        if (solve()) return true;
        stepBack();
        unfixVarValue(x);
    }

    unsetVariables.emplace(x);
    return false;
}

void Solver::displaySolution() const{
    std::cout << "SOLUTION" << std::endl;
    for (auto [var,value] : setVariables) {
        std::cout << var << ":" << value << std::endl;
    }
}