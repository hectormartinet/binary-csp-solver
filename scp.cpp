#include<cmath>
#include<iostream>

#include "scp.h"

SCP::SCP(unsigned int nbVar) {
    domains = std::vector<std::set<int>>(nbVar, std::set<int>());
    constraints = std::map<unsigned int,std::map<unsigned int,Constraint>>();
}

bool SCP::feasible(const std::vector<int>& sol) const{
    
    // Checking Domains
    for (unsigned int i=0; i<nbVar(); i++)  {
        if (domains[i].count(sol[i])==0)
            return false;
    }

    // Checking constraints
    for (const auto& constraintLine : constraints) {
        for (const auto& constraint : constraintLine.second) {
            if (not constraint.second.feasible(sol))
                return false;
        }
    }

    return true;
}

void SCP::makeNQueen(){
    size_t n = nbVar();

    // Domains
    for (unsigned int i=0; i<n; i++) {
        for (int j=0; j<int(n); j++) {
            domains[i].emplace(j);
        }
    }

    // Constraints
    for (unsigned int i=0; i<n; i++) {
        constraints.emplace(i,std::map<unsigned int,Constraint>());
        for (unsigned int j=i+1; j<n; j++) {
            constraints.at(i).emplace(j,Constraint(i,j));
            Constraint* c = &constraints.at(i).at(j);
            for (int a=0; a<int(n); a++) {
                for (int b=0; b<int(n); b++) {
                    if (a!=b && std::abs(a-b)!=std::abs(int(i)-int(j)))
                        c->addPair(a,b);
                }
            }
        }
    }
}

void SCP::display() const {
    std::cout << "DOMAINS" << std::endl;
    for (unsigned int i=0; i<nbVar(); i++) {
        std::cout << i << ":";
        for (int j:domains[i]) {
            std::cout << j << ",";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "CONSTRAINTS" << std::endl;
    for (const auto& constraintLine : constraints) {
        for (const auto& constraint : constraintLine.second) {
            constraint.second.display();
        }
    }
}