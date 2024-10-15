#include "solver.h"
#include <iostream>

Solver::Solver(CSP _problem, bool _verbosity) : problem(_problem), verbosity(_verbosity) {
    unsetVariables = problem.getVariables();
    varChooser = std::make_unique<SmallestDomainVariableChooser>();
    valueChooser = std::make_unique<CopyValueChooser>();
}

bool Solver::removeVariableValue(int x, int a) {
    if (state == State::Solve) deltaDomains.back().push_back(std::make_pair(x, a));
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
    if (state == State::Solve) deltaFixedVars.back().push_back(var);
    setVariables.emplace(var,value);
    problem.fixValue(var, value);
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
            lazyPropagate(var, value);
            break;
        }
        default:
            break;
        }
    }
    std::cout << "Presolved fixed " << setVariables.size()<< "/" << problem.nbVar() << " variables"<<std::endl;
    std::cout << std::endl;
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

void Solver::solve_verbosity() {
    std::cout << "Nodes exp     | Best depth"  << std::endl;
    std::cout << "--------------------------" << std::endl;
    std::cout << "0" << std::endl;
    while (state == State::Solve) {
        std::cout << nbNodesExplored << " " << bestDepth << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
    std::cout << nbNodesExplored << " " << bestDepth << std::endl;
    std::cout << "--------------------------" << std::endl;
    std::string a = (foundSolution) ? std::to_string(nbNodesExplored) + " nodes explored - Found solution" 
                                    : "infeasible";
    std::cout << a << std::endl;
}

void Solver::solve() {
    presolve();
    // problem.cleanConstraints();
    // problem.initAC4();
    // problem.AC4();

    clock_t solve_time = clock();

    std::cout << "Launch solve with ";
    std::cout << "verbosity=" << std::to_string(verbosity);
    std::cout << ":"  << std::endl;
    state = State::Solve;
    std::vector<std::thread> threads;
    threads.emplace_back(std::thread(&Solver::launchSolve, this));
    if (verbosity) threads.emplace_back(std::thread(&Solver::solve_verbosity, this));
    for (auto& t : threads) t.join();

    solve_time = clock() - solve_time;

    if (hasFoundSolution()) {
        std::cout << "Solve time: " << (float)solve_time/CLOCKS_PER_SEC << std::endl;
        // displaySolution();
    } else {
        std::cout << "No solution found" << std::endl;
    }
}

void Solver::launchSolve() {
    foundSolution = recursiveSolve();
    state = State::Stop;
}

bool Solver::recursiveSolve() {
    if (unsetVariables.empty()) return true;
    std::size_t currentDepth = setVariables.size() + 1;
    if (currentDepth > bestDepth) bestDepth = currentDepth;
    int var = chooseVar();
    std::vector<int> values = chooseValue(var);
    for (int value : values) {
        branchOnVar(var, value);
        bool feasibility = lazyPropagate(var, value);
        if (!feasibility) {
            flashback();
            continue;
        }
        if (recursiveSolve()) return true;
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