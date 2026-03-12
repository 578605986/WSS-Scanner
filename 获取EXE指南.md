# WSS - 获取EXE文件的三种方法

## 方法一：GitHub Actions 自动编译（推荐，最简单）

### 步骤1：创建GitHub仓库
1. 访问 https://github.com/new
2. 仓库名称：WSS-Scanner
3. 选择"Public"（公开）
4. 点击"Create repository"

### 步骤2：上传代码
```bash
cd WSS-Scanner
git init
git add .
git commit -m "Initial commit"
git branch -M main
git remote add origin https://github.com/你的用户名/WSS-Scanner.git
git push -u origin main
```

### 步骤3：自动编译
上传后，GitHub Actions会自动开始编译（约5-10分钟）

### 步骤4：下载EXE
1. 进入GitHub仓库页面
2. 点击 "Actions" 标签
3. 等待编译完成（绿色✓）
4. 点击最新的工作流
5. 下载 "WSS-Windows-EXE"  artifact
6. 解压即可使用！

---

## 方法二：本地一键编译（需要安装环境）

### 准备工作（只需一次）

#### 1. 安装Visual Studio 2022
- 下载：https://visualstudio.microsoft.com/
- 安装组件："使用C++的桌面开发"
- 大小：约5GB

#### 2. 安装Qt 6.5.0
- 下载：https://www.qt.io/download
- 选择：Qt 6.5.0 → MSVC 2019 64-bit
- 大小：约3GB

#### 3. 安装CMake
- 下载：https://cmake.org/download/
- 选择：Windows x64 Installer
- 安装时选择"添加到PATH"

### 开始编译

#### 方式A：双击编译（最简单）
1. 双击 `build.bat`
2. 等待编译完成（5-10分钟）
3. 在 `WSS-Release` 目录中找到 `WSS.exe`

#### 方式B：命令行编译
```cmd
cd WSS-Scanner
mkdir build && cd build

# 配置
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.5.0/msvc2019_64" -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build . --config Release --parallel

# 输出在 build/Release/WSS.exe
```

---

## 方法三：Python演示版（无需编译，立即使用）

### 安装依赖（1分钟）
```cmd
pip install PyQt6
```

### 运行程序
```cmd
python wss_demo.py
```

**优点：**
- 无需编译，立即使用
- 功能完整，界面相同
- 适合测试和学习

**缺点：**
- 性能不如C++版
- 不如C++版专业

---

## 常见问题

### Q: 编译报错找不到Qt？
A: 修改build.bat中的Qt路径，或设置QT_DIR环境变量

### Q: 提示缺少DLL？
A: 需要将Qt的bin目录添加到PATH，或复制DLL到exe目录

### Q: GitHub Actions编译失败？
A: 检查CMakeLists.txt路径是否正确，或查看Actions日志

### Q: 杀毒软件报毒？
A: 这是正常现象，安全软件常被误报。请将WSS.exe添加到白名单。

---

## 推荐方案

| 方案 | 难度 | 时间 | 效果 |
|:---|:---:|:---:|:---:|
| GitHub Actions | ⭐ 简单 | 10分钟 | ⭐⭐⭐ 专业 |
| 本地编译 | ⭐⭐ 中等 | 30分钟 | ⭐⭐⭐ 专业 |
| Python版 | ⭐ 简单 | 2分钟 | ⭐⭐ 可用 |

**新手推荐：先用Python版测试 → 再用GitHub Actions获取专业版**

---

**需要帮助？提交Issue或联系开发者！** 🛡️
