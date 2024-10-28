#ifndef FILE_READER_H_
#define FILE_READER_H_

#include <vector>
#include <string>

#include "instances.h"

class ProblemReader {
public:
    static ColorProblem readColorProblem(std::string path);
    static SudokuProblem readSudokuProblem(std::string path);
    static QueenProblem readQueenProblem(std::string path);
    static BlockedQueenProblem readBlockedQueenProblem(std::string path);
    static NonogramProblem readNonogramProblem(std::string path);
    static GenericProblem readGenericProblem(std::string path);
};

#endif