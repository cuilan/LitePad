#import <Cocoa/Cocoa.h>
#include "MainWindow.h"
#include "Editor.h"
#include "PluginManager.h"
#include "ConfigManager.h"
#include <memory>

// Objective-C 接口声明
@interface LitePadWindowController : NSWindowController <NSWindowDelegate, NSTextViewDelegate>
{
    std::shared_ptr<Editor> editor_;
    std::shared_ptr<PluginManager> pluginManager_;
    std::shared_ptr<ConfigManager> configManager_;
}

@property (strong) NSTextView *textView;
@property (strong) NSScrollView *scrollView;
@property (strong) NSMenu *pluginMenu;

- (instancetype)initWithEditor:(std::shared_ptr<Editor>)editor
                pluginManager:(std::shared_ptr<PluginManager>)pluginManager
                configManager:(std::shared_ptr<ConfigManager>)configManager;

// 菜单动作
- (IBAction)newFile:(id)sender;
- (IBAction)openFile:(id)sender;
- (IBAction)saveFile:(id)sender;
- (IBAction)saveAsFile:(id)sender;
- (IBAction)showPluginManager:(id)sender;
- (IBAction)showAbout:(id)sender;

@end

@implementation LitePadWindowController

- (instancetype)initWithEditor:(std::shared_ptr<Editor>)editor
                pluginManager:(std::shared_ptr<PluginManager>)pluginManager
                configManager:(std::shared_ptr<ConfigManager>)configManager
{
    self = [super init];
    if (self) {
        editor_ = editor;
        pluginManager_ = pluginManager;
        configManager_ = configManager;
        [self setupWindow];
        [self setupMenuBar];
        [self setupTextView];
        [self bindCallbacks];
    }
    return self;
}

- (void)setupWindow
{
    // 创建窗口
    NSRect windowFrame = NSMakeRect(100, 100, 1024, 768);
    NSWindow *window = [[NSWindow alloc] initWithContentRect:windowFrame
                                                   styleMask:(NSWindowStyleMaskTitled | 
                                                             NSWindowStyleMaskClosable | 
                                                             NSWindowStyleMaskMiniaturizable | 
                                                             NSWindowStyleMaskResizable)
                                                     backing:NSBackingStoreBuffered
                                                       defer:NO];
    
    [window setTitle:@"LitePad - Lightweight Code Editor"];
    [window setDelegate:self];
    [window center];
    
    self.window = window;
}

- (void)setupMenuBar
{
    NSMenu *mainMenu = [[NSMenu alloc] init];
    
    // 应用程序菜单
    NSMenuItem *appMenuItem = [[NSMenuItem alloc] init];
    NSMenu *appMenu = [[NSMenu alloc] init];
    
    [appMenu addItemWithTitle:@"About LitePad" 
                       action:@selector(showAbout:) 
                keyEquivalent:@""];
    [appMenu addItem:[NSMenuItem separatorItem]];
    [appMenu addItemWithTitle:@"Hide LitePad" 
                       action:@selector(hide:) 
                keyEquivalent:@"h"];
    [appMenu addItemWithTitle:@"Quit LitePad" 
                       action:@selector(terminate:) 
                keyEquivalent:@"q"];
    
    [appMenuItem setSubmenu:appMenu];
    [mainMenu addItem:appMenuItem];
    
    // 文件菜单
    NSMenuItem *fileMenuItem = [[NSMenuItem alloc] init];
    NSMenu *fileMenu = [[NSMenu alloc] initWithTitle:@"File"];
    
    [fileMenu addItemWithTitle:@"New" 
                        action:@selector(newFile:) 
                 keyEquivalent:@"n"];
    [fileMenu addItemWithTitle:@"Open..." 
                        action:@selector(openFile:) 
                 keyEquivalent:@"o"];
    [fileMenu addItem:[NSMenuItem separatorItem]];
    [fileMenu addItemWithTitle:@"Save" 
                        action:@selector(saveFile:) 
                 keyEquivalent:@"s"];
    [fileMenu addItemWithTitle:@"Save As..." 
                        action:@selector(saveAsFile:) 
                 keyEquivalent:@"S"];
    
    [fileMenuItem setSubmenu:fileMenu];
    [mainMenu addItem:fileMenuItem];
    
    // 插件菜单
    NSMenuItem *pluginMenuItem = [[NSMenuItem alloc] init];
    self.pluginMenu = [[NSMenu alloc] initWithTitle:@"Plugins"];
    
    [self.pluginMenu addItemWithTitle:@"Plugin Manager..." 
                               action:@selector(showPluginManager:) 
                        keyEquivalent:@""];
    [self.pluginMenu addItem:[NSMenuItem separatorItem]];
    
    [pluginMenuItem setSubmenu:self.pluginMenu];
    [mainMenu addItem:pluginMenuItem];
    
    [NSApp setMainMenu:mainMenu];
}

- (void)setupTextView
{
    // 创建滚动视图
    NSRect contentFrame = [self.window.contentView bounds];
    self.scrollView = [[NSScrollView alloc] initWithFrame:contentFrame];
    [self.scrollView setHasVerticalScroller:YES];
    [self.scrollView setHasHorizontalScroller:YES];
    [self.scrollView setAutohidesScrollers:NO];
    [self.scrollView setBorderType:NSNoBorder];
    [self.scrollView setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
    
    // 创建文本视图
    NSRect textFrame = [self.scrollView.contentView bounds];
    self.textView = [[NSTextView alloc] initWithFrame:textFrame];
    [self.textView setDelegate:self];
    [self.textView setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
    [self.textView setFont:[NSFont fontWithName:@"Consolas" size:12]];
    [self.textView setAllowsUndo:YES];
    [self.textView setRichText:NO];
    [self.textView setImportsGraphics:NO];
    
    // 设置文本容器
    [[self.textView textContainer] setContainerSize:NSMakeSize(FLT_MAX, FLT_MAX)];
    [[self.textView textContainer] setWidthTracksTextView:YES];
    
    [self.scrollView setDocumentView:self.textView];
    [self.window.contentView addSubview:self.scrollView];
}

- (void)bindCallbacks
{
    // 绑定编辑器回调
    editor_->setContentChangedCallback([self]() {
        dispatch_async(dispatch_get_main_queue(), ^{
            [self updateTextViewFromEditor];
        });
    });
    
    editor_->setFilePathChangedCallback([self](const std::string& filePath) {
        dispatch_async(dispatch_get_main_queue(), ^{
            NSString *title = filePath.empty() ? @"Untitled" : [NSString stringWithUTF8String:filePath.c_str()];
            [self.window setTitle:[NSString stringWithFormat:@"LitePad - %@", title]];
        });
    });
}

- (void)updateTextViewFromEditor
{
    std::string content = editor_->getContent();
    NSString *nsContent = [NSString stringWithUTF8String:content.c_str()];
    [self.textView setString:nsContent];
}

- (void)updateEditorFromTextView
{
    NSString *content = [self.textView string];
    std::string stdContent = [content UTF8String];
    editor_->setContent(stdContent);
}

#pragma mark - Menu Actions

- (IBAction)newFile:(id)sender
{
    editor_->clear();
    [self updateTextViewFromEditor];
}

- (IBAction)openFile:(id)sender
{
    NSOpenPanel *openPanel = [NSOpenPanel openPanel];
    [openPanel setCanChooseFiles:YES];
    [openPanel setCanChooseDirectories:NO];
    [openPanel setAllowsMultipleSelection:NO];
    
    [openPanel beginWithCompletionHandler:^(NSInteger result) {
        if (result == NSModalResponseOK) {
            NSURL *url = [[openPanel URLs] objectAtIndex:0];
            std::string filePath = [[url path] UTF8String];
            if (editor_->openFile(filePath)) {
                [self updateTextViewFromEditor];
            }
        }
    }];
}

- (IBAction)saveFile:(id)sender
{
    [self updateEditorFromTextView];
    
    if (editor_->getFilePath().empty()) {
        [self saveAsFile:sender];
    } else {
        editor_->saveFile();
    }
}

- (IBAction)saveAsFile:(id)sender
{
    [self updateEditorFromTextView];
    
    NSSavePanel *savePanel = [NSSavePanel savePanel];
    [savePanel setAllowedFileTypes:@[@"txt", @"cpp", @"h", @"mm", @"m"]];
    
    [savePanel beginWithCompletionHandler:^(NSInteger result) {
        if (result == NSModalResponseOK) {
            NSURL *url = [savePanel URL];
            std::string filePath = [[url path] UTF8String];
            editor_->saveAs(filePath);
        }
    }];
}

- (IBAction)showPluginManager:(id)sender
{
    // TODO: 实现插件管理器对话框
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:@"Plugin Manager"];
    [alert setInformativeText:@"Plugin Manager will be implemented soon."];
    [alert addButtonWithTitle:@"OK"];
    [alert runModal];
}

- (IBAction)showAbout:(id)sender
{
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:@"About LitePad"];
    [alert setInformativeText:@"LitePad v1.0.0\nA lightweight code editor with plugin support."];
    [alert addButtonWithTitle:@"OK"];
    [alert runModal];
}

#pragma mark - NSTextViewDelegate

- (void)textDidChange:(NSNotification *)notification
{
    [self updateEditorFromTextView];
}

#pragma mark - NSWindowDelegate

- (BOOL)windowShouldClose:(NSWindow *)window
{
    if (editor_->isModified()) {
        NSAlert *alert = [[NSAlert alloc] init];
        [alert setMessageText:@"Save changes?"];
        [alert setInformativeText:@"Your document has unsaved changes."];
        [alert addButtonWithTitle:@"Save"];
        [alert addButtonWithTitle:@"Don't Save"];
        [alert addButtonWithTitle:@"Cancel"];
        
        NSInteger result = [alert runModal];
        if (result == NSAlertFirstButtonReturn) {
            [self saveFile:nil];
        } else if (result == NSAlertThirdButtonReturn) {
            return NO;
        }
    }
    return YES;
}

@end

// C++ MainWindow 类实现
class MainWindow::Impl {
public:
    LitePadWindowController *windowController;
    std::shared_ptr<Editor> editor;
    std::shared_ptr<PluginManager> pluginManager;
    std::shared_ptr<ConfigManager> configManager;
};

MainWindow::MainWindow() : pImpl(std::make_unique<Impl>()) {
    pImpl->editor = std::make_shared<Editor>();
    pImpl->pluginManager = std::make_shared<PluginManager>();
    pImpl->configManager = std::make_shared<ConfigManager>();
    
    pImpl->windowController = [[LitePadWindowController alloc] 
        initWithEditor:pImpl->editor 
        pluginManager:pImpl->pluginManager 
        configManager:pImpl->configManager];
}

MainWindow::~MainWindow() = default;

void MainWindow::show() {
    [pImpl->windowController showWindow:nil];
    [pImpl->windowController.window makeKeyAndOrderFront:nil];
}

void MainWindow::hide() {
    [pImpl->windowController.window orderOut:nil];
}

void MainWindow::setTitle(const std::string& title) {
    NSString *nsTitle = [NSString stringWithUTF8String:title.c_str()];
    [pImpl->windowController.window setTitle:nsTitle];
    title_ = title;
}

std::string MainWindow::getTitle() const {
    return title_;
}

void MainWindow::setSize(int width, int height) {
    NSRect frame = [pImpl->windowController.window frame];
    frame.size.width = width;
    frame.size.height = height;
    [pImpl->windowController.window setFrame:frame display:YES];
    width_ = width;
    height_ = height;
}

void MainWindow::getSize(int& width, int& height) const {
    width = width_;
    height = height_;
}

void MainWindow::setPosition(int x, int y) {
    NSRect frame = [pImpl->windowController.window frame];
    frame.origin.x = x;
    frame.origin.y = y;
    [pImpl->windowController.window setFrame:frame display:YES];
    x_ = x;
    y_ = y;
}

void MainWindow::getPosition(int& x, int& y) const {
    x = x_;
    y = y_;
}

void MainWindow::maximize() {
    [pImpl->windowController.window zoom:nil];
    maximized_ = true;
}

void MainWindow::minimize() {
    [pImpl->windowController.window miniaturize:nil];
    minimized_ = true;
}

void MainWindow::restore() {
    if (minimized_) {
        [pImpl->windowController.window deminiaturize:nil];
        minimized_ = false;
    }
    if (maximized_) {
        [pImpl->windowController.window zoom:nil];
        maximized_ = false;
    }
}

bool MainWindow::isMaximized() const {
    return [pImpl->windowController.window isZoomed];
}

bool MainWindow::isMinimized() const {
    return [pImpl->windowController.window isMiniaturized];
}

void MainWindow::newFile() {
    pImpl->editor->clear();
}

void MainWindow::openFile() {
    // 由 Objective-C 代码处理
}

void MainWindow::saveFile() {
    // 由 Objective-C 代码处理
}

void MainWindow::saveAs() {
    // 由 Objective-C 代码处理
}

void MainWindow::quit() {
    [NSApp terminate:nil];
}

void MainWindow::showPluginManager() {
    // TODO: 实现插件管理器
}

void MainWindow::showSettings() {
    // TODO: 实现设置对话框
}

void MainWindow::showAbout() {
    // 由 Objective-C 代码处理
}

std::shared_ptr<Editor> MainWindow::getEditor() const {
    return pImpl->editor;
}

std::shared_ptr<PluginManager> MainWindow::getPluginManager() const {
    return pImpl->pluginManager;
}

std::shared_ptr<ConfigManager> MainWindow::getConfigManager() const {
    return pImpl->configManager;
}

void MainWindow::setStatusBarText(const std::string& text) {
    // TODO: 实现状态栏
}

void MainWindow::updateMenuState() {
    // TODO: 更新菜单状态
}

void MainWindow::handleFileDrop(const std::string& filePath) {
    pImpl->editor->openFile(filePath);
}

// 私有方法的空实现
void MainWindow::initializeUI() {}
void MainWindow::createMenuBar() {}
void MainWindow::createToolBar() {}
void MainWindow::createStatusBar() {}
void MainWindow::createEditorArea() {}
void MainWindow::bindEvents() {}
void MainWindow::onWindowClose() {}
void MainWindow::onFileChanged() {}
void MainWindow::onPluginStateChanged() {}
void MainWindow::loadWindowConfig() {}
void MainWindow::saveWindowConfig() {}
