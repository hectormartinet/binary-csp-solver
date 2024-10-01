#include <cmath>
#include <iostream>
#include <cassert>

#include "csp.h"

void CSP::addVariable(int i) {
    variables.emplace(i);
    if (domains.count(i) == 0)
        domains.emplace(i,std::unordered_set<int>());
}

void CSP::addVariableValue(int var, int value) {
    domains.at(var).emplace(value);
}

void CSP::addConstraint(int i, int j) {
    assert(i!=j);
    if (constraints.count(i) == 0)
        constraints.emplace(i,std::unordered_map<int,Constraint>());
    if (constraints.at(i).count(j) == 0)
        constraints.at(i).emplace(j,Constraint(i,j));
    
    // add the symmetric constraint
    if (constraints.count(j) == 0)
        constraints.emplace(j,std::unordered_map<int,Constraint>());
    if (constraints.at(j).count(i) == 0)
        constraints.at(j).emplace(i,Constraint(j,i));
}

void CSP::addConstraintValuePair(int i, int j, int a, int b) {
    constraints.at(i).at(j).addPair(a,b);
    constraints.at(j).at(i).addPair(b,a);// add symmetric constraint values
}

void CSP::removeConstraintValuePair(int i, int j, int a, int b) {
    constraints.at(i).at(j).removePair(a,b);
    constraints.at(j).at(i).removePair(b,a);
}

bool CSP::feasible(const std::unordered_map<int,int>& partSol) const{

    // Checking Domains
    for (const auto& [var,value] : partSol)  {
        if (domains.at(var).count(value)==0)
            return false;
    }

    // Checking constraints
    for (const auto& [i,iConstraints] : constraints) {
        if (partSol.count(i)==0) continue;
        for (const auto& [j,ijConstraint] : iConstraints) {
            if (i>j) continue; // do not check the symmetric version of the constraint
            if (partSol.count(j)==0) continue;
            if (!ijConstraint.feasible(partSol))
                return false;
        }
    }

    return true;
}

void CSP::init(QueenProblem problem){
    int n = problem.nb_queens;

    // Domains
    for (int var=0; var<n; var++) {
        addVariable(var);
        for (int value=0; value<n; value++) {
            addVariableValue(var,value);
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

void CSP::init(ColorProblem problem, int nbColors) {

    // Domains
    // Conventions on files -> starting from 1
    for (int var=1; var<=problem.nb_nodes; var++) {
        addVariable(var);
        for (int col=0; col<nbColors; col++) {
            addVariableValue(var,col);
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

void CSP::display(bool removeSymmetry) const {
    std::cout << "VARIABLES" << std::endl;
    for (int var : variables) {
        std::cout << var << ",";
    }
    std::cout<< std::endl << std::endl;

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
            if (i>j && removeSymmetry) continue;
            ijConstraints.display();
        }
    }
}