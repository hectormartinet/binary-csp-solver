#ifndef VALUE_CHOOSER_H_
#define VALUE_CHOOSER_H_

#include "csp.h"
#include <cassert>

class ValueChooser {
public:
    virtual ~ValueChooser(){};
    virtual std::vector<int> choose(const CSP& problem, int var)const=0;
};

class CopyValueChooser : public ValueChooser {
protected:
    std::vector<int> choose(const CSP& problem, int var) const{
        std::vector<int> order;
        order.insert(order.end(), problem.getDomain(var).begin(), problem.getDomain(var).end());
        return order;
    }
};

class SmallestValueChooser : public ValueChooser {
protected:
    std::vector<int> choose(const CSP& problem, int var) const{
        std::vector<int> order;
        order.insert(order.end(), problem.getDomain(var).begin(), problem.getDomain(var).end());
        std::sort(order.begin(), order.end());
        return order;
    }
};

class RandomValueChooser : public ValueChooser {
protected:
    std::vector<int> choose(const CSP& problem, int var) const{
        std::vector<int> order;
        order.insert(order.end(), problem.getDomain(var).begin(), problem.getDomain(var).end());
        std::random_shuffle (order.begin(), order.end());
        return order;
    }
};

class LambdaValueChooser : public ValueChooser {
    protected:
        std::function<bool(int,int)> sortFunction;
        std::vector<int> choose(const CSP& problem, int var) const{
            std::vector<int> order;
            order.insert(order.end(), problem.getDomain(var).begin(), problem.getDomain(var).end());
            std::sort(order.begin(), order.end(), sortFunction);
            return order;
        }
    public:
        LambdaValueChooser(std::function<bool(int,int)> _sortFunction) : ValueChooser(), sortFunction{_sortFunction} {};
};

#endif