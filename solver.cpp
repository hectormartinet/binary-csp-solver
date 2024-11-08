#include "solver.h"
#include <iostream>

Solver::Solver(CSP _problem, const std::vector<std::string> _parameters, bool _verbosity) : problem(_problem), parameters(_parameters), verbosity(_verbosity) {
    unsetVariables = problem.getVariables();
    translateParameters(parameters);
    initAllDifferent();
}

Solver::Solver(CSP _problem) : problem(_problem) {
    unsetVariables = problem.getVariables();
    setDefaultParameters();
    initAllDifferent();
}

void Solver::setDefaultParameters() {
    rootSolveMethod = SolveMethod::LazyPropagate;
    nodeSolveMethod = SolveMethod::LazyPropagate;
    varChooser = std::make_unique<SmallestDomainVariableChooser>();
    valueChooser = std::make_unique<CopyValueChooser>();
    randomSeed = (unsigned int)(time(NULL));
    parameters = {"LP", "LP", "smallest", "copy", " ", std::to_string(randomSeed), "1", "1"};
}

void Solver::setRootSolveMethod(const std::string _rootSolveMethod) {
    if (_rootSolveMethod == "AC4") rootSolveMethod = SolveMethod::AC4;
    else if (_rootSolveMethod == "AC3") rootSolveMethod = SolveMethod::AC3;
    else if (_rootSolveMethod == "FC") rootSolveMethod = SolveMethod::ForwardChecking;
    else if (_rootSolveMethod == "LP") rootSolveMethod = SolveMethod::LazyPropagate;
    else throw std::logic_error("Wrong root solve method");
    parameters[0] = _rootSolveMethod;
}

void Solver::setNodeSolveMethod(const std::string _nodeSolveMethod) {
    if (_nodeSolveMethod == "AC4") nodeSolveMethod = SolveMethod::AC4;
    else if (_nodeSolveMethod == "AC3") nodeSolveMethod = SolveMethod::AC3;
    else if (_nodeSolveMethod == "FC") nodeSolveMethod = SolveMethod::ForwardChecking;
    else if (_nodeSolveMethod == "LP") nodeSolveMethod = SolveMethod::LazyPropagate;
    else throw std::logic_error("Wrong node solve method");
    parameters[1] = _nodeSolveMethod;
}

void Solver::setVarChooser(const std::string _variableChooser) {
    if (_variableChooser == "smallest") varChooser = std::make_unique<SmallestDomainVariableChooser>();
    else if (_variableChooser == "random") varChooser = std::make_unique<RandomVariableChooser>();
    else if (_variableChooser == "max") varChooser = std::make_unique<MaxConstraintVariableChooser>();
    else throw std::logic_error("Wrong variable chooser");
    parameters[2] = _variableChooser;
}

void Solver::setValChooser(const std::string _valueChooser) {
    if (_valueChooser == "copy") valueChooser = std::make_unique<CopyValueChooser>();
    else if (_valueChooser == "smallest") valueChooser = std::make_unique<SmallestValueChooser>();
    else if (_valueChooser == "random") valueChooser = std::make_unique<RandomValueChooser>();
    else throw std::logic_error("Wrong value chooser");
    parameters[3] = _valueChooser;
}

void Solver::setValLambdaChooser(const std::function<bool(int,int)> lambda) {
    valueChooser = std::make_unique<LambdaValueChooser>(lambda);
    parameters[3] = "lambda";
}

void Solver::setTimeLimit(const int _timeLimit) {
    if (_timeLimit>=0) {
        timeLimit = _timeLimit;
        parameters[4] = std::to_string(_timeLimit);
    }
}

void Solver::setRandomSeed(const unsigned int _randomSeed) {
    randomSeed =_randomSeed;
    parameters[5] = std::to_string(_randomSeed);
}

void Solver::setNbSolutions(const unsigned int _nbSolutions) {
    nbSolutions = _nbSolutions; 
    parameters[6] = (_nbSolutions == INT_MAX) ? "all" : std::to_string(_nbSolutions);
}

void Solver::setAllDifferent(const bool _allDifferent) {
    allDifferent = _allDifferent;
    parameters[7] = std::to_string(_allDifferent);
}

void Solver::translateParameters(const std::vector<std::string> _parameters){
    assert(_parameters.size() == 8);
    setRootSolveMethod(_parameters[0]);
    setNodeSolveMethod(_parameters[1]);
    setVarChooser(_parameters[2]);
    setValChooser(_parameters[3]);
    setTimeLimit(std::stoi(parameters[4]));

    if (parameters[5] == "" || std::stoi(parameters[5]) < 0) setRandomSeed((unsigned int)(time(NULL))) ;
    else setRandomSeed(std::stoul(parameters[5]));

    unsigned int _nbSolutions = (parameters[6] == "all") ? INT_MAX : std::stoul(parameters[6]);
    setNbSolutions(_nbSolutions);

    setAllDifferent(bool(std::stoi(parameters[7])));
}

void Solver::initAllDifferent() {
    for (int var:problem.getVariables()) {
        varToAllDifferentFamilyIdx.emplace(var,std::vector<unsigned int>());
    }
    if (!allDifferent) return;
    unsigned int idx = 0;
    for (const auto& family : problem.getAllDifferentFamilies()) {
        allDifferentFamilies.push_back(AllDifferentFamily(family,problem.getDomains()));
        for (int var:family) {
            varToAllDifferentFamilyIdx.at(var).push_back(idx);
        }
        idx++;
    }
}

void Solver::checkFeasibility(CSP _problem) {
    for (const auto &sol : solutions) {
        assert(sol.size() == _problem.nbVar());
        assert(_problem.feasible(sol));
    }
}

void Solver::removeConstraintValuePair(int x, int y, int a, int b) {
    if (state == State::Solve) deltaConstrValPair.back().push_back(std::make_tuple(x,y,a,b));
    problem.removeConstraintValuePair(x, y, a, b);
}

void Solver::updateAddAllDiff(int var, int value) {
    for (unsigned int familyIdx : varToAllDifferentFamilyIdx.at(var)) {
        allDifferentFamilies[familyIdx].add(var, value);
    }
}

bool Solver::updateRemoveAllDiff(int var, int value) {
    std::vector<std::pair<int,int>> varsToFix;
    for (unsigned int familyIdx : varToAllDifferentFamilyIdx.at(var)) {
        if (!allDifferentFamilies[familyIdx].remove(var, value, varsToFix)) return false;
    }
    return fixVariables(varsToFix);
}

bool Solver::updateSetAllDiff(int var, int value, const std::vector<int>& values) {
    if (varToAllDifferentFamilyIdx.at(var).empty()) return true;
    for (int valToRemove : values) {
        if (valToRemove != value) {
            if (!updateRemoveAllDiff(var, valToRemove)) return false;
        }
    }
    return true;
}

void Solver::backtrackAllDiff(int var, const std::vector<int>& values) {
    if (varToAllDifferentFamilyIdx.at(var).empty()) return;
    for (int value : values) {
        updateAddAllDiff(var,value);
    }
}

bool Solver::fixVariables(const std::vector<std::pair<int,int>>& varsToFix) {
    for (auto [var,value] : varsToFix) {
        if (setVariables.count(var)) continue;
        for (int valToRemove : problem.getDomainCopy(var)) {
            if (valToRemove != value) {
                if (solveMethod == SolveMethod::AC4) addAC4List(var, valToRemove);
                if (!removeVarValue(var, valToRemove)) return false;
            }
            if (solveMethod == SolveMethod::AC3) {
                for (const auto& [y,Cxy] : problem.getConstraints().at(var)) {
                    if (unsetVariables.count(y)) addAC3List(y, var);
                }
            }
        }
    }
    return true;
}


bool Solver::removeVarValue(int var, int value) {
    if (problem.getDomain(var).count(value)==0) return true;
    if (state == State::Solve) deltaDomains.back().push_back(std::make_pair(var,value));
    problem.removeVariableValue(var, value);
    if (!updateRemoveAllDiff(var, value)) return false;
    switch (problem.getDomainSize(var)) 
    {
    case 1:
    {
        int onlyValue = *problem.getDomain(var).begin();
        if (solveMethod == SolveMethod::LazyPropagate) 
            lazyPropagateList.emplace(std::make_pair(var,onlyValue));
        break;
    }
    case 0: 
        return false;
    default: 
        break;
    }
    return true;
}

void Solver::addVarValue(int var, int value) {
    problem.addVariableValue(var, value);
    updateAddAllDiff(var, value);
}

bool Solver::forwardChecking(int x, int a) {
    assert(solveMethod == SolveMethod::LazyPropagate || solveMethod == SolveMethod::ForwardChecking);
    for (const auto& [y, Cxy] : problem.getConstraints().at(x)) {
        if (unsetVariables.count(y)) {
            for (int b : Cxy->getForbiddenValues(a, problem.getDomain(y))) {
                if (!removeVarValue(y, b)) return false;
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
    if (!forwardChecking(var,value)) return false;
    
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

void Solver::removeAC3List(int x, int y) {
    AC3List.erase(std::make_pair(x,y));
    int onlyVal = *problem.getDomain(y).begin();
    if (problem.getDomainSize(y) == 1) setVar(y,onlyVal);
}

bool Solver::AC3() {
    while (!AC3List.empty()) {
        auto [x,y] = *AC3List.begin();
        removeAC3List(x,y);
        std::vector<int> domain;
        domain.insert(domain.end(), problem.getDomain(x).begin(), problem.getDomain(x).end());
        for (int v : domain) {
            bool hasSupport = false;
            for (int w : problem.getDomain(y)) {
                if (problem.getConstraints().at(x).at(y)->feasible(v,w)) {
                    hasSupport = true;
                    break;
                }
            }
            if (!hasSupport) {
                if (!removeVarValue(x, v)) return false;
                for (const auto& [z, Cxz] : problem.getConstraints().at(x)) {
                    if (unsetVariables.count(z) && z != y) addAC3List(z, x);
                }
            }
        }
    }
    return true;
}

bool Solver::initAC3Root() {
    assert(solveMethod == SolveMethod::AC3);
    assert(state == State::Preprocess);
    for (const auto& [x,Cx] : problem.getConstraints()) {
        for (const auto& [y,Cxy] : Cx) {
            addAC3List(x, y);
        }
    }
    return true;
}

bool Solver::initAC3Solve(int var) {
    assert(solveMethod == SolveMethod::AC3);
    assert(state == State::Solve);
    for (const auto& [y,Cxy] : problem.getConstraints().at(var)) {
        if (unsetVariables.count(y)) addAC3List(y, var);
    }
    return true;
}

bool Solver::initAC4Solve(int var, int value, std::vector<int> oldDomain) {
    assert(state != State::Preprocess);
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
    problem.fixValue(var,value);
}

void Solver::unsetVar(int var) {
    setVariables.erase(var);
    unsetVariables.emplace(var);
}

void Solver::backtrack() {
    AC4List.clear();
    AC3List.clear();
    lazyPropagateList.clear();
    for (auto [y,b] : deltaDomains.back()) {
        addVarValue(y, b);
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

void Solver::preprocess() {
    std::cout << "Launch presolve with rootSolveMethod=" << parameters[0] << ":" << std::endl;
    if (solveMethod == SolveMethod::AC4) {
        std::cout << "Extensify constraints for AC4..." << std::endl;
        problem.extensify();
        std::cout << "Done." << std::endl;
    }
}

bool Solver::presolve() {
    std::vector<std::pair<int,int>> varsToFix;
    for (const AllDifferentFamily& family : allDifferentFamilies) {
        if (!family.init(varsToFix)) return false;
    }
    fixVariables(varsToFix);
    if (solveMethod == SolveMethod::AC4) {
        bool consistent = initAC4Root() && AC4();
        if(!consistent) return false;
        assert(checkAC());
    }
    if (solveMethod == SolveMethod::AC3) {
        bool consistent = initAC3Root() && AC3();
        if (!consistent) return false;
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
    if (unsetVariables.size() == 0) solutions.push_back(setVariables);
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
        addVarValue(var, value);
    }
}

void Solver::solve() {
    displayLogo();
    displayModelInformation();
    if (nodeSolveMethod > rootSolveMethod) {
        rootSolveMethod = nodeSolveMethod;
        parameters[0] = parameters[1];
    }
    solveMethod = rootSolveMethod;

    preprocess();

    if (!presolve()) {
        std::cout << "inconsistent" << std::endl;
        return;
    }
    if (hasFoundSolution()) {
        displayFinalInformation();
        return;
    }
    solveMethod = nodeSolveMethod;
    state = State::Solve;
    displaySolveInformation();
    start_time = clock();
    std::vector<std::thread> threads;
    if (timeLimit < INT_MAX) threads.emplace_back(std::thread(&Solver::timeThread, this));
    threads.emplace_back(std::thread(&Solver::launchSolve, this));
    if (verbosity) threads.emplace_back(std::thread(&Solver::solveVerbosity, this));
    for (auto& t : threads) t.join();

    displayFinalInformation();
}

void Solver::timeThread() {
    while(state == State::Solve) {
        int time = (int)(clock() - start_time)/CLOCKS_PER_SEC;
        if (time >= timeLimit) state = State::Stop;
    }
}

void Solver::launchSolve() {
    srand(randomSeed);
    recursiveSolve();
    solve_time = clock() - start_time;
    state = State::Stop;
}

bool Solver::checkConsistent(int var, int value) {
    switch (solveMethod) 
    {
    case SolveMethod::AC4: 
       return AC4();
    case SolveMethod::AC3:
        return AC3();
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
    if (state == State::Stop) return false;
    if (unsetVariables.empty()) {
        solutions.push_back(setVariables);
        if (solutions.size() == nbSolutions) return true;
        return false;
    }
    int currentDepth = (int) setVariables.size() + 1;
    if (currentDepth > bestDepth) bestDepth = currentDepth;
    int var = chooseVar();
    std::vector<int> values = chooseValue(var);

    for (int value : values) {
        branchOnVar(var, value);
        if (!updateSetAllDiff(var, value, values)) {
            backtrack();
            backtrackAllDiff(var, values);
            continue;
        }
        if (solveMethod == SolveMethod::AC4) initAC4Solve(var, value, values);
        else if (solveMethod == SolveMethod::AC3) initAC3Solve(var);
        if (!checkConsistent(var, value)) {
            backtrack();
            backtrackAllDiff(var, values);
            continue;
        }
        if (solveMethod == SolveMethod::AC4) assert(checkAC());
        
        if (recursiveSolve()) return true;
        if (state == State::Stop) return false;
        backtrack();
        backtrackAllDiff(var, values);
    }
    unbranchOnVar(var, values);

    return false;
}

void Solver::solveVerbosity() {
    std::cout << " Time | n solutions | Best depth | Nodes explored"  << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    while (state == State::Solve) {
        int time = std::max((int)(clock() - start_time)/CLOCKS_PER_SEC,0);
        std::cout << time << "       "  << solutions.size() << "            " << bestDepth << "            " << nbNodesExplored << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
    int actualTime = std::min(std::max((int)(solve_time)/CLOCKS_PER_SEC,0), timeLimit);
    std::cout << actualTime << "       "  << solutions.size() << "            " << bestDepth << "            " << nbNodesExplored << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
}

void Solver::displayModelInformation() const{
    std::cout << std::endl;
    std::cout << "Initial problem: ";
    std::cout << problem.nbVar() << " variables / ";
    std::cout << problem.nbConstraints() << " constraints" << std::endl;
    std::cout << std::endl;
}

void Solver::displaySolveInformation() const{
    std::cout << "Launch solve with";
    std::cout << " nodeSolveMethod=" << parameters[1];
    std::cout << "; varChooser=" << parameters[2];
    std::cout << "; valChooser=" << parameters[3];
    if (timeLimit < INT_MAX) std::cout << "; timeLimit=" << parameters[4];
    if ((parameters[2] == "random" || parameters[3] == "random")) std::cout << "; randomSeed=" << std::to_string(randomSeed);
    std::cout << "; nbSolutions=" << parameters[6];
    std::cout << "; verbosity=" << std::to_string(verbosity);
    std::cout << ":"  << std::endl;
}

void Solver::displayFinalInformation() const{
    if (hasFoundSolution()) std::cout << std::to_string(nbNodesExplored) + " nodes explored - Found " << solutions.size() << " solution(s)" << std::endl;
    else if (solve_time >= timeLimit) std::cout << std::to_string(nbNodesExplored) + " nodes explored - no solution found" << std::endl;
    else std::cout << "infeasible" << std::endl;
    if (state == State::Stop)
        std::cout << "Solve time: " << (float)solve_time/CLOCKS_PER_SEC << " s" << std::endl;
}

void Solver::displaySolution() const{
    for (unsigned int i = 0; i < solutions.size(); i++) {
        std::cout << std::endl;
        std::cout << "SOLUTION " << i + 1 << std::endl;
        std::vector<int> sol(problem.nbVar(), 0);
        for (auto [var,value] : solutions[i]) {
            sol[var] = value;
        }
        for (unsigned int i = 0; i < sol.size(); i++) {
            std::cout << i+1 << ":" << sol[i] + 1 << "; ";
        }
        std::cout << std::endl;
        if (problem.getProblemType() == Problem::Queens && problem.nbVar() <= 30) {
            for (unsigned int i = 0; i < sol.size(); i++) {
                std::cout << "|";
                for (int k = 0; k < sol[i]; k++) {
                    std::cout << " |";
                }
                std::cout << "o";
                for (unsigned int k = 0; k < problem.nbVar() - sol[i] - 1; k++) {
                    std::cout << "| ";
                }
                std::cout << "|" << std::endl;
            }
        }
    }
    std::cout << std::endl;
}

void Solver::displayLogo() const{
    std::cout << R"(
  ___                    _ ___      _ 
 | _ ) __ _ _ _  __ __ _| / __| ___| |_ _____ _ _         ___ 
 | _ \/ _` | ' \/ _/ _` | \__ \/ _ \ \ V / -_) '_|       /   \
 |___/\__,_|_||_\__\__,_|_|___/\___/_|\_/\___|_|>=-=-=-='\___/
)" << std::endl;
}