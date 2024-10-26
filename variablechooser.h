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
        int best_var = 0;
        size_t best_size = INT_MAX;
        for (int var : variables) {
            size_t size = problem.getDomainSize(var);
            if (size < best_size) {
                best_size = size;
                best_var = var;
            } 
        }
        return best_var;
    }
};

#endif