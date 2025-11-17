cmake_minimum_required(VERSION 3.16)
project(template_insight_core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTING "Build unit tests" ON)

include(FetchContent)

# ---------- Dependencies for main code ----------
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.14.1
)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)

FetchContent_MakeAvailable(spdlog nlohmann_json)

# ---------- Core library ----------
add_library(template_insight_core
    src/api.cpp
    src/config.cpp
    src/issues.cpp
)

target_include_directories(template_insight_core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(template_insight_core
    PUBLIC spdlog::spdlog nlohmann_json::nlohmann_json
)

# ---------- CLI executable ----------
add_executable(template_insight_cli
    src/main_cli.cpp
)

target_link_libraries(template_insight_cli
    PRIVATE template_insight_core spdlog::spdlog
)

# ---------- Tests ----------
if(BUILD_TESTING)
    include(CTest)
    enable_testing()

    # Try system GTest first
    find_package(GTest QUIET)

    if(NOT GTest_FOUND)
        message(STATUS "GTest not found in system, fetching from GitHub...")

        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.15.0
        )

        set(gtest_force_shared_crt ON CACHE BOOL "Use shared CRT" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()

    add_subdirectory(tests)
endif()
