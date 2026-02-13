#include "newton/boost_utils.hpp"

#ifdef USE_BOOST

#include <iostream>

namespace newton {
namespace boost_utils {

bool fileExists(const std::string& path) {
    return boost::filesystem::exists(path);
}

std::vector<std::string> listFiles(const std::string& dir_path) {
    std::vector<std::string> files;

    if (!boost::filesystem::exists(dir_path)) {
        std::cerr << "Directory does not exist: " << dir_path << std::endl;
        return files;
    }

    if (!boost::filesystem::is_directory(dir_path)) {
        std::cerr << "Path is not a directory: " << dir_path << std::endl;
        return files;
    }

    for (const auto& entry : boost::filesystem::directory_iterator(dir_path)) {
        if (boost::filesystem::is_regular_file(entry)) {
            files.push_back(entry.path().string());
        }
    }

    return files;
}

bool createDirectory(const std::string& dir_path) {
    try {
        if (boost::filesystem::exists(dir_path)) {
            return boost::filesystem::is_directory(dir_path);
        }
        return boost::filesystem::create_directories(dir_path);
    } catch (const boost::filesystem::filesystem_error& e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        return false;
    }
}

} // namespace boost_utils
} // namespace newton

#endif // USE_BOOST
