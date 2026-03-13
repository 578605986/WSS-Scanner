cmake_minimum_required(VERSION 3.20)
project(WSS VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(WSS src/main.cpp)

target_link_libraries(WSS PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

if(WIN32)
    set_target_properties(WSS PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
