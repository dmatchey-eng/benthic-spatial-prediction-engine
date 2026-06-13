cmake_minimum_required(VERSION 3.15)
project(BenthicWyvernEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure include search paths
include_directories(include)

# Define executable and source files linkage
add_executable(BenthicWyvernEngine
    src/main.cpp
    src/CyclicalCipher.cpp
    src/BenthicMesh.cpp
)
