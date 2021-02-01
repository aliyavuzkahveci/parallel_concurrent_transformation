
#ifndef _MERGE_SORT_H_
#define _MERGE_SORT_H_

#include <thread>
#include <chrono>
#include <algorithm>
#include <iostream>

namespace mergesort {

template <typename T>
void merge(T *array, T *auxiliary, unsigned int start, unsigned int mid, unsigned int end) {

    for (unsigned int i = start; i <= end; ++i) {
        auxiliary[i] = array[i];
    }

    unsigned int firstHead = start;
    unsigned int secondHead = mid + 1;
    for (unsigned int counter = start; counter <= end; counter++) {
        if (firstHead > mid) {
            // first half completed merging
            array[counter] = auxiliary[secondHead];
            secondHead++;
        } else if (secondHead > end) {
            // second half completed merging
            array[counter] = auxiliary[firstHead];
            firstHead++;
        } else if (auxiliary[secondHead] < auxiliary[firstHead]) {
            // element in 2nd part is smaller!
            array[counter] = auxiliary[secondHead];
            secondHead++;
        }
        else {
            // element in the 1st half is smaller 
            array[counter] = auxiliary[firstHead];
            firstHead++;
        }
    }
}

template <typename T>
void sequential_merge_sort(T *array, T *auxiliary, unsigned int start, unsigned int end) {
    if (end <= start) {
        return; // empty or 1-sized array is already sorted!
    }

    // divide the array into 2 halves
    unsigned int mid = start + ((end - start) / 2);
    sequential_merge_sort<T>(array, auxiliary, start, mid);
    sequential_merge_sort<T>(array, auxiliary, mid + 1, end);
    merge<T>(array, auxiliary, start, mid, end);
}

template <typename T>
void parallel_merge_sort(T *array, T *auxiliary, unsigned int start, unsigned int end, unsigned int depth = 0) {
    if (depth >= std::log(std::thread::hardware_concurrency())) {
		sequential_merge_sort(array, auxiliary, start, end);
	}
	else {
		unsigned int mid = (start + end) / 2;
		std::thread left_thread = std::thread(parallel_merge_sort<T>, array, auxiliary, start, mid, depth+1);
        parallel_merge_sort(array, auxiliary, mid+1, start, depth+1);
		left_thread.join();
		merge(array, auxiliary, start, mid, end);
	}
}

void mergeSort_trials() {
    const auto EVAL_ROUND = 100;
    const auto NUM_OF_ELEMENT = 100'000;

    int original_array[NUM_OF_ELEMENT];
    int auxiliary_array[NUM_OF_ELEMENT];
    int sequential_array[NUM_OF_ELEMENT];
    int parallel_array[NUM_OF_ELEMENT];

    // initialize ramdom generator with current time
    std::srand(std::time(nullptr));

    // initialize original array
    for(auto i=0; i<NUM_OF_ELEMENT; ++i) {
        original_array[i] = std::rand() % NUM_OF_ELEMENT + 1; // [1-100]
    }

    std::cout << "Evaluating sequential merge sort..." << std::endl;
    std::chrono::duration<double> sequential_time(0);
    for(auto i=0; i<EVAL_ROUND; ++i) {
        // copy back the original values
        std::copy(original_array, original_array+NUM_OF_ELEMENT, sequential_array);

        auto start_time = std::chrono::high_resolution_clock::now();
        sequential_merge_sort<int>(sequential_array, auxiliary_array, 0, NUM_OF_ELEMENT-1);
        auto time_passed = std::chrono::high_resolution_clock::now() - start_time;
        sequential_time += time_passed;
    }
    sequential_time /= EVAL_ROUND;

    std::cout << "Evaluating parallel merge sort..." << std::endl;
    std::chrono::duration<double> parallel_time(0);
    for(auto i=0; i<EVAL_ROUND; ++i) {
        // copy back the original values
        std::copy(original_array, original_array+NUM_OF_ELEMENT, parallel_array);

        auto start_time = std::chrono::high_resolution_clock::now();
        parallel_merge_sort<int>(parallel_array, auxiliary_array, 0, NUM_OF_ELEMENT-1);
        auto time_passed = std::chrono::high_resolution_clock::now() - start_time;
        parallel_time += time_passed;
    }
    parallel_time /= EVAL_ROUND;

    // print out the performance comparison results
    std::cout << "Average Sequential Time: " << sequential_time.count()*1000 << "ms" << std::endl;
    std::cout << "Average Parallel Time:   " << parallel_time.count()*1000 << "ms" << std::endl;
    std::cout << "SpeedUp: " << sequential_time / parallel_time << std::endl;
    std::cout << "Efficiency: " << 100*(sequential_time/parallel_time)/std::thread::hardware_concurrency() << "%" << std::endl;
}

}

#endif // _MERGE_SORT_H_
