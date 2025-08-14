#include "WindowsWindow.h"

#ifdef WIN32

#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>
#include <iostream>
#include "../Editor.h"

// WindowsWindow::Impl 类实现
class WindowsWindow::Impl {
public:
    HWND hwnd;
    HWND textEdit;
    HWND statusBar;
    HMENU menuBar;
    HFONT textFont;
    
    std::function<void(const std::string&)> textChangedCallback;
    std::function<bool()> windowCloseCallback;
    
    std::shared_ptr<Editor> editor;
    std::shared_ptr<PluginManager> pluginManager;
    std::shared_ptr<ConfigManager> configManager;
    
    Impl() : hwnd(nullptr), textEdit(nullptr), statusBar(nullptr), menuBar(nullptr), textFont(nullptr) {}
    
    ~Impl() {
        if (textFont) {
            DeleteObject(textFont);
        }
    }
};

// 窗口类名
static const char* WINDOW_CLASS_NAME = "LitePadWindow";
static bool classRegistered = false;

// 字符串转换辅助函数
std::string wstringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::wstring stringToWstring(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

WindowsWindow::WindowsWindow() : pImpl(std::make_unique<Impl>()) {
    // 更大的默认窗口尺寸，类似现代应用
    width_ = 1000;
    height_ = 700;
    
    // 居中显示窗口
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    x_ = (screenWidth - width_) / 2;
    y_ = (screenHeight - height_) / 2;
    
    title_ = "LitePad";
    
    // 注册窗口类
    if (!classRegistered) {
        WNDCLASSEX wc = {};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        // 使用白色背景，更现代化
        wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
        wc.lpszClassName = WINDOW_CLASS_NAME;
        wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
        
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
        pImpl->hwnd = CreateWindowEx(
            0,
            WINDOW_CLASS_NAME,
            title_.c_str(),
            WS_OVERLAPPEDWINDOW,
            x_, y_, width_, height_,
            nullptr, nullptr,
            GetModuleHandle(nullptr),
            this
        );
        
        if (pImpl->hwnd) {
            // 创建菜单栏
            createMenuBar();
            
            // 创建文本编辑控件 - 现代化样式
            pImpl->textEdit = CreateWindowEx(
                0, // 移除边框，使用更现代的样式
                "EDIT",
                "",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN,
                10, 35, width_ - 20, height_ - 65, // 添加内边距，为菜单栏和状态栏留出空间
                pImpl->hwnd,
                (HMENU)2000, // 改变ID以避免与菜单项冲突
                GetModuleHandle(nullptr),
                nullptr
            );
            
            // 设置现代化字体 - 类似Windows 11记事本
            if (pImpl->textEdit) {
                pImpl->textFont = CreateFont(
                    16,                        // 字体高度
                    0,                         // 字体宽度
                    0,                         // 角度
                    0,                         // 基线角度
                    FW_NORMAL,                 // 字体粗细
                    FALSE,                     // 斜体
                    FALSE,                     // 下划线
                    FALSE,                     // 删除线
                    DEFAULT_CHARSET,           // 字符集
                    OUT_DEFAULT_PRECIS,        // 输出精度
                    CLIP_DEFAULT_PRECIS,       // 剪切精度
                    CLEARTYPE_QUALITY,         // 字体质量 - 使用ClearType
                    DEFAULT_PITCH | FF_DONTCARE, // 字体间距和家族
                    "Consolas"                 // 字体名称 - 使用Consolas，类似记事本
                );
                SendMessage(pImpl->textEdit, WM_SETFONT, (WPARAM)pImpl->textFont, TRUE);
                
                // 设置文本编辑控件的背景色为纯白色
                SetWindowLongPtr(pImpl->textEdit, GWL_EXSTYLE, 
                    GetWindowLongPtr(pImpl->textEdit, GWL_EXSTYLE) | WS_EX_STATICEDGE);
            }
            
            // 创建状态栏
            pImpl->statusBar = CreateWindowEx(
                0,
                STATUSCLASSNAME,
                nullptr,
                WS_CHILD | WS_VISIBLE,
                0, 0, 0, 0,
                pImpl->hwnd,
                (HMENU)2001, // 改变ID以避免与菜单项冲突
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
        SetWindowText(pImpl->hwnd, title.c_str());
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
    char szFile[260] = {};
    
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = pImpl->hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "All Files\0*.*\0Text Files\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileName(&ofn)) {
        std::string filename(szFile);
        
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
    char szFile[260] = {};
    
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = pImpl->hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "All Files\0*.*\0Text Files\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    
    if (GetSaveFileName(&ofn)) {
        std::string filename(szFile);
        
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
    MessageBox(pImpl->hwnd, "LitePad v1.0.0\nLightweight Code Editor", "About", MB_OK | MB_ICONINFORMATION);
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
        SetWindowText(pImpl->textEdit, content.c_str());
    }
}

std::string WindowsWindow::getTextContent() const {
    if (!pImpl->textEdit) return "";
    
    int length = GetWindowTextLength(pImpl->textEdit);
    if (length == 0) return "";
    
    std::string content(length + 1, '\0');
    GetWindowText(pImpl->textEdit, &content[0], length + 1);
    content.resize(length); // 移除末尾的空字符
    
    return content;
}

void WindowsWindow::setTextChangedCallback(std::function<void(const std::string&)> callback) {
    pImpl->textChangedCallback = callback;
}

void WindowsWindow::setWindowCloseCallback(std::function<bool()> callback) {
    pImpl->windowCloseCallback = callback;
}

void WindowsWindow::setStatusText(const std::string& text) {
    if (pImpl->statusBar) {
        SendMessage(pImpl->statusBar, SB_SETTEXT, 0, (LPARAM)text.c_str());
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
                    // 获取菜单栏高度
                    MENUBARINFO mbi = {};
                    mbi.cbSize = sizeof(MENUBARINFO);
                    int menuHeight = 25; // 默认菜单栏高度
                    if (GetMenuBarInfo(hwnd, OBJID_MENU, 0, &mbi)) {
                        menuHeight = mbi.rcBar.bottom - mbi.rcBar.top;
                        if (menuHeight <= 0) menuHeight = 25;
                    }
                    
                    // 获取状态栏高度
                    RECT statusRect;
                    int statusHeight = 20;
                    if (window->pImpl->statusBar) {
                        GetWindowRect(window->pImpl->statusBar, &statusRect);
                        statusHeight = statusRect.bottom - statusRect.top;
                    }
                    
                    SetWindowPos(window->pImpl->textEdit, nullptr, 10, menuHeight + 10, width - 20, height - menuHeight - statusHeight - 20, SWP_NOZORDER);
                }
                if (window->pImpl->statusBar) {
                    SendMessage(window->pImpl->statusBar, WM_SIZE, 0, 0);
                }
                break;
            }
            
            case WM_COMMAND:
                if (HIWORD(wParam) == EN_CHANGE && LOWORD(wParam) == 2000) {
                    // 文本内容改变
                    if (window->pImpl->textChangedCallback) {
                        window->pImpl->textChangedCallback(window->getTextContent());
                    }
                } else if (HIWORD(wParam) == 0) {
                    // 菜单命令
                    window->handleMenuCommand(LOWORD(wParam));
                }
                break;
                
            case WM_CLOSE:
                if (window->pImpl->windowCloseCallback) {
                    if (!window->pImpl->windowCloseCallback()) {
                        return 0; // 阻止关闭
                    }
                }
                break;
                
            case WM_CTLCOLOREDIT:
                // 设置编辑控件的颜色 - 现代化外观
                if ((HWND)lParam == window->pImpl->textEdit) {
                    HDC hdc = (HDC)wParam;
                    SetTextColor(hdc, RGB(0, 0, 0));           // 黑色文字
                    SetBkColor(hdc, RGB(255, 255, 255));       // 白色背景
                    return (LRESULT)CreateSolidBrush(RGB(255, 255, 255));
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

void WindowsWindow::createMenuBar() {
    // 创建主菜单
    pImpl->menuBar = CreateMenu();
    
    // 文件菜单
    HMENU fileMenu = CreatePopupMenu();
    AppendMenu(fileMenu, MF_STRING, ID_FILE_NEW, "&New\tCtrl+N");
    AppendMenu(fileMenu, MF_STRING, ID_FILE_OPEN, "&Open...\tCtrl+O");
    AppendMenu(fileMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenu(fileMenu, MF_STRING, ID_FILE_SAVE, "&Save\tCtrl+S");
    AppendMenu(fileMenu, MF_STRING, ID_FILE_SAVE_AS, "Save &As...\tCtrl+Shift+S");
    AppendMenu(fileMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenu(fileMenu, MF_STRING, ID_FILE_EXIT, "E&xit\tAlt+F4");
    AppendMenu(pImpl->menuBar, MF_POPUP, (UINT_PTR)fileMenu, "&File");
    
    // 编辑菜单
    HMENU editMenu = CreatePopupMenu();
    AppendMenu(editMenu, MF_STRING, ID_EDIT_UNDO, "&Undo\tCtrl+Z");
    AppendMenu(editMenu, MF_STRING, ID_EDIT_REDO, "&Redo\tCtrl+Y");
    AppendMenu(editMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenu(editMenu, MF_STRING, ID_EDIT_CUT, "Cu&t\tCtrl+X");
    AppendMenu(editMenu, MF_STRING, ID_EDIT_COPY, "&Copy\tCtrl+C");
    AppendMenu(editMenu, MF_STRING, ID_EDIT_PASTE, "&Paste\tCtrl+V");
    AppendMenu(editMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenu(editMenu, MF_STRING, ID_EDIT_SELECT_ALL, "Select &All\tCtrl+A");
    AppendMenu(editMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenu(editMenu, MF_STRING, ID_EDIT_FIND, "&Find...\tCtrl+F");
    AppendMenu(pImpl->menuBar, MF_POPUP, (UINT_PTR)editMenu, "&Edit");
    
    // 帮助菜单
    HMENU helpMenu = CreatePopupMenu();
    AppendMenu(helpMenu, MF_STRING, ID_HELP_ABOUT, "&About LitePad");
    AppendMenu(pImpl->menuBar, MF_POPUP, (UINT_PTR)helpMenu, "&Help");
    
    // 设置菜单到窗口
    SetMenu(pImpl->hwnd, pImpl->menuBar);
}

void WindowsWindow::handleMenuCommand(int menuId) {
    switch (menuId) {
        case ID_FILE_NEW:
            newFile();
            break;
        case ID_FILE_OPEN:
            openFile();
            break;
        case ID_FILE_SAVE:
            saveFile();
            break;
        case ID_FILE_SAVE_AS:
            saveAs();
            break;
        case ID_FILE_EXIT:
            quit();
            break;
            
        case ID_EDIT_UNDO:
            // 发送撤销命令到编辑控件
            if (pImpl->textEdit) {
                SendMessage(pImpl->textEdit, WM_UNDO, 0, 0);
            }
            break;
        case ID_EDIT_CUT:
            if (pImpl->textEdit) {
                SendMessage(pImpl->textEdit, WM_CUT, 0, 0);
            }
            break;
        case ID_EDIT_COPY:
            if (pImpl->textEdit) {
                SendMessage(pImpl->textEdit, WM_COPY, 0, 0);
            }
            break;
        case ID_EDIT_PASTE:
            if (pImpl->textEdit) {
                SendMessage(pImpl->textEdit, WM_PASTE, 0, 0);
            }
            break;
        case ID_EDIT_SELECT_ALL:
            if (pImpl->textEdit) {
                SendMessage(pImpl->textEdit, EM_SETSEL, 0, -1);
            }
            break;
        case ID_EDIT_FIND:
            // TODO: 实现查找对话框
            MessageBox(pImpl->hwnd, "Find functionality not yet implemented", "Info", MB_OK | MB_ICONINFORMATION);
            break;
            
        case ID_HELP_ABOUT:
            showAbout();
            break;
    }
}

#endif // WIN32
