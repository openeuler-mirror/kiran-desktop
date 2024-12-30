set(TARGET_NAME network-demo-example)

file(GLOB_RECURSE EXAMPLE_SRC "*.cpp" "*.h")
add_executable(${TARGET_NAME} ${EXAMPLE_SRC})
target_include_directories(${TARGET_NAME} PRIVATE "../include")
target_link_libraries(${TARGET_NAME} PRIVATE 
    Qt5::Core
    Qt5::Gui
    Qt5::Svg
    Qt5::X11Extras
    Qt5::Widgets
    KF5::NetworkManagerQt
    network-demo)