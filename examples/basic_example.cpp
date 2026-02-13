#include "newton/core.hpp"
#include <iostream>
#include <vector>

int main() {
    std::cout << "Newton Cluster C++ Infrastructure - Basic Example" << std::endl;
    std::cout << "Version: " << newton::Core::getVersion() << std::endl;
    std::cout << std::endl;

    // Create a Newton Core instance
    newton::Core core;

    // Initialize
    if (!core.initialize()) {
        std::cerr << "Failed to initialize Newton Core" << std::endl;
        return 1;
    }

    std::cout << std::endl;

    // Process some data
    std::vector<double> input_data = {1.0, 2.0, 3.0, 4.0, 5.0};
    std::cout << "Input data: ";
    for (const auto& val : input_data) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    auto result = core.processData(input_data);
    std::cout << "Processed data: ";
    for (const auto& val : result) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    std::cout << "\nExample completed successfully!" << std::endl;
    return 0;
}
