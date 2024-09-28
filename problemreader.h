#ifndef FILE_READER_H_
#define FILE_READER_H_

#include <vector>
#include <string>

class ProblemReader {

public:
    struct ColorProblem {
        int nb_nodes;
        int nb_edges;
        std::vector<std::pair<int,int>> edges;
    };

    static ColorProblem readColorProblem(std::string path);

};

#endif