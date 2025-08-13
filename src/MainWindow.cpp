#include "MainWindow.h"
#include "PlatformWindow.h"
#include "Editor.h"
#include "PluginManager.h"
#include "ConfigManager.h"

MainWindow::MainWindow() 
    : width_(1024), height_(768), x_(100), y_(100), maximized_(false), minimized_(false)
{
    // 创建平台特定的窗口
    platformWindow_ = PlatformWindowFactory::createWindow();
    
    // 创建核心组件
    editor_ = std::make_shared<Editor>();
    pluginManager_ = std::make_shared<PluginManager>();
    configManager_ = std::make_shared<ConfigManager>();
    
    // 设置平台窗口的核心组件
    platformWindow_->setEditor(editor_);
    platformWindow_->setPluginManager(pluginManager_);
    platformWindow_->setConfigManager(configManager_);
    
    // 设置回调
    platformWindow_->setTextChangedCallback([this](const std::string& content) {
        // 处理文本变化
        editor_->setContent(content);
    });
    
    platformWindow_->setWindowCloseCallback([this]() -> bool {
        // 处理窗口关闭
        if (editor_->isModified()) {
            // 这里应该显示保存对话框，但为了简化先返回true
            return true;
        }
        return true;
    });
    
    // 初始化默认设置
    title_ = "LitePad - Lightweight Code Editor";
    platformWindow_->setTitle(title_);
    platformWindow_->setSize(width_, height_);
    platformWindow_->setPosition(x_, y_);
}

MainWindow::~MainWindow() = default;

void MainWindow::show() {
    platformWindow_->show();
}

void MainWindow::hide() {
    platformWindow_->hide();
}

void MainWindow::setTitle(const std::string& title) {
    title_ = title;
    platformWindow_->setTitle(title);
}

std::string MainWindow::getTitle() const {
    return title_;
}

void MainWindow::setSize(int width, int height) {
    width_ = width;
    height_ = height;
    platformWindow_->setSize(width, height);
}

void MainWindow::getSize(int& width, int& height) const {
    width = width_;
    height = height_;
}

void MainWindow::setPosition(int x, int y) {
    x_ = x;
    y_ = y;
    platformWindow_->setPosition(x, y);
}

void MainWindow::getPosition(int& x, int& y) const {
    x = x_;
    y = y_;
}

void MainWindow::maximize() {
    maximized_ = true;
    platformWindow_->maximize();
}

void MainWindow::minimize() {
    minimized_ = true;
    platformWindow_->minimize();
}

void MainWindow::restore() {
    maximized_ = false;
    minimized_ = false;
    platformWindow_->restore();
}

bool MainWindow::isMaximized() const {
    return platformWindow_->isMaximized();
}

bool MainWindow::isMinimized() const {
    return platformWindow_->isMinimized();
}

void MainWindow::newFile() {
    platformWindow_->newFile();
}

void MainWindow::openFile() {
    platformWindow_->openFile();
}

void MainWindow::saveFile() {
    platformWindow_->saveFile();
}

void MainWindow::saveAs() {
    platformWindow_->saveAs();
}

void MainWindow::quit() {
    platformWindow_->quit();
}

void MainWindow::showPluginManager() {
    platformWindow_->showPluginManager();
}

void MainWindow::showSettings() {
    platformWindow_->showSettings();
}

void MainWindow::showAbout() {
    platformWindow_->showAbout();
}

std::shared_ptr<Editor> MainWindow::getEditor() const {
    return editor_;
}

std::shared_ptr<PluginManager> MainWindow::getPluginManager() const {
    return pluginManager_;
}

std::shared_ptr<ConfigManager> MainWindow::getConfigManager() const {
    return configManager_;
}

void MainWindow::setStatusBarText(const std::string& text) {
    platformWindow_->setStatusText(text);
}

void MainWindow::updateMenuState() {
    // TODO: 实现菜单状态更新
}

void MainWindow::handleFileDrop(const std::string& filePath) {
    platformWindow_->handleFileDrop(filePath);
}

// 私有方法的实现（现在大部分由平台层处理）
void MainWindow::initializeUI() {
    // 由平台层处理
}

void MainWindow::createMenuBar() {
    // 由平台层处理
}

void MainWindow::createToolBar() {
    // TODO: 如果需要的话可以在这里实现
}

void MainWindow::createStatusBar() {
    // 由平台层处理
}

void MainWindow::createEditorArea() {
    // 由平台层处理
}

void MainWindow::bindEvents() {
    // 由平台层处理
}

void MainWindow::onWindowClose() {
    // 由回调处理
}

void MainWindow::onFileChanged() {
    // 由回调处理
}

void MainWindow::onPluginStateChanged() {
    // TODO: 实现插件状态变化处理
}

void MainWindow::loadWindowConfig() {
    // TODO: 从配置管理器加载窗口配置
    if (configManager_) {
        width_ = configManager_->getInt("Window.width", 1024);
        height_ = configManager_->getInt("Window.height", 768);
        x_ = configManager_->getInt("Window.x", 100);
        y_ = configManager_->getInt("Window.y", 100);
        maximized_ = configManager_->getBool("Window.maximized", false);
        
        setSize(width_, height_);
        setPosition(x_, y_);
        if (maximized_) {
            maximize();
        }
    }
}

void MainWindow::saveWindowConfig() {
    // TODO: 保存窗口配置到配置管理器
    if (configManager_) {
        configManager_->setInt("Window.width", width_);
        configManager_->setInt("Window.height", height_);
        configManager_->setInt("Window.x", x_);
        configManager_->setInt("Window.y", y_);
        configManager_->setBool("Window.maximized", isMaximized());
    }
}
