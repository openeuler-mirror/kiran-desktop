set(TARGET_NAME network-window)

file(GLOB_RECURSE EXAMPLE_SRC "*.cpp" "*.h")
add_executable(${TARGET_NAME} ${EXAMPLE_SRC})
target_include_directories(${TARGET_NAME} PRIVATE "../include")
target_link_libraries(${TARGET_NAME} PRIVATE 
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Svg
    KF5::NetworkManagerQt
    kiran-desktop)