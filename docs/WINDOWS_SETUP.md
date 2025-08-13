# LitePad Windows 开发环境搭建指南

## 概述

本指南将帮助您在Windows系统上搭建LitePad代码编辑器的开发环境。LitePad是一个使用C++17开发的跨平台轻量级代码编辑器。

## 系统要求

- Windows 10 或更高版本
- 至少4GB内存
- 至少1GB可用磁盘空间

## 必需工具

### 1. C++编译器

推荐使用Visual Studio 2022 Community（免费）：

**自动安装（推荐）：**
```bash
# 运行项目根目录下的脚本
setup_windows_dev.bat
```

**手动安装：**
1. 下载 [Visual Studio 2022 Community](https://visualstudio.microsoft.com/zh-hans/vs/community/)
2. 安装时选择 **"C++ 桌面开发"** 工作负荷
3. 确保包含以下组件：
   - MSVC v143 编译器工具集
   - Windows 10/11 SDK（最新版本）
   - CMake工具（可选，也可单独安装）

**替代方案 - MinGW-w64：**
```bash
# 使用 winget 安装 MSYS2
winget install -e --id MSYS2.MSYS2

# 在 MSYS2 中安装 MinGW-w64
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-make
```

### 2. CMake

**使用 winget 安装：**
```bash
winget install -e --id Kitware.CMake
```

**手动安装：**
1. 访问 [CMake官网](https://cmake.org/download/)
2. 下载Windows安装包
3. 安装时选择 "Add CMake to system PATH"

### 3. Git

**使用 winget 安装：**
```bash
winget install -e --id Git.Git
```

**手动安装：**
1. 访问 [Git官网](https://git-scm.com/download/win)
2. 下载并安装

## 快速开始

### 1. 克隆项目
```bash
git clone <your-repo-url> LitePad
cd LitePad
```

### 2. 构建项目
```bash
# 使用提供的构建脚本
build.bat

# 或者手动构建
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### 3. 运行应用程序
```bash
# 从构建目录运行
build\bin\Release\LitePad.exe

# 或者双击可执行文件
```

## 项目结构

```
LitePad/
├── src/                    # 源代码
│   ├── main.cpp           # 程序入口
│   ├── MainWindow.h/cpp   # 主窗口类
│   ├── Editor.h/cpp       # 编辑器核心
│   ├── platforms/         # 平台特定实现
│   │   ├── WindowsWindow.h    # Windows窗口头文件
│   │   └── WindowsWindow.cpp  # Windows窗口实现
│   └── ...
├── config/                # 配置文件
├── plugins/               # 插件系统
├── build.bat             # Windows构建脚本
├── setup_windows_dev.bat # 开发环境搭建脚本
└── CMakeLists.txt        # CMake配置文件
```

## 开发环境配置

### 推荐的IDE

1. **Visual Studio 2022**
   - 完整的C++开发环境
   - 内置CMake支持
   - 强大的调试功能

2. **Visual Studio Code**
   - 轻量级编辑器
   - 需要安装C++扩展包
   - 良好的CMake集成

3. **CLion**
   - JetBrains的C++ IDE
   - 优秀的代码分析
   - 内置CMake支持

### 配置Visual Studio Code

安装推荐扩展：
```
- C/C++ (Microsoft)
- CMake Tools (Microsoft)
- CMake (twxs)
- Git History (Don Jayamanne)
```

配置launch.json：
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug LitePad",
            "type": "cppvsdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/bin/Debug/LitePad.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "console": "externalTerminal"
        }
    ]
}
```

## Windows特定实现

### 窗口系统
- 使用Win32 API实现原生Windows界面
- 支持文件拖放
- 集成Windows通用控件

### 关键特性
- 原生Windows外观和感觉
- 支持高DPI显示
- 文件对话框集成
- 系统托盘支持（计划中）

## 构建配置

### Debug构建
```bash
mkdir build-debug
cd build-debug
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
```

### Release构建
```bash
mkdir build-release
cd build-release
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

## 常见问题

### 1. 编译器未找到
**问题：** `'cl' is not recognized as an internal or external command`

**解决方案：**
- 确保安装了Visual Studio的C++工作负荷
- 使用"Developer Command Prompt for VS 2022"
- 或者在普通命令行中运行：
  ```bash
  "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
  ```

### 2. CMake配置失败
**问题：** CMake找不到编译器或生成器

**解决方案：**
```bash
# 指定生成器
cmake .. -G "Visual Studio 17 2022" -A x64

# 或使用Ninja（如果安装了）
cmake .. -G "Ninja" -DCMAKE_CXX_COMPILER=cl
```

### 3. 缺少Windows SDK
**问题：** 找不到windows.h或相关头文件

**解决方案：**
- 在Visual Studio Installer中安装Windows SDK
- 确保选择了最新版本的Windows SDK

### 4. 链接错误
**问题：** 找不到某些库文件

**解决方案：**
- 检查CMakeLists.txt中的库链接配置
- 确保所有必需的Windows库都已链接

## 调试技巧

### 1. 使用Visual Studio调试器
```bash
# 在Visual Studio中打开项目
devenv build/LitePad.sln
```

### 2. 命令行调试
```bash
# 使用Visual Studio调试器
devenv /debugexe build\bin\Debug\LitePad.exe
```

### 3. 内存泄漏检测
在Debug模式下，可以使用Visual Studio的诊断工具检测内存泄漏。

## 性能优化

### 编译器优化选项
Release构建会自动启用优化，包括：
- `/O2` - 最大化速度
- `/DNDEBUG` - 禁用断言
- `/GL` - 全程序优化

### 运行时优化
- 使用Release构建进行性能测试
- 启用链接时代码生成(LTCG)
- 考虑使用Profile Guided Optimization (PGO)

## 部署

### 创建分发包
```bash
# 构建Release版本
build.bat

# 复制必要文件
mkdir distribution
copy build\bin\Release\LitePad.exe distribution\
copy config\default.conf distribution\config\
```

### 依赖项
LitePad使用静态链接，因此通常不需要额外的运行时库。但是可能需要：
- Visual C++ Redistributable（如果使用动态链接）
- Windows通用运行时（Windows 10+自带）

## 贡献指南

1. **代码规范**
   - 使用C++17标准
   - 代码注释使用中文
   - 遵循现有的代码风格

2. **提交规范**
   - 使用清晰的提交信息
   - 一次提交只做一件事
   - 包含必要的测试

3. **测试**
   - 在提交前确保构建成功
   - 测试基本功能是否正常
   - 在不同Windows版本上测试

## 获取帮助

如果在搭建开发环境时遇到问题：

1. 检查本文档的"常见问题"部分
2. 查看项目的Issue页面
3. 在项目仓库中创建新的Issue
4. 提供详细的错误信息和系统配置

---

**最后更新：** 2024年12月  
**适用版本：** LitePad v1.0+
