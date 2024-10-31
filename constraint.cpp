#include<iostream>

#include "constraint.h"


ExtensiveConstraint::ExtensiveConstraint(int _x, int _y, const std::vector<std::pair<int,int>>& pairs): Constraint(_x,_y) {
    for (const auto& [a,b] : pairs) {
        addPair(a,b);
    }
}

void ExtensiveConstraint::addPair(int a, int b) {
    if (list.count(a)==0)
        list.emplace(a,std::unordered_set<int>());
    list.at(a).emplace(b);
}

void ExtensiveConstraint::removePair(int a, int b) {
    if (list.count(a)==0) return;
    list.at(a).erase(b);
}

std::vector<std::pair<int,int>> ExtensiveConstraint::getUselessPairs(const std::unordered_set<int>& Dx) const{
    std::vector<std::pair<int,int>> uselessPairs;
    for (const auto& [a,bSet]:list) {
        if (!Dx.count(a)) {
            for (int b : bSet) {
                uselessPairs.push_back(std::make_pair(a,b));
            }
        }
    }
    return uselessPairs;
}

std::vector<int> ExtensiveConstraint::getForbiddenValues(int a, const std::unordered_set<int>& Dy) {
    std::vector<int> forbiddenValues;
    for (int b : Dy) {
        if (!feasible(a,b)) forbiddenValues.push_back(b);
    }
    return forbiddenValues;
}

void ExtensiveConstraint::display() const {
    std::cout << x << "," << y << ":";
    for (const auto& [a,bSet] : list) {
        for (int b : bSet) {
            std::cout << "(" << a << "," << b << "),";
        }
    }
    std::cout << std::endl;
}

std::unique_ptr<Constraint> IntensiveConstraint::extensify(const std::unordered_set<int>& Dx, const std::unordered_set<int>& Dy) {
    ExtensiveConstraint* constraint = new ExtensiveConstraint(x,y);
    for (int a : Dx) {
        for (int b : Dy) {
            if (feasible(a,b)) constraint->addPair(a,b);
        }
    }
    return std::unique_ptr<ExtensiveConstraint> (constraint);
}

std::vector<int> IntensiveConstraint::getForbiddenValues(int a, const std::unordered_set<int>& Dy) {
    std::vector<int> forbiddenValues;
    for (int b : Dy) {
        if (!feasible(a,b)) forbiddenValues.push_back(b);
    }
    return forbiddenValues;
}


std::unique_ptr<Constraint> DifferenceConstraint::extensify(const std::unordered_set<int>& Dx, const std::unordered_set<int>& Dy) {
    ExtensiveConstraint* constraint = new ExtensiveConstraint(x,y);
    for (int a : Dx) {
        for (int b : Dy) {
            if (a!=b) constraint->addPair(a,b);
        }
    }
    return std::unique_ptr<ExtensiveConstraint> (constraint);
}

std::vector<int> DifferenceConstraint::getForbiddenValues(int a, const std::unordered_set<int>& Dy) {
    if (Dy.count(a)) return {a};
    return {};
}