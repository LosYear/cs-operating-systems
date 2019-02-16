#include <fstream>
#include <iostream>
#include <exception>
#include <ctime>
#include <chrono>
#include <thread>
#include <cstring>
#include "matrix.h"

/*
 * Arguments:
 * 1. filename (required)
 * 2. thread counts (required), zero for calculating in main thread, auto = logical CPUs
 */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Missing required arguments: FILENAME and THREADS" << std::endl;
        return 1;
    }

    std::ifstream input(argv[1]);
    unsigned int availableCPUs = std::thread::hardware_concurrency();
    unsigned long threads = availableCPUs;

    if (std::strcmp("auto", argv[2]) != 0) {
        threads = static_cast<unsigned int>(std::stoi(argv[2]));
    }

    auto leftOperand = readMatrix(input), rightOperand = readMatrix(input);

    auto redundantThreadsWarning = std::make_pair<bool, unsigned long>(false, 0);

    if (threads > leftOperand->size()) {
        redundantThreadsWarning.first = true;
        redundantThreadsWarning.second = threads;
        threads = leftOperand->size();
    }

    auto startMoment = std::chrono::system_clock::now();
    auto result = multiplyMatrices(leftOperand, rightOperand, threads);
    auto endMoment = std::chrono::system_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endMoment - startMoment);
    auto elapsedTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime);

    std::ofstream output("output.txt");
    output << "Available logical CPUs: " << availableCPUs << std::endl;
    output << "Spawned threads: " << threads << std::endl;
    output << "Time elapsed: " << elapsedTime.count() << " microseconds or " << elapsedTimeMs.count() << " milliseconds"
           << std::endl;
    if (redundantThreadsWarning.first) {
        output << "WARNING: set threads count (" << redundantThreadsWarning.second
               << ") exceeds rows number. Automatically downscaled to " << threads
               << std::endl;
    }
    output << std::endl;
    printMatrix(result, output);

    input.close();
    output.close();
    return 0;
}