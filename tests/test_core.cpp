#include "newton/core.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

void test_version() {
    std::string version = newton::Core::getVersion();
    assert(!version.empty());
    assert(version == "0.1.0");
    std::cout << "✓ Version test passed" << std::endl;
}

void test_initialization() {
    newton::Core core;
    assert(core.initialize());
    std::cout << "✓ Initialization test passed" << std::endl;
}

void test_process_data() {
    newton::Core core;
    core.initialize();

    std::vector<double> input = {1.0, 2.0, 3.0, 4.0};
    std::vector<double> result = core.processData(input);

    assert(result.size() == input.size());
    assert(std::abs(result[0] - 1.0) < 1e-9);
    assert(std::abs(result[1] - 4.0) < 1e-9);
    assert(std::abs(result[2] - 9.0) < 1e-9);
    assert(std::abs(result[3] - 16.0) < 1e-9);

    std::cout << "✓ Process data test passed" << std::endl;
}

void test_uninitialized_core() {
    newton::Core core;
    std::vector<double> input = {1.0, 2.0};
    std::vector<double> result = core.processData(input);
    assert(result.empty());
    std::cout << "✓ Uninitialized core test passed" << std::endl;
}

int main() {
    std::cout << "Running Newton Core tests..." << std::endl;
    std::cout << std::endl;

    try {
        test_version();
        test_initialization();
        test_process_data();
        test_uninitialized_core();

        std::cout << std::endl;
        std::cout << "All tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
