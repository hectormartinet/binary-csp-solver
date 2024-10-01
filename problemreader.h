#ifndef FILE_READER_H_
#define FILE_READER_H_

#include <vector>
#include <string>

#include "instances.h"

class ProblemReader {
public:
    static ColorProblem readColorProblem(std::string path);
};

#endif