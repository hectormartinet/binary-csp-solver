#ifndef SOLVER_H_
#define SOLVER_H_

#include "csp.h"
#include "variablechooser.h"
#include "valuechooser.h"

#include <memory>  

struct PairHash {
public:
    size_t hash_combine(size_t seed, int v) const {
        return seed ^= std::hash<int>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    std::size_t operator()(const std::pair<int, int> &pair) const
    {
        return hash_combine(std::hash<int>{}(pair.first),pair.second);
    }
};
enum class State {Preprocess, Solve, Stop};
enum class SolveMethod {ForwardChecking, LazyPropagate, AC4};

class Solver {

private:
    CSP problem;
    std::unique_ptr<VariableChooser> varChooser;
    std::unique_ptr<ValueChooser> valueChooser;
    SolveMethod solveMethod;
    std::vector<std::string> parameters;
    bool verbosity=true;
    int timeLimit;
    unsigned int randomSeed;

    std::unordered_map<int,int> setVariables;
    std::unordered_set<int> unsetVariables;
    std::vector<std::vector<std::pair<int,int>>> deltaDomains;
    std::vector<std::vector<int>> deltaSetVars;
    std::vector<std::vector<std::tuple<int, int, int, int>>> deltaConstrValPair;

    State state = State::Preprocess;
    unsigned int nbNodesExplored=0;
    int bestDepth=0;
    clock_t solve_time;
    bool foundSolution=false;

    std::unordered_set<std::pair<int,int>,PairHash> AC4List;
    std::unordered_set<std::pair<int,int>,PairHash> lazyPropagateList;

public:
    Solver(CSP _problem, const std::vector<std::string> _parameters, bool _verbosity);
    void translateParameters(const std::vector<std::string> _parameters);

    bool feasible() const{return problem.feasible(setVariables);}
    bool feasible(int var, int value) const {return problem.feasible(setVariables,var,value);}
    void checkFeasibility(CSP _problem);
    void preprocess();
    bool presolve();
    void launchSolve();
    void timeThread();
    void branchOnVar(int var, int value);
    void unbranchOnVar(int var, std::vector<int> values);
    void solve();
    void flashback();
    bool recursiveSolve();
    bool checkConsistent(int var, int value);
    
    bool forwardChecking(int x, int a);
    bool lazyPropagate(int x, int a);
    void removeLazyPropagateList(int x, int a);

    void cleanConstraints();
    bool initAC4Root();
    bool initAC4Solve(int var, int value, std::vector<int> values);    
    bool AC4();
    bool checkAC();
    void addAC4List(int x, int a) {AC4List.emplace(std::make_pair(x, a));}
    void removeAC4List(int x, int a);

    int chooseVar() {return varChooser->choose(problem,unsetVariables);}
    std::vector<int> chooseValue(int var) {return valueChooser->choose(problem,var);}

    void setVar(int var, int value);
    void unsetVar(int var);
    bool removeVarValue(int var, int value);
    void removeConstraintValuePair(int x, int y, int a, int b);
    std::unordered_map<int,int> retrieveSolution() const{return setVariables;}
    unsigned int getNbNodesExplored() const{return nbNodesExplored;}
    bool hasFoundSolution() {return foundSolution;}

    void solve_verbosity();
    void displayModelInformation() const;
    void displaySolveInformation() const;
    void displayFinalInformation() const;
    void displaySolution() const;
    void displayLogo() const;

};

#endif