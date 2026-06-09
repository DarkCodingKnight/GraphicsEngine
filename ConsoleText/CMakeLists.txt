cmake_minimum_required(VERSION 3.12)

set(PROJECT_NAME ConsoleText)
set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} INTERFACE includes/ConsoleText.hpp)
target_include_directories(${PROJECT_NAME} INTERFACE includes)