# Test files
add_executable(test_core test_core.cpp)
target_link_libraries(test_core newton_core)

# Add test
add_test(NAME test_core COMMAND test_core)
