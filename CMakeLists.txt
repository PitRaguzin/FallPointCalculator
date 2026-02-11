cmake_minimum_required(VERSION 3.16)

project(FallPointCalculator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core Gui LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Gui LinguistTools)

set(PROJECT_SOURCES main.cpp)

add_subdirectory(resources)
add_subdirectory(gui)
add_subdirectory(sub)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(FallPointCalculator MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
    if(ANDROID)
        add_library(FallPointCalculator SHARED ${PROJECT_SOURCES})
    else()
        add_executable(FallPointCalculator ${PROJECT_SOURCES})
    endif()
endif()

target_link_libraries(FallPointCalculator PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    MainWindow
)
