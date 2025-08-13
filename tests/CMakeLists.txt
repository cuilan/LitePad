# 测试配置

# 启用测试
enable_testing()

# 测试源文件
set(TEST_SOURCES
    test_main.cpp
    test_editor.cpp
    test_plugin_manager.cpp
    test_config_manager.cpp
)

# 创建测试可执行文件
add_executable(LitePadTests ${TEST_SOURCES})

# 设置包含目录
target_include_directories(LitePadTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../plugins
)

# 链接库
target_link_libraries(LitePadTests
    BasicPlugins
)

# 添加测试
add_test(NAME EditorTests COMMAND LitePadTests)
add_test(NAME PluginManagerTests COMMAND LitePadTests)
add_test(NAME ConfigManagerTests COMMAND LitePadTests)
