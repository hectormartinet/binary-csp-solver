#ifndef SOLVER_H_
#define SOLVER_H_

#include "csp.h"
#include "variablechooser.h"
#include "valuechooser.h"

#include <memory>  


class Solver {

private:
    CSP problem;
    std::unordered_map<int,int> setVariables;
    std::unordered_set<int> unsetVariables;
    std::vector<std::vector<std::pair<int,int>>> deltaDomains;
    unsigned int nbNodesExplored=0;

    std::unique_ptr<VariableChooser> varChooser;
    std::unique_ptr<ValueChooser> valueChooser;

public:
    Solver(CSP _problem);

    bool feasible() const{return problem.feasible(setVariables);}
    bool feasible(int var, int value) const {return problem.feasible(setVariables,var,value);}
    bool forwardChecking(int x, int a);
    void backOldDomains();
    bool presolve();
    bool solve();

    void fixVarValue(int var, int value) {setVariables.emplace(var,value);}
    void unfixVarValue(int var) {setVariables.erase(var);}
    std::unordered_map<int,int> retrieveSolution() const{return setVariables;}
    unsigned int getNbNodesExplored() const{return nbNodesExplored;}
    void displaySolution() const;

};

#endif