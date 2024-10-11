#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <functional>
#include <stdexcept>
#include <iostream>

class Constraint {

public:
    int x;
    int y;

    Constraint(){}
    Constraint(int _x, int _y): x{_x}, y{_y}{}
    virtual ~Constraint(){}
    
    virtual std::unique_ptr<Constraint> clone()=0;

    virtual void addPair(int a, int b)=0;
    virtual void removePair(int a, int b)=0;
    
    virtual bool feasible(int a, int b) const=0;
    virtual bool feasible(const std::unordered_map<int,int>& partSol) const=0;

    // Get pairs where x=a is not in the domain Dx in input
    virtual std::vector<std::pair<int,int>> getUselessPairs(const std::unordered_set<int>&) const{return std::vector<std::pair<int,int>>();};

    virtual const std::unordered_set<int>& getSupport(int a) const=0;
    virtual size_t getSupportSize(int value) const=0;

    virtual void display() const=0;
};

class ExtensiveConstraint : public Constraint {

protected:
    std::unordered_map<int, std::unordered_set<int>> list;

public:
    ExtensiveConstraint(int _x, int _y): Constraint(_x,_y) {};
    ExtensiveConstraint(int _x, int _y, const std::vector<std::pair<int,int>>& pairs);

    std::unique_ptr<Constraint> clone() {return std::unique_ptr<ExtensiveConstraint>(new ExtensiveConstraint{*this});}

    void addPair(int a, int b);
    void removePair(int a, int b);
    
    bool feasible(int a, int b) const {return list.count(a)>0 && list.at(a).count(b);}
    bool feasible(const std::unordered_map<int,int>& partSol) const{return feasible(partSol.at(x),partSol.at(y));}

    std::vector<std::pair<int,int>> getUselessPairs(const std::unordered_set<int>& Dx) const;

    const std::unordered_set<int>& getSupport(int a) const {return list.at(a);}
    size_t getSupportSize(int value) const {return list.count(value) ? list.at(value).size():0;}

    void display() const;
};


class IntensiveConstraint: public Constraint {

protected:
    std::function<bool(int,int)> feasibleFunction;

public:
    IntensiveConstraint(int _x, int _y, std::function<bool(int,int)> _feasibleFunction) : Constraint(_x,_y), feasibleFunction{_feasibleFunction} {}

    std::unique_ptr<Constraint> clone() {return std::unique_ptr<IntensiveConstraint>(new IntensiveConstraint{*this});}

    void addPair(int, int) {throw std::logic_error("Cannot add pair to intensive constraint");};
    void removePair(int, int){throw std::logic_error("Cannot remove pair from intensive constraint");};

    bool feasible(int a, int b) const{return feasibleFunction(a,b);}
    bool feasible(const std::unordered_map<int,int>& partSol) const{return feasible(partSol.at(x),partSol.at(y));}

    std::vector<std::pair<int,int>> getUselessPairs(const std::unordered_set<int>&) const{return std::vector<std::pair<int,int>>();};

    const std::unordered_set<int>& getSupport(int) const{throw std::logic_error("Not implemented lol");};
    size_t getSupportSize(int) const{throw std::logic_error("Not implemented lol");};

    void display() const {std::cout << x << "," << y << ": Intensive constraint"<<std::endl;}
};

#endif