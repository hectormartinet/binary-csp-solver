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
    if (valToVar.at(value).count(var)) return;
    valToVar.at(value).emplace(var);
    if (valToVar.at(value).size() == 1)
        nbValPossible++;
}

bool AllDifferentFamily::remove(int var, int value, std::vector<std::pair<int,int>>& varsToFix) {
    if (!valToVar.at(value).count(var)) return true;
    valToVar.at(value).erase(var);
    if (valToVar.at(value).size() == 0) {
        nbValPossible--;
        if (nbValPossible < nbVar()) return false;
    }
    if (valToVar.at(value).size() == 1 && nbValPossible==nbVar()) {
        int varToFix = *valToVar.at(value).begin();
        varsToFix.push_back(std::make_pair(varToFix,value));
    }
    return true;
}

bool AllDifferentFamily::init(std::vector<std::pair<int,int>>& varsToFix) const {
    for (const auto& [value, valSet] : valToVar) {
        if (valSet.empty()) return false;
        if (valSet.size()==1) {
            int varToFix = *valToVar.at(value).begin();
            varsToFix.push_back(std::make_pair(varToFix,value));
        }
    }
    return true;
}


bool AllDifferentFamily::isCoherent(const std::unordered_map<int,std::unordered_set<int>>& domains) const {
    unsigned int linesCheckSum = 0;
    unsigned int columnsCheckSum = 0;
    for (int var : variables) {
        linesCheckSum += domains.at(var).size();
        for (int val : domains.at(var)) {
            if (!valToVar.at(val).count(var)) return false;
        }
    }

    for (const auto& [val, varSet] : valToVar) {
        columnsCheckSum += varSet.size();
    }

    return linesCheckSum==columnsCheckSum;
}
