@echo off
echo Building LitePad...

REM 检查是否安装了CMake
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: CMake not found. Please install CMake first.
    pause
    exit /b 1
)

REM 检查是否安装了编译器
where cl >nul 2>nul
if %errorlevel% neq 0 (
    echo Warning: Visual Studio compiler not found. Trying to use MinGW...
    where g++ >nul 2>nul
    if %errorlevel% neq 0 (
        echo Error: No C++ compiler found. Please install Visual Studio or MinGW.
        pause
        exit /b 1
    )
)

REM 创建构建目录
if not exist build mkdir build
cd build

REM 配置项目
echo Configuring project...
cmake .. -G "Visual Studio 17 2022" -A x64
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
echo Executable location: build\bin\Release\LitePad.exe

REM 复制配置文件到输出目录
if not exist bin\Release\config mkdir bin\Release\config
copy ..\config\default.conf bin\Release\config\

REM 复制插件到输出目录（如果存在）
if exist plugins\Release\*.dll (
    if not exist bin\Release\plugins mkdir bin\Release\plugins
    copy plugins\Release\*.dll bin\Release\plugins\
)

echo Files copied to output directory.
pause
