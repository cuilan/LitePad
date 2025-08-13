# LitePad 开发文档

## 项目概述

LitePad 是一个使用C++实现的轻量级代码编辑器，具有以下特点：

- **轻量化**: 追求极致的性能和轻量级设计
- **插件化**: 通过插件系统实现功能扩展
- **高度可配置**: 支持丰富的配置选项
- **跨平台**: 支持 Windows、macOS、Linux 等多平台

## 项目结构

```
LitePad/
├── src/                    # 主程序源代码
│   ├── main.cpp           # 程序入口点
│   ├── MainWindow.h       # 主窗口类
│   ├── Editor.h           # 编辑器核心类
│   ├── PluginManager.h    # 插件管理器
│   ├── ConfigManager.h    # 配置管理器
│   └── PluginInterface.h  # 插件接口定义
├── plugins/               # 插件系统
│   ├── LineNumberPlugin.h     # 行号插件
│   ├── SyntaxHighlightPlugin.h # 语法高亮插件
│   ├── AutoSavePlugin.h       # 自动保存插件
│   └── TerminalPlugin.h       # 终端插件
├── config/                # 配置文件
│   └── default.conf       # 默认配置
├── tests/                 # 测试代码
├── docs/                  # 文档
├── CMakeLists.txt         # CMake主配置
├── build.bat              # Windows构建脚本
├── build.sh               # Unix构建脚本
└── .gitignore            # Git忽略文件
```

## 核心架构

### 1. 插件系统

LitePad 采用插件化架构，所有扩展功能都通过插件实现：

- **插件接口**: `PluginInterface` 定义了所有插件必须实现的方法
- **插件管理器**: `PluginManager` 负责插件的加载、卸载、启用、禁用
- **基础插件**: 包含行号、语法高亮、自动保存、终端等基础功能

### 2. 编辑器核心

`Editor` 类提供基础的文本编辑功能：

- 文件打开/保存
- 文本编辑操作
- 撤销/重做
- 查找/替换
- 行管理

### 3. 配置系统

`ConfigManager` 类管理应用程序配置：

- 支持多种数据类型（字符串、整数、布尔值、浮点数）
- 分组配置管理
- 配置变化通知
- 配置文件持久化

## 构建说明

### 环境要求

- **编译器**: C++17 兼容的编译器（GCC 7+, Clang 5+, MSVC 2017+）
- **构建工具**: CMake 3.16+
- **操作系统**: Windows 10+, macOS 10.14+, Linux (Ubuntu 18.04+)

### Windows 构建

1. 安装 Visual Studio 2019+ 或 MinGW
2. 安装 CMake
3. 运行构建脚本：
   ```cmd
   build.bat
   ```

### Linux/macOS 构建

1. 安装开发工具：
   ```bash
   # Ubuntu/Debian
   sudo apt-get install build-essential cmake
   
   # macOS
   brew install cmake
   ```
2. 运行构建脚本：
   ```bash
   chmod +x build.sh
   ./build.sh
   ```

### 手动构建

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## 插件开发

### 创建新插件

1. 继承 `PluginInterface` 类
2. 实现所有必需的虚函数
3. 在 `plugins/CMakeLists.txt` 中添加插件
4. 实现插件入口点函数

### 插件接口方法

```cpp
class MyPlugin : public PluginInterface {
public:
    std::string getName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    bool initialize() override;
    void cleanup() override;
    void enable() override;
    void disable() override;
    bool isEnabled() const override;
};
```

### 插件导出

```cpp
extern "C" {
    PLUGIN_EXPORT PluginInterface* createPlugin();
    PLUGIN_EXPORT void destroyPlugin(PluginInterface* plugin);
}
```

## 配置系统

### 配置文件格式

配置文件使用 INI 格式，支持分组和多种数据类型：

```ini
[Section]
key1 = value1
key2 = 123
key3 = true
key4 = 3.14
```

### 配置管理

```cpp
auto config = std::make_shared<ConfigManager>();
config->setString("Editor.font_family", "Consolas");
config->setInt("Editor.font_size", 12);
config->setBool("Editor.line_numbers", true);
```

## 测试

项目包含完整的测试框架：

```bash
cd build
make test
```

## 贡献指南

1. Fork 项目
2. 创建功能分支
3. 提交更改
4. 推送到分支
5. 创建 Pull Request

## 许可证

本项目采用 MIT 许可证。

## 联系方式

如有问题或建议，请通过以下方式联系：

- 提交 Issue
- 创建 Pull Request
- 发送邮件至项目维护者

## 更新日志

### v1.0.0
- 初始版本发布
- 基础编辑器功能
- 插件系统
- 四种基础插件
- 跨平台支持
