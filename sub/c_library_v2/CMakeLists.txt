cmake_minimum_required(VERSION 3.10)

project(MAVlink)

set(MAVLINK_SRC main.cpp)

add_library(${PROJECT_NAME} ${MAVLINK_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)
