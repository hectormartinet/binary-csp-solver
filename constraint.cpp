#include<iostream>

#include "constraint.h"


Constraint::Constraint(unsigned int _x, unsigned int _y, const std::vector<std::pair<int,int>>& pairs):x{_x},y{_y} {
    for (std::pair<int,int> pair : pairs) {
        addPair(pair.first,pair.second);
    }
}

void Constraint::addPair(int a, int b) {
    if (list.count(a)==0)
        list.emplace(a,std::set<int>());
    list.at(a).emplace(b);
}

bool Constraint::feasible(int a, int b) const{
    return list.count(a)>0 && list.at(a).count(b);
}

void Constraint::display() const {
    std::cout << x << "," << y << ":";
    for (const auto& iter : list) {
        for (int b : iter.second) {
            std::cout << "(" << iter.first << "," << b << "),";
        }
    }
    std::cout << std::endl;
}