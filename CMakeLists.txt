cmake_minimum_required(VERSION 3.20)
project(WSS VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# 源文件 - 最小可运行版本
set(SOURCES
    src/main.cpp
    src/scanner/FileScanner.cpp
    src/scanner/SignatureDB.cpp
    src/monitor/RealTimeMonitor.cpp
    src/quarantine/QuarantineManager.cpp
    src/gui/MainWindow.cpp
    src/utils/Logger.cpp
    src/utils/HashCalculator.cpp
)

# 头文件
set(HEADERS
    include/scanner/FileScanner.h
    include/scanner/SignatureDB.h
    include/monitor/RealTimeMonitor.h
    include/quarantine/QuarantineManager.h
    include/gui/MainWindow.h
    include/utils/Logger.h
    include/utils/HashCalculator.h
)

# 可执行文件
add_executable(WSS ${SOURCES} ${HEADERS})

# 包含目录
target_include_directories(WSS PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

# 链接库
target_link_libraries(WSS PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Windows特定设置
if(WIN32)
    set_target_properties(WSS PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
