cmake_minimum_required(VERSION 3.10)
project(Resources)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(RESOURCES_SUFFIX Sources)

set(PRJ_RESOURCES_NAME ${PROJECT_NAME}${RESOURCES_SUFFIX})
set(RESOURCES_FILE resources.qrc)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_resources(${PRJ_RESOURCES_NAME} ${RESOURCES_FILE})
    qt_add_library(${PROJECT_NAME} ${${PRJ_RESOURCES_NAME}}
        resources.qrc)
else()
    qt5_add_resources(${PRJ_RESOURCES_NAME} ${RESOURCES_FILE})

    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED ${${PRJ_RESOURCES_NAME}})
    else()
        add_library(${PROJECT_NAME} ${${PRJ_RESOURCES_NAME}})
    endif()
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
)
