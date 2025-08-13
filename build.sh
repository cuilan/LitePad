#!/bin/bash

echo "Building LitePad..."

# 检查是否安装了CMake
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake not found. Please install CMake first."
    exit 1
fi

# 检查是否安装了编译器
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "Error: No C++ compiler found. Please install g++ or clang++."
    exit 1
fi

# 创建构建目录
mkdir -p build
cd build

# 配置项目
echo "Configuring project..."
if command -v clang++ &> /dev/null; then
    cmake .. -DCMAKE_CXX_COMPILER=clang++
else
    cmake ..
fi

if [ $? -ne 0 ]; then
    echo "Error: CMake configuration failed."
    exit 1
fi

# 构建项目
echo "Building project..."
make -j$(nproc)
if [ $? -ne 0 ]; then
    echo "Error: Build failed."
    exit 1
fi

echo "Build completed successfully!"
echo "Executable location: build/bin/LitePad"

# 复制配置文件到输出目录
mkdir -p bin/config
cp ../config/default.conf bin/config/

# 复制插件到输出目录
mkdir -p bin/plugins
cp plugins/libBasicPlugins.so bin/plugins/

echo "Files copied to output directory."
