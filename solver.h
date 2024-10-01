#ifndef SOLVER_H_
#define SOLVER_H_

#include "csp.h"


class Solver {

private:
    CSP problem;
    std::unordered_map<int,int> setVariables;
    std::unordered_set<int> unsetVariables;
    unsigned int nbNodesExplored=0;

public:
    Solver(){}
    Solver(CSP _problem);

    bool feasible() const{return problem.feasible(setVariables);}
    bool feasible(int var, int value) const {return problem.feasible(setVariables,var,value);}
    bool solve();

    std::unordered_map<int,int> retrieveSolution() const{return setVariables;}
    unsigned int getNbNodesExplored() const{return nbNodesExplored;}
    void displaySolution() const;

};

#endif