#ifndef ALL_DIFFERENT_FAMILY_H_
#define ALL_DIFFERENT_FAMILY_H_

#include<unordered_set>
#include<unordered_map>
#include<vector>

class AllDifferentFamily {

private:
    std::unordered_set<int> variables;
    std::unordered_map<int,std::unordered_set<int>> valToVar;

    unsigned int nbValPossible;

public:
    AllDifferentFamily(std::vector<int> _variables, const std::unordered_map<int,std::unordered_set<int>>& domains);

    unsigned int nbVar() {return (unsigned int)(variables.size());}
    unsigned int nbVal() {return (unsigned int)(valToVar.size());}

    bool remove(int var, int val);
    void add(int var, int val);

};

#endif