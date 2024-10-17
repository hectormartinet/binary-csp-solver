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

struct BlockedQueenProblem {
    int nb_queens;
    std::vector<std::pair<int,int>> blockedSquares;
};

struct SudokuProblem {
    unsigned int n;
    std::vector<std::vector<int>> grid;
};

#endif