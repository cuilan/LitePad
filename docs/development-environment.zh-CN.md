# LitePad 开发环境规范

## 概述

本文档规定了 LitePad 项目在不同平台上的开发工具链要求和版本规范，确保项目能够在各平台上正确构建和运行。

## 通用要求

### 编程语言标准
- **C++ 标准**: C++17 (ISO/IEC 14882:2017)
- **编码格式**: UTF-8
- **换行符**: Unix 风格 (LF)

### 构建系统
- **CMake**: 3.16.0 或更高版本
- **构建类型**: Debug, Release, RelWithDebInfo, MinSizeRel

### 版本控制
- **Git**: 2.20.0 或更高版本
- **Git LFS**: 2.7.0 或更高版本（如需要大文件支持）

## Windows 平台

### 开发环境
- **操作系统**: Windows 10 (1903) 或更高版本
- **架构**: x64 (推荐), x86 (支持)

### 编译器选择
#### 主推荐: Microsoft Visual Studio
- **版本**: Visual Studio 2019 (16.0) 或更高版本
- **工具集**: MSVC v142 或更高版本
- **组件要求**:
  - C++ CMake tools for Visual Studio
  - Windows 10 SDK (10.0.18362.0 或更高)
  - Git for Windows

#### 备选: MinGW-w64
- **版本**: GCC 9.0 或更高版本
- **发行版**: MSYS2 推荐
- **环境**: MINGW64 环境

### IDE 推荐
- **主要**: Visual Studio 2019/2022
- **备选**: Visual Studio Code + C/C++ Extension
- **备选**: CLion

### 依赖库
- **GUI框架**: Win32 API (系统自带)
- **其他依赖**: 无额外依赖

## macOS 平台

### 开发环境
- **操作系统**: macOS 10.14 (Mojave) 或更高版本
- **架构**: x86_64, arm64 (Apple Silicon)

### 编译器
- **Xcode**: 12.0 或更高版本
- **Command Line Tools**: 对应 Xcode 版本
- **Clang**: 12.0.0 或更高版本
- **支持**: Objective-C++ (必需)

### IDE 推荐
- **主要**: Xcode
- **备选**: Visual Studio Code + C/C++ Extension
- **备选**: CLion

### 依赖库
- **GUI框架**: Cocoa/AppKit (系统自带)
- **系统框架**:
  - Foundation.framework
  - AppKit.framework
  - Cocoa.framework

### 包管理器 (可选)
- **Homebrew**: 最新版本
- **MacPorts**: 2.6.0 或更高版本

## Linux 平台

### 开发环境
- **发行版**: Ubuntu 18.04 LTS 或更高版本 (主要支持)
- **发行版**: CentOS 7/8, Fedora 30+, Debian 10+ (次要支持)
- **架构**: x86_64 (主要), ARM64 (实验性)

### 编译器
- **GCC**: 7.0 或更高版本 (推荐 9.0+)
- **Clang**: 8.0 或更高版本 (可选)

### IDE 推荐
- **主要**: Visual Studio Code + C/C++ Extension
- **备选**: CLion
- **备选**: Qt Creator
- **备选**: Code::Blocks

### 依赖库
- **GUI框架**: GTK+ 3.0
- **包管理器安装**:
  ```bash
  # Ubuntu/Debian
  sudo apt-get install libgtk-3-dev pkg-config
  
  # CentOS/RHEL
  sudo yum install gtk3-devel pkgconfig
  
  # Fedora
  sudo dnf install gtk3-devel pkgconfig
  ```

### 系统依赖
- **必需**:
  - build-essential
  - cmake
  - pkg-config
  - libgtk-3-dev
- **推荐**:
  - git
  - ninja-build
  - ccache

## 构建配置

### CMake 配置选项
```bash
# 基础配置
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# 指定编译器 (可选)
cmake -B build -S . -DCMAKE_CXX_COMPILER=g++-9

# 启用详细输出
cmake -B build -S . -DCMAKE_VERBOSE_MAKEFILE=ON

# 并行构建
cmake --build build -j$(nproc)
```

### 平台特定构建选项

#### Windows (Visual Studio)
```bash
cmake -B build -S . -G "Visual Studio 16 2019" -A x64
cmake --build build --config Release
```

#### macOS (Xcode)
```bash
cmake -B build -S . -G Xcode
cmake --build build --config Release
```

#### Linux (Ninja)
```bash
cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## 代码规范

### 编译器标志
- **通用**: `-std=c++17 -Wall -Wextra`
- **Debug**: `-g -O0 -DDEBUG`
- **Release**: `-O3 -DNDEBUG`

### 静态分析工具
- **Clang Static Analyzer**: 推荐使用
- **Cppcheck**: 推荐使用
- **PVS-Studio**: 可选商业工具

### 代码格式化
- **工具**: clang-format
- **配置**: 项目根目录 `.clang-format` 文件
- **标准**: 基于 Google C++ Style Guide 的修改版

## 测试环境

### 单元测试框架
- **主要**: Google Test (gtest) 1.10.0+
- **备选**: Catch2 2.13.0+

### 持续集成
- **GitHub Actions**: 主要 CI/CD 平台
- **支持平台**: Windows Server 2019, macOS 10.15, Ubuntu 18.04/20.04

### 测试覆盖率
- **工具**: gcov + lcov (Linux), OpenCppCoverage (Windows)
- **目标**: 代码覆盖率 > 80%

## 性能分析工具

### 内存检测
- **Linux**: Valgrind, AddressSanitizer
- **Windows**: Application Verifier, Visual Studio Diagnostic Tools
- **macOS**: Xcode Instruments, AddressSanitizer

### 性能分析
- **Linux**: perf, gprof
- **Windows**: Visual Studio Performance Profiler
- **macOS**: Xcode Instruments

## 文档生成

### API 文档
- **工具**: Doxygen 1.8.17+
- **输出格式**: HTML, PDF
- **配置**: 项目根目录 `Doxyfile`

### 用户文档
- **格式**: Markdown
- **工具**: GitBook, MkDocs
- **位置**: `docs/` 目录

## 发布打包

### Windows
- **安装程序**: NSIS 3.0+ 或 WiX Toolset
- **打包格式**: MSI, EXE
- **签名**: Authenticode (生产环境)

### macOS
- **打包格式**: DMG, PKG
- **签名**: Apple Developer Certificate (生产环境)
- **公证**: Apple Notarization (macOS 10.15+)

### Linux
- **打包格式**: DEB, RPM, AppImage
- **发行**: PPA (Ubuntu), COPR (Fedora)

## 开发工作流

### 分支策略
- **主分支**: `main` (稳定版本)
- **开发分支**: `develop` (开发版本)
- **功能分支**: `feature/*` (新功能)
- **修复分支**: `hotfix/*` (紧急修复)

### 提交规范
- **格式**: Conventional Commits
- **示例**: `feat: add syntax highlighting support`
- **类型**: feat, fix, docs, style, refactor, test, chore

### 代码审查
- **要求**: 所有代码变更必须经过代码审查
- **工具**: GitHub Pull Request
- **检查项**: 功能正确性、代码质量、测试覆盖率

## 环境变量

### 开发环境变量
```bash
# 通用
export CMAKE_BUILD_TYPE=Debug
export LITEPAD_DEBUG=1

# Linux 特定
export PKG_CONFIG_PATH=/usr/lib/pkgconfig
export GTK_DEBUG=interactive

# macOS 特定  
export DEVELOPER_DIR=/Applications/Xcode.app/Contents/Developer

# Windows 特定 (PowerShell)
$env:VCPKG_ROOT = "C:\vcpkg"
$env:CMAKE_TOOLCHAIN_FILE = "$env:VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake"
```

---

**文档版本**: v0.1.0
**创建日期**: 2025年08月 
**最后更新**: 2028年08月
**维护者**: LitePad 开发者
