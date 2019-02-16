#include "matrix.h"
#include <cmath>
#include <thread>
#include <vector>

std::shared_ptr<const matrix> readMatrix(std::istream &input) {
    unsigned long m, n;
    input >> m >> n;

    std::shared_ptr<matrix> result = std::make_shared<matrix>();
    result->resize(m);

    for (unsigned long i = 0; i < m; ++i) {
        for (unsigned long j = 0; j < n; ++j) {
            double tmp;
            input >> tmp;

            (*result)[i].push_back(tmp);
        }
    }

    return static_cast<std::shared_ptr<const matrix>>(result);
}

std::shared_ptr<matrix> generateEmptyMatrix(unsigned long n, unsigned long m) {
    std::shared_ptr<matrix> result = std::make_shared<matrix>();
    result->resize(n);

    for (unsigned long i = 0; i < n; ++i) {
        for (unsigned long j = 0; j < m; ++j) {
            (*result)[i].push_back(0);
        }
    }

    return result;
}


void printMatrix(std::shared_ptr<matrix> matrix, std::ostream &output) {
    for (const auto &row: *matrix) {
        for (const auto el: row) {
            output << el << " ";
        }
        output << std::endl;
    }
}


std::shared_ptr<matrix>
multiplyMatrices(std::shared_ptr<const matrix> leftOperand, std::shared_ptr<const matrix> rightOperand,
                 unsigned long threads) {
    if ((*leftOperand)[0].size() != rightOperand->size()) {
        throw std::invalid_argument("Incompatible matrices sizes");
    }

    auto resultingMatrix = generateEmptyMatrix(leftOperand->size(), (*rightOperand)[0].size());

    if (threads == 0) {
        auto args = std::make_shared<_MultiplicationThreadArgument>(leftOperand, rightOperand, resultingMatrix);

        for (unsigned long i = 0; i < leftOperand->size(); ++i) {
            args->rows.push(i);
        }

        _multiplyRows(args);
    } else {
        auto rowsPerThread = static_cast<unsigned long>(std::ceil(
                static_cast<double>(leftOperand->size()) / threads));
        std::vector<std::thread> runningThreads;

        // Spawn threads
        for (unsigned long i = 0; i < threads; ++i) {
            auto args = std::make_shared<_MultiplicationThreadArgument>(leftOperand, rightOperand, resultingMatrix);
            for (unsigned row = 0; row < rowsPerThread && i * rowsPerThread + row < leftOperand->size(); ++row) {
                args->rows.push(i * rowsPerThread + row);
            }

            runningThreads.emplace_back(std::thread(_multiplyRows, args));
        }

        // Wait for all threads to execute
        for (auto &thread : runningThreads) {
            thread.join();
        }
    }


    return resultingMatrix;
}

void _multiplyRows(std::shared_ptr<_MultiplicationThreadArgument> args) {
    while (!args->rows.empty()) {
        unsigned long rowNum = args->rows.front();

        for (unsigned long i = 0; i < (*args->rightOperand)[0].size(); ++i) {
            double &cell = (*args->result)[rowNum][i];
            cell = 0;

            for (unsigned long j = 0; j < (*args->leftOperand)[0].size(); ++j) {
                cell += (*args->leftOperand)[rowNum][j] * (*args->rightOperand)[j][i];
            }
        }

        args->rows.pop();
    }
}
