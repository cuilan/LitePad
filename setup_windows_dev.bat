@echo off
echo ========================================
echo LitePad Windows 开发环境搭建脚本
echo ========================================
echo.

REM 检查管理员权限
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo 警告: 建议以管理员权限运行此脚本以安装工具
    echo.
)

REM 检查 winget 是否可用
where winget >nul 2>nul
if %errorlevel% neq 0 (
    echo 错误: winget 不可用，请手动安装所需工具
    echo 请访问以下网址手动下载：
    echo - Visual Studio 2022 Community: https://visualstudio.microsoft.com/zh-hans/vs/community/
    echo - CMake: https://cmake.org/download/
    echo - Git: https://git-scm.com/download/win
    pause
    exit /b 1
)

echo 正在检查和安装开发工具...
echo.

REM 检查并安装 Git
echo [1/3] 检查 Git...
where git >nul 2>nul
if %errorlevel% neq 0 (
    echo 正在安装 Git...
    winget install -e --id Git.Git --accept-source-agreements --accept-package-agreements
) else (
    echo Git 已安装
)

REM 检查并安装 CMake
echo [2/3] 检查 CMake...
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo 正在安装 CMake...
    winget install -e --id Kitware.CMake --accept-source-agreements --accept-package-agreements
) else (
    echo CMake 已安装
)

REM 检查 Visual Studio
echo [3/3] 检查 Visual Studio...
where cl >nul 2>nul
if %errorlevel% neq 0 (
    echo Visual Studio 编译器未找到
    echo 正在安装 Visual Studio 2022 Community...
    echo 注意: 安装时请选择 "C++ 桌面开发" 工作负荷
    winget install -e --id Microsoft.VisualStudio.2022.Community --accept-source-agreements --accept-package-agreements
    echo.
    echo 重要提示：
    echo 1. Visual Studio 安装完成后，请重新启动命令提示符
    echo 2. 确保安装了 "C++ 桌面开发" 工作负荷
    echo 3. 然后运行 build.bat 来构建项目
) else (
    echo Visual Studio 编译器已安装
)

echo.
echo ========================================
echo 开发环境检查完成！
echo ========================================
echo.
echo 下一步操作：
echo 1. 如果刚安装了 Visual Studio，请重启命令提示符
echo 2. 运行 'build.bat' 来构建项目
echo 3. 运行 'build\bin\Release\LitePad.exe' 来启动应用程序
echo.
echo 开发建议：
echo - 使用 Visual Studio Code 或 Visual Studio IDE 进行开发
echo - 项目使用 C++17 标准
echo - 代码注释请使用中文
echo.

pause
