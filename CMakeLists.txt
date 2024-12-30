cmake_minimum_required(VERSION 3.0)
project(network-demo VERSION 2.5)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(PkgConfig REQUIRED)
find_package(KF5NetworkManagerQt REQUIRED)

if (${KF5NetworkManagerQt_VERSION} VERSION_GREATER_EQUAL 5.62)
    add_definitions(-DWIRELESS_NETWORK_SCAN_FINISHED_SIGNAL)
endif()

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        Multimedia
        Network
        X11Extras
        Svg
        DBus
        Xml
        Concurrent
        LinguistTools)

file(GLOB_RECURSE SOURCES_FILE "./src/*.cpp" "./src/*.h" "./include/*.h")
add_library(${PROJECT_NAME} SHARED ${SOURCES_FILE})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ./include)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt5::Core
    Qt5::Gui
    Qt5::Svg
    Qt5::X11Extras
    Qt5::Widgets
    KF5::NetworkManagerQt)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR}/network-demo/)

file(GLOB_RECURSE HEADERS_FILE "./include/*.h")
install(FILES ${HEADERS_FILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

add_subdirectory(example)