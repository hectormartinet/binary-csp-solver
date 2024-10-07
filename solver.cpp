#include "solver.h"
#include <iostream>

Solver::Solver(CSP _problem) : problem(_problem) {
    unsetVariables = problem.getVariables();
    varChooser = std::make_unique<SmallestDomainVariableChooser>();
    valueChooser = std::make_unique<CopyValueChooser>();
}

bool Solver::forwardChecking(int x, int a) {
    std::vector<std::pair<int,int>> modifs;
    for (const auto& [y, Cxy] : problem.getConstraints().at(x)) {
        if (unsetVariables.count(y)) {
            unsigned int nbInfeasInDomain = 0;
            for (int b : problem.getDomain(y)) {
                if (!Cxy.feasible(a,b)) {
                    nbInfeasInDomain++;
                    modifs.push_back(std::make_pair(y, b));
                }
            }
            if (nbInfeasInDomain == problem.sizeDomain(y)) return false;
        }
    }
    for (auto [y,b] : modifs) {
        problem.removeVariableValue(y, b);
    }
    deltaDomains.push_back(modifs);
    return true;
}

void Solver::backOldDomains() {
    for (auto [y,b] : deltaDomains.back()) {
        problem.addVariableValue(y, b);
    }
    deltaDomains.pop_back();
}

bool Solver::presolve() {
    for (int var: problem.getVariables()) {
        switch (problem.getDomainSize(var))
        {
        case 0:
            return false;
        case 1:
        {
            int value =*problem.getDomain(var).begin(); // not pretty
            if (!feasible(var,value)) return false;
            unsetVariables.erase(var);
            setVariables.emplace(var,value);
            break;
        }
        default:
            break;
        }
    }
    std::cout << "Presolved fixed " << setVariables.size()<< "/" << problem.nbVar() << " variables"<<std::endl;
    return true;
}

bool Solver::solve() {
    if (unsetVariables.empty()) return true;
    int var = varChooser->choose(problem,unsetVariables);
    unsetVariables.erase(var);

    for (int value : valueChooser->choose(problem,var)) {
        nbNodesExplored++;
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