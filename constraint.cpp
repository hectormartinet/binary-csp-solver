#include<iostream>

#include "constraint.h"


ExtensiveConstraint::ExtensiveConstraint(int _x, int _y, const std::vector<std::pair<int,int>>& pairs):x{_x},y{_y} {
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


void ExtensiveConstraint::display() const {
    std::cout << x << "," << y << ":";
    for (const auto& [a,bSet] : list) {
        for (int b : bSet) {
            std::cout << "(" << a << "," << b << "),";
        }
    }
    std::cout << std::endl;
}