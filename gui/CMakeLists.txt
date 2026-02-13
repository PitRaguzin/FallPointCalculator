cmake_minimum_required(VERSION 3.10)
project(MainWindow)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)

set(MAINWINDOW_SRC mainwindow.cpp mainwindow.ui)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_library(${PROJECT_NAME} ${MAINWINDOW_SRC})
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED ${MAINWINDOW_SRC})
    else()
        add_library(${PROJECT_NAME} ${MAINWINDOW_SRC})
    endif()
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)

target_link_libraries(${PROJECT_NAME}
    ResourcesLib
    Mathematics
    LogFileParser
)
