#ifndef NEWTON_BOOST_UTILS_HPP
#define NEWTON_BOOST_UTILS_HPP

#ifdef USE_BOOST

#include <string>
#include <vector>
#include <boost/filesystem.hpp>

namespace newton {
namespace boost_utils {

/**
 * @brief Check if a file exists using Boost.Filesystem
 * @param path Path to check
 * @return true if file exists
 */
bool fileExists(const std::string& path);

/**
 * @brief List files in a directory using Boost.Filesystem
 * @param dir_path Directory path
 * @return Vector of file paths
 */
std::vector<std::string> listFiles(const std::string& dir_path);

/**
 * @brief Create directory if it doesn't exist
 * @param dir_path Directory path to create
 * @return true if directory was created or already exists
 */
bool createDirectory(const std::string& dir_path);

} // namespace boost_utils
} // namespace newton

#endif // USE_BOOST

#endif // NEWTON_BOOST_UTILS_HPP
