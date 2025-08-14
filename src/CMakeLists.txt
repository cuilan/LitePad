# 主程序源文件
set(MAIN_SOURCES
    main.cpp
    MainWindow.cpp
    Editor.cpp
    PluginManager.cpp
    ConfigManager.cpp
    PlatformWindow.cpp
)

# 平台特定源文件
if(APPLE)
    list(APPEND MAIN_SOURCES platform/macos/MacOSWindow.mm)
elseif(WIN32)
    list(APPEND MAIN_SOURCES platform/windows/WindowsWindow.cpp)
elseif(UNIX AND NOT APPLE)
    list(APPEND MAIN_SOURCES platform/linux/LinuxWindow.cpp)
endif()

# 主程序头文件
set(MAIN_HEADERS
    MainWindow.h
    Editor.h
    PluginManager.h
    ConfigManager.h
    PluginInterface.h
    PlatformWindow.h
)

# 平台特定头文件
if(APPLE)
    list(APPEND MAIN_HEADERS 
        platform/macos/MacOSWindow.h
        platform/macos/MacOSEventLoop.h
    )
elseif(WIN32)
    list(APPEND MAIN_HEADERS platform/windows/WindowsWindow.h)
elseif(UNIX AND NOT APPLE)
    list(APPEND MAIN_HEADERS platform/linux/LinuxWindow.h)
endif()

# 创建主程序可执行文件
add_executable(LitePad ${MAIN_SOURCES} ${MAIN_HEADERS})

# 设置包含目录
target_include_directories(LitePad PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins
    ${CMAKE_CURRENT_SOURCE_DIR}/platform
)

# 链接库
target_link_libraries(LitePad
    ${PLATFORM_SPECIFIC_LIBS}
)

# 设置编译选项
if(MSVC)
    target_compile_options(LitePad PRIVATE /W4)
else()
    target_compile_options(LitePad PRIVATE -Wall -Wextra -Wpedantic)
endif()
