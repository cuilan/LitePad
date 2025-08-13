#ifndef PLATFORM_WINDOW_H
#define PLATFORM_WINDOW_H

#include <string>
#include <memory>
#include <functional>

// 前向声明
class Editor;
class PluginManager;
class ConfigManager;

/**
 * 平台窗口抽象接口
 * 定义所有平台窗口必须实现的接口
 */
class PlatformWindow {
public:
    virtual ~PlatformWindow() = default;
    
    // 窗口基础操作
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void close() = 0;
    
    // 窗口属性
    virtual void setTitle(const std::string& title) = 0;
    virtual std::string getTitle() const = 0;
    virtual void setSize(int width, int height) = 0;
    virtual void getSize(int& width, int& height) const = 0;
    virtual void setPosition(int x, int y) = 0;
    virtual void getPosition(int& x, int& y) const = 0;
    
    // 窗口状态
    virtual void maximize() = 0;
    virtual void minimize() = 0;
    virtual void restore() = 0;
    virtual bool isMaximized() const = 0;
    virtual bool isMinimized() const = 0;
    
    // 文件操作
    virtual void newFile() = 0;
    virtual void openFile() = 0;
    virtual void saveFile() = 0;
    virtual void saveAs() = 0;
    
    // 应用程序操作
    virtual void showPluginManager() = 0;
    virtual void showSettings() = 0;
    virtual void showAbout() = 0;
    virtual void quit() = 0;
    
    // 组件访问
    virtual void setEditor(std::shared_ptr<Editor> editor) = 0;
    virtual void setPluginManager(std::shared_ptr<PluginManager> pluginManager) = 0;
    virtual void setConfigManager(std::shared_ptr<ConfigManager> configManager) = 0;
    
    // 状态更新
    virtual void setStatusBarText(const std::string& text) = 0;
    virtual void updateMenuState() = 0;
    virtual void handleFileDrop(const std::string& filePath) = 0;
    
    // 事件回调
    virtual void setCloseCallback(std::function<bool()> callback) = 0;
    virtual void setFileChangedCallback(std::function<void()> callback) = 0;
};

/**
 * 平台窗口工厂
 * 根据当前平台创建相应的窗口实现
 */
class PlatformWindowFactory {
public:
    static std::unique_ptr<PlatformWindow> createWindow();
};

#endif // PLATFORM_WINDOW_H
