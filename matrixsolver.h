#ifndef MATRIXSOLVER_H
#define MATRIXSOLVER_H

#include <QVector>
#include <QDebug>

class MatrixSolver
{
public:

    static QVector<double> solveWithFixedLastVariable(
        const QVector<QVector<double>>& A,
        const QVector<double>& B,
        double fixedValue
        );

private:
    static QVector<double> solveSystem(
        const QVector<QVector<double>>& A,
        const QVector<double>& B
        );
};

#endif // MATRIXSOLVER_H
