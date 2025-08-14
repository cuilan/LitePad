#include "LinuxWindow.h"

#ifdef LINUX

#include <gtk/gtk.h>
#include <iostream>

// LinuxWindow::Impl 类实现
class LinuxWindow::Impl {
public:
    GtkWidget* window;
    GtkWidget* vbox;
    GtkWidget* textView;
    GtkTextBuffer* textBuffer;
    GtkWidget* statusBar;
    
    std::function<void(const std::string&)> textChangedCallback;
    std::function<bool()> windowCloseCallback;
    
    std::shared_ptr<Editor> editor;
    std::shared_ptr<PluginManager> pluginManager;
    std::shared_ptr<ConfigManager> configManager;
    
    Impl() : window(nullptr), vbox(nullptr), textView(nullptr), 
             textBuffer(nullptr), statusBar(nullptr) {}
};

LinuxWindow::LinuxWindow() : pImpl(std::make_unique<Impl>()) {
    width_ = 800;
    height_ = 600;
    x_ = 100;
    y_ = 100;
    title_ = "LitePad";
}

LinuxWindow::~LinuxWindow() {
    if (pImpl->window) {
        gtk_widget_destroy(pImpl->window);
    }
}

void LinuxWindow::show() {
    if (!pImpl->window) {
        // 创建主窗口
        pImpl->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(pImpl->window), title_.c_str());
        gtk_window_set_default_size(GTK_WINDOW(pImpl->window), width_, height_);
        gtk_window_move(GTK_WINDOW(pImpl->window), x_, y_);
        
        // 创建垂直布局容器
        pImpl->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_container_add(GTK_CONTAINER(pImpl->window), pImpl->vbox);
        
        // 创建文本编辑器
        pImpl->textView = gtk_text_view_new();
        pImpl->textBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pImpl->textView));
        
        // 创建滚动窗口
        GtkWidget* scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
                                     GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_container_add(GTK_CONTAINER(scrolledWindow), pImpl->textView);
        
        // 创建状态栏
        pImpl->statusBar = gtk_statusbar_new();
        
        // 将组件添加到主容器
        gtk_box_pack_start(GTK_BOX(pImpl->vbox), scrolledWindow, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(pImpl->vbox), pImpl->statusBar, FALSE, FALSE, 0);
        
        // 连接信号
        g_signal_connect(pImpl->window, "delete-event", G_CALLBACK(onDeleteEvent), this);
        g_signal_connect(pImpl->window, "destroy", G_CALLBACK(onDestroy), this);
        g_signal_connect(pImpl->textBuffer, "changed", G_CALLBACK(onTextChanged), this);
        
        gtk_widget_show_all(pImpl->window);
    } else {
        gtk_widget_show(pImpl->window);
    }
}

void LinuxWindow::hide() {
    if (pImpl->window) {
        gtk_widget_hide(pImpl->window);
    }
}

void LinuxWindow::close() {
    if (pImpl->window) {
        gtk_widget_destroy(pImpl->window);
        pImpl->window = nullptr;
    }
}

void LinuxWindow::setTitle(const std::string& title) {
    title_ = title;
    if (pImpl->window) {
        gtk_window_set_title(GTK_WINDOW(pImpl->window), title.c_str());
    }
}

std::string LinuxWindow::getTitle() const {
    return title_;
}

void LinuxWindow::setSize(int width, int height) {
    width_ = width;
    height_ = height;
    if (pImpl->window) {
        gtk_window_resize(GTK_WINDOW(pImpl->window), width, height);
    }
}

void LinuxWindow::getSize(int& width, int& height) const {
    width = width_;
    height = height_;
}

void LinuxWindow::setPosition(int x, int y) {
    x_ = x;
    y_ = y;
    if (pImpl->window) {
        gtk_window_move(GTK_WINDOW(pImpl->window), x, y);
    }
}

void LinuxWindow::getPosition(int& x, int& y) const {
    x = x_;
    y = y_;
}

void LinuxWindow::maximize() {
    if (pImpl->window) {
        gtk_window_maximize(GTK_WINDOW(pImpl->window));
    }
}

void LinuxWindow::minimize() {
    if (pImpl->window) {
        gtk_window_iconify(GTK_WINDOW(pImpl->window));
    }
}

void LinuxWindow::restore() {
    if (pImpl->window) {
        gtk_window_unmaximize(GTK_WINDOW(pImpl->window));
        gtk_window_deiconify(GTK_WINDOW(pImpl->window));
    }
}

bool LinuxWindow::isMaximized() const {
    // GTK+ 实现待完善
    return false;
}

bool LinuxWindow::isMinimized() const {
    // GTK+ 实现待完善
    return false;
}

void LinuxWindow::newFile() {
    if (pImpl->editor) {
        pImpl->editor->clear();
        setTextContent("");
    }
}

void LinuxWindow::openFile() {
    // 实现文件打开对话框
    GtkWidget* dialog = gtk_file_chooser_dialog_new("打开文件",
                                                  GTK_WINDOW(pImpl->window),
                                                  GTK_FILE_CHOOSER_ACTION_OPEN,
                                                  "取消", GTK_RESPONSE_CANCEL,
                                                  "打开", GTK_RESPONSE_ACCEPT,
                                                  NULL);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (pImpl->editor && filename) {
            if (pImpl->editor->openFile(filename)) {
                setTextContent(pImpl->editor->getContent());
                setTitle("LitePad - " + std::string(filename));
            }
        }
        g_free(filename);
    }
    
    gtk_widget_destroy(dialog);
}

void LinuxWindow::saveFile() {
    if (pImpl->editor) {
        pImpl->editor->setContent(getTextContent());
        pImpl->editor->saveFile();
    }
}

void LinuxWindow::saveAs() {
    // 实现另存为对话框
    GtkWidget* dialog = gtk_file_chooser_dialog_new("另存为",
                                                  GTK_WINDOW(pImpl->window),
                                                  GTK_FILE_CHOOSER_ACTION_SAVE,
                                                  "取消", GTK_RESPONSE_CANCEL,
                                                  "保存", GTK_RESPONSE_ACCEPT,
                                                  NULL);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (pImpl->editor && filename) {
            pImpl->editor->setContent(getTextContent());
            if (pImpl->editor->saveAs(filename)) {
                setTitle("LitePad - " + std::string(filename));
            }
        }
        g_free(filename);
    }
    
    gtk_widget_destroy(dialog);
}

void LinuxWindow::quit() {
    gtk_main_quit();
}

void LinuxWindow::showAbout() {
    GtkWidget* dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "LitePad");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "1.0.0");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "轻量级代码编辑器");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void LinuxWindow::showPluginManager() {
    // TODO: 实现插件管理器界面
}

void LinuxWindow::showSettings() {
    // TODO: 实现设置界面
}

void LinuxWindow::setEditor(std::shared_ptr<Editor> editor) {
    pImpl->editor = editor;
}

void LinuxWindow::setPluginManager(std::shared_ptr<PluginManager> pluginManager) {
    pImpl->pluginManager = pluginManager;
}

void LinuxWindow::setConfigManager(std::shared_ptr<ConfigManager> configManager) {
    pImpl->configManager = configManager;
}

void LinuxWindow::setTextContent(const std::string& content) {
    if (pImpl->textBuffer) {
        gtk_text_buffer_set_text(pImpl->textBuffer, content.c_str(), -1);
    }
}

std::string LinuxWindow::getTextContent() const {
    if (!pImpl->textBuffer) return "";
    
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(pImpl->textBuffer, &start);
    gtk_text_buffer_get_end_iter(pImpl->textBuffer, &end);
    
    gchar* text = gtk_text_buffer_get_text(pImpl->textBuffer, &start, &end, FALSE);
    std::string result(text);
    g_free(text);
    
    return result;
}

void LinuxWindow::setTextChangedCallback(std::function<void(const std::string&)> callback) {
    pImpl->textChangedCallback = callback;
}

void LinuxWindow::setWindowCloseCallback(std::function<bool()> callback) {
    pImpl->windowCloseCallback = callback;
}

void LinuxWindow::setStatusText(const std::string& text) {
    if (pImpl->statusBar) {
        guint contextId = gtk_statusbar_get_context_id(GTK_STATUSBAR(pImpl->statusBar), "main");
        gtk_statusbar_pop(GTK_STATUSBAR(pImpl->statusBar), contextId);
        gtk_statusbar_push(GTK_STATUSBAR(pImpl->statusBar), contextId, text.c_str());
    }
}

void LinuxWindow::handleFileDrop(const std::string& filePath) {
    if (pImpl->editor) {
        if (pImpl->editor->openFile(filePath)) {
            setTextContent(pImpl->editor->getContent());
            setTitle("LitePad - " + filePath);
        }
    }
}

// 静态回调函数
gboolean LinuxWindow::onDeleteEvent(GtkWidget* widget, GdkEvent* event, gpointer userData) {
    LinuxWindow* window = static_cast<LinuxWindow*>(userData);
    if (window->pImpl->windowCloseCallback) {
        return !window->pImpl->windowCloseCallback(); // GTK返回TRUE阻止关闭
    }
    return FALSE;
}

void LinuxWindow::onDestroy(GtkWidget* widget, gpointer userData) {
    LinuxWindow* window = static_cast<LinuxWindow*>(userData);
    window->pImpl->window = nullptr;
}

void LinuxWindow::onTextChanged(GtkTextBuffer* textBuffer, gpointer userData) {
    LinuxWindow* window = static_cast<LinuxWindow*>(userData);
    if (window->pImpl->textChangedCallback) {
        window->pImpl->textChangedCallback(window->getTextContent());
    }
}

#endif // LINUX
