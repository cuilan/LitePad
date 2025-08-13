# 插件系统构建配置

# 基础插件源文件
set(BASIC_PLUGINS_SOURCES
    LineNumberPlugin.cpp
    SyntaxHighlightPlugin.cpp
    AutoSavePlugin.cpp
    TerminalPlugin.cpp
)

# 基础插件头文件
set(BASIC_PLUGINS_HEADERS
    LineNumberPlugin.h
    SyntaxHighlightPlugin.h
    AutoSavePlugin.h
    TerminalPlugin.h
)

# 创建插件库
add_library(BasicPlugins SHARED
    ${BASIC_PLUGINS_SOURCES}
    ${BASIC_PLUGINS_HEADERS}
)

# 设置包含目录
target_include_directories(BasicPlugins PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

# 设置编译选项
if(MSVC)
    target_compile_options(BasicPlugins PRIVATE /W4)
else()
    target_compile_options(BasicPlugins PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 设置输出名称
set_target_properties(BasicPlugins PROPERTIES
    OUTPUT_NAME "BasicPlugins"
    PREFIX ""
)
