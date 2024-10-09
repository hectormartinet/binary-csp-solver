#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_

#include <unordered_map>
#include <unordered_set>
#include <vector>

class Constraint{

private:
    int x;
    int y;
    std::unordered_map<int, std::unordered_set<int>> list;

public:
    Constraint(int _x, int _y): x{_x}, y{_y} {};
    Constraint(int _x, int _y, const std::vector<std::pair<int,int>>& pairs);

    void addPair(int a, int b);
    void removePair(int a, int b);
    
    bool feasible(int a, int b) const {return list.count(a)>0 && list.at(a).count(b);}
    bool feasible(const std::unordered_map<int,int>& partSol) const{return feasible(partSol.at(x),partSol.at(y));}

    // Get pairs where x=a is not in the domain Dx in input
    std::vector<std::pair<int,int>> getUselessPairs(const std::unordered_set<int>& Dx) const;

    const std::unordered_set<int>& getSupport(int a) const {return list.at(a);}
    size_t getSupportSize(int value) const {return list.count(value) ? list.at(value).size():0;}

    void display() const;
};

#endif