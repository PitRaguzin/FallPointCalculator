cmake_minimum_required(VERSION 3.5)

project(Mathematics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Kalman)

add_library(${PROJECT_NAME} mathematics.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/)

target_link_libraries(${PROJECT_NAME}
    Kalman
)
