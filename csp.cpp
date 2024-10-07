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
    assert(domains.count(var));
    domains.at(var).emplace(value);
}

bool CSP::removeVariableValue(int var, int value) {
    return domains.at(var).erase(value);
}

bool CSP::isInDomain(int var, int a) {
    return domains.at(var).count(a);
}

void CSP::fixValue(int var, int a) {
    assert(isInDomain(var, a));
    // alternative version
    // domains.at(var) = std::unordered_set<int> {a};
    for (int value : getDomain(var)) {
        if (value != a) {
            domains.at(var).erase(value);
        }
    }
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

void CSP::addConstraint(int i, int j, const std::function<bool(int,int)>& validPair) {
    addConstraint(i,j);
    for (int a : domains.at(i)) {
        for (int b : domains.at(j)) {
            if (validPair(a,b)) {
                constraints.at(i).at(j).addPair(a,b);
                // add the symmetric constraint
                constraints.at(j).at(i).addPair(b,a);
            }
        }
    }
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
            if (!ijConstraint.feasible(partSol)) return false;
        }
    }

    return true;
}

bool CSP::feasible(const std::unordered_map<int,int>& partSol, int var, int value) const{
    
    if (domains.at(var).count(value)==0) return false;
    if (constraints.count(var)==0) return true;
    
    for (const auto& [j,constraint] : constraints.at(var)) {
        if (partSol.count(j)==0) continue;
        if (!constraint.feasible(value,partSol.at(j))) return false;
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
            addConstraint(i,j,[&](int a, int b) {return a!=b && std::abs(a-b)!=std::abs(i-j);});
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

    auto lambdaDifferent = [](int a,int b) {return a != b;};

    //Constraints
    for (std::pair<int,int> edge : problem.edges) {
        addConstraint(edge,lambdaDifferent);
    }
}

void CSP::init(SudokuProblem problem) {
    assert(problem.grid.size() == problem.n);

    unsigned int n = problem.n;
    int nInt = int(n); // to calm down gcc warnings
    int sqrLen = int(sqrt(n));
    
    // Domains
    for (unsigned int i=0; i<n; i++) {
        assert(problem.grid[i].size() == n);
        for (unsigned int j=0; j<n; j++) {
            int varIdx = int(n*i+j);
            addVariable(varIdx);
            if (problem.grid[i][j] > 0) {
                assert(problem.grid[i][j] <= nInt);
                addVariableValue(varIdx,problem.grid[i][j]);
                continue;
            }
            for (int val=1; val<=(int)n; val++) {
                addVariableValue(varIdx,val);
            }
        }
    }

    auto lambdaDifferent = [](int a,int b) {return a != b;};

    // Constraints
    for (int i=0; i<nInt; i++) {
        for (int j=0; j<nInt; j++) {
            int varIdx = nInt*i+j;

            // Line constraint
            for (int i2=i+1; i2<nInt; i2++) {
                int var2Idx = nInt*i2+j;
                addConstraint(varIdx,var2Idx,lambdaDifferent);
            }

            // Column Constraint
            for (int j2=j+1; j2<nInt; j2++) {
                int var2Idx = nInt*i+j2;
                addConstraint(varIdx,var2Idx,lambdaDifferent);
            }

            // Square Constraint
            for (int i2=i+1; i2<sqrLen*(i/sqrLen+1); i2++) {
                int j_start = j-j%sqrLen;
                for (int j2=j_start; j2<sqrLen*(j/sqrLen+1); j2++) {
                    if (j2==j) continue;
                    int var2Idx = nInt*i2+j2;
                    addConstraint(varIdx,var2Idx,lambdaDifferent);
                }
            }
        }
    }
}

void CSP::cleanConstraints(){
    for (const auto& [x,xConstraints] : constraints) {
        for (const auto& [y,xyConstraint] : xConstraints) {
            for (const auto& [a,bSet] : xyConstraint.getConstraints()) {
                if (!isInDomain(x,a)) {
                    for (int b : bSet) {
                        removeConstraintValuePair(x,y,a,b);
                    }
                }
            }
        }
    }
}

void CSP::initAC4() {
    cleanConstraints();
    for (const auto& [x,xConstraints] : constraints) {
        for (const auto& [y,xyConstraint] : xConstraints) {
            for (int a : getDomain(x)) {
                if (xyConstraint.supportSize(a)==0) {
                    addAC4List(x, a);
                }
            }
        }
    }
    for (auto [x,a]:AC4List) {   
        removeVariableValue(x,a);
    }
}

void CSP::AC4() {
    while(!AC4List.empty()) {
        auto [y,b] = *AC4List.begin();
        removeAC4List(y,b);
        std::vector<std::pair<int,int>> toPropagate;
        for (const auto& [x, yxConstraint]:constraints.at(y)) {
            if (yxConstraint.supportSize(b)==0) continue;
            for (int a:yxConstraint.getConstraints(b)) {
                toPropagate.push_back(std::make_pair(x,a));
            }
        }
        for (auto [x,a] : toPropagate) {
            removeConstraintValuePair(x,y,a,b);
            // Lazy evaluation
            if (constraints.at(x).at(y).supportSize(a) == 0 && removeVariableValue(x,a)) {
                addAC4List(x,a);
            }
        }
    }
}

bool CSP::checkAC() {
    cleanConstraints();
    for (const auto& [x,xConstraints] : constraints) {
        for (const auto& [y,xyConstraint] : xConstraints) {
            for (int a : getDomain(x)) {
                if (xyConstraint.supportSize(a)==0) {
                    return false;
                }
            }
        }
    }
    return true;
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
    std::cout << std::endl;
}