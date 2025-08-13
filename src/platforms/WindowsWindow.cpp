#include "WindowsWindow.h"

#ifdef WIN32

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../Editor.h"
#include "../PluginManager.h"
#include "../ConfigManager.h"

// 链接必要的库
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "shell32.lib")

// 窗口类名
static const wchar_t* WINDOW_CLASS_NAME = L"LitePadMainWindow";

// 控件ID
#define ID_EDIT_MAIN        1001
#define ID_STATUS_BAR       1002
#define ID_MENU_FILE_NEW    2001
#define ID_MENU_FILE_OPEN   2002
#define ID_MENU_FILE_SAVE   2003
#define ID_MENU_FILE_SAVEAS 2004
#define ID_MENU_FILE_EXIT   2005
#define ID_MENU_PLUGINS     2006
#define ID_MENU_SETTINGS    2007
#define ID_MENU_ABOUT       2008

/**
 * WindowsWindow的私有实现
 */
class WindowsWindow::Impl {
public:
    HWND hwnd = nullptr;              // 主窗口句柄
    HWND hEdit = nullptr;             // 编辑框句柄
    HWND hStatusBar = nullptr;        // 状态栏句柄
    HINSTANCE hInstance = nullptr;    // 应用程序实例
    
    // 核心组件
    std::shared_ptr<Editor> editor;
    std::shared_ptr<PluginManager> pluginManager;
    std::shared_ptr<ConfigManager> configManager;
    
    // 回调函数
    std::function<void(const std::string&)> textChangedCallback;
    std::function<bool()> windowCloseCallback;
    
    // 窗口状态
    bool isVisible = false;
    
    /**
     * 窗口过程函数
     */
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        WindowsWindow::Impl* pThis = nullptr;
        
        if (uMsg == WM_NCCREATE) {
            // 获取创建参数中的this指针
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            pThis = reinterpret_cast<WindowsWindow::Impl*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        } else {
            // 从窗口数据中获取this指针
            pThis = reinterpret_cast<WindowsWindow::Impl*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }
        
        if (pThis) {
            return pThis->HandleMessage(hwnd, uMsg, wParam, lParam);
        }
        
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    
    /**
     * 处理窗口消息
     */
    LRESULT HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_CREATE:
            return OnCreate(hwnd);
            
        case WM_SIZE:
            return OnSize(hwnd, LOWORD(lParam), HIWORD(lParam));
            
        case WM_COMMAND:
            return OnCommand(hwnd, HIWORD(wParam), LOWORD(wParam), reinterpret_cast<HWND>(lParam));
            
        case WM_CLOSE:
            return OnClose(hwnd);
            
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        case WM_DROPFILES:
            return OnDropFiles(hwnd, reinterpret_cast<HDROP>(wParam));
            
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    
    /**
     * 处理窗口创建
     */
    LRESULT OnCreate(HWND hwnd) {
        this->hwnd = hwnd;
        
        // 启用拖放
        DragAcceptFiles(hwnd, TRUE);
        
        // 创建菜单
        CreateMenuBar(hwnd);
        
        // 创建编辑框
        hEdit = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            L"EDIT",
            L"",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
            0, 0, 0, 0,
            hwnd,
            reinterpret_cast<HMENU>(ID_EDIT_MAIN),
            hInstance,
            nullptr
        );
        
        if (!hEdit) {
            return -1;
        }
        
        // 设置编辑框字体
        HFONT hFont = CreateFont(
            16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN,
            L"Consolas"
        );
        SendMessage(hEdit, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
        
        // 创建状态栏
        hStatusBar = CreateWindowEx(
            0,
            STATUSCLASSNAME,
            nullptr,
            WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
            0, 0, 0, 0,
            hwnd,
            reinterpret_cast<HMENU>(ID_STATUS_BAR),
            hInstance,
            nullptr
        );
        
        return 0;
    }
    
    /**
     * 处理窗口大小变化
     */
    LRESULT OnSize(HWND hwnd, int width, int height) {
        if (hEdit && hStatusBar) {
            // 获取状态栏高度
            RECT statusRect;
            GetWindowRect(hStatusBar, &statusRect);
            int statusHeight = statusRect.bottom - statusRect.top;
            
            // 调整状态栏大小
            SendMessage(hStatusBar, WM_SIZE, 0, 0);
            
            // 调整编辑框大小
            SetWindowPos(hEdit, nullptr, 0, 0, width, height - statusHeight,
                        SWP_NOZORDER | SWP_NOACTIVATE);
        }
        return 0;
    }
    
    /**
     * 处理命令消息
     */
    LRESULT OnCommand(HWND hwnd, WORD hiWord, WORD loWord, HWND hControl) {
        // 处理编辑框文本变化
        if (hControl == hEdit && hiWord == EN_CHANGE) {
            if (textChangedCallback) {
                int textLength = GetWindowTextLength(hEdit);
                std::wstring wtext(textLength + 1, L'\0');
                GetWindowText(hEdit, &wtext[0], textLength + 1);
                
                // 转换为UTF-8
                std::string text = WideStringToUTF8(wtext);
                textChangedCallback(text);
            }
            return 0;
        }
        
        // 处理菜单命令
        switch (loWord) {
        case ID_MENU_FILE_NEW:
            NewFile();
            break;
        case ID_MENU_FILE_OPEN:
            OpenFile();
            break;
        case ID_MENU_FILE_SAVE:
            SaveFile();
            break;
        case ID_MENU_FILE_SAVEAS:
            SaveAs();
            break;
        case ID_MENU_FILE_EXIT:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        case ID_MENU_PLUGINS:
            ShowPluginManager();
            break;
        case ID_MENU_SETTINGS:
            ShowSettings();
            break;
        case ID_MENU_ABOUT:
            ShowAbout();
            break;
        }
        
        return 0;
    }
    
    /**
     * 处理窗口关闭
     */
    LRESULT OnClose(HWND hwnd) {
        bool canClose = true;
        if (windowCloseCallback) {
            canClose = windowCloseCallback();
        }
        
        if (canClose) {
            DestroyWindow(hwnd);
        }
        
        return 0;
    }
    
    /**
     * 处理文件拖放
     */
    LRESULT OnDropFiles(HWND hwnd, HDROP hDrop) {
        wchar_t filePath[MAX_PATH];
        UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
        
        if (fileCount > 0) {
            // 只处理第一个文件
            DragQueryFile(hDrop, 0, filePath, MAX_PATH);
            std::string path = WideStringToUTF8(filePath);
            HandleFileDrop(path);
        }
        
        DragFinish(hDrop);
        return 0;
    }
    
    /**
     * 创建菜单栏
     */
    void CreateMenuBar(HWND hwnd) {
        HMENU hMenuBar = CreateMenu();
        
        // 文件菜单
        HMENU hFileMenu = CreatePopupMenu();
        AppendMenu(hFileMenu, MF_STRING, ID_MENU_FILE_NEW, L"新建(&N)\tCtrl+N");
        AppendMenu(hFileMenu, MF_STRING, ID_MENU_FILE_OPEN, L"打开(&O)\tCtrl+O");
        AppendMenu(hFileMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenu(hFileMenu, MF_STRING, ID_MENU_FILE_SAVE, L"保存(&S)\tCtrl+S");
        AppendMenu(hFileMenu, MF_STRING, ID_MENU_FILE_SAVEAS, L"另存为(&A)\tCtrl+Shift+S");
        AppendMenu(hFileMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenu(hFileMenu, MF_STRING, ID_MENU_FILE_EXIT, L"退出(&X)\tAlt+F4");
        AppendMenu(hMenuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(hFileMenu), L"文件(&F)");
        
        // 插件菜单
        AppendMenu(hMenuBar, MF_STRING, ID_MENU_PLUGINS, L"插件(&P)");
        
        // 帮助菜单
        HMENU hHelpMenu = CreatePopupMenu();
        AppendMenu(hHelpMenu, MF_STRING, ID_MENU_SETTINGS, L"设置(&S)");
        AppendMenu(hHelpMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenu(hHelpMenu, MF_STRING, ID_MENU_ABOUT, L"关于(&A)");
        AppendMenu(hMenuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(hHelpMenu), L"帮助(&H)");
        
        SetMenu(hwnd, hMenuBar);
    }
    
    /**
     * 文件操作实现
     */
    void NewFile() {
        SetWindowText(hEdit, L"");
        if (editor) {
            editor->newFile();
        }
        SetStatusText("新建文件");
    }
    
    void OpenFile() {
        OPENFILENAME ofn = {};
        wchar_t szFile[MAX_PATH] = {};
        
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
        ofn.lpstrFilter = L"所有文件\0*.*\0文本文件\0*.txt\0C++文件\0*.cpp;*.h\0";
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        
        if (GetOpenFileName(&ofn)) {
            std::string filePath = WideStringToUTF8(szFile);
            LoadFileContent(filePath);
        }
    }
    
    void SaveFile() {
        // TODO: 实现保存当前文件
        if (editor && !editor->getCurrentFilePath().empty()) {
            SaveFileContent(editor->getCurrentFilePath());
        } else {
            SaveAs();
        }
    }
    
    void SaveAs() {
        OPENFILENAME ofn = {};
        wchar_t szFile[MAX_PATH] = {};
        
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
        ofn.lpstrFilter = L"所有文件\0*.*\0文本文件\0*.txt\0C++文件\0*.cpp;*.h\0";
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
        
        if (GetSaveFileName(&ofn)) {
            std::string filePath = WideStringToUTF8(szFile);
            SaveFileContent(filePath);
        }
    }
    
    /**
     * 加载文件内容
     */
    void LoadFileContent(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::binary);
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
            file.close();
            
            // 转换为宽字符并设置到编辑框
            std::wstring wcontent = UTF8ToWideString(content);
            SetWindowText(hEdit, wcontent.c_str());
            
            if (editor) {
                editor->openFile(filePath);
            }
            
            SetStatusText("文件已打开: " + filePath);
        } else {
            MessageBox(hwnd, L"无法打开文件", L"错误", MB_OK | MB_ICONERROR);
        }
    }
    
    /**
     * 保存文件内容
     */
    void SaveFileContent(const std::string& filePath) {
        int textLength = GetWindowTextLength(hEdit);
        std::wstring wtext(textLength + 1, L'\0');
        GetWindowText(hEdit, &wtext[0], textLength + 1);
        
        std::string content = WideStringToUTF8(wtext);
        
        std::ofstream file(filePath, std::ios::binary);
        if (file.is_open()) {
            file.write(content.c_str(), content.length());
            file.close();
            
            if (editor) {
                editor->saveFile(filePath);
            }
            
            SetStatusText("文件已保存: " + filePath);
        } else {
            MessageBox(hwnd, L"无法保存文件", L"错误", MB_OK | MB_ICONERROR);
        }
    }
    
    /**
     * 处理文件拖放
     */
    void HandleFileDrop(const std::string& filePath) {
        LoadFileContent(filePath);
    }
    
    /**
     * 设置状态栏文本
     */
    void SetStatusText(const std::string& text) {
        if (hStatusBar) {
            std::wstring wtext = UTF8ToWideString(text);
            SendMessage(hStatusBar, SB_SETTEXT, 0, reinterpret_cast<LPARAM>(wtext.c_str()));
        }
    }
    
    /**
     * 显示对话框
     */
    void ShowPluginManager() {
        MessageBox(hwnd, L"插件管理器功能正在开发中", L"插件管理器", MB_OK | MB_ICONINFORMATION);
    }
    
    void ShowSettings() {
        MessageBox(hwnd, L"设置功能正在开发中", L"设置", MB_OK | MB_ICONINFORMATION);
    }
    
    void ShowAbout() {
        MessageBox(hwnd, L"LitePad v1.0\n轻量级代码编辑器\n\n使用C++开发，支持跨平台", 
                  L"关于 LitePad", MB_OK | MB_ICONINFORMATION);
    }
    
    /**
     * 字符串转换工具
     */
    std::string WideStringToUTF8(const std::wstring& wstr) {
        if (wstr.empty()) return {};
        
        int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
        std::string result(size - 1, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, nullptr, nullptr);
        return result;
    }
    
    std::wstring UTF8ToWideString(const std::string& str) {
        if (str.empty()) return {};
        
        int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
        std::wstring result(size - 1, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
        return result;
    }
};

// WindowsWindow 实现

WindowsWindow::WindowsWindow() : pImpl(std::make_unique<Impl>()) {
    pImpl->hInstance = GetModuleHandle(nullptr);
    
    // 注册窗口类
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = Impl::WindowProc;
    wc.hInstance = pImpl->hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    
    if (!RegisterClassEx(&wc)) {
        throw std::runtime_error("Failed to register window class");
    }
    
    // 初始化通用控件
    INITCOMMONCONTROLSEX icex = {};
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);
    
    // 设置默认值
    title_ = "LitePad";
    width_ = 1024;
    height_ = 768;
    x_ = CW_USEDEFAULT;
    y_ = CW_USEDEFAULT;
}

WindowsWindow::~WindowsWindow() = default;

void WindowsWindow::show() {
    if (!pImpl->hwnd) {
        // 创建窗口
        pImpl->hwnd = CreateWindowEx(
            WS_EX_ACCEPTFILES,
            WINDOW_CLASS_NAME,
            UTF8ToWideString(title_).c_str(),
            WS_OVERLAPPEDWINDOW,
            x_, y_, width_, height_,
            nullptr, nullptr, pImpl->hInstance, pImpl.get()
        );
        
        if (!pImpl->hwnd) {
            throw std::runtime_error("Failed to create window");
        }
    }
    
    ShowWindow(pImpl->hwnd, SW_SHOW);
    UpdateWindow(pImpl->hwnd);
    pImpl->isVisible = true;
}

void WindowsWindow::hide() {
    if (pImpl->hwnd) {
        ShowWindow(pImpl->hwnd, SW_HIDE);
        pImpl->isVisible = false;
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
        SetWindowText(pImpl->hwnd, UTF8ToWideString(title).c_str());
    }
}

std::string WindowsWindow::getTitle() const {
    return title_;
}

void WindowsWindow::setSize(int width, int height) {
    width_ = width;
    height_ = height;
    if (pImpl->hwnd) {
        SetWindowPos(pImpl->hwnd, nullptr, 0, 0, width, height,
                    SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
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
        SetWindowPos(pImpl->hwnd, nullptr, x, y, 0, 0,
                    SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
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
    pImpl->NewFile();
}

void WindowsWindow::openFile() {
    pImpl->OpenFile();
}

void WindowsWindow::saveFile() {
    pImpl->SaveFile();
}

void WindowsWindow::saveAs() {
    pImpl->SaveAs();
}

void WindowsWindow::quit() {
    PostQuitMessage(0);
}

void WindowsWindow::showAbout() {
    pImpl->ShowAbout();
}

void WindowsWindow::showPluginManager() {
    pImpl->ShowPluginManager();
}

void WindowsWindow::showSettings() {
    pImpl->ShowSettings();
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
    if (pImpl->hEdit) {
        std::wstring wcontent = UTF8ToWideString(content);
        SetWindowText(pImpl->hEdit, wcontent.c_str());
    }
}

std::string WindowsWindow::getTextContent() const {
    if (pImpl->hEdit) {
        int textLength = GetWindowTextLength(pImpl->hEdit);
        std::wstring wtext(textLength + 1, L'\0');
        GetWindowText(pImpl->hEdit, &wtext[0], textLength + 1);
        return WideStringToUTF8(wtext);
    }
    return {};
}

void WindowsWindow::setTextChangedCallback(std::function<void(const std::string&)> callback) {
    pImpl->textChangedCallback = callback;
}

void WindowsWindow::setWindowCloseCallback(std::function<bool()> callback) {
    pImpl->windowCloseCallback = callback;
}

void WindowsWindow::setStatusText(const std::string& text) {
    pImpl->SetStatusText(text);
}

void WindowsWindow::handleFileDrop(const std::string& filePath) {
    pImpl->HandleFileDrop(filePath);
}

// 辅助函数（移到私有实现中）
std::string WideStringToUTF8(const std::wstring& wstr) {
    if (wstr.empty()) return {};
    
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string result(size - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, nullptr, nullptr);
    return result;
}

std::wstring UTF8ToWideString(const std::string& str) {
    if (str.empty()) return {};
    
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring result(size - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
    return result;
}

#endif // WIN32
