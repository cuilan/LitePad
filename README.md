# LitePad

A lightweight, cross-platform code editor built with C++17.

## Overview

LitePad is a lightweight code editor with the following features:

- **Lightweight**: Pursuit of ultimate performance and lightweight design
- **Cross-platform**: Support for Windows, macOS, Linux and other platforms
- **Plugin-based**: Functionality extension through plugin system
- **Highly configurable**: Support for rich configuration options

## Documentation

- [Requirements (中文)](docs/requirements.zh-CN.md)
- [Development Environment (中文)](docs/development-environment.zh-CN.md)
- [Project Milestones (中文)](docs/milestones.zh-CN.md)
- [Task List](docs/TODO.md)

## Quick Start

### Prerequisites

- C++17 compatible compiler
- CMake 3.16 or higher
- Platform-specific dependencies:
  - **Windows**: Visual Studio 2019+ or MinGW-w64
  - **macOS**: Xcode 12+ with Command Line Tools
  - **Linux**: GCC 7+ or Clang 8+, GTK+ 3.0 development libraries

### Building

```bash
# Clone the repository
git clone https://github.com/your-username/LitePad.git
cd LitePad

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(nproc)  # Linux/macOS
# or
cmake --build . --config Release  # Cross-platform
```

### Running

```bash
# From build directory
./bin/LitePad

# Or with a file argument
./bin/LitePad path/to/your/file.txt
```

## Platform Support

| Platform | Status | GUI Framework |
|----------|--------|---------------|
| Windows  | ✅ Supported | Win32 API |
| macOS    | ✅ Supported | Cocoa/AppKit |
| Linux    | ✅ Supported | GTK+ 3.0 |

## Architecture

```
LitePad/
├── src/                    # Main source code
│   ├── platform/          # Platform-specific implementations
│   │   ├── windows/       # Windows-specific code
│   │   ├── macos/         # macOS-specific code
│   │   └── linux/         # Linux-specific code
│   ├── Editor.cpp         # Core editor functionality
│   ├── PluginManager.cpp  # Plugin system
│   └── ConfigManager.cpp  # Configuration management
├── plugins/               # Plugin headers and implementations
├── config/                # Configuration files
├── docs/                  # Documentation
└── tests/                 # Test suite
```

## Contributing

We welcome contributions! Please follow these steps:

1. Fork the project
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

### Development Guidelines

- Follow C++17 standards
- Use consistent code formatting (see `.clang-format`)
- Write tests for new features
- Update documentation as needed
- Follow conventional commit format

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contact

- **Issues**: [GitHub Issues](https://github.com/your-username/LitePad/issues)
- **Discussions**: [GitHub Discussions](https://github.com/your-username/LitePad/discussions)
- **Pull Requests**: [GitHub Pull Requests](https://github.com/your-username/LitePad/pulls)

## Changelog

### v0.1.0 (Current)
- ✅ Initial project structure
- ✅ Cross-platform build system
- ✅ Basic editor core implementation
- ✅ Platform-specific window systems
- ✅ Plugin system architecture
- ✅ Configuration management system

### Upcoming
- [ ] GUI implementation
- [ ] Plugin system completion
- [ ] Syntax highlighting
- [ ] File management
- [ ] Advanced editing features

---

**English** | [中文](README.zh-CN.md)
