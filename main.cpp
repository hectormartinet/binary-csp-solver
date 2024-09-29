#include <iostream>
#include <cassert>

#include "solver.h"

int main() {
    int n = 4;

    CSP queenProblem;
    queenProblem.makeNQueen(n);
    queenProblem.display();
    Solver solver(queenProblem);
    if (solver.solve()) {
        std::cout << "found solution" << std::endl;
        auto sol = solver.retrieveSolution();
        assert(queenProblem.feasible(sol));
        solver.displaySolution();
    } else {
        std::cout << "no solution found" << std::endl;
    }

    // ProblemReader::ColorProblem instance = ProblemReader::readColorProblem("graph_color_instances/myciel3.col");
    // CSP colorProblem;
    // colorProblem.init(instance,3);
    // colorProblem.display();
    return 0;
}