# 主程序源文件
if(APPLE)
    set(MAIN_SOURCES
        main.cpp
        MainWindow.mm  # macOS 使用 Objective-C++
        Editor.cpp
        PluginManager.cpp
        ConfigManager.cpp
    )
else()
    set(MAIN_SOURCES
        main.cpp
        MainWindow.cpp
        Editor.cpp
        PluginManager.cpp
        ConfigManager.cpp
    )
endif()

# 主程序头文件
set(MAIN_HEADERS
    MainWindow.h
    Editor.h
    PluginManager.h
    ConfigManager.h
    PluginInterface.h
)

# 创建主程序可执行文件
add_executable(LitePad ${MAIN_SOURCES} ${MAIN_HEADERS})

# 设置包含目录
target_include_directories(LitePad PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins
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
