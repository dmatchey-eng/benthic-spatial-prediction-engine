cmake_minimum_required(VERSION 3.15)
project(BenthicWyvernEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# 1. Main Executable Application Target
add_executable(BenthicWyvernEngine
    src/main.cpp
    src/CyclicalCipher.cpp
    src/BenthicMesh.cpp
    src/SparseLoader.cpp
    src/SurveyDigester.cpp
)

# 2. Test Suite Suite Target
add_executable(BenthicTests
    tests/test.cpp
    src/CyclicalCipher.cpp
    src/BenthicMesh.cpp
    src/SparseLoader.cpp
    src/SurveyDigester.cpp
)
