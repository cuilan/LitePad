#include <iostream>
#include <memory>
#include <stdexcept>
#include "MainWindow.h"
#include "ConfigManager.h"

#ifdef MACOS
#include "platform/macos/MacOSEventLoop.h"
#elif defined(WIN32)
#include <windows.h>
#endif

/**
 * 主程序入口点
 * @param argc 命令行参数数量
 * @param argv 命令行参数数组
 * @return 程序退出码
 */
int main(int argc, char* argv[]) {
    try {
        
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
        
        // 进入平台特定的事件循环
#ifdef MACOS
        // macOS 需要特殊的应用程序初始化和事件循环
        runMacOSEventLoop();
#elif defined(WIN32)
        // Windows 消息循环
        MSG msg = {};
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
#elif defined(LINUX)
        // Linux GTK 事件循环
        // TODO: 实现 GTK 事件循环
#else
        // fallback：简单的控制台等待
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
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
