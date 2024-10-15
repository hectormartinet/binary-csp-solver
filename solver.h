#ifndef SOLVER_H_
#define SOLVER_H_

#include "csp.h"
#include "variablechooser.h"
#include "valuechooser.h"

#include <memory>  
enum class State {Preprocess, Solve, Stop};

class Solver {

private:
    CSP problem;
    std::unique_ptr<VariableChooser> varChooser;
    std::unique_ptr<ValueChooser> valueChooser;

    std::unordered_map<int,int> setVariables;
    std::unordered_set<int> unsetVariables;
    std::vector<std::vector<std::pair<int,int>>> deltaDomains;
    std::vector<std::vector<int>> deltaFixedVars;
    std::vector<std::pair<int,int>> lazyPropagateList;

    State state = State::Preprocess;

    unsigned int nbNodesExplored=0;
    int bestDepth=0;
    bool verbosity=true;
    bool foundSolution=false;
    clock_t solve_time;



public:
    Solver(CSP _problem, bool _verbosity);

    bool feasible() const{return problem.feasible(setVariables);}
    bool feasible(int var, int value) const {return problem.feasible(setVariables,var,value);}
    bool lazyPropagate(int x, int a);
    bool forwardChecking(int x, int a);
    void flashback();
    void branchOnVar(int var, int value);
    void unbranchVar(int var, std::vector<int> values);
    bool presolve();
    void solve();
    void launchSolve();
    bool recursiveSolve();
    int chooseVar() {return varChooser->choose(problem,unsetVariables);}
    std::vector<int> chooseValue(int var) {return valueChooser->choose(problem,var);}
    void solve_verbosity();

    void fixVarValue(int var, int value);
    void unfixVarValue(int var);
    bool removeVariableValue(int var, int value);
    std::unordered_map<int,int> retrieveSolution() const{return setVariables;}
    unsigned int getNbNodesExplored() const{return nbNodesExplored;}
    void displaySolution() const;
    bool hasFoundSolution() {return foundSolution;}

    void displayModelInformation() const;
    void displaySolveInformation() const;
    void displayFinalInformation() const;

};

#endif