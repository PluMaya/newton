#include "newton/core.hpp"

#ifdef USE_BOOST
#include "newton/boost_utils.hpp"
#include <iostream>
#include <vector>

int main() {
    std::cout << "Newton Cluster C++ Infrastructure - Boost Example" << std::endl;
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

    // Test Boost filesystem utilities
    std::cout << "Testing Boost.Filesystem utilities:" << std::endl;

    // Check if current directory exists
    std::string current_dir = ".";
    std::cout << "Current directory exists: "
              << (newton::boost_utils::fileExists(current_dir) ? "yes" : "no")
              << std::endl;

    // Create a test directory
    std::string test_dir = "/tmp/newton_test";
    std::cout << "Creating test directory: " << test_dir << std::endl;
    if (newton::boost_utils::createDirectory(test_dir)) {
        std::cout << "Directory created successfully" << std::endl;
    } else {
        std::cout << "Failed to create directory" << std::endl;
    }

    // List files in the include directory
    std::string include_dir = "../include/newton";
    std::cout << "\nListing files in " << include_dir << ":" << std::endl;
    auto files = newton::boost_utils::listFiles(include_dir);
    for (const auto& file : files) {
        std::cout << "  - " << file << std::endl;
    }

    std::cout << "\nExample completed successfully!" << std::endl;
    return 0;
}

#else

int main() {
    std::cerr << "This example requires Boost support. "
              << "Please rebuild with -DUSE_BOOST=ON" << std::endl;
    return 1;
}

#endif // USE_BOOST
