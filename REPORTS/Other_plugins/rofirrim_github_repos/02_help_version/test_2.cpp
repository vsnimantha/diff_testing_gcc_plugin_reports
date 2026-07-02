// test2_io_math.cpp
#include <iostream>
#include <cmath>
#include <vector>

int main() {
    std::vector<double> numbers = {1.0, 2.0, 3.0, 4.0};
    double sum = 0;
    
    for (double n : numbers) {
        sum += std::pow(n, 2);
    }
    
    std::cout << "Sum of squares: " << sum << std::endl;
    return 0;
}