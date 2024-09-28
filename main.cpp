#include <iostream>
#include <cassert>

#include "scp.h"

int main() {

    SCP queenProblem;
    queenProblem.makeNQueen(4);
    queenProblem.display();
    std::unordered_map<int,int> sol_ok = {{0,1},{1,3},{2,0},{3,2}};
    std::unordered_map<int,int> sol_ko = {{0,1},{1,2},{2,0},{3,2}}; // two queens on the same line...
    assert(queenProblem.feasible(sol_ok));
    assert(!queenProblem.feasible(sol_ko));

    ProblemReader::ColorProblem instance = ProblemReader::readColorProblem("graph_color_instances/myciel3.col");
    SCP colorProblem;
    colorProblem.init(instance,3);
    colorProblem.display();
    return 0;
}