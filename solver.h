#ifndef SOLVER_H_
#define SOLVER_H_

#include "csp.h"


class Solver {

private:
    CSP problem;
    std::unordered_map<int,int> setVariables;
    std::unordered_set<int> unsetVariables;

public:
    Solver(){}
    Solver(CSP _problem);

    bool feasible() {return problem.feasible(setVariables);}
    bool solve();

    std::unordered_map<int,int> retrieveSolution() {return setVariables;}
    void displaySolution();

};

#endif