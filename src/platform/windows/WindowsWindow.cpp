#include "WindowsWindow.h"

#ifdef WIN32

#include <windows.h>
#include <commdlg.h>
#include <iostream>

// WindowsWindow::Impl 类实现
class WindowsWindow::Impl {
public:
    HWND hwnd;
    HWND textEdit;
    HWND statusBar;
    
    std::function<void(const std::string&)> textChangedCallback;
    std::function<bool()> windowCloseCallback;
    
    std::shared_ptr<Editor> editor;
    std::shared_ptr<PluginManager> pluginManager;
    std::shared_ptr<ConfigManager> configManager;
    
    Impl() : hwnd(nullptr), textEdit(nullptr), statusBar(nullptr) {}
};

// 窗口类名
static const wchar_t* WINDOW_CLASS_NAME = L"LitePadWindow";
static bool classRegistered = false;

WindowsWindow::WindowsWindow() : pImpl(std::make_unique<Impl>()) {
    width_ = 800;
    height_ = 600;
    x_ = 100;
    y_ = 100;
    title_ = "LitePad";
    
    // 注册窗口类
    if (!classRegistered) {
        WNDCLASSEX wc = {};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = WINDOW_CLASS_NAME;
        
        RegisterClassEx(&wc);
        classRegistered = true;
    }
}

WindowsWindow::~WindowsWindow() {
    if (pImpl->hwnd) {
        DestroyWindow(pImpl->hwnd);
    }
}

void WindowsWindow::show() {
    if (!pImpl->hwnd) {
        // 创建主窗口
        std::wstring wTitle(title_.begin(), title_.end());
        pImpl->hwnd = CreateWindowEx(
            0,
            WINDOW_CLASS_NAME,
            wTitle.c_str(),
            WS_OVERLAPPEDWINDOW,
            x_, y_, width_, height_,
            nullptr, nullptr,
            GetModuleHandle(nullptr),
            this
        );
        
        if (pImpl->hwnd) {
            // 创建文本编辑控件
            pImpl->textEdit = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                L"EDIT",
                L"",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                0, 0, width_, height_ - 20,
                pImpl->hwnd,
                (HMENU)1001,
                GetModuleHandle(nullptr),
                nullptr
            );
            
            // 创建状态栏
            pImpl->statusBar = CreateWindowEx(
                0,
                STATUSCLASSNAME,
                nullptr,
                WS_CHILD | WS_VISIBLE,
                0, 0, 0, 0,
                pImpl->hwnd,
                (HMENU)1002,
                GetModuleHandle(nullptr),
                nullptr
            );
        }
    }
    
    if (pImpl->hwnd) {
        ShowWindow(pImpl->hwnd, SW_SHOW);
        UpdateWindow(pImpl->hwnd);
    }
}

void WindowsWindow::hide() {
    if (pImpl->hwnd) {
        ShowWindow(pImpl->hwnd, SW_HIDE);
    }
}

void WindowsWindow::close() {
    if (pImpl->hwnd) {
        DestroyWindow(pImpl->hwnd);
        pImpl->hwnd = nullptr;
    }
}

void WindowsWindow::setTitle(const std::string& title) {
    title_ = title;
    if (pImpl->hwnd) {
        std::wstring wTitle(title.begin(), title.end());
        SetWindowText(pImpl->hwnd, wTitle.c_str());
    }
}

std::string WindowsWindow::getTitle() const {
    return title_;
}

void WindowsWindow::setSize(int width, int height) {
    width_ = width;
    height_ = height;
    if (pImpl->hwnd) {
        SetWindowPos(pImpl->hwnd, nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
    }
}

void WindowsWindow::getSize(int& width, int& height) const {
    width = width_;
    height = height_;
}

void WindowsWindow::setPosition(int x, int y) {
    x_ = x;
    y_ = y;
    if (pImpl->hwnd) {
        SetWindowPos(pImpl->hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }
}

void WindowsWindow::getPosition(int& x, int& y) const {
    x = x_;
    y = y_;
}

void WindowsWindow::maximize() {
    if (pImpl->hwnd) {
        ShowWindow(pImpl->hwnd, SW_MAXIMIZE);
    }
}

void WindowsWindow::minimize() {
    if (pImpl->hwnd) {
        ShowWindow(pImpl->hwnd, SW_MINIMIZE);
    }
}

void WindowsWindow::restore() {
    if (pImpl->hwnd) {
        ShowWindow(pImpl->hwnd, SW_RESTORE);
    }
}

bool WindowsWindow::isMaximized() const {
    if (pImpl->hwnd) {
        WINDOWPLACEMENT wp = {};
        wp.length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement(pImpl->hwnd, &wp);
        return wp.showCmd == SW_MAXIMIZE;
    }
    return false;
}

bool WindowsWindow::isMinimized() const {
    if (pImpl->hwnd) {
        WINDOWPLACEMENT wp = {};
        wp.length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement(pImpl->hwnd, &wp);
        return wp.showCmd == SW_MINIMIZE;
    }
    return false;
}

void WindowsWindow::newFile() {
    if (pImpl->editor) {
        pImpl->editor->clear();
        setTextContent("");
    }
}

void WindowsWindow::openFile() {
    OPENFILENAME ofn = {};
    wchar_t szFile[260] = {};
    
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = pImpl->hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"所有文件\0*.*\0文本文件\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileName(&ofn)) {
        std::wstring wFilename(szFile);
        std::string filename(wFilename.begin(), wFilename.end());
        
        if (pImpl->editor) {
            if (pImpl->editor->openFile(filename)) {
                setTextContent(pImpl->editor->getContent());
                setTitle("LitePad - " + filename);
            }
        }
    }
}

void WindowsWindow::saveFile() {
    if (pImpl->editor) {
        pImpl->editor->setContent(getTextContent());
        pImpl->editor->saveFile();
    }
}

void WindowsWindow::saveAs() {
    OPENFILENAME ofn = {};
    wchar_t szFile[260] = {};
    
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = pImpl->hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"所有文件\0*.*\0文本文件\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    
    if (GetSaveFileName(&ofn)) {
        std::wstring wFilename(szFile);
        std::string filename(wFilename.begin(), wFilename.end());
        
        if (pImpl->editor) {
            pImpl->editor->setContent(getTextContent());
            if (pImpl->editor->saveAs(filename)) {
                setTitle("LitePad - " + filename);
            }
        }
    }
}

void WindowsWindow::quit() {
    PostQuitMessage(0);
}

void WindowsWindow::showAbout() {
    MessageBox(pImpl->hwnd, L"LitePad v1.0.0\n轻量级代码编辑器", L"关于", MB_OK | MB_ICONINFORMATION);
}

void WindowsWindow::showPluginManager() {
    // TODO: 实现插件管理器界面
}

void WindowsWindow::showSettings() {
    // TODO: 实现设置界面
}

void WindowsWindow::setEditor(std::shared_ptr<Editor> editor) {
    pImpl->editor = editor;
}

void WindowsWindow::setPluginManager(std::shared_ptr<PluginManager> pluginManager) {
    pImpl->pluginManager = pluginManager;
}

void WindowsWindow::setConfigManager(std::shared_ptr<ConfigManager> configManager) {
    pImpl->configManager = configManager;
}

void WindowsWindow::setTextContent(const std::string& content) {
    if (pImpl->textEdit) {
        std::wstring wContent(content.begin(), content.end());
        SetWindowText(pImpl->textEdit, wContent.c_str());
    }
}

std::string WindowsWindow::getTextContent() const {
    if (!pImpl->textEdit) return "";
    
    int length = GetWindowTextLength(pImpl->textEdit);
    if (length == 0) return "";
    
    std::wstring wContent(length + 1, L'\0');
    GetWindowText(pImpl->textEdit, &wContent[0], length + 1);
    
    return std::string(wContent.begin(), wContent.end() - 1);
}

void WindowsWindow::setTextChangedCallback(std::function<void(const std::string&)> callback) {
    pImpl->textChangedCallback = callback;
}

void WindowsWindow::setWindowCloseCallback(std::function<bool()> callback) {
    pImpl->windowCloseCallback = callback;
}

void WindowsWindow::setStatusText(const std::string& text) {
    if (pImpl->statusBar) {
        std::wstring wText(text.begin(), text.end());
        SendMessage(pImpl->statusBar, SB_SETTEXT, 0, (LPARAM)wText.c_str());
    }
}

void WindowsWindow::handleFileDrop(const std::string& filePath) {
    if (pImpl->editor) {
        if (pImpl->editor->openFile(filePath)) {
            setTextContent(pImpl->editor->getContent());
            setTitle("LitePad - " + filePath);
        }
    }
}

// 窗口过程函数
LRESULT CALLBACK WindowsWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    WindowsWindow* window = nullptr;
    
    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<WindowsWindow*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    } else {
        window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    
    if (window) {
        switch (uMsg) {
            case WM_SIZE: {
                int width = LOWORD(lParam);
                int height = HIWORD(lParam);
                window->width_ = width;
                window->height_ = height;
                
                // 调整子控件大小
                if (window->pImpl->textEdit) {
                    SetWindowPos(window->pImpl->textEdit, nullptr, 0, 0, width, height - 20, SWP_NOZORDER);
                }
                if (window->pImpl->statusBar) {
                    SendMessage(window->pImpl->statusBar, WM_SIZE, 0, 0);
                }
                break;
            }
            
            case WM_COMMAND:
                if (HIWORD(wParam) == EN_CHANGE && LOWORD(wParam) == 1001) {
                    // 文本内容改变
                    if (window->pImpl->textChangedCallback) {
                        window->pImpl->textChangedCallback(window->getTextContent());
                    }
                }
                break;
                
            case WM_CLOSE:
                if (window->pImpl->windowCloseCallback) {
                    if (!window->pImpl->windowCloseCallback()) {
                        return 0; // 阻止关闭
                    }
                }
                break;
                
            case WM_DESTROY:
                window->pImpl->hwnd = nullptr;
                PostQuitMessage(0);
                break;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

#endif // WIN32
