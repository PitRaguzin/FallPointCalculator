cmake_minimum_required(VERSION 3.16)

project(FallPointCalculator VERSION 1.0.0)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CPACK_GENERATOR WIX)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION "1.0.0")

include(CPack)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core Gui LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Gui LinguistTools)

set(PROJECT_SOURCES main.cpp)

add_subdirectory(resources)
add_subdirectory(gui)
add_subdirectory(sub)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCES})
    else()
        add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
    endif()
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    MainWindow
)

if(WIN32)
    set(WIX_PATH "C:/Program Files (x86)/WiX Toolset v3.11/bin")

    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/deploy)
    file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.exe DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/deploy/)
    file(COPY ${CMAKE_CURRENT_BINARY_DIR}/gui/MainWindow.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/deploy/)

    qt_deploy_runtime_dependencies(${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/deploy/)

    add_custom_target(Installer
        COMMAND ${WIX_PATH}/candle.exe ${CMAKE_CURRENT_SOURCE_DIR}/installer/instconf.wxs -o ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_install.wixobj
        COMMAND ${WIX_PATH}/light.exe ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_install.wixobj -o ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_install.msi
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/installer
        COMMENT "Building MSI Installer..."
    )
endif()
