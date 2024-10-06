#ifndef CSP_H_
#define CSP_H_

#include <unordered_set>
#include <vector>
#include <unordered_map>
#include <functional>
#include "constraint.h"
#include "problemreader.h"

class CSP {

private:
    std::unordered_set<int> variables;
    std::unordered_map<int,std::unordered_set<int>> domains;
    std::unordered_map<int,std::unordered_map<int,Constraint>> constraints;

public:
    CSP(){};

    std::size_t nbVar() const{return domains.size();}

    void addVariable(int i);
    void addVariableValue(int var, int value);
    void addConstraint(int i, int j);
    void addConstraint(int i, int j, const std::function<bool(int,int)>& validPair);
    void addConstraint(std::pair<int,int> pair) {return addConstraint(pair.first, pair.second);}
    void addConstraint(std::pair<int,int> pair, const std::function<bool(int,int)>& validPair) {return addConstraint(pair.first, pair.second, validPair);}
    void addConstraintValuePair(int i, int j, int a, int b);
    void removeConstraintValuePair(int i, int j, int a, int b);

    std::unordered_set<int> getVariables() {return variables;}
   
    struct DomainIterator {

        const std::unordered_set<int>& domain;

        DomainIterator(const std::unordered_set<int>& _domain) : domain(_domain) {};
        auto begin() {return domain.begin();}
        auto end() {return domain.end();}
    };
    DomainIterator getDomain(int var) {return DomainIterator(domains.at(var));}

    bool feasible(const std::unordered_map<int,int>& partSol) const;
    // Check if the added variable do not produce infeasibility with the given value
    // assuming the partial solution without this variable is feasible
    bool feasible(const std::unordered_map<int,int>& partSol, int var, int value) const;
    
    void init(ColorProblem problem, int nbColors);
    void init(QueenProblem problem);
    void init(SudokuProblem problem);
    
    void display(bool removeSymmetry = true) const;
};

#endif