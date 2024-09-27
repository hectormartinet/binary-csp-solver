#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_

#include <map>
#include <set>
#include <vector>

class Constraint{

private:
    unsigned int x;
    unsigned int y;
    std::map<int, std::set<int>> list;

public:
    Constraint(unsigned int _x, unsigned int _y): x{_x}, y{_y} {};
    Constraint(unsigned int _x, unsigned int _y, const std::vector<std::pair<int,int>>& pairs);

    void addPair(int a, int b);
    
    bool feasible(int a, int b) const;
    bool feasible(const std::vector<int>& sol) const {return feasible(sol[x],sol[y]);}

    void display() const;
};

#endif