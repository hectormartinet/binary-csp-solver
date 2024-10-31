#ifndef INSTANCES_H_
#define INSTANCES_H_

#include <bitset>
#include <vector>

struct GenericProblem {
    unsigned int nbVar;
    unsigned int nbConstr;
    std::vector<int> variables;
    std::vector<std::vector<int>> domains;
    std::vector<std::pair<int,int>> constrVar;
    std::vector<std::vector<std::pair<int,int>>> constrVal;
};

struct ColorProblem {
    int nb_colors;
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

struct NonogramProblem {
    unsigned int w;
    unsigned int h;
    std::vector<std::vector<unsigned int>> verticalClues;
    std::vector<std::vector<unsigned int>> horizontalClues;

    std::vector<std::vector<bool>> getAllPossible(const std::vector<unsigned int>& clues, unsigned int n) const;
    void recursiveComplete(const std::vector<unsigned int>& clues, std::vector<bool>& curSol, unsigned int idx, unsigned int start, unsigned int end, std::vector<std::vector<bool>>& allSolutions) const;
};

#endif