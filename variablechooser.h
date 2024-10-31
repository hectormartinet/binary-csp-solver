#ifndef VARIABLE_CHOOSER_H_
#define VARIABLE_CHOOSER_H_

#include "csp.h"
#include <cassert>

class VariableChooser {
public:
    virtual ~VariableChooser(){};
    virtual int choose(const CSP& problem, const std::unordered_set<int>& variables)const=0;
};

class RandomVariableChooser : public VariableChooser {
protected:
    int choose(const CSP&, const std::unordered_set<int>& variables) const{
        int idx = rand()%int(variables.size());
        int i = 0;
        for (int var : variables) {
            if (i++==idx) return var;
        }
        assert(false);
        return INT_MAX;
    }
};

class SmallestDomainVariableChooser : public VariableChooser {
protected:
    int choose(const CSP& problem, const std::unordered_set<int>& variables) const{
        int bestVar = 0;
        size_t bestSize = INT_MAX;
        for (int var : variables) {
            size_t size = problem.getDomainSize(var);
            if (size < bestSize) {
                bestSize = size;
                bestVar = var;
            } 
        }
        return bestVar;
    }
};

class MaxConstraintVariableChooser : public VariableChooser {
protected:
    int choose(const CSP& problem, const std::unordered_set<int>& variables) const{
        std::unordered_map<int, int> countVariables;
        for (const auto &x : variables) {
            if (countVariables.count(x) == 0) countVariables.emplace(x, 0);
            for (const auto&[y, Cxy] : problem.getConstraints().at(x)) {
                if (x < y && variables.count(y)) {
                    countVariables.at(x)++;
                    if (countVariables.count(y) == 0)
                        countVariables.emplace(y, 1);
                    else
                        countVariables.at(y)++;
                }
            }
        }
        int bestVar = -1;
        int nbAssocConstraints = -1;
        for (const auto& [var,value] : countVariables) {
            if (value > nbAssocConstraints) {
                nbAssocConstraints = value;
                bestVar = var;
            }
        }
        return bestVar;
    }
};

#endif