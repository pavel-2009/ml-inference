cmake_minimum_required(VERSION 3.10)
project(ML-Inference VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project configuration
message(STATUS "Building ML-Inference ${PROJECT_VERSION}")

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(PROJECT_SOURCES
    src/main.cpp
    src/models/average_model.cpp
    src/models/model_factory.cpp
    src/models/model_manager.cpp
    src/models/sort_model.cpp
    src/models/sum_model.cpp
    src/threadpool/task_queue.cpp
    src/threadpool/threadpool.cpp
)

# Create executable
add_executable(ml_inference ${PROJECT_SOURCES})

# Target properties
target_compile_options(ml_inference PRIVATE -Wall -Wextra -Wpedantic)

# Installation (optional)
install(TARGETS ml_inference DESTINATION bin)

# Testing
enable_testing()
add_test(NAME ml_inference_test COMMAND ml_inference)

message(STATUS "Project configured successfully")