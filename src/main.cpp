#include <iostream>
#include <memory>
#include <stdexcept>
#include "MainWindow.h"
#include "ConfigManager.h"

#ifdef MACOS
#import <Cocoa/Cocoa.h>
#endif

/**
 * 主程序入口点
 * @param argc 命令行参数数量
 * @param argv 命令行参数数组
 * @return 程序退出码
 */
int main(int argc, char* argv[]) {
    try {
#ifdef MACOS
        // 初始化 Cocoa 应用程序
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
#endif
        
        // 初始化配置管理器
        auto configManager = std::make_shared<ConfigManager>();
        
        // 加载默认配置
        configManager->loadConfig("config/default.conf");
        
        // 创建主窗口
        auto mainWindow = std::make_unique<MainWindow>();
        
        // 设置窗口标题
        mainWindow->setTitle("LitePad - Lightweight Code Editor");
        
        // 设置默认窗口大小
        mainWindow->setSize(1024, 768);
        
        // 显示主窗口
        mainWindow->show();
        
        // 如果有命令行参数，尝试打开文件
        if (argc > 1) {
            std::string filePath = argv[1];
            mainWindow->handleFileDrop(filePath);
        }
        
        std::cout << "LitePad started successfully!" << std::endl;
        
#ifdef MACOS
        // 进入 macOS 事件循环
        [NSApp activateIgnoringOtherApps:YES];
        [NSApp run];
#else
        // 其他平台的事件循环
        // TODO: 实现其他平台的GUI事件循环
#endif
        
        // 保存配置
        configManager->saveConfig("config/user.conf");
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
