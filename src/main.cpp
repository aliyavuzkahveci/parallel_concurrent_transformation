#include <iostream>

#include "matrix_multiplication.h"
#include "merge_sort.h"
#include "image_download.h"

int main()
{
    std::cout << "starting main" << std::endl;

    std::cout << "===============================================================" << std::endl;
    matrix::matrixMultiplication_trials();
    std::cout << "===============================================================" << std::endl;

    std::cout << "===============================================================" << std::endl;
    mergesort::mergeSort_trials();
    std::cout << "===============================================================" << std::endl;

    std::cout << "===============================================================" << std::endl;
    image_download::imageDownload_trials();
    std::cout << "===============================================================" << std::endl;

    std::cout << "end of main!" << std::endl;
    return 0;
}
