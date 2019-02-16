#include <vector>
#include <memory>
#include <iostream>
#include <queue>

#ifndef EX1_HELPERS_H
#define EX1_HELPERS_H

typedef std::vector<std::vector<double>> matrix;

std::shared_ptr<const matrix> readMatrix(std::istream &input);

std::shared_ptr<matrix> generateEmptyMatrix(unsigned long n, unsigned long m);

void printMatrix(std::shared_ptr<matrix> matrix, std::ostream &output);

std::shared_ptr<matrix>
multiplyMatrices(std::shared_ptr<const matrix> leftOperand, std::shared_ptr<const matrix> rightOperand,
                 unsigned long threads);

struct _MultiplicationThreadArgument {
    std::shared_ptr<const matrix> leftOperand;
    std::shared_ptr<const matrix> rightOperand;
    std::shared_ptr<matrix> result;

    std::queue<unsigned long> rows;

    _MultiplicationThreadArgument(std::shared_ptr<const matrix> &_leftOperand,
                                  std::shared_ptr<const matrix> &_rightOperand, std::shared_ptr<matrix> &_result)
            : leftOperand(_leftOperand), rightOperand(_rightOperand), result(_result) {}

};

void _multiplyRows(std::shared_ptr<_MultiplicationThreadArgument> args);

#endif //EX1_HELPERS_H
