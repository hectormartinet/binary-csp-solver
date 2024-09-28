#include <cmath>
#include <iostream>
#include <cassert>

#include "scp.h"

void SCP::addConstraint(int i, int j) {
    if (constraints.count(i) == 0)
        constraints.emplace(i,std::unordered_map<int,Constraint>());
    if (constraints.at(i).count(j) == 0)
        constraints.at(i).emplace(j,Constraint(i,j));
}

void SCP::addConstraintValuePair(int i, int j, int a, int b) {
    constraints.at(i).at(j).addPair(a,b);
}

bool SCP::feasible(const std::unordered_map<int,int>& partSol) const{

    // Checking Domains
    for (std::pair<int, int> keyValue : partSol)  {
        if (domains.at(keyValue.first).count(keyValue.second)==0)
            return false;
    }

    // Checking constraints
    for (const auto& [i,iConstraints] : constraints) {
        if (partSol.count(i)==0) continue;
        for (const auto& [j,ijConstraint] : iConstraints) {
            if (partSol.count(j)==0) continue;
            if (not ijConstraint.feasible(partSol))
                return false;
        }
    }

    return true;
}

void SCP::makeNQueen(int n){

    // Domains
    for (int i=0; i<n; i++) {
        domains.emplace(i,std::unordered_set<int>());
        for (int j=0; j<n; j++) {
            domains.at(i).emplace(j);
        }
    }

    // Constraints
    for (int i=0; i<n; i++) {
        for (int j=i+1; j<n; j++) {
            addConstraint(i,j);
            for (int a=0; a<n; a++) {
                for (int b=0; b<n; b++) {
                    if (a!=b && std::abs(a-b)!=std::abs(i-j))
                        addConstraintValuePair(i,j,a,b);
                }
            }
        }
    }
}

void SCP::init(ProblemReader::ColorProblem problem, int nbColors) {

    // Domains
    // Conventions on files -> starting from 1
    for (int i=1; i<=problem.nb_nodes; i++) {
        domains.emplace(i,std::unordered_set<int>());
        for (int col=0; col<nbColors; col++) {
            domains.at(i).emplace(col);
        }
    }

    //Constraint
    for (std::pair<int,int> edge : problem.edges) {
        addConstraint(edge);
        for (int col1=0; col1<nbColors; col1++) {
            for (int col2=0; col2<nbColors; col2++) {
                if (col1 != col2)
                    addConstraintValuePair(edge.first,edge.second,col1,col2);
            }
        }
    }
}

void SCP::display() const {
    std::cout << "DOMAINS" << std::endl;
    for (const auto&[var,domain]:domains) {
        std::cout << var << ":";
        for (int value:domain) {
            std::cout << value << ",";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "CONSTRAINTS" << std::endl;
    for (const auto& [i,iConstraints] : constraints) {
        for (const auto& [j,ijConstraints] : iConstraints) {
            ijConstraints.display();
        }
    }
}