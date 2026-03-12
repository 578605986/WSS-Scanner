# Windows Security Scanner (WSS)
# 专业级Windows安全扫描工具

## 项目说明
- 语言：C++17
- 框架：Qt 6
- 功能：文件扫描 + 实时监控 + 自动隔离
- 用途：个人防护 + 商业安全服务

## 目录结构
```
WSS/
├── src/                 # 源代码
│   ├── main.cpp        # 入口
│   ├── scanner/        # 扫描引擎
│   ├── monitor/        # 监控模块
│   ├── quarantine/     # 隔离系统
│   └── gui/            # 图形界面
├── include/            # 头文件
├── lib/                # 第三方库
├── signatures/         # 病毒签名库
├── build/              # 编译输出
└── CMakeLists.txt      # CMake配置
```

## 快速开始

### 1. 安装依赖
- Visual Studio 2022 (C++桌面开发)
- Qt 6.5+ (MSVC 2019 64-bit)
- CMake 3.20+

### 2. 编译
```bash
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.5.0/msvc2019_64"
cmake --build . --config Release
```

### 3. 运行
```bash
./WSS.exe
```

## 功能特性
- ✅ 多线程文件扫描
- ✅ 实时监控 (文件/进程/注册表)
- ✅ 自动隔离威胁
- ✅ 专业GUI界面
- ✅ 详细日志报告

## 许可证
MIT License - 可自由用于商业服务
