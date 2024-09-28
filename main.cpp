#include <iostream>

#include "scp.h"

int main() {

    int n = 4;

    std::unordered_map<int,int> sol_ok = {{0,1},{1,3},{2,0},{3,2}};
    std::unordered_map<int,int> sol_ko = {{0,1},{1,2},{2,0},{3,2}}; // two queens on the same line...
    SCP queenProblem;
    queenProblem.makeNQueen(n);
    queenProblem.display();
    std::cout << queenProblem.feasible(sol_ok) << std::endl;
    std::cout << queenProblem.feasible(sol_ko) << std::endl;

    ProblemReader::ColorProblem instance = ProblemReader::readColorProblem("graph_color_instances/queen5_5.col");
    SCP colorProblem;
    colorProblem.init(instance,3);
    colorProblem.display();
    return 0;
}