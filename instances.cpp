#include "instances.h"

#include <cassert>
#include <numeric>

std::vector<std::vector<bool>> NonogramProblem::getAllPossible(const std::vector<unsigned int>& clues, unsigned int n) const{
    assert(!clues.empty());
    std::vector<bool> sol(n, false);
    if (clues.size() == 1 && clues[0] == 0) {
        return std::vector<std::vector<bool>> {sol};
    }

    std::vector<std::vector<bool>> allSolutions;
    unsigned int end = n + 2 - std::accumulate(clues.begin(), clues.end(), (unsigned int)(clues.size()));

    recursiveComplete(clues, sol, 0, 0, end, allSolutions);

    return allSolutions;
}


void NonogramProblem::recursiveComplete(const std::vector<unsigned int>& clues, std::vector<bool>& curSol,  unsigned int idx, unsigned int start, unsigned int end, std::vector<std::vector<bool>>& allSolutions) const{
    if (idx >= clues.size()) {
        allSolutions.push_back(curSol);
        return;
    }
     
    assert(start < end);
    unsigned int len = clues[idx];
    assert(len > 0);
    
    for (unsigned int i=start; i<start+len; i++) {
        curSol[i] = 1;
    }
    recursiveComplete(clues, curSol, idx+1, start+len+1, end+len+1, allSolutions);

    for (unsigned int i=start+1; i<end; i++) {
        curSol[i-1] = 0;
        curSol[i+len-1] = 1;
        recursiveComplete(clues, curSol, idx+1, i+len+1, end+len+1, allSolutions);
    }

    for (unsigned int i=end-1; i<end-1+len; i++) {
        curSol[i] = 0;
    }
}
