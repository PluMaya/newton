# Library source files
add_library(newton_core
    core.cpp
)

target_include_directories(newton_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

if(USE_BOOST AND Boost_FOUND)
    target_sources(newton_core PRIVATE boost_utils.cpp)
    target_link_libraries(newton_core PUBLIC
        Boost::filesystem
        Boost::system
    )
endif()

# Set library properties
set_target_properties(newton_core PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Installation
install(TARGETS newton_core
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
