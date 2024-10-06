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
            unsigned int infeas = 0;
            for (int b : problem.getDomain(y)) {
                if (!ijConstraint.feasible(a,b)) {
                    infeas++;
                    modifs.push_back(std::make_pair(y, b));
                }
            }
            if (infeas == problem.sizeDomain(y)) return false;
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
    // std::pair<std::unordered_map<int, std::unordered_map<int,std::unordered_map<int,int>>>, std::vector<std::pair<int,int>>> modifs(problem.AC4());
    for (int a : problem.getDomain(x)) {
        fixVarValue(x, a);
        if (!forwardChecking(x, a)) {
            unfixVarValue(x);
            return false;
        }
        if (solve()) return true;
        stepBack();
        unfixVarValue(x);
    }
    // for (const auto& [x,yCount] : modifs.first) {
    //     for (const auto& [y,aCount] : yCount) {
    //         for (const auto& [a,count] : aCount) {
    //             problem.initCount(x, y, a, count);
    //         }
    //     }
    // }

    unsetVariables.emplace(x);
    return false;
}

void Solver::displaySolution() const{
    std::cout << "SOLUTION" << std::endl;
    for (auto [var,value] : setVariables) {
        std::cout << var << ":" << value << std::endl;
    }
}