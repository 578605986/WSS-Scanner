@echo off
chcp 65001 >nul
echo ============================================
echo WSS - Windows Security Scanner 一键编译脚本
echo ============================================
echo.

REM 检查CMake
cmake --version >nul 2>&1
if errorlevel 1 (
    echo [错误] 未找到CMake，请先安装CMake 3.20+
    echo 下载地址: https://cmake.org/download/
    pause
    exit /b 1
)

REM 检查Visual Studio
where cl >nul 2>&1
if errorlevel 1 (
    echo [错误] 未找到Visual Studio编译器
    echo 请先安装Visual Studio 2022 (包含C++桌面开发)
    pause
    exit /b 1
)

REM 检查Qt
if not exist "C:\Qt\6.5.0\msvc2019_64" (
    echo [错误] 未找到Qt 6.5.0
    echo 请先安装Qt 6.5.0 (MSVC 2019 64-bit)
    echo 下载地址: https://www.qt.io/download
    pause
    exit /b 1
)

echo [1/4] 清理旧构建...
if exist build rmdir /s /q build
echo 完成.
echo.

echo [2/4] 配置项目...
cmake -B build -S . -DCMAKE_PREFIX_PATH="C:/Qt/6.5.0/msvc2019_64" -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo [错误] CMake配置失败
    pause
    exit /b 1
)
echo 完成.
echo.

echo [3/4] 编译项目（可能需要几分钟）...
cmake --build build --config Release --parallel
echo.
if errorlevel 1 (
    echo [错误] 编译失败
    pause
    exit /b 1
)
echo 完成.
echo.

echo [4/4] 准备发布包...
if exist WSS-Release rmdir /s /q WSS-Release
mkdir WSS-Release
copy build\Release\WSS.exe WSS-Release\
xcopy /e /i /y signatures WSS-Release\signatures\
copy README.md WSS-Release\
copy QUICKSTART.md WSS-Release\
copy 使用说明.txt WSS-Release\
echo 完成.
echo.

echo ============================================
echo 编译成功！
echo ============================================
echo.
echo 输出文件: WSS-Release/WSS.exe
echo.
echo 使用方法:
echo 1. 进入 WSS-Release 目录
echo 2. 双击运行 WSS.exe
echo.
pause
