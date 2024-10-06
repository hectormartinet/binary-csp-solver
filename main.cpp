#include <iostream>
#include <cassert>

#include "solver.h"

int main() {
    int n=75;
    clock_t time_req = clock();

    CSP queenProblem;
    queenProblem.init(QueenProblem{n});
    // queenProblem.display();
    Solver solver(queenProblem);
    if (solver.solve()) {
        std::cout << "Explored " << solver.getNbNodesExplored() << " nodes" << std::endl;
        std::cout << "Found solution" << std::endl;
        auto sol = solver.retrieveSolution();
        assert(queenProblem.feasible(sol));
        solver.displaySolution();
    } else {
        std::cout << "No solution found" << std::endl;
    }

    time_req = clock()- time_req;
    std::cout << (float)time_req/CLOCKS_PER_SEC << std::endl;

    // ColorProblem instance = ProblemReader::readColorProblem("graph_color_instances/myciel3.col");
    // CSP colorProblem;
    // colorProblem.init(instance,3);
    // colorProblem.display();
    return 0;
}