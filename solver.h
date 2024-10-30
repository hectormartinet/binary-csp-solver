#ifndef SOLVER_H_
#define SOLVER_H_

#include "csp.h"
#include "variablechooser.h"
#include "valuechooser.h"
#include "alldifferentfamily.h"

#include <memory>  

struct PairHash {
public:
    size_t hash_combine(size_t seed, int v) const {
        return seed ^= std::hash<int>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    std::size_t operator()(const std::pair<int, int> &pair) const
    {
        return hash_combine(std::hash<int>{}(pair.first),pair.second);
    }
};
enum class State {Preprocess, Solve, Stop};
enum class SolveMethod {ForwardChecking, LazyPropagate, AC4};

class Solver {

private:
    CSP problem;
    std::unique_ptr<VariableChooser> varChooser;
    std::unique_ptr<ValueChooser> valueChooser;
    SolveMethod rootSolveMethod;
    SolveMethod nodeSolveMethod;
    SolveMethod solveMethod;
    std::vector<std::string> parameters;
    bool verbosity=true;
    int timeLimit=INT_MAX;
    unsigned int randomSeed;
    unsigned int nbSolutions=1;

    std::unordered_map<int,int> setVariables;
    std::vector<std::vector<int>> deltaSetVars;
    std::unordered_set<int> unsetVariables;
    std::vector<std::vector<std::pair<int,int>>> deltaDomains;
    std::vector<std::vector<std::tuple<int, int, int, int>>> deltaConstrValPair;

    std::unordered_set<std::pair<int,int>,PairHash> AC4List;
    std::unordered_set<std::pair<int,int>,PairHash> lazyPropagateList;

    std::vector<AllDifferentFamily> allDifferentFamilies;
    std::unordered_map<int,std::vector<AllDifferentFamily*>> varToAllDifferent; 

    State state = State::Preprocess;
    unsigned int nbNodesExplored=0;
    int bestDepth=0;
    clock_t start_time;
    clock_t solve_time=0.;
    std::vector<std::unordered_map<int,int>> solutions;

public:
    Solver(CSP _problem, const std::vector<std::string> _parameters, bool _verbosity);
    Solver(CSP _problem);
    void translateParameters(const std::vector<std::string> _parameters);
    void setDefaultParameters();

    void setRootSolveMethod(const std::string _rootSolveMethod);
    void setNodeSolveMethod(const std::string _nodeSolveMethod);
    void setVarChooser(const std::string _varChooser);
    void setValChooser(const std::string _valChooser);
    void setValLambdaChooser(const std::function<bool(int,int)> lambda);
    void setRandomSeed(const unsigned int _randomSeed);
    void setTimeLimit(const int _timeLimit);
    void setVerbosity(const bool _verbosity) {verbosity=_verbosity;}
    void setNbSolutions(const unsigned int _nbSolutions);
    void initAllDifferent();

    bool feasible() const{return problem.feasible(setVariables);}
    bool feasible(int var, int value) const {return problem.feasible(setVariables,var,value);}
    void checkFeasibility(CSP _problem);
    void preprocess();
    bool presolve();
    void launchSolve();
    void timeThread();
    void branchOnVar(int var, int value);
    void unbranchOnVar(int var, std::vector<int> values);
    void solve();
    void backtrack();
    bool recursiveSolve();
    bool checkConsistent(int var, int value);
    
    bool forwardChecking(int x, int a);
    bool lazyPropagate(int x, int a);
    void removeLazyPropagateList(int x, int a);

    void cleanConstraints();
    bool initAC4Root();
    bool initAC4Solve(int var, int value, std::vector<int> values);    
    bool AC4();
    bool checkAC();
    void addAC4List(int x, int a) {AC4List.emplace(std::make_pair(x, a));}
    void removeAC4List(int x, int a);

    int chooseVar() {return varChooser->choose(problem,unsetVariables);}
    std::vector<int> chooseValue(int var) {return valueChooser->choose(problem,var);}

    void setVar(int var, int value);
    void unsetVar(int var);
    bool removeVarValue(int var, int value);
    void removeConstraintValuePair(int x, int y, int a, int b);
    std::unordered_map<int,int> retrieveSolution() const{return setVariables;}
    unsigned int getNbNodesExplored() const{return nbNodesExplored;}
    bool hasFoundSolution() const {return (solutions.size() > 0);}

    void solveVerbosity();
    void displayModelInformation() const;
    void displaySolveInformation() const;
    void displayFinalInformation() const;
    void displaySolution() const;
    void displayLogo() const;

};

#endif