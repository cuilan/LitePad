#ifndef WINDOWS_WINDOW_H
#define WINDOWS_WINDOW_H

#include "../PlatformWindow.h"
#include <memory>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/**
 * Windows 平台窗口实现
 * 使用 Win32 API 实现原生 Windows 界面
 */
class WindowsWindow : public PlatformWindow {
public:
    WindowsWindow();
    ~WindowsWindow() override;
    
    // PlatformWindow 接口实现
    void show() override;
    void hide() override;
    void close() override;
    
    void setTitle(const std::string& title) override;
    std::string getTitle() const override;
    void setSize(int width, int height) override;
    void getSize(int& width, int& height) const override;
    void setPosition(int x, int y) override;
    void getPosition(int& x, int& y) const override;
    
    void maximize() override;
    void minimize() override;
    void restore() override;
    bool isMaximized() const override;
    bool isMinimized() const override;
    
    void newFile() override;
    void openFile() override;
    void saveFile() override;
    void saveAs() override;
    
    void quit() override;
    void showAbout() override;
    void showPluginManager() override;
    void showSettings() override;
    
    void setEditor(std::shared_ptr<Editor> editor) override;
    void setPluginManager(std::shared_ptr<PluginManager> pluginManager) override;
    void setConfigManager(std::shared_ptr<ConfigManager> configManager) override;
    
    void setTextContent(const std::string& content) override;
    std::string getTextContent() const override;
    
    void setTextChangedCallback(std::function<void(const std::string&)> callback) override;
    void setWindowCloseCallback(std::function<bool()> callback) override;
    
    void setStatusText(const std::string& text) override;
    void handleFileDrop(const std::string& filePath) override;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string title_;
    int width_, height_;
    int x_, y_;
    
    // Windows 消息处理函数
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    
    // 菜单相关方法
    void createMenuBar();
    void handleMenuCommand(int menuId);
};

// 菜单项ID定义
#define ID_FILE_NEW         1001
#define ID_FILE_OPEN        1002
#define ID_FILE_SAVE        1003
#define ID_FILE_SAVE_AS     1004
#define ID_FILE_EXIT        1005

#define ID_EDIT_UNDO        2001
#define ID_EDIT_REDO        2002
#define ID_EDIT_CUT         2003
#define ID_EDIT_COPY        2004
#define ID_EDIT_PASTE       2005
#define ID_EDIT_SELECT_ALL  2006
#define ID_EDIT_FIND        2007

#define ID_HELP_ABOUT       3001

#endif // WIN32

#endif // WINDOWS_WINDOW_H
