#include "solver.h"
#include <iostream>

Solver::Solver(CSP _problem, SolveMethod _solveMethod, bool _verbosity) : problem(_problem), solveMethod(_solveMethod), verbosity(_verbosity) {
    unsetVariables = problem.getVariables();
    varChooser = std::make_unique<SmallestDomainVariableChooser>();
    valueChooser = std::make_unique<CopyValueChooser>();
}

void Solver::removeConstraintValuePair(int x, int y, int a, int b) {
    if (state == State::Solve) deltaConstrValPair.back().push_back(std::make_tuple(x,y,a,b));
    problem.removeConstraintValuePair(x, y, a, b);
}

bool Solver::removeVarValue(int x, int a) {
    if (state == State::Solve) deltaDomains.back().push_back(std::make_pair(x,a));
    problem.removeVariableValue(x, a);
    switch (problem.getDomainSize(x)) 
    {
    case 1:
    {
        int onlyValue = *problem.getDomain(x).begin();
        if (solveMethod == SolveMethod::LazyPropagate) 
            lazyPropagateList.emplace(std::make_pair(x,onlyValue));
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
    assert(solveMethod == SolveMethod::LazyPropagate || solveMethod == SolveMethod::ForwardChecking);
    for (const auto& [y, Cxy] : problem.getConstraints().at(x)) {
        if (unsetVariables.count(y)) {
            std::vector<int> domain;
            domain.insert(domain.end(), problem.getDomain(y).begin(), problem.getDomain(y).end());
            for (int b : domain) {
                if (!Cxy->feasible(a,b) && !removeVarValue(y, b)) return false;
            }
        }
    }
    return true;
}

void Solver::removeLazyPropagateList(int x, int a) {
    lazyPropagateList.erase(std::make_pair(x,a));
    setVar(x,a);
}

bool Solver::lazyPropagate(int var, int value) {
    assert(solveMethod == SolveMethod::LazyPropagate);
    lazyPropagateList.emplace(std::make_pair(var,value));
    while (!lazyPropagateList.empty()) {
        auto [x,a] = *lazyPropagateList.begin();
        removeLazyPropagateList(x,a);
        if (!forwardChecking(x,a)) return false;
    }
    return true;
}

void Solver::cleanConstraints(){
    for (const auto& [x,Cx] : problem.getConstraints()) {
        for (const auto& [y,Cxy] : Cx) {
            for (auto [a,b] : Cxy->getUselessPairs(problem.getDomain(x))) {
                removeConstraintValuePair(x,y,a,b);
            }
        }
    }
}

bool Solver::initAC4Root() {
    assert(solveMethod == SolveMethod::AC4);
    assert(state == State::Preprocess);
    cleanConstraints();
    for (const auto& [x,Cx] : problem.getConstraints()) {
        for (const auto& [y,Cxy] : Cx) {
            for (int a : problem.getDomain(x)) {
                if (Cxy->getSupportSize(a)==0) {
                    addAC4List(x, a);
                }
            }
        }
    }
    for (auto [x,a] : AC4List) {   
        if (!removeVarValue(x,a)) return false;
    }
    return true;
}

bool Solver::initAC4Solve(int var, int value, std::vector<int> oldDomain) {
    assert(state == State::Solve);
    for (int d : oldDomain) {
        if (d != value) addAC4List(var, d);
    }
    return true;
}

void Solver::removeAC4List(int x, int a) {
    AC4List.erase(std::make_pair(x,a));
    int onlyVal = *problem.getDomain(x).begin();
    if (problem.getDomainSize(x) == 1) setVar(x,onlyVal);
}

bool Solver::AC4() {
    assert(solveMethod == SolveMethod::AC4);
    while(!AC4List.empty()) {
        auto [y,b] = *AC4List.begin();
        removeAC4List(y,b);
        std::vector<std::pair<int,int>> toPropagate;
        for (const auto& [x, Cyx]:problem.getConstraints().at(y)) {
            if (Cyx->getSupportSize(b)==0) continue;
            for (int a:Cyx->getSupport(b)) {
                toPropagate.push_back(std::make_pair(x,a));
            }
        }
        for (auto [x,a] : toPropagate) {
            removeConstraintValuePair(x,y,a,b);
            // Lazy evaluation
            if (problem.getConstraints().at(x).at(y)->getSupportSize(a) == 0 && problem.getDomain(x).count(a)) {
                if (removeVarValue(x,a)) addAC4List(x,a);
                else return false;
            }
        }
    }
    return true;
}

bool Solver::checkAC() {
    cleanConstraints();
    for (const auto& [x,Cx] : problem.getConstraints()) {
        for (const auto& [y,Cxy] : Cx) {
            for (int a : problem.getDomain(x)) {
                if (Cxy->getSupportSize(a)==0) {
                    return false;
                }
            }
        }
    }
    return true;
}

void Solver::setVar(int var, int value) {
    if (!unsetVariables.erase(var)) return;
    if (state == State::Solve) deltaSetVars.back().push_back(var);
    setVariables.emplace(var,value);
    problem.fixValue(var, value);
}

void Solver::unsetVar(int var) {
    setVariables.erase(var);
    unsetVariables.emplace(var);
}

void Solver::flashback() {
    AC4List.clear();
    lazyPropagateList.clear();
    for (auto [y,b] : deltaDomains.back()) {
        problem.addVariableValue(y, b);
    }
    deltaDomains.pop_back();

    for (auto[x, y, a, b] : deltaConstrValPair.back()) {
        problem.addConstraintValuePair(x, y, a, b);
    }
    deltaConstrValPair.pop_back();

    for (int y : deltaSetVars.back()) {
        unsetVar(y);
    }
    deltaSetVars.pop_back();
}

bool Solver::presolve() {
    if (solveMethod == SolveMethod::AC4) {
        bool consistent = initAC4Root() && AC4();
        if(!consistent) return false;
        assert(checkAC());
    }
    for (int var: problem.getVariables()) {
        switch (problem.getDomainSize(var))
        {
        case 0:
            return false;
        case 1:
        {
            int value = *problem.getDomain(var).begin();
            setVar(var, value);
            // if (!feasible(var,value)) return false;
            switch (solveMethod) 
            {
            case SolveMethod::ForwardChecking:
            {
                if (!forwardChecking(var, value)) return false;
                break;
            }
            case SolveMethod::LazyPropagate:
            {
                if (!lazyPropagate(var, value)) return false;
                break;
            }
            default:
                break;
            }
        }
        default:
            break;
        }
    }
    std::cout << "Presolve fixed " << setVariables.size()<< "/" << problem.nbVar() << " variables"<<std::endl;
    std::cout << std::endl;
    return true;
}

void Solver::branchOnVar(int var, int value) {
    nbNodesExplored++;
    deltaSetVars.push_back({});
    deltaDomains.push_back({});
    deltaConstrValPair.push_back({});
    setVar(var, value);
}

void Solver::unbranchOnVar(int var, std::vector<int> values) {
    for (int value : values) {
        problem.addVariableValue(var, value);
    }
}

void Solver::displayLogo() const{
    std::cout << R"(
  ___                    _ ___      _ 
 | _ ) __ _ _ _  __ __ _| / __| ___| |_ _____ _ _         ___ 
 | _ \/ _` | ' \/ _/ _` | \__ \/ _ \ \ V / -_) '_|       /   \
 |___/\__,_|_||_\__\__,_|_|___/\___/_|\_/\___|_|>=-=-=-='\___/
)" << std::endl;
}

void Solver::solve() {
    displayLogo();
    displayModelInformation();

    if (!presolve()) {
        std::cout << "inconsistent" << std::endl;
        return;
    }

    state = State::Solve;
    displaySolveInformation();
    
    std::vector<std::thread> threads;
    threads.emplace_back(std::thread(&Solver::launchSolve, this));
    if (verbosity) threads.emplace_back(std::thread(&Solver::solve_verbosity, this));
    for (auto& t : threads) t.join();

    displayFinalInformation();
}

void Solver::launchSolve() {
    solve_time = clock();
    foundSolution = recursiveSolve();
    solve_time = clock() - solve_time;
    state = State::Stop;
}

bool Solver::checkConsistent(int var, int value) {
    switch (solveMethod) 
    {
    case SolveMethod::AC4: 
       return AC4();
    case SolveMethod::ForwardChecking: 
        return forwardChecking(var, value);
    case SolveMethod::LazyPropagate: 
        return lazyPropagate(var, value);
    default:
        break;
    }
    return false;

}

bool Solver::recursiveSolve() {
    if (unsetVariables.empty()) return true;
    int currentDepth = (int) setVariables.size() + 1;
    if (currentDepth > bestDepth) bestDepth = currentDepth;
    int var = chooseVar();
    std::vector<int> values = chooseValue(var);

    for (int value : values) {
        branchOnVar(var, value);
        if (solveMethod == SolveMethod::AC4) initAC4Solve(var, value, values);
        if (!checkConsistent(var, value)) {
            flashback();
            continue;
        }
        if (solveMethod == SolveMethod::AC4) assert(checkAC());
        
        if (recursiveSolve()) return true;
        flashback();
    }
    unbranchOnVar(var, values);

    return false;
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
}

void Solver::displayModelInformation() const{
    std::cout << std::endl;
    std::cout << "Initial problem: ";
    std::cout << problem.nbVar() << " variables / ";
    std::cout << problem.nbConstraints() << " constraints" << std::endl;
}

void Solver::displaySolveInformation() const{
    std::cout << "Launch solve with ";
    std::cout << "verbosity=" << std::to_string(verbosity);
    std::cout << ":"  << std::endl;
}

void Solver::displayFinalInformation() const{
    std::string a = (foundSolution) ? std::to_string(nbNodesExplored) + " nodes explored - Found solution" 
                                    : "infeasible";
    std::cout << a << std::endl;

    std::cout << "Solve time: " << (float)solve_time/CLOCKS_PER_SEC << std::endl;
}

void Solver::displaySolution() const{
    std::cout << "SOLUTION" << std::endl;
    for (auto [var,value] : setVariables) {
        std::cout << var << ":" << value << std::endl;
    }
}