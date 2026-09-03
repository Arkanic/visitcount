cmake_minimum_required(VERSION 3.31)

project(visitcount)

include(FetchContent)

# crow server
FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow
    GIT_TAG v1.3.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(Crow)

add_executable(visitcount)
target_sources(visitcount
    PRIVATE
        src/main.cpp
        src/filenumbersync.cpp
)
target_include_directories(visitcount
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(visitcount
    PRIVATE
        Crow::Crow
)