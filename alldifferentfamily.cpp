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

    for (const auto& [val, varSet] : valToVar) {
        assert(!varSet.empty());
        if (varSet.size()==1) {
            uniqueValues.emplace(val,*varSet.begin());
        }
    }
}

void AllDifferentFamily::add(int var, int value) {
    if (!valToVar.at(value).emplace(var).second) return;
    switch (valToVar.at(value).size())
    {
    case 1:
        nbValPossible++;
        uniqueValues.emplace(value, var);
        return;
    case 2:
        uniqueValues.erase(value);
        return;
    default:
        return;
    }
}

bool AllDifferentFamily::remove(int var, int value, std::vector<std::pair<int,int>>& varsToFix) {
    if (!valToVar.at(value).erase(var)) return true;

    switch (valToVar.at(value).size())
    {
    case 0:
        nbValPossible--;
        uniqueValues.erase(value);
        if (nbValPossible != nbVar()) return nbValPossible < nbVar();
        fillUniqueValues(varsToFix);
        return true;
    case 1:
    {
        int uniqueVar = *valToVar.at(value).begin();
        uniqueValues.emplace(value, uniqueVar);
        if (nbValPossible==nbVar()) varsToFix.push_back(std::make_pair(uniqueVar,value));
        return true;
    }
    default:
        return true;
    }
}

bool AllDifferentFamily::init(std::vector<std::pair<int,int>>& varsToFix) const {
    if (nbVar() > nbVal()) return false;
    if (nbVar() < nbVal()) return true;
    fillUniqueValues(varsToFix);
    return true;
}

void AllDifferentFamily::fillUniqueValues(std::vector<std::pair<int,int>>& varsToFix) const {
    for (auto [value, var] : uniqueValues) {
        varsToFix.push_back(std::make_pair(var,value));
    }
}

bool AllDifferentFamily::isCoherent(const std::unordered_map<int,std::unordered_set<int>>& domains) const {
    unsigned int linesCheckSum = 0;
    unsigned int columnsCheckSum = 0;
    for (int var : variables) {
        linesCheckSum += (unsigned int)(domains.at(var).size());
        for (int val : domains.at(var)) {
            if (!valToVar.at(val).count(var)) return false;
        }
    }

    for (const auto& [val, varSet] : valToVar) {
        columnsCheckSum += (unsigned int)(varSet.size());
    }

    return linesCheckSum==columnsCheckSum;
}
