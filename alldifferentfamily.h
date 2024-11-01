#ifndef ALL_DIFFERENT_FAMILY_H_
#define ALL_DIFFERENT_FAMILY_H_

#include<unordered_set>
#include<unordered_map>
#include<vector>

class AllDifferentFamily {

private:
    std::unordered_set<int> variables;
    std::unordered_map<int,std::unordered_set<int>> valToVar;
    // Values such that there is exactly one variable that can take this value
    std::unordered_map<int,int> uniqueValues;

    unsigned int nbValPossible;

public:
    AllDifferentFamily(std::vector<int> _variables, const std::unordered_map<int,std::unordered_set<int>>& domains);

    unsigned int nbVar() const{return (unsigned int)(variables.size());}
    unsigned int nbVal() const{return (unsigned int)(valToVar.size());}

    bool remove(int var, int val, std::vector<std::pair<int,int>>& varsToFix);
    void add(int var, int val);

    bool init(std::vector<std::pair<int,int>>& varsToFix) const;

    void fillUniqueValues(std::vector<std::pair<int,int>>& varsToFix) const;

    bool isCoherent(const std::unordered_map<int,std::unordered_set<int>>& domains) const;

};

#endif