#include <stdio.h>
#include <chrono>
#include <vector>
#include <climits>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <execution>

int main () {
    srand(1);
    const size_t n = 100000000;
    std::vector<float> numbers;

    // Create a large random array
    for (size_t i = 0; i < n; i++)
    {
        float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        numbers.push_back(r); // = r;
    }

    // Start measuring time
    auto begin = std::chrono::high_resolution_clock::now();

    //float sum = std::reduce(numbers.begin(), numbers.end(), 0.0);
    float sum = std::reduce(std::execution::par, numbers.cbegin(), numbers.cend());

    // Stop measuring time and calculate the elapsed time
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    printf("Time measured: %.8f seconds.\n", elapsed.count() * 1e-9);

    std::cout << sum << std::endl;
    return 0;
}