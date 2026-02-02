cmake_minimum_required(VERSION 3.15)
project(TaskCLI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find json library if you use find_package (optional for now)
# find_package(nlohmann_json 3.2 REQUIRED)   # if installed via vcpkg/conan

add_executable(taskCLI
    taskCLI.cpp
    utils.cpp
)

# If you use nlohmann/json as header-only (most common)
target_include_directories(taskCLI PRIVATE ./libs)   # your -I./libs