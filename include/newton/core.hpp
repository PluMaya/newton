#ifndef NEWTON_CORE_HPP
#define NEWTON_CORE_HPP

#include <string>
#include <vector>
#include <memory>

namespace newton {

/**
 * @brief Core class for Newton cluster operations
 */
class Core {
public:
    Core();
    ~Core();

    /**
     * @brief Initialize the Newton system
     * @param config_path Path to configuration file
     * @return true if initialization succeeded
     */
    bool initialize(const std::string& config_path = "");

    /**
     * @brief Get the version string
     * @return Version string
     */
    static std::string getVersion();

    /**
     * @brief Process data (placeholder for future cluster operations)
     * @param data Input data vector
     * @return Processed data vector
     */
    std::vector<double> processData(const std::vector<double>& data);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace newton

#endif // NEWTON_CORE_HPP
