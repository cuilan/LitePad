#ifndef LINUX_WINDOW_H
#define LINUX_WINDOW_H

#include "../PlatformWindow.h"
#include <memory>

#ifdef LINUX

/**
 * Linux 平台窗口实现
 * 使用 GTK+ 实现原生 Linux 界面
 */
class LinuxWindow : public PlatformWindow {
public:
    LinuxWindow();
    ~LinuxWindow() override;
    
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
    
    // GTK+ 回调函数
    static gboolean onDeleteEvent(GtkWidget* widget, GdkEvent* event, gpointer userData);
    static void onDestroy(GtkWidget* widget, gpointer userData);
    static void onTextChanged(GtkTextBuffer* textBuffer, gpointer userData);
};

#endif // LINUX

#endif // LINUX_WINDOW_H
