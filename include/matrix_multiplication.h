
#ifndef _MATRIX_MULTIPLICATION_H_
#define _MATRIX_MULTIPLICATION_H_

#include <thread>
#include <chrono>
#include <vector>
#include <iostream>
#include <exception>
#include <cmath>
#include <cstdlib>

namespace matrix {

template<typename T>
std::vector<std::vector<T>> sequential_multiplication(const std::vector<std::vector<T>>& matrix1, const std::vector<std::vector<T>>& matrix2) {
    auto m1_row = matrix1.size();
    auto m1_column = matrix1[0].size();
    auto m2_row = matrix2.size();
    auto m2_column = matrix2[0].size();

    if(m1_column != m2_row) {
        throw std::runtime_error("matrix multiplication is not possible! matrix1 column count must be equal to matrix2 row count!");
    }

    std::vector<std::vector<T>> result(m1_row, std::vector<T>(m2_column, T{}));
    for(auto i=0; i<m1_row; ++i) {
        for(auto j=0; j<m2_column; ++j) {
            result[i][j] = 0; // initialize
            // calculate the multiplication!
            for(auto k=0; k<m1_column/*m2_row*/; ++k) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }

    return result;
}

template<typename T>
void parallel_worker(const std::vector<std::vector<T>>& matrix1, const std::vector<std::vector<T>>& matrix2, 
                     std::vector<std::vector<T>>& result, size_t start_row, size_t end_row) {
    auto m1_column = matrix1[0].size();
    auto m2_column = matrix2[0].size();

    for(auto i=start_row; i<end_row; ++i) {
        for(auto j=0; j<m2_column; ++j) {
            result[i][j] = 0; // initialize
            // calculate the multiplication!
            for(auto k=0; k<m1_column/*m2_row*/; ++k) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

template<typename T>
std::vector<std::vector<T>> parallel_multiplication(const std::vector<std::vector<T>>& matrix1, const std::vector<std::vector<T>>& matrix2) {
    auto m1_row = matrix1.size();
    auto m1_column = matrix1[0].size();
    auto m2_row = matrix2.size();
    auto m2_column = matrix2[0].size();

    if(m1_column != m2_row) {
        throw std::runtime_error("matrix multiplication is not possible! matrix1 column count must be equal to matrix2 row count!");
    }

    auto thread_count = std::thread::hardware_concurrency();
    size_t chunk_size = ceil((double)m1_row / thread_count);

    std::vector<std::thread> threads(thread_count);

    std::vector<std::vector<T>> result(m1_row, std::vector<T>(m2_column, T{}));
    for(auto i=0; i<thread_count; ++i) {
        auto start_row = std::min(i*chunk_size, m1_row);
        auto end_row = std::min((i+1)*chunk_size, m1_row);

        // give each thread their own portion
        threads[i] = std::thread(&parallel_worker<int>, std::ref(matrix1), std::ref(matrix2), std::ref(result), start_row, end_row);
    }

    // wait for all the threads to compete their execution
    for(auto & iter : threads) {
        iter.join();
    }


    return result;
}

void matrixMultiplication_trials() {
    const auto EVAL_ROUND = 3;
    const auto M1_ROW_COUNT = 1000;
    const auto M1_COLUMN_COUNT = 1000;
    const auto M2_ROW_COUNT = M1_COLUMN_COUNT;
    const auto M2_COLUMN_COUNT = 1000;
    const auto MAX_NUM = 1000;

    // initialize matrices with 0s
    std::vector<std::vector<int>> matrix1(M1_ROW_COUNT, std::vector<int>(M1_COLUMN_COUNT, 0));
    std::vector<std::vector<int>> matrix2(M2_ROW_COUNT, std::vector<int>(M2_COLUMN_COUNT, 0));

    // initialize ramdom generator with current time
    std::srand(std::time(nullptr));

    // initialize matrix1
    for(auto i=0; i<M1_ROW_COUNT; ++i) {
        for(auto j=0; j<M1_COLUMN_COUNT; ++j) {
            matrix1[i][j] = std::rand() % MAX_NUM + 1;
        }
    }

    // initialize matrix2
    for(auto i=0; i<M2_ROW_COUNT; ++i) {
        for(auto j=0; j<M2_COLUMN_COUNT; ++j) {
            matrix2[i][j] = std::rand() % MAX_NUM + 1;
        }
    }

    std::vector<std::vector<int>> result_sequential;
    std::vector<std::vector<int>> result_parallel;

    std::cout << "Evaluating sequential matrix multiplication..." << std::endl;
    std::chrono::duration<double> sequential_time(0);
    for(auto i=0; i<EVAL_ROUND; ++i) {
        auto start_time = std::chrono::high_resolution_clock::now();
        result_sequential = sequential_multiplication<int>(matrix1, matrix2);
        auto time_passed = std::chrono::high_resolution_clock::now() - start_time;
        sequential_time += time_passed;
    }
    sequential_time /= EVAL_ROUND;

    std::cout << "Evaluating parallel matrix multiplication..." << std::endl;
    std::chrono::duration<double> parallel_time(0);
    for(auto i=0; i<EVAL_ROUND; ++i) {
        auto start_time = std::chrono::high_resolution_clock::now();
        result_parallel = parallel_multiplication<int>(matrix1, matrix2);
        auto time_passed = std::chrono::high_resolution_clock::now() - start_time;
        parallel_time += time_passed;
    }
    parallel_time /= EVAL_ROUND;

    // validate multiplication results by comparing the two result matrices
    for(auto i=0; i<M1_ROW_COUNT; ++i) {
        for(auto j=0; j<M2_COLUMN_COUNT; ++j) {
            if(result_sequential[i][j] != result_parallel[i][j]) {
                std::cout << "Values do not match from both matrices at postiion (" << i << ", " << j << ")" << std::endl;
                std::cout << "sequential[" << i << "][" << j << "] = " << result_sequential[i][j] << " vs";
                std::cout << "parallel[" << i << "][" << j << "] = " << result_parallel[i][j] << std::endl;
            }
        }
    }

    // print out the performance comparison results
    std::cout << "Average Sequential Time: " << sequential_time.count()*1000 << "ms" << std::endl;
    std::cout << "Average Parallel Time:   " << parallel_time.count()*1000 << "ms" << std::endl;
    std::cout << "SpeedUp: " << sequential_time / parallel_time << std::endl;
    std::cout << "Efficiency: " << 100*(sequential_time/parallel_time)/std::thread::hardware_concurrency() << "%" << std::endl;
}

}

#endif // _MATRIX_MULTIPLICATION_H_
