#include "solver.h"
#include <iostream>

Solver::Solver(CSP _problem) : problem(_problem) {
    unsetVariables = problem.getVariables();
    varChooser = std::make_unique<SmallestDomainVariableChooser>();
    valueChooser = std::make_unique<CopyValueChooser>();
}

bool Solver::removeVariableValue(int x, int a) {
    deltaDomains.back().push_back(std::make_pair(x, a));
    problem.removeVariableValue(x, a);
    switch (problem.getDomainSize(x)) 
    {
    case 1:
    {
        int c = *problem.getDomain(x).begin();
        lazyPropagateList.push_back(std::make_pair(x,c));
        break;
    }
    case 0: 
        return false;
    default: 
        break;
    }
    return true;
}

bool Solver::forwardChecking(int x, int a) {
    for (const auto& [y, Cxy] : problem.getConstraints().at(x)) {
        if (unsetVariables.count(y)) {
            std::vector<int> domain;
            domain.insert(domain.end(), problem.getDomain(y).begin(), problem.getDomain(y).end());
            for (int b : domain) {
                if (!Cxy->feasible(a,b) && !removeVariableValue(y, b)) return false;
            }
        }
    }
    return true;
}

bool Solver::lazyPropagate(int z, int d) {
    lazyPropagateList.push_back(std::make_pair(z,d));
    while (lazyPropagateList.size() > 0) {
        auto [x, a] = lazyPropagateList.back();
        fixVarValue(x,a);
        lazyPropagateList.pop_back();
        bool feasible = forwardChecking(x, a);
        if (!feasible) {
            lazyPropagateList.clear();
            return false;
        }
    }
    return true;
}


void Solver::fixVarValue(int var, int value) {
    if (!unsetVariables.erase(var)) return;
    setVariables.emplace(var,value);
    problem.fixValue(var, value);
    deltaFixedVars.back().push_back(var);
    lazyPropagateList.push_back(std::make_pair(var, value));
}

void Solver::unfixVarValue(int var) {
    setVariables.erase(var);
    unsetVariables.emplace(var);
}

void Solver::flashback() {
    for (auto [y,b] : deltaDomains.back()) {
        problem.addVariableValue(y, b);
    }
    deltaDomains.pop_back();

    for (int var : deltaFixedVars.back()) {
        unfixVarValue(var);
    }
    deltaFixedVars.pop_back();
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

void Solver::branchOnVar(int var, int value) {
    nbNodesExplored++;
    deltaFixedVars.push_back({});
    deltaDomains.push_back({});
    fixVarValue(var, value);
}

void Solver::unbranchVar(int var, std::vector<int> values) {
    for (int value : values) {
        problem.addVariableValue(var, value);
    }
}

bool Solver::solve() {
    if (unsetVariables.empty()) return true;
    int var = chooseVar();
    std::vector<int> values = chooseValue(var);
    for (int value : values) {
        branchOnVar(var, value);
        bool feasibility = lazyPropagate(var, value);
        if (!feasibility) {
            flashback();
            continue;
        }
        if (solve()) return true;
        flashback();
    }
    unbranchVar(var, values);

    return false;
}

void Solver::displaySolution() const{
    std::cout << "SOLUTION" << std::endl;
    for (auto [var,value] : setVariables) {
        std::cout << var << ":" << value << std::endl;
    }
}