# Example programs
add_executable(basic_example basic_example.cpp)
target_link_libraries(basic_example newton_core)

if(USE_BOOST AND Boost_FOUND)
    add_executable(boost_example boost_example.cpp)
    target_link_libraries(boost_example newton_core)
endif()

# Installation
install(TARGETS basic_example DESTINATION bin)
if(USE_BOOST AND Boost_FOUND)
    install(TARGETS boost_example DESTINATION bin)
endif()
