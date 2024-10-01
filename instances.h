#ifndef INSTANCES_H_
#define INSTANCES_H_

struct ColorProblem {
    int nb_nodes;
    int nb_edges;
    std::vector<std::pair<int,int>> edges;
};

struct QueenProblem {
    int nb_queens;
};

#endif