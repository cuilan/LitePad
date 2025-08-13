cmake_minimum_required(VERSION 3.16)
project(LitePad VERSION 1.0.0 LANGUAGES CXX OBJCXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找必要的包（仅在Linux上需要PkgConfig）
if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
endif()

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
    
    # 启用 Objective-C++ 支持（仅限Clang）
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc")
    endif()
    
elseif(UNIX AND NOT APPLE)
    # Linux GTK+ 支持
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    
    set(PLATFORM_SPECIFIC_LIBS ${GTK3_LIBRARIES})
    include_directories(${GTK3_INCLUDE_DIRS})
    link_directories(${GTK3_LIBRARY_DIRS})
    add_definitions(${GTK3_CFLAGS_OTHER})
    
    add_definitions(-DLINUX)
    
endif()

# 添加子目录
add_subdirectory(src)
# TODO: 实现插件后启用
# add_subdirectory(plugins)
# add_subdirectory(tests)

# 设置安装规则
install(TARGETS LitePad
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 安装配置文件
install(DIRECTORY config/ DESTINATION share/litepad/config)
install(DIRECTORY plugins/ DESTINATION share/litepad/plugins)
