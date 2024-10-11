#ifndef CSP_H_
#define CSP_H_

#include <unordered_set>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include "constraint.h"
#include "problemreader.h"

class CSP {

private:
    std::unordered_set<int> variables;
    std::unordered_map<int,std::unordered_set<int>> domains;
    std::unordered_map<int,std::unordered_map<int,std::unique_ptr<Constraint>>> constraints;
    
    struct PairHash {
    public:
        size_t hash_combine(size_t seed, int v) const {
            return seed ^= std::hash<int>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        std::size_t operator()(const std::pair<int, int> &pair) const
        {
            return hash_combine(std::hash<int>{}(pair.first),pair.second);
        }
    };
    std::unordered_set<std::pair<int,int>,PairHash> AC4List;
public:

    CSP(){};
    CSP(const CSP& csp); // copy constructor

    std::size_t nbVar() const{return domains.size();}
    std::size_t sizeDomain(int var) const{return domains.at(var).size();}

    void addVariable(int var);
    void addVariableValue(int var, int value);
    bool removeVariableValue(int var, int value);
    void fixValue(int var, int value);
    bool isInDomain(int var, int value);
    void addConstraint(int x, int y);
    void addConstraint(int x, int y, const std::function<bool(int,int)>& validPair);
    void addConstraint(std::pair<int,int> pair) {return addConstraint(pair.first, pair.second);}
    void addConstraint(std::pair<int,int> pair, const std::function<bool(int,int)>& validPair) {return addConstraint(pair.first, pair.second, validPair);}
    void addIntensiveConstraint(int x, int y, const std::function<bool(int,int)>& validPair, bool symetricFunction=false);
    
    void addConstraintValuePair(int x, int y, int a, int b);
    void removeConstraintValuePair(int x, int y, int a, int b);

    const std::unordered_set<int>& getVariables() const{return variables;}
    const std::unordered_map<int,std::unordered_set<int>>& getDomains() const{return domains;}
    const std::unordered_map<int,std::unordered_map<int,std::unique_ptr<Constraint>>>& getConstraints() const{return constraints;}
   
    const std::unordered_set<int>& getDomain(int var) const{return domains.at(var);}
    size_t getDomainSize(int var) const{return domains.at(var).size();}

    bool feasible(const std::unordered_map<int,int>& partSol) const;
    // Check if the added variable do not produce infeasibility with the given value
    // assuming the partial solution without this variable is feasible
    bool feasible(const std::unordered_map<int,int>& partSol, int var, int value) const;
    
    void init(ColorProblem problem, int nbColors);
    void init(QueenProblem problem);
    void init(SudokuProblem problem);

    void cleanConstraints();
    
    void addAC4List(int x, int a) {AC4List.emplace(std::make_pair(x, a));}
    void removeAC4List(int x, int a) {AC4List.erase(std::make_pair(x, a));}
    void initAC4();
    void AC4();
    bool checkAC();
    void display(bool removeSymmetry = true) const;
};

#endif