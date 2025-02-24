cmake_minimum_required(VERSION 3.0)

project(kiran-desktop VERSION 2.7)
set(KIRAN_DESKTOP_VERSION 2.7)
set(KIRAN_DESKTOP_DEV_API_VERSION 2)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(PkgConfig REQUIRED)
find_package(KF5NetworkManagerQt REQUIRED)

pkg_search_module(KLOG REQUIRED klog-qt5)
find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        Svg
        DBus
        Network
        LinguistTools)

if (${KF5NetworkManagerQt_VERSION} VERSION_GREATER_EQUAL 5.62)
    add_definitions(-DWIRELESS_NETWORK_SCAN_FINISHED_SIGNAL)
endif()

file(GLOB_RECURSE SOURCES_FILE "./lib/*.cpp" "./lib/*.h" "./include/*.h")

add_library(${PROJECT_NAME} SHARED ${SOURCES_FILE})

set_target_properties(${PROJECT_NAME} PROPERTIES
        SOVERSION ${KIRAN_DESKTOP_DEV_API_VERSION})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_BINARY_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${KLOG_INCLUDE_DIRS})

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${KLOG_LIBRARIES}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Svg
    Qt5::Network
    KF5::NetworkManagerQt)

set(KIRAN_DESKTOP_LIB_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/)
install(TARGETS ${PROJECT_NAME} DESTINATION ${KIRAN_DESKTOP_LIB_DIR})

file(GLOB_RECURSE HEADERS_FILE "./include/*.h")
set(KIRAN_DESKTOP_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/)
set(KIRAN_DESKTOP_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/)
install(FILES ${HEADERS_FILE} DESTINATION ${KIRAN_DESKTOP_INCLUDE_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/kiran-desktop.pc.in kiran-desktop.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kiran-desktop.pc
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/pkgconfig/)
add_subdirectory(example)