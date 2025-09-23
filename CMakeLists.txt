cmake_minimum_required(VERSION 3.10)

# Project name and language
project(ConcurrentWebServer LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files
set(SOURCES
    src/main.cpp
    src/tcp-server/tcp-server.cpp
)

# Create the executable
add_executable(concurrent-web-server ${SOURCES})

# Include directories (so "tcp-server/tcp-server.h" is found)
target_include_directories(concurrent-web-server
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

# --- Copy static files (e.g., index.html) to build directory ---
file(COPY ${CMAKE_SOURCE_DIR}/public/index.html
     DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/public/big.html
     DESTINATION ${CMAKE_BINARY_DIR})