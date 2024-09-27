#ifndef SCP_H_
#define SCP_H_

#include <set>
#include <vector>
#include <map>
#include "constraint.h"

class SCP {

private:
    std::vector<std::set<int>> domains;
    std::map<unsigned int,std::map<unsigned int,Constraint>> constraints;

public:
    SCP(unsigned int nbVar);

    std::size_t nbVar() const{return domains.size();}
    
    bool feasible(const std::vector<int>& sol) const;
    void makeNQueen();
    void display() const;
};

#endif