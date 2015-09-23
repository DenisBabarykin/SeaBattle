#ifndef SOLVER_H
#define SOLVER_H

#include "navy.h"
#include "Net.h"

class Solver : public Navy
{
public:
    Solver();
    virtual ~Solver();
    virtual void fire(int &, int &);

private:
    struct shot{int x; int y;};
    NeuralNetwork::Net * net;

    NeuralNetwork::real *input;
    NeuralNetwork::real *output;

};

#endif // SOLVER_H
