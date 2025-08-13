cmake_minimum_required(VERSION 3.16)
project(LitePad VERSION 1.0.0 LANGUAGES CXX OBJCXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找必要的包
find_package(PkgConfig REQUIRED)

# 平台特定设置
if(WIN32)
    set(PLATFORM_SPECIFIC_LIBS "")
    add_definitions(-DWIN32)
elseif(APPLE)
    # macOS 原生框架
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(FOUNDATION_FRAMEWORK Foundation)
    find_library(APPKIT_FRAMEWORK AppKit)
    
    set(PLATFORM_SPECIFIC_LIBS 
        ${COCOA_FRAMEWORK}
        ${FOUNDATION_FRAMEWORK}
        ${APPKIT_FRAMEWORK}
    )
    
    add_definitions(-DMACOS)
    
    # 启用 Objective-C++ 支持
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc")
    
elseif(UNIX)
    set(PLATFORM_SPECIFIC_LIBS "")
    add_definitions(-DLINUX)
endif()

# 添加子目录
add_subdirectory(src)
add_subdirectory(plugins)
add_subdirectory(tests)

# 设置安装规则
install(TARGETS LitePad
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 安装配置文件
install(DIRECTORY config/ DESTINATION share/litepad/config)
install(DIRECTORY plugins/ DESTINATION share/litepad/plugins)
