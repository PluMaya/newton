cmake_minimum_required(VERSION 3.14)
project(newton VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compile options
add_compile_options(-Wall -Wextra -Wpedantic)

# Option to enable/disable Boost
option(USE_BOOST "Enable Boost library support" ON)

# Find Boost if enabled
if(USE_BOOST)
    find_package(Boost 1.65 COMPONENTS system filesystem QUIET)
    if(Boost_FOUND)
        message(STATUS "Boost found: ${Boost_VERSION}")
        add_definitions(-DUSE_BOOST)
    else()
        message(STATUS "Boost not found, building without Boost support")
        set(USE_BOOST OFF)
    endif()
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
if(USE_BOOST AND Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

# Add subdirectories
add_subdirectory(src)
add_subdirectory(examples)

# Enable testing
enable_testing()
add_subdirectory(tests)

# Installation rules
install(DIRECTORY include/ DESTINATION include)
