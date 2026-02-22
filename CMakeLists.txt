cmake_minimum_required(VERSION 3.28)
project(newton)

set(CMAKE_CXX_STANDARD 17)

# Find Boost
find_package(Boost REQUIRED COMPONENTS program_options log thread filesystem system)
include_directories(${Boost_INCLUDE_DIRS})

# Enable profiling
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -g")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")

include_directories("include")

# All library sources, excluding the program entry point
file(GLOB_RECURSE LIB_SRC_FILES "src/*.cpp" "src/**/*.cpp" "include/**/*.h" "include/*.h")
list(REMOVE_ITEM LIB_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# Main executable
add_executable(MultivaluedHeuristicSearch ${LIB_SRC_FILES} src/main.cpp)
target_link_libraries(MultivaluedHeuristicSearch ${Boost_LIBRARIES})

# Test executable (shares all library sources, has its own main via doctest)
file(GLOB_RECURSE TEST_FILES "tests/*.cpp")
add_executable(RunAllTests ${LIB_SRC_FILES} ${TEST_FILES})
target_link_libraries(RunAllTests ${Boost_LIBRARIES})
