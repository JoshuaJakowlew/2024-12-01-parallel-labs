cmake_minimum_required(VERSION 3.22)

project(parallel-labs
    VERSION 0.0.0
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

find_package(spdlog REQUIRED)
find_package(range-v3 REQUIRED)
find_package(bshoshany-thread-pool REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(gmp REQUIRED)

# Cannon

add_executable(cannon
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cannon.cpp
)

set_target_properties(cannon PROPERTIES
    CXX_STANDARD          20
    CXX_STANDARD_REQUIRED TRUE
)

target_include_directories(cannon
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(cannon
    spdlog::spdlog
    range-v3::range-v3
    bshoshany-thread-pool::bshoshany-thread-pool
)

# Monte-Carlo

add_executable(monte-carlo
    ${CMAKE_CURRENT_SOURCE_DIR}/src/monte_carlo.cpp
)

set_target_properties(monte-carlo PROPERTIES
    CXX_STANDARD          20
    CXX_STANDARD_REQUIRED TRUE
)

target_include_directories(monte-carlo
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(monte-carlo
    spdlog::spdlog
)

# Merge sort

add_executable(merge
    ${CMAKE_CURRENT_SOURCE_DIR}/src/merge.cpp
)

set_target_properties(merge PROPERTIES
    CXX_STANDARD          20
    CXX_STANDARD_REQUIRED TRUE
)

target_include_directories(merge
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(merge
    spdlog::spdlog
)

# Max search

add_executable(max
    ${CMAKE_CURRENT_SOURCE_DIR}/src/max.cpp
)

set_target_properties(max PROPERTIES
    CXX_STANDARD          20
    CXX_STANDARD_REQUIRED TRUE
)

target_include_directories(max
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(max
    spdlog::spdlog
)

# Word count

add_executable(wc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/wc.cpp
)

set_target_properties(wc PROPERTIES
    CXX_STANDARD          20
    CXX_STANDARD_REQUIRED TRUE
)

target_include_directories(wc
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(wc
    spdlog::spdlog
    range-v3::range-v3
    bshoshany-thread-pool::bshoshany-thread-pool
)

# BFS

add_executable(bfs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bfs.cpp
)

set_target_properties(bfs PROPERTIES
    CXX_STANDARD          20
    CXX_STANDARD_REQUIRED TRUE
)

target_include_directories(bfs
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(bfs
    spdlog::spdlog
    bshoshany-thread-pool::bshoshany-thread-pool
    nlohmann_json::nlohmann_json
)

# Fibonacci

add_executable(fib
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fib.cpp
)

set_target_properties(fib PROPERTIES
    CXX_STANDARD          20
    CXX_STANDARD_REQUIRED TRUE
)

target_include_directories(fib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(fib
    spdlog::spdlog
    bshoshany-thread-pool::bshoshany-thread-pool
    gmp::gmp
)