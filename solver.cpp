#include "solver.h"
#include <iostream>

Solver::Solver(CSP _problem) : problem(_problem) {
    unsetVariables = problem.getVariables();
    varChooser = std::make_unique<SmallestDomainVariableChooser>();
    valueChooser = std::make_unique<CopyValueChooser>();
}

bool Solver::forwardChecking(int z, int d) {
    std::vector<std::pair<int,int>> modifs;
    std::vector<std::pair<int,int>> toFix = {std::make_pair(z, d)};
    std::vector<int> fixedVar = {};
    while (toFix.size() > 0) {
        auto [x, a] = toFix.back();
        toFix.pop_back();
        fixVarValue(x, a);
        fixedVar.push_back(x);
        for (const auto& [y, Cxy] : problem.getConstraints().at(x)) {
            if (unsetVariables.count(y)) {
                std::vector<int> domain;
                domain.insert(domain.end(), problem.getDomain(y).begin(), problem.getDomain(y).end());
                for (int b : domain) {
                    if (!Cxy.feasible(a,b)) {
                        modifs.push_back(std::make_pair(y, b));
                        problem.removeVariableValue(y, b);
                    }
                }
                size_t domainSize = problem.sizeDomain(y);
                if (domainSize == 0) {
                    deltaDomains.push_back(modifs);
                    deltaFixedValues.push_back(fixedVar);
                    getOldProblem();
                    return false;
                } else if (domainSize == 1) {
                    int c = *problem.getDomain(y).begin();
                    toFix.push_back(std::make_pair(y,c));
                }
            }
        }
    }
    deltaDomains.push_back(modifs);
    deltaFixedValues.push_back(fixedVar);
    return true;
}


void Solver::fixVarValue(int var, int value) {
    unsetVariables.erase(var);
    setVariables.emplace(var,value);
    problem.fixValue(var, value);
}

void Solver::unfixVarValue(int var) {
    setVariables.erase(var);
    unsetVariables.emplace(var);
}

void Solver::getOldProblem() {
    for (auto [y,b] : deltaDomains.back()) {
        problem.addVariableValue(y, b);
    }
    deltaDomains.pop_back();

    for (int var : deltaFixedValues.back()) {
        unfixVarValue(var);
    }
    deltaFixedValues.pop_back();
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
    std::vector<int> values = valueChooser->choose(problem,var);
    for (int value : values) {
        nbNodesExplored++;
        if (!forwardChecking(var, value)) continue;
        if (solve()) return true;
        getOldProblem();
    }
    for (int value : values) {
        problem.addVariableValue(var, value);
    }

    return false;
}

void Solver::displaySolution() const{
    std::cout << "SOLUTION" << std::endl;
    for (auto [var,value] : setVariables) {
        std::cout << var << ":" << value << std::endl;
    }
}