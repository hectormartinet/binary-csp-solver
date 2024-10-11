#include <cmath>
#include <iostream>
#include <cassert>
#include <typeinfo>

#include "csp.h"

CSP::CSP(const CSP& csp) {
    variables = csp.getVariables();
    domains = csp.getDomains();
    for (const auto& [x,Cx]: csp.getConstraints()) {
        constraints.emplace(x,std::unordered_map<int,std::unique_ptr<Constraint>>());
        for (const auto& [y,Cxy]: Cx) {
            constraints.at(x).emplace(y,Cxy->clone()); // Copy unique ptrs
        }
    }
}

void CSP::addVariable(int var) {
    variables.emplace(var);
    if (domains.count(var) == 0)
        domains.emplace(var,std::unordered_set<int>());
}

void CSP::addVariableValue(int var, int value) {
    assert(domains.count(var));
    domains.at(var).emplace(value);
}

bool CSP::removeVariableValue(int var, int value) {
    return domains.at(var).erase(value);
}

bool CSP::isInDomain(int var, int value) {
    return domains.at(var).count(value);
}

void CSP::fixValue(int var, int value) {
    // assert(isInDomain(var, value));
    domains.at(var) = std::unordered_set<int> {value};
}

void CSP::addConstraint(int x, int y) {
    assert(x!=y);
    if (constraints.count(x) == 0)
        constraints.emplace(x,std::unordered_map<int,std::unique_ptr<Constraint>>());
    if (constraints.at(x).count(y) == 0)
        constraints.at(x).emplace(y,std::make_unique<ExtensiveConstraint>(x,y));
    
    // add the symmetric constraint
    if (constraints.count(y) == 0)
        constraints.emplace(y,std::unordered_map<int,std::unique_ptr<Constraint>>());
    if (constraints.at(y).count(x) == 0)
        constraints.at(y).emplace(x,std::make_unique<ExtensiveConstraint>(y,x));
}

void CSP::addConstraint(int x, int y, const std::function<bool(int,int)>& validPair) {
    addConstraint(x,y);
    for (int a : domains.at(x)) {
        for (int b : domains.at(y)) {
            if (validPair(a,b)) {
                addConstraintValuePair(x,y,a,b);
            }
        }
    }
}

void CSP::addIntensiveConstraint(int x, int y, const std::function<bool(int,int)>& validPair, bool symetricFunction) {
    assert(x!=y);
    if (constraints.count(x) == 0)
        constraints.emplace(x,std::unordered_map<int,std::unique_ptr<Constraint>>());
    if (constraints.at(x).count(y) == 0)
        constraints.at(x).emplace(y,std::make_unique<IntensiveConstraint>(x,y,validPair));
    
    // add the symmetric constraint
    if (constraints.count(y) == 0)
        constraints.emplace(y,std::unordered_map<int,std::unique_ptr<Constraint>>());
    if (constraints.at(y).count(x) == 0) {
        if (symetricFunction)
            constraints.at(y).emplace(x,std::make_unique<IntensiveConstraint>(y,x,validPair));
        else 
            constraints.at(y).emplace(x,std::make_unique<IntensiveConstraint>(y,x,[validPair] (int a, int b){return validPair(a,b);}));
    }
}

void CSP::addConstraintValuePair(int x, int y, int a, int b) {
    constraints.at(x).at(y)->addPair(a,b);
    constraints.at(y).at(x)->addPair(b,a);// add symmetric constraint values
}

void CSP::removeConstraintValuePair(int x, int y, int a, int b) {
    constraints.at(x).at(y)->removePair(a,b);
    constraints.at(y).at(x)->removePair(b,a);// remove symmetric constraint values
}

bool CSP::feasible(const std::unordered_map<int,int>& partSol) const{

    // Checking Domains
    for (const auto& [var,value] : partSol)  {
        if (domains.at(var).count(value)==0)
            return false;
    }

    // Checking constraints
    for (const auto& [x,Cx] : constraints) {
        if (partSol.count(x)==0) continue;
        for (const auto& [y,Cxy] : Cx) {
            if (x>y) continue; // do not check the symmetric version of the constraint
            if (partSol.count(y)==0) continue;
            if (!Cxy->feasible(partSol)) return false;
        }
    }

    return true;
}

bool CSP::feasible(const std::unordered_map<int,int>& partSol, int var, int value) const{
    
    if (domains.at(var).count(value)==0) return false;
    if (constraints.count(var)==0) return true;
    
    for (const auto& [j,constraint] : constraints.at(var)) {
        if (partSol.count(j)==0) continue;
        if (!constraint->feasible(value,partSol.at(j))) return false;
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
    for (int x=0; x<n; x++) {
        for (int y=x+1; y<n; y++) {
            addConstraint(x,y,[&](int a, int b) {return a!=b && std::abs(a-b)!=std::abs(x-y);});
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

            // Column constraint
            for (int j2=j+1; j2<nInt; j2++) {
                int var2Idx = nInt*i+j2;
                addConstraint(varIdx,var2Idx,lambdaDifferent);
            }

            // Square constraint
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
    for (const auto& [x,Cx] : constraints) {
        for (const auto& [y,Cxy] : Cx) {
            for (auto [a,b] : Cxy->getUselessPairs(getDomain(x))) {
                removeConstraintValuePair(x,y,a,b);
            }
        }
    }
}

void CSP::initAC4() {
    cleanConstraints();
    for (const auto& [x,Cx] : constraints) {
        for (const auto& [y,Cxy] : Cx) {
            for (int a : getDomain(x)) {
                if (Cxy->getSupportSize(a)==0) {
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
        for (const auto& [x, Cyx]:constraints.at(y)) {
            if (Cyx->getSupportSize(b)==0) continue;
            for (int a:Cyx->getSupport(b)) {
                toPropagate.push_back(std::make_pair(x,a));
            }
        }
        for (auto [x,a] : toPropagate) {
            removeConstraintValuePair(x,y,a,b);
            // Lazy evaluation
            if (constraints.at(x).at(y)->getSupportSize(a) == 0 && removeVariableValue(x,a)) {
                addAC4List(x,a);
            }
        }
    }
}

bool CSP::checkAC() {
    cleanConstraints();
    for (const auto& [x,Cx] : constraints) {
        for (const auto& [y,Cxy] : Cx) {
            for (int a : getDomain(x)) {
                if (Cxy->getSupportSize(a)==0) {
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
    for (const auto& [x,Cx] : constraints) {
        for (const auto& [y,Cxy] : Cx) {
            if (x>y && removeSymmetry) continue;
            Cxy->display();
        }
    }
    std::cout << std::endl;
}