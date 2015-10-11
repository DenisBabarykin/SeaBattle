#ifndef SOLVER_H
#define SOLVER_H

#include "navy.h"
#include "Net.h"
#include <QVector>

class Solver : public Navy
{
public:
    Solver();
    virtual ~Solver();
    virtual void fire(int &, int &);

private:
    struct shot{int x; int y;};
    NeuralNetwork::Net * net;

    QVector<NeuralNetwork::real> input;
//    QVector<NeuralNetwork::real> output;
    NeuralNetwork::real *output;

};

#endif // SOLVER_H
