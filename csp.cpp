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

void CSP::removeVariableValue(int var, int value) {
    if (domains.count(var)==0) return;
    domains.at(var).erase(value);
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

void CSP::initCount(int i, int j, int a, int val) {
    if (counts.count(i) == 0)
        counts.emplace(i,std::unordered_map<int,std::unordered_map<int, int>>());
    if (counts.at(i).count(j) == 0)
        counts.at(i).emplace(j,std::unordered_map<int, int>());
    if (counts.at(i).at(j).count(a)==0)
        counts.at(i).at(j).emplace(a, val);
    else 
        counts.at(i).at(j).at(a) = val;
}

void CSP::reduceCount(int i, int j, int a, int val) {
    counts.at(i).at(j).at(a)-=val;
}

int CSP::getCount(int i, int j, int a) {
    return counts.at(i).at(j).at(a);
}

void CSP::addSupport(int i, int a, int j, int b) {
    if (support.count(j) == 0)
        support.emplace(j,std::unordered_map<int,std::vector<std::pair<int,int>>>());
    if (support.at(j).count(b) == 0)
        support.at(j).emplace(b,std::vector<std::pair<int, int>>());
    support.at(j).at(b).push_back(std::make_pair(i, a));
}

void CSP::addAC4List(int i, int a) {
    AC4List.push_back(std::make_pair(i, a));
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
            if (problem.grid[i][j] <= 0) {
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
            for (int i2=i+1; i2<nInt; i++) {
                int var2Idx = nInt*i2+j;
                addConstraint(varIdx,var2Idx,lambdaDifferent);
            }

            // Column Constraint
            for (int j2=j+1; j<nInt; j++) {
                int var2Idx = nInt*i+j2;
                addConstraint(varIdx,var2Idx,lambdaDifferent);
            }

            // Square Constraint
            for (int i2=i; i2<sqrLen*(i/sqrLen+1); i2++) {
                for (int j2=j; j2<sqrLen*(j/sqrLen+1); j2++) {
                    if (i2==i && j2==j) continue;
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
            for (const auto& [a,bSet] : xyConstraint) {
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
    for (const auto& [x,iConstraints] : constraints) {
        for (const auto& [y,ijConstraint] : iConstraints) {
            for (int a : getDomain(x)) {
                int total = 0;
                for (int b : getDomain(y)) {
                    if (ijConstraint.feasible(a,b)) {
                        total++;
                        addSupport(x, a, y, b);
                    }
                }
                initCount(x, y, a, total);
                if (total==0) {
                    removeVariableValue(x, a);
                    addAC4List(x, a);
                }
            }
        }
    }   
}

std::pair<std::unordered_map<int,std::unordered_map<int,std::unordered_map<int,int>>>, std::vector<std::pair<int,int>>> CSP::AC4() {
    std::unordered_map<int, std::unordered_map<int,std::unordered_map<int,int>>> initCount;
    std::vector<std::pair<int,int>> removedValues;
    while (AC4List.size() > 0) {
        std::pair<int,int> firstCouple = AC4List.front();
        int y = firstCouple.first;
        int b = firstCouple.second;
        AC4List.erase(AC4List.begin());
        if (support.count(y) != 0 && support.at(y).count(b) != 0) {
            for (std::pair<int,int> secondCouple : support.at(y).at(b)){
                int x = secondCouple.first;
                int a = secondCouple.second;
                if (!initCount.count(x)) initCount.emplace(x, std::unordered_map<int, std::unordered_map<int,int>>());
                if (!initCount.at(x).count(y)) initCount.at(x).emplace(y, std::unordered_map<int,int>());
                if (!initCount.at(x).at(y).count(a)) initCount.at(x).at(y).emplace(a, getCount(x, y, a));

                reduceCount(x, y, a, 1);
                if (getCount(x, y, a) == 0 && isInDomain(x, a)) {
                    removeVariableValue(x, a);
                    addAC4List(x, a);
                    removedValues.push_back(std::make_pair(x, a));
                }

            }
        }
    }
    return std::make_pair(initCount, removedValues);
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