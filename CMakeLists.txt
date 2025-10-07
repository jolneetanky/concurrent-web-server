# cmake_minimum_required(VERSION 3.10)

# # Project name and language
# project(ConcurrentWebServer LANGUAGES CXX)

# # Set C++ standard
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

# # Add source files
# set(SOURCES
#     src/main.cpp
#     src/tcp-server/single-thread-server.cpp
#     src/tcp-server/job-queue.cpp
#     src/tcp-server/multi-thread-server.cpp
# )

# # Create the executable
# add_executable(concurrent-web-server ${SOURCES})

# # Include directories (so "tcp-server/tcp-server.h" is found)
# target_include_directories(concurrent-web-server
#     PRIVATE
#         ${CMAKE_SOURCE_DIR}/src
# )


cmake_minimum_required(VERSION 3.14)
project(concurrent-web-server CXX)

set(CMAKE_CXX_STANDARD 17)

# --- Server executable ---
file(GLOB_RECURSE SERVER_SOURCES src/*.cpp)
add_executable(concurrent-web-server ${SERVER_SOURCES})

target_include_directories(concurrent-web-server PRIVATE src)

# --- GoogleTest via FetchContent ---
include(FetchContent)
include(GoogleTest)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# prevent gtest messing with compiler/linker flags
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

# --- Tests ---
# file(GLOB TEST_SOURCES tests/*.cpp)
# add_executable(job-queue-test ${TEST_SOURCES})

# target_link_libraries(job-queue-test PRIVATE
#   GTest::gtest_main
#   pthread
# )

# gtest_discover_tests(job-queue-test)

# GoogleTest sources
file(GLOB GTEST_SOURCES tests/*-test.cpp)
add_executable(job-queue-test ${GTEST_SOURCES})
target_link_libraries(job-queue-test PRIVATE GTest::gtest_main pthread)
gtest_discover_tests(job-queue-test)

# Standalone demo (doesn't use GoogleTest)
add_executable(job-queue-thread-unsafe tests/job-queue-thread-unsafe.cpp)
target_link_libraries(job-queue-thread-unsafe PRIVATE pthread)

# --- Copy static files (e.g., index.html) to build directory ---
file(COPY ${CMAKE_SOURCE_DIR}/public/index.html
     DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/public/big.html
     DESTINATION ${CMAKE_BINARY_DIR})