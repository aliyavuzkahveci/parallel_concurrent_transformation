
#ifndef _DOWNLOAD_IMAGE_H_
#define _DOWNLOAD_IMAGE_H_

#include <thread>
#include <future>
#include <chrono>
#include <iostream>
#include <vector>

#include <curl/curl.h>

namespace image_download {


// support function for the download_image helper function
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    return size * nmemb;
}

// helper function to download a single image and return size in bytes
size_t image_download(const std::string& imageUrl) {

    CURLcode res;
    curl_off_t num_bytes = 0;
    CURL *curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, imageUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        res = curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &num_bytes);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_getinfo() failed: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    return num_bytes;
}

// sequential implementation of image download
size_t sequential_image_download(const std::vector<std::string> images) {
	size_t total_bytes = 0;
	for (auto const & image : images) {
		total_bytes += image_download(image);
	}
	return total_bytes;
}

// parallel implementation of image download
size_t parallel_image_download(const std::vector<std::string> images) {
	size_t total_bytes = 0;
    std::vector<std::future<size_t>> downloaded_byte_counts;
    for (auto const & image : images) {
        downloaded_byte_counts.push_back(std::async(std::launch::async, image_download, image));
    }
    for (auto & byte_count : downloaded_byte_counts) {
        total_bytes += byte_count.get();
    }
	return total_bytes;
}

void imageDownload_trials() {
    const auto EVAL_ROUND = 3;
    std::vector<std::string> imageURLs = { "https://unsplash.com/photos/OTJIkWDSVyE",
                                           "https://unsplash.com/photos/klb7XRaQzRc",
                                           "https://unsplash.com/photos/uGHdtM5J30g",
                                           "https://unsplash.com/photos/q6GaM4Fe6vY",
                                           "https://unsplash.com/photos/uGHdtM5J30g",
                                           "https://unsplash.com/photos/AXz8NGMrvyk",
                                           "https://unsplash.com/photos/BvMklgZ0eUQ",
                                           "https://unsplash.com/photos/cnUkuiRI1l0" };
    
    std::cout << "Evaluating SEQUENTIAL image download..." << std::endl;
    size_t sequential_total_bytes = sequential_image_download(imageURLs); // first run for calculating total downloaded bytes
	std::chrono::duration<double> sequential_time(0);
    for (int i=0; i<EVAL_ROUND; i++) {
        auto startTime = std::chrono::high_resolution_clock::now();
        sequential_image_download(imageURLs);
        sequential_time += std::chrono::high_resolution_clock::now() - startTime;
    }
    sequential_time /= EVAL_ROUND;

    std::cout << "Evaluating PARALLEL image download..." << std::endl;
    size_t parallel_total_bytes = parallel_image_download(imageURLs); // first run for calculating total downloaded bytes
	std::chrono::duration<double> parallel_time(0);
    for (int i=0; i<EVAL_ROUND; i++) {
        auto startTime = std::chrono::high_resolution_clock::now();
        parallel_image_download(imageURLs);
        parallel_time += std::chrono::high_resolution_clock::now() - startTime;
    }
    parallel_time /= EVAL_ROUND;

    // display sequential and parallel results for comparison
    if (sequential_total_bytes != parallel_total_bytes) {
        std::cerr << "ERROR: Result mismatch!" << std::endl 
                  << "Sequentially Downloaded Byte Count = " << sequential_total_bytes << std::endl 
                  << "Parallelly Downloaded Byte Count = " << parallel_total_bytes << std::endl;
    }

    // print out the performance comparison results
    std::cout << "Average Sequential Time: " << sequential_time.count()*1000 << "ms" << std::endl;
    std::cout << "Average Parallel Time:   " << parallel_time.count()*1000 << "ms" << std::endl;
    std::cout << "SpeedUp: " << sequential_time / parallel_time << std::endl;
    std::cout << "Efficiency: " << 100*(sequential_time/parallel_time)/std::thread::hardware_concurrency() << "%" << std::endl;
    
}

}

#endif // _DOWNLOAD_IMAGE_H_
