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

FetchContent_Declare(
    Sha256
    GIT_REPOSITORY https://github.com/Arkanic/sha256-cpp
    GIT_TAG v1.1.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(Sha256)

add_executable(visitcount)
target_compile_features(visitcount
    PRIVATE
        cxx_std_20
)
target_sources(visitcount
    PRIVATE
        src/main.cpp
        src/filenumbersync.cpp
        src/nonce.cpp
)
target_include_directories(visitcount
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${sha256_SOURCE_DIR}/sha256-cpp
)
target_link_libraries(visitcount
    PRIVATE
        Crow::Crow
)