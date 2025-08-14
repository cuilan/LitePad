@echo off
echo Building LitePad with MinGW GCC...

REM 检查是否安装了CMake
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: CMake not found. Please install CMake first.
    pause
    exit /b 1
)

REM 检查是否安装了GCC
where gcc >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: GCC not found. Please install w64devkit or MinGW.
    pause
    exit /b 1
)

REM 显示编译器版本
echo Using compiler:
gcc --version | findstr "gcc"

REM 创建构建目录
if not exist build mkdir build
cd build

REM 配置项目 - 使用MinGW Makefiles生成器
echo Configuring project with MinGW...
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 (
    echo Error: CMake configuration failed.
    pause
    exit /b 1
)

REM 构建项目
echo Building project...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo Error: Build failed.
    pause
    exit /b 1
)

echo Build completed successfully!
echo Executable location: build\bin\LitePad.exe

REM 复制配置文件到输出目录
if not exist bin\config mkdir bin\config
copy ..\config\default.conf bin\config\
if %errorlevel% neq 0 (
    echo Warning: Failed to copy config file.
)

REM 复制插件到输出目录（如果存在）
if exist lib\*.dll (
    if not exist bin\plugins mkdir bin\plugins
    copy lib\*.dll bin\plugins\
)

echo Files copied to output directory.
echo.
echo To run the application:
echo   cd build\bin
echo   .\LitePad.exe
echo.
pause
