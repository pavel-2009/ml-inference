cmake_minimum_required(VERSION 3.10)
project(ML-Inference VERSION 1.0.0)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Project configuration
message(STATUS "Building ML-Inference ${PROJECT_VERSION}")

# Find dependencies
find_package(nlohmann_json REQUIRED)

# Fetch Crow
include(FetchContent)

FetchContent_Declare(
    crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.2.1
)

FetchContent_MakeAvailable(crow)

# Source files
set(PROJECT_SOURCES
    src/inference_server.cpp  # Новый главный файл
    src/model/models/average_model.cpp
    src/model/model_factory.cpp
    src/model/model_manager.cpp
    src/model/models/sort_model.cpp
    src/model/models/sum_model.cpp
    src/threadpool/task_queue.cpp
    src/threadpool/threadpool.cpp
    src/async_loader/async_loader.cpp
    src/inference_service/inference_service.cpp
    src/router/router.cpp
    src/http/crow_server.cpp
)

# Create executable
add_executable(ml_inference_server ${PROJECT_SOURCES})  # Новое имя

# Include directories
target_include_directories(ml_inference_server PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/async_loader
    ${PROJECT_SOURCE_DIR}/include/threadpool
    ${PROJECT_SOURCE_DIR}/include/model
    ${PROJECT_SOURCE_DIR}/include/inference-service
    ${PROJECT_SOURCE_DIR}/include/http
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/async_loader
    ${PROJECT_SOURCE_DIR}/src/threadpool
    ${PROJECT_SOURCE_DIR}/src/model
    ${PROJECT_SOURCE_DIR}/src/inference-service
)

# Link libraries
target_link_libraries(ml_inference_server
    nlohmann_json::nlohmann_json
    Crow::Crow
)

# Compiler options
if(MSVC)
    target_compile_options(ml_inference_server PRIVATE /W4 /std:c++20)
else()
    target_compile_options(ml_inference_server PRIVATE -Wall -Wextra -Wpedantic -std=c++20)
    target_compile_features(ml_inference_server PRIVATE cxx_std_20)
endif()

# Testing - можно оставить старый main.cpp как тест
add_executable(ml_inference_test src/main.cpp ${PROJECT_SOURCES})
# ... остальные настройки для теста

message(STATUS "Project configured successfully")
message(STATUS "Main server: ml_inference_server")
message(STATUS "Test binary: ml_inference_test")