#include <cmath>
#include <iostream>
#include <cassert>
#include <typeinfo>
#include <fstream>

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
    nConstraints = csp.nbConstraints();
    allDifferentFamilies = csp.allDifferentFamilies;
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

void CSP::addVariableRange(int var, int start, int end) {
    for (int value=start; value<end; value++) {
        addVariableValue(var,value);
    }
}

bool CSP::removeVariableValue(int var, int value) {
    return domains.at(var).erase(value);
}

bool CSP::isInDomain(int var, int value) {
    return domains.at(var).count(value);
}

void CSP::fixValue(int var, int value) {
    domains.at(var) = std::unordered_set<int> {value};
}

void CSP::addConstraint(int x, int y) {
    assert(x!=y);
    if (constraints.count(x) == 0)
        constraints.emplace(x,std::unordered_map<int,std::unique_ptr<Constraint>>());
    if (constraints.at(x).count(y) == 0) {
        constraints.at(x).emplace(y,std::make_unique<ExtensiveConstraint>(x,y));
        nConstraints++;
    }
    
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
    if (constraints.at(x).count(y) == 0) {
        constraints.at(x).emplace(y,std::make_unique<IntensiveConstraint>(x,y,validPair));
        nConstraints++;
    }

    // add the symmetric constraint
    if (constraints.count(y) == 0)
        constraints.emplace(y,std::unordered_map<int,std::unique_ptr<Constraint>>());
    if (constraints.at(y).count(x) == 0) {
        if (symetricFunction)
            constraints.at(y).emplace(x,std::make_unique<IntensiveConstraint>(y,x,validPair));
        else 
            constraints.at(y).emplace(x,std::make_unique<IntensiveConstraint>(y,x,[validPair] (int b, int a){return validPair(a,b);}));
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

void CSP::addAllDifferentConstraint(const std::vector<int>& vars) {
    std::function<bool(int,int)> lambdaDifferent = [](int a, int b) {return a != b;};
    for (unsigned int i=0; i<vars.size(); i++) {
        for (unsigned int j=i+1; j<vars.size(); j++) {
            addConstraint(vars[i], vars[j], lambdaDifferent);
        }
    }
    addAllDifferentFamily(vars);
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

void CSP::init(const QueenProblem& problem){
    int n = problem.nb_queens;

    // Domains
    for (int var=0; var<n; var++) {
        addVariable(var);
        addVariableRange(var,0,n);
    }

    // Constraints
    for (int x=0; x<n; x++) {
        for (int y=x+1; y<n; y++) {
            addIntensiveConstraint(x,y, [x, y](int a, int b) {return a!=b && std::abs(a-b)!=std::abs(x-y);}, true);
        }
    }

    std::vector<int> vars((unsigned int)(n),0);
    std::iota(vars.begin(), vars.end(), 0);
    addAllDifferentFamily(vars);
}

void CSP::init(const BlockedQueenProblem& problem){
    int n = problem.nb_queens;

    // Domains
    for (int var=0; var<n; var++) {
        addVariable(var);
        addVariableRange(var, 0, n);
    }

    for (auto [var,value] : problem.blockedSquares) {
        removeVariableValue(var,value);
    }

    // Constraints
    for (int x=0; x<n; x++) {
        for (int y=x+1; y<n; y++) {
            addIntensiveConstraint(x,y,[x,y](int a, int b) {return a!=b && std::abs(a-b)!=std::abs(x-y);}, true);
        }
    }

    std::vector<int> vars((unsigned int)(n),0);
    std::iota(vars.begin(), vars.end(), 0);
    addAllDifferentFamily(vars);
}

void CSP::readProblemType(std::string path) {
    std::ifstream inputFile(path);
    if (!inputFile.is_open()) {
        throw std::logic_error("Cannot open file - check the path you gave me");
    }
    std::string type;
    std::getline(inputFile, type);
    if (type == "queens") problemType = Problem::Queens;
    if (type == "blocked_queens") problemType = Problem::BlockedQueens;
    if (type == "sudoku") problemType = Problem::Sudoku;
    if (type == "color") problemType = Problem::Color;
    if (type == "nonogram") problemType = Problem::Nonogram; 
    if (type == "generic") problemType = Problem::Generic;
}

void CSP::init(std::string path) {
    readProblemType(path);
    std::cout << "Generating csp..." << std::endl;
    switch (problemType) 
    {
    case Problem::Queens: return init(ProblemReader::readQueenProblem(path));
    case Problem::BlockedQueens: return init(ProblemReader::readBlockedQueenProblem(path));
    case Problem::Color: return init(ProblemReader::readColorProblem(path));
    case Problem::Sudoku: return init(ProblemReader::readSudokuProblem(path));
    case Problem::Nonogram: return init(ProblemReader::readNonogramProblem(path));
    case Problem::Generic: return init(ProblemReader::readGenericProblem(path));
    default: std::cerr << "Wrong model" << path << std::endl;
    }
}

void CSP::init(const ColorProblem& problem) {
    int nbColors = problem.nb_nodes;
    nbColors = std::min(nbColors,int(0.5*(std::sqrt(4*problem.nb_edges+1))));
    std::vector<int> degrees((std::size_t) problem.nb_nodes,0);
    for (std::pair<int,int> edge : problem.edges) {
        degrees[(unsigned int)(edge.first)]++;
        degrees[(unsigned int)(edge.second)]++;
    }
    int degreeMax = 0;
    for (int d:degrees) {
        degreeMax = std::max(degreeMax, d);
    }
    nbColors = std::min(nbColors,degreeMax+1);
    // Domains
    // Conventions on files -> starting from 1
    for (int var=1; var<=problem.nb_nodes; var++) {
        addVariable(var);
        addVariableRange(var, 0, nbColors);
    }

    auto lambdaDifferent = [](int a,int b) {return a != b;};

    //Constraints
    for (std::pair<int,int> edge : problem.edges) {
        addIntensiveConstraint(edge, lambdaDifferent, true);
    }
}

void CSP::init(const SudokuProblem& problem) {
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
            addVariableRange(varIdx, 1, nInt + 1);
        }
    }

    // Constraints
    // Lines
    for (int i=0; i<nInt; i++) {
        std::vector<int> vars;
        for (int j=0; j<nInt; j++) {
            vars.push_back(nInt*i+j);
        }
        addAllDifferentConstraint(vars);
    }

    // Columns
    for (int j=0; j<nInt; j++) {
        std::vector<int> vars;
        for (int i=0; i<nInt; i++) {
            vars.push_back(nInt*i+j);
        }
        addAllDifferentConstraint(vars);
    }

    // Squares
    for (int iSqr=0; iSqr<sqrLen; iSqr++){
        for (int jSqr=0; jSqr<sqrLen; jSqr++){
            std::vector<int> vars;
            for (int i=iSqr*sqrLen; i<(iSqr+1)*sqrLen; i++){
                for (int j=jSqr*sqrLen; j<(jSqr+1)*sqrLen; j++){
                    vars.push_back(nInt*i+j);
                }
            }
            addAllDifferentConstraint(vars);
        }
    }
}

void CSP::init(const NonogramProblem& problem) {
    std::vector<std::vector<std::vector<bool>>> allVerticalPossibilities;
    std::vector<std::vector<std::vector<bool>>> allHorizontalPossibilities;

    int w = int(problem.w);
    int h = int(problem.h);

    // Domains
    for (int i=0; i<w; i++) {
        allVerticalPossibilities.push_back(problem.getAllPossible(problem.verticalClues[(unsigned int)(i)], problem.h));
        addVariable(i);
        addVariableRange(i, 0, int(allVerticalPossibilities.back().size()));
    }
    for (int j=0; j<h; j++) {
        allHorizontalPossibilities.push_back(problem.getAllPossible(problem.horizontalClues[(unsigned int)(j)], problem.w));
        addVariable(w + j);
        addVariableRange(w + j, 0, int(allHorizontalPossibilities.back().size()));
    }

    // Constraints
    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            std::vector<bool> iValues(getDomainSize(i));
            for (int a : getDomain(i)) {
                iValues[(unsigned int)(a)] = allVerticalPossibilities[(unsigned int)(i)][(unsigned int)(a)][(unsigned int)(j)];
            }
            std::vector<bool> jValues(getDomainSize(w+j));
            for (int b : getDomain(w+j)) {
                jValues[(unsigned int)(b)] = allHorizontalPossibilities[(unsigned int)(j)][(unsigned int)(b)][(unsigned int)(i)];
            }
            addIntensiveConstraint(i, w + j, [iValues,jValues](int a, int b) {return iValues[(unsigned int)(a)] == jValues[(unsigned int)(b)];});
        }
    }
}

void CSP::init(const GenericProblem& problem) {

    // Variables & Domains
    for (unsigned int i=0; i<problem.nbVar; i++) {
        int var = problem.variables[i];
        addVariable(var);
        for (int value:problem.domains[i]) {
            addVariableValue(var,value);
        }
    }

    // Constraints
    for (unsigned int i=0; i<problem.nbConstr; i++) {
        std::pair<int,int> vars = problem.constrVar[i];
        addConstraint(vars);
        for (std::pair<int,int> values : problem.constrVal[i]) {
            addConstraintValuePair(vars,values);
        }
    }
}

void CSP::extensify() {
    for (auto& [x,Cx] : constraints) {
        for (auto& [y,Cxy] : Cx) {
            if (!Cxy->isExtensive) {
                Cxy = Cxy->extensify(domains.at(x), domains.at(y));
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
    for (const auto& [x,Cx] : constraints) {
        for (const auto& [y,Cxy] : Cx) {
            if (x>y && removeSymmetry) continue;
            Cxy->display();
        }
    }
    std::cout << std::endl;
}

std::vector<int> CSP::getDomainCopy(int var) const {
    std::vector<int> copy(domains.at(var).size());
    std::copy(domains.at(var).begin(), domains.at(var).end(), copy.begin());
    return copy;
}