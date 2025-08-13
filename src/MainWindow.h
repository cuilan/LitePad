#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <memory>
#include <string>

// 前向声明
class Editor;
class PluginManager;
class ConfigManager;
class PlatformWindow;

/**
 * 主窗口类
 * 应用程序的主要界面，包含菜单栏、工具栏和编辑器区域
 */
class MainWindow {
public:
    MainWindow();
    ~MainWindow();
    
    /**
     * 显示主窗口
     */
    void show();
    
    /**
     * 隐藏主窗口
     */
    void hide();
    
    /**
     * 设置窗口标题
     * @param title 窗口标题
     */
    void setTitle(const std::string& title);
    
    /**
     * 获取窗口标题
     * @return 窗口标题
     */
    std::string getTitle() const;
    
    /**
     * 设置窗口大小
     * @param width 宽度
     * @param height 高度
     */
    void setSize(int width, int height);
    
    /**
     * 获取窗口大小
     * @param width 输出宽度
     * @param height 输出高度
     */
    void getSize(int& width, int& height) const;
    
    /**
     * 设置窗口位置
     * @param x X坐标
     * @param y Y坐标
     */
    void setPosition(int x, int y);
    
    /**
     * 获取窗口位置
     * @param x 输出X坐标
     * @param y 输出Y坐标
     */
    void getPosition(int& x, int& y) const;
    
    /**
     * 最大化窗口
     */
    void maximize();
    
    /**
     * 最小化窗口
     */
    void minimize();
    
    /**
     * 恢复窗口
     */
    void restore();
    
    /**
     * 检查窗口是否最大化
     * @return 是否最大化
     */
    bool isMaximized() const;
    
    /**
     * 检查窗口是否最小化
     * @return 是否最小化
     */
    bool isMinimized() const;
    
    /**
     * 新建文件
     */
    void newFile();
    
    /**
     * 打开文件
     */
    void openFile();
    
    /**
     * 保存文件
     */
    void saveFile();
    
    /**
     * 另存为
     */
    void saveAs();
    
    /**
     * 退出应用程序
     */
    void quit();
    
    /**
     * 显示插件管理器
     */
    void showPluginManager();
    
    /**
     * 显示设置对话框
     */
    void showSettings();
    
    /**
     * 显示关于对话框
     */
    void showAbout();
    
    /**
     * 获取编辑器实例
     * @return 编辑器指针
     */
    std::shared_ptr<Editor> getEditor() const;
    
    /**
     * 获取插件管理器实例
     * @return 插件管理器指针
     */
    std::shared_ptr<PluginManager> getPluginManager() const;
    
    /**
     * 获取配置管理器实例
     * @return 配置管理器指针
     */
    std::shared_ptr<ConfigManager> getConfigManager() const;
    
    /**
     * 设置状态栏文本
     * @param text 状态栏文本
     */
    void setStatusBarText(const std::string& text);
    
    /**
     * 更新菜单状态
     */
    void updateMenuState();
    
    /**
     * 处理文件拖放
     * @param filePath 拖放的文件路径
     */
    void handleFileDrop(const std::string& filePath);

private:
    // 缓存的窗口状态
    std::string title_;
    int width_;
    int height_;
    int x_;
    int y_;
    bool maximized_;
    bool minimized_;
    
    // 使用平台抽象层
    std::unique_ptr<PlatformWindow> platformWindow_;
    std::shared_ptr<Editor> editor_;
    std::shared_ptr<PluginManager> pluginManager_;
    std::shared_ptr<ConfigManager> configManager_;
    
    /**
     * 初始化用户界面
     */
    void initializeUI();
    
    /**
     * 创建菜单栏
     */
    void createMenuBar();
    
    /**
     * 创建工具栏
     */
    void createToolBar();
    
    /**
     * 创建状态栏
     */
    void createStatusBar();
    
    /**
     * 创建编辑器区域
     */
    void createEditorArea();
    
    /**
     * 绑定事件处理函数
     */
    void bindEvents();
    
    /**
     * 处理窗口关闭事件
     */
    void onWindowClose();
    
    /**
     * 处理文件变化事件
     */
    void onFileChanged();
    
    /**
     * 处理插件状态变化事件
     */
    void onPluginStateChanged();
    
    /**
     * 加载窗口配置
     */
    void loadWindowConfig();
    
    /**
     * 保存窗口配置
     */
    void saveWindowConfig();
};

#endif // MAIN_WINDOW_H
