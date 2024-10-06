#ifndef CSP_H_
#define CSP_H_

#include <unordered_set>
#include <vector>
#include <unordered_map>
#include "constraint.h"
#include "problemreader.h"

class CSP {

private:
    std::unordered_set<int> variables;
    std::unordered_map<int,std::unordered_set<int>> domains;
    std::unordered_map<int,std::unordered_map<int,std::unordered_map<int,int>>> counts;
    std::unordered_map<int,std::unordered_map<int,std::vector<std::pair<int,int>>>> support;
    std::vector<std::pair<int,int>> AC4List;
    std::unordered_map<int,std::unordered_map<int,Constraint>> constraints;
public:

    CSP(){};

    std::size_t nbVar() const{return domains.size();}
    std::size_t sizeDomain(int var) const{return domains.at(var).size();}

    void addVariable(int i);
    void addVariableValue(int var, int value);
    void removeVariableValue(int var, int value);
    void fixValue(int var, int a);
    bool isInDomain(int var, int a);
    void addConstraint(int i, int j);
    void addConstraint(std::pair<int,int> pair) {return addConstraint(pair.first, pair.second);}
    void addConstraintValuePair(int i, int j, int a, int b);
    void removeConstraintValuePair(int i, int j, int a, int b);
    void initCount(int i, int j, int a, int val);
    void reduceCount(int i, int j, int a, int val);
    int getCount(int i, int j, int a);
    void addSupport(int i, int a, int j, int b);
    void addAC4List(int i, int a);

    std::unordered_set<int> getVariables() {return variables;}
    std::unordered_map<int,std::unordered_map<int,Constraint>> getConstraints() {return constraints;}
   
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
    void initAC4();
    std::pair<std::unordered_map<int,std::unordered_map<int,std::unordered_map<int,int>>>, std::vector<std::pair<int,int>>> AC4();
    void display(bool removeSymmetry = true) const;
};

#endif