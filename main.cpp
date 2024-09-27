#include "constraint.h"
#include "scp.h"

int main() {

    unsigned int n = 3;

    SCP problem(n);
    problem.makeNQueen();
    problem.display();

    return 0;
}