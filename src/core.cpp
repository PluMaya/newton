#include "newton/core.hpp"
#include <iostream>
#include <algorithm>

namespace newton {

// Pimpl implementation
class Core::Impl {
public:
    bool initialized = false;
    std::string config_path;
};

Core::Core() : pImpl(std::make_unique<Impl>()) {}

Core::~Core() = default;

bool Core::initialize(const std::string& config_path) {
    if (pImpl->initialized) {
        std::cerr << "Newton Core already initialized" << std::endl;
        return false;
    }

    pImpl->config_path = config_path;
    pImpl->initialized = true;

    std::cout << "Newton Core initialized successfully" << std::endl;
    if (!config_path.empty()) {
        std::cout << "Configuration path: " << config_path << std::endl;
    }

    return true;
}

std::string Core::getVersion() {
    return "0.1.0";
}

std::vector<double> Core::processData(const std::vector<double>& data) {
    if (!pImpl->initialized) {
        std::cerr << "Newton Core not initialized" << std::endl;
        return {};
    }

    // Placeholder implementation: square each element
    std::vector<double> result;
    result.reserve(data.size());

    std::transform(data.begin(), data.end(), std::back_inserter(result),
                   [](double x) { return x * x; });

    return result;
}

} // namespace newton
