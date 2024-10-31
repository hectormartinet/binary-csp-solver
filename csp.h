#ifndef CSP_H_
#define CSP_H_

#include <unordered_set>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <thread>
#include <optional>
#include "constraint.h"
#include "problemreader.h"
#include <random>

enum class Problem {Queens, BlockedQueens, Color, Sudoku, Nonogram, Generic};
class CSP {

private:
    std::unordered_set<int> variables;
    std::unordered_map<int,std::unordered_set<int>> domains;
    std::unordered_map<int,std::unordered_map<int,std::unique_ptr<Constraint>>> constraints;
    Problem problemType;

    std::vector<std::vector<int>> allDifferentFamilies;

    unsigned int nConstraints=0;
public:

    CSP(){};
    CSP(std::string path){init(path);}
    CSP(const CSP& csp); // copy constructor

    std::size_t nbVar() const{return domains.size();}
    std::size_t sizeDomain(int var) const{return domains.at(var).size();}
    unsigned int nbConstraints() const {return nConstraints;}

    void addVariable(int var);
    void addVariableValue(int var, int value);
    void addVariableRange(int var, int start, int end);
    bool removeVariableValue(int var, int value);
    void fixValue(int var, int value);
    bool isInDomain(int var, int value);
    void addConstraint(int x, int y);
    void addConstraint(int x, int y, const std::function<bool(int,int)>& validPair);
    void addConstraint(std::pair<int,int> pair) {return addConstraint(pair.first, pair.second);}
    void addConstraint(std::pair<int,int> pair, const std::function<bool(int,int)>& validPair) {return addConstraint(pair.first, pair.second, validPair);}
    void addIntensiveConstraint(int x, int y, const std::function<bool(int,int)>& validPair, bool symetricFunction=false, const std::optional<std::function<std::vector<int>(int)>>& forbiddenValuesFunction={});
    void addIntensiveConstraint(std::pair<int,int> pair, const std::function<bool(int,int)>& validPair, bool symetricFunction=false, const std::optional<std::function<std::vector<int>(int)>>& forbiddenValuesFunction={}) {return addIntensiveConstraint(pair.first, pair.second, validPair, symetricFunction, forbiddenValuesFunction);};
    void addDifferenceConstraint(int x, int y);
    void addDifferenceConstraint(std::pair<int,int> pair) {return addDifferenceConstraint(pair.first, pair.second);};
    void addAllDifferentConstraint(const std::vector<int>& vars);
    void addAllDifferentFamily(const std::vector<int>& vars) {allDifferentFamilies.push_back(vars);}

    void addConstraintValuePair(int x, int y, int a, int b);
    void addConstraintValuePair(std::pair<int,int> vars, std::pair<int,int> values) {return addConstraintValuePair(vars.first, vars.second, values.first, values.second);}
    void removeConstraintValuePair(int x, int y, int a, int b);

    const std::unordered_set<int>& getVariables() const{return variables;}
    const std::unordered_map<int,std::unordered_set<int>>& getDomains() const{return domains;}
    const std::unordered_map<int,std::unordered_map<int,std::unique_ptr<Constraint>>>& getConstraints() const{return constraints;}
   
    const std::unordered_set<int>& getDomain(int var) const{return domains.at(var);}
    std::vector<int> getDomainCopy(int var) const;
    size_t getDomainSize(int var) const{return domains.at(var).size();}

    const std::vector<std::vector<int>>& getAllDifferentFamilies() {return allDifferentFamilies;}

    bool feasible(const std::unordered_map<int,int>& partSol) const;
    // Check if the added variable do not produce infeasibility with the given value
    // assuming the partial solution without this variable is feasible
    bool feasible(const std::unordered_map<int,int>& partSol, int var, int value) const;

    void readProblemType(std::string path);
    void init(std::string path);
    void init(const ColorProblem& problem);
    void init(const QueenProblem& problem);
    void init(const BlockedQueenProblem& problem);
    void init(const SudokuProblem& problem);
    void init(const NonogramProblem& problem);
    void init(const GenericProblem& problem);

    void extensify();
    
    void display(bool removeSymmetry = true) const;
};

#endif