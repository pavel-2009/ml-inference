cmake_minimum_required(VERSION 3.10)
project(ml_inference)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем тесты
enable_testing()

# Находим nlohmann_json
find_package(nlohmann_json REQUIRED)

# Основная библиотека
set(SOURCES
    src/models/average_model.cpp
    src/models/sort_model.cpp
    src/models/sum_model.cpp
    src/models/model_factory.cpp
    src/models/model_manager.cpp
    src/threadpool/task_queue.cpp
    src/threadpool/threadpool.cpp
)

set(HEADERS
    include/models/average_model.hpp
    include/models/base_model.hpp
    include/models/imodel.hpp
    include/models/inference_request.hpp
    include/models/inference_response.hpp
    include/models/model_factory.hpp
    include/models/model_info.hpp
    include/models/model_manager.hpp
    include/models/sort_model.hpp
    include/models/sum_model.hpp
    include/threadpool/task_queue.hpp
    include/threadpool/threadpool.hpp
)

add_library(ml_inference_lib ${SOURCES} ${HEADERS})
target_include_directories(ml_inference_lib PUBLIC include)
target_link_libraries(ml_inference_lib nlohmann_json::nlohmann_json)

# Основной исполняемый файл (опционально)
add_executable(ml_inference src/main.cpp)
target_link_libraries(ml_inference ml_inference_lib)

# Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

FetchContent_MakeAvailable(googletest)

# Тесты
add_executable(ml_inference_tests
    tests/test_models.cpp
    tests/test_model_factory.cpp
    tests/test_model_manager.cpp
    tests/test_threadpool.cpp
    tests/test_integration.cpp
    src/models/average_model.cpp
    src/models/sort_model.cpp
    src/models/sum_model.cpp
    src/models/model_factory.cpp
    src/models/model_manager.cpp
    src/threadpool/task_queue.cpp
    src/threadpool/threadpool.cpp
)

target_include_directories(ml_inference_tests PRIVATE include)
target_link_libraries(ml_inference_tests
    nlohmann_json::nlohmann_json
    gtest_main
    pthread
)

include(GoogleTest)
gtest_discover_tests(ml_inference_tests)