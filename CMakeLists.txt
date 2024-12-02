cmake_minimum_required(VERSION 3.22)

project(parallel-labs
    VERSION 0.0.0
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

find_package(spdlog REQUIRED)
find_package(range-v3 REQUIRED)
find_package(bshoshany-thread-pool REQUIRED)

# TODO: Refactor me

add_executable(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/monte_carlo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cannon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD          20
    CXX_STANDARD_REQUIRED TRUE
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}
    spdlog::spdlog
    range-v3::range-v3
    bshoshany-thread-pool::bshoshany-thread-pool
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