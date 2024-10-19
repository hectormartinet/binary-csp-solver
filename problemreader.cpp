#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "problemreader.h"

ColorProblem ProblemReader::readColorProblem(std::string path) {
    std::ifstream inputFile(path);
    ColorProblem problem;

    std::string line;
    std::string word;
    std::getline(inputFile, line);
    while (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        ss >> word;
        if (word == "p") {
            ss >> word;
            ss >> word;
            problem.nb_nodes = std::stoi(word);
            ss >> word;
            problem.nb_edges = std::stoi(word);
        } else if (word == "e") {
            ss >> word;
            int i = std::stoi(word);
            ss >> word;
            int j = std::stoi(word);
            problem.edges.push_back(std::make_pair(i,j));
        }
    }
    return problem;
}

SudokuProblem ProblemReader::readSudokuProblem(std::string path) {
    unsigned int n = 9;
    std::ifstream inputFile(path);
    SudokuProblem problem;
    problem.n = n;
    
    std::string line;
    std::getline(inputFile, line);
    std::getline(inputFile, line);
    for (unsigned int i=0; i<n; i++) {
        problem.grid.push_back(std::vector<int>());
        for (unsigned int j=0; j<n; j++) {
            problem.grid.back().push_back(line[n*i+j]-'0');
        }
    }
    return problem;
}

QueenProblem ProblemReader::readQueenProblem(std::string path) {
    std::ifstream inputFile(path);
    QueenProblem problem;

    std::string line;
    std::getline(inputFile, line);
    std::getline(inputFile, line);
    problem.nb_queens = stoi(line);
    return problem;
}

BlockedQueenProblem ProblemReader::readBlockedQueenProblem(std::string path) {
    std::ifstream inputFile(path);
    BlockedQueenProblem problem;

    std::string line;
    std::getline(inputFile, line);
    std::getline(inputFile, line);
    problem.nb_queens = stoi(line);
    std::string word;
    while (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        ss >> word;
        int i = std::stoi(word);
        ss >> word;
        int j = std::stoi(word);
        problem.blockedSquares.push_back(std::make_pair(i,j));
    }
    return problem;
}

NonogramProblem ProblemReader::readNonogramProblem(std::string path) {
    std::ifstream inputFile(path);
    NonogramProblem problem;

    std::string line;
    std::string word;
    std::stringstream ss;
    std::getline(inputFile, line);
    std::getline(inputFile, line);
    ss = std::stringstream(line);
    ss >> word;
    problem.w = std::stoul(word);
    ss >> word;
    problem.h = std::stoul(word);

    for (unsigned int i=0; i<problem.w; i++) {
        std::getline(inputFile, line);
        ss = std::stringstream(line);
        problem.verticalClues.push_back(std::vector<unsigned int>());

        while (ss >> word) {
            problem.verticalClues.back().push_back(std::stoul(word));
        }
    }

    for (unsigned int j=0; j<problem.h; j++) {
        std::getline(inputFile, line);
        ss = std::stringstream(line);
        problem.horizontalClues.push_back(std::vector<unsigned int>());
        
        while (ss >> word) {
            problem.horizontalClues.back().push_back(std::stoul(word));
        }
    }
    return problem;
}