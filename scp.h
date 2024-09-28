#ifndef SCP_H_
#define SCP_H_

#include <unordered_set>
#include <vector>
#include <unordered_map>
#include "constraint.h"
#include "problemreader.h"

class SCP {

private:
    std::unordered_map<int,std::unordered_set<int>> domains;
    std::unordered_map<int,std::unordered_map<int,Constraint>> constraints;

public:
    SCP(){};

    std::size_t nbVar() const{return domains.size();}

    void addConstraint(int i, int j);
    void addConstraint(std::pair<int,int> pair) {return addConstraint(pair.first, pair.second);}
    void addConstraintValuePair(int i, int j, int a, int b);

    bool feasible(const std::unordered_map<int,int>& sol) const;
    void makeNQueen(int n);
    void init(ProblemReader::ColorProblem problem, int nbColors);
    void display() const;
};

#endif