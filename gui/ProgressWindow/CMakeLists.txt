cmake_minimum_required(VERSION 3.10)
project(ProgressWindow)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)

set(PROGRESSWINDOW_SRC progresswindow.cpp progresswindow.ui)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_library(${PROJECT_NAME} ${PROGRESSWINDOW_SRC})
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED ${PROGRESSWINDOW_SRC})
    else()
        add_library(${PROJECT_NAME} ${PROGRESSWINDOW_SRC})
    endif()
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
)
