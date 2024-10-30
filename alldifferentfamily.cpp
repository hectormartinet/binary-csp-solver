#include "alldifferentfamily.h"

#include <cassert>

AllDifferentFamily::AllDifferentFamily(std::vector<int> _variables, const std::unordered_map<int,std::unordered_set<int>>& domains) {
    for (int var : _variables) {
        variables.emplace(var);
        for (int val : domains.at(var)) {
            if (valToVar.count(val) == 0)
                valToVar.emplace(val,std::unordered_set<int>());
            valToVar.at(val).emplace(var);
        }
    }
    nbValPossible = nbVal();
}

void AllDifferentFamily::add(int var, int value) {
    assert(valToVar.at(value).count(var)==0);
    valToVar.at(value).emplace(var);
    if (valToVar.at(value).size() == 1)
        nbValPossible++;
}

bool AllDifferentFamily::remove(int var, int value) {
    assert(valToVar.at(value).count(var));
    valToVar.at(value).erase(var);
    if (valToVar.at(value).size() == 0) {
        nbValPossible--;
        if (nbValPossible < nbVar()) return false;
    }
    // if (valToVar.at(value) == 1 && nbValPossible==nbVar()) {
    //     // fix variable
    // }
    return true;
}
