#ifdef MACOS

#import <Cocoa/Cocoa.h>
#include "MacOSWindow.h"
#include "../Editor.h"
#include "../PluginManager.h"
#include "../ConfigManager.h"
#include <functional>

// 全局应用程序委托
@interface LitePadAppDelegate : NSObject <NSApplicationDelegate>
@end

@implementation LitePadAppDelegate
- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    // 应用程序完成启动
    NSLog(@"LitePad application did finish launching");
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES; // 最后一个窗口关闭时退出应用程序
}

- (void)applicationWillTerminate:(NSNotification *)notification {
    NSLog(@"LitePad application will terminate");
}
@end

// Objective-C 窗口控制器
@interface LitePadWindowController : NSWindowController <NSWindowDelegate, NSTextViewDelegate>
{
    std::shared_ptr<Editor> editor_;
    std::shared_ptr<PluginManager> pluginManager_;
    std::shared_ptr<ConfigManager> configManager_;
    std::function<void(const std::string&)> textChangedCallback_;
    std::function<bool()> windowCloseCallback_;
}

@property (strong) NSTextView *textView;
@property (strong) NSScrollView *scrollView;
@property (strong) NSMenu *pluginMenu;
@property (strong) NSTextField *statusLabel;

- (void)setupWindow;
- (void)setupMenuBar;
- (void)setupTextView;
- (void)setupStatusBar;
- (void)bindCallbacks;

// 菜单动作
- (IBAction)newFile:(id)sender;
- (IBAction)openFile:(id)sender;
- (IBAction)saveFile:(id)sender;
- (IBAction)saveAsFile:(id)sender;
- (IBAction)showPluginManager:(id)sender;
- (IBAction)showSettings:(id)sender;
- (IBAction)showAbout:(id)sender;

@end

@implementation LitePadWindowController

- (void)setupWindow
{
    NSRect windowFrame = NSMakeRect(100, 100, 1024, 768);
    NSWindow *window = [[NSWindow alloc] initWithContentRect:windowFrame
                                                   styleMask:(NSWindowStyleMaskTitled | 
                                                             NSWindowStyleMaskClosable | 
                                                             NSWindowStyleMaskMiniaturizable | 
                                                             NSWindowStyleMaskResizable)
                                                     backing:NSBackingStoreBuffered
                                                       defer:NO];
    
    [window setTitle:@"LitePad"];
    [window setDelegate:self];
    [window center];
    [window setAcceptsMouseMovedEvents:YES];
    
    // 启用文件拖放
    [window registerForDraggedTypes:@[NSPasteboardTypeFileURL]];
    
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
    [appMenu addItemWithTitle:@"Preferences..." 
                       action:@selector(showSettings:) 
                keyEquivalent:@","];
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
    
    // 编辑菜单
    NSMenuItem *editMenuItem = [[NSMenuItem alloc] init];
    NSMenu *editMenu = [[NSMenu alloc] initWithTitle:@"Edit"];
    
    [editMenu addItemWithTitle:@"Undo" 
                        action:@selector(undo:) 
                 keyEquivalent:@"z"];
    [editMenu addItemWithTitle:@"Redo" 
                        action:@selector(redo:) 
                 keyEquivalent:@"Z"];
    [editMenu addItem:[NSMenuItem separatorItem]];
    [editMenu addItemWithTitle:@"Cut" 
                        action:@selector(cut:) 
                 keyEquivalent:@"x"];
    [editMenu addItemWithTitle:@"Copy" 
                        action:@selector(copy:) 
                 keyEquivalent:@"c"];
    [editMenu addItemWithTitle:@"Paste" 
                        action:@selector(paste:) 
                 keyEquivalent:@"v"];
    [editMenu addItem:[NSMenuItem separatorItem]];
    [editMenu addItemWithTitle:@"Select All" 
                        action:@selector(selectAll:) 
                 keyEquivalent:@"a"];
    [editMenu addItem:[NSMenuItem separatorItem]];
    [editMenu addItemWithTitle:@"Find..." 
                        action:@selector(performFindPanelAction:) 
                 keyEquivalent:@"f"];
    
    [editMenuItem setSubmenu:editMenu];
    [mainMenu addItem:editMenuItem];
    
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
    NSRect contentFrame = [self.window.contentView bounds];
    contentFrame.size.height -= 22; // 为状态栏留出空间
    
    self.scrollView = [[NSScrollView alloc] initWithFrame:contentFrame];
    [self.scrollView setHasVerticalScroller:YES];
    [self.scrollView setHasHorizontalScroller:YES];
    [self.scrollView setAutohidesScrollers:NO];
    [self.scrollView setBorderType:NSNoBorder];
    [self.scrollView setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
    
    NSRect textFrame = [self.scrollView.contentView bounds];
    self.textView = [[NSTextView alloc] initWithFrame:textFrame];
    [self.textView setDelegate:self];
    [self.textView setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
    [self.textView setFont:[NSFont fontWithName:@"SF Mono" size:12] ?: [NSFont fontWithName:@"Monaco" size:12]];
    [self.textView setAllowsUndo:YES];
    [self.textView setRichText:NO];
    [self.textView setImportsGraphics:NO];
    [self.textView setUsesFindBar:YES];
    [self.textView setIncrementalSearchingEnabled:YES];
    
    // 设置文本容器
    [[self.textView textContainer] setContainerSize:NSMakeSize(FLT_MAX, FLT_MAX)];
    [[self.textView textContainer] setWidthTracksTextView:YES];
    
    [self.scrollView setDocumentView:self.textView];
    [self.window.contentView addSubview:self.scrollView];
}

- (void)setupStatusBar
{
    NSRect statusFrame = NSMakeRect(0, 0, [self.window.contentView bounds].size.width, 22);
    NSView *statusBar = [[NSView alloc] initWithFrame:statusFrame];
    [statusBar setAutoresizingMask:NSViewWidthSizable];
    
    // 状态栏背景
    [statusBar setWantsLayer:YES];
    statusBar.layer.backgroundColor = [NSColor controlBackgroundColor].CGColor;
    
    // 状态标签
    NSRect labelFrame = NSMakeRect(8, 2, statusFrame.size.width - 16, 18);
    self.statusLabel = [[NSTextField alloc] initWithFrame:labelFrame];
    [self.statusLabel setBordered:NO];
    [self.statusLabel setDrawsBackground:NO];
    [self.statusLabel setEditable:NO];
    [self.statusLabel setSelectable:NO];
    [self.statusLabel setFont:[NSFont systemFontOfSize:11]];
    [self.statusLabel setStringValue:@"Ready"];
    [self.statusLabel setAutoresizingMask:NSViewWidthSizable];
    
    [statusBar addSubview:self.statusLabel];
    [self.window.contentView addSubview:statusBar];
}

- (void)bindCallbacks
{
    // 这些回调将在 MacOSWindow 类中设置
}

#pragma mark - Public Methods

- (void)setEditor:(std::shared_ptr<Editor>)editor
{
    editor_ = editor;
    if (editor_) {
        editor_->setContentChangedCallback([self]() {
            dispatch_async(dispatch_get_main_queue(), ^{
                [self updateTextViewFromEditor];
            });
        });
        
        editor_->setFilePathChangedCallback([self](const std::string& filePath) {
            dispatch_async(dispatch_get_main_queue(), ^{
                NSString *fileName = filePath.empty() ? @"Untitled" : [[NSString stringWithUTF8String:filePath.c_str()] lastPathComponent];
                [self.window setTitle:[NSString stringWithFormat:@"LitePad - %@", fileName]];
            });
        });
    }
}

- (void)setPluginManager:(std::shared_ptr<PluginManager>)pluginManager
{
    pluginManager_ = pluginManager;
}

- (void)setConfigManager:(std::shared_ptr<ConfigManager>)configManager
{
    configManager_ = configManager;
}

- (void)setTextChangedCallback:(std::function<void(const std::string&)>)callback
{
    textChangedCallback_ = callback;
}

- (void)setWindowCloseCallback:(std::function<bool()>)callback
{
    windowCloseCallback_ = callback;
}

- (void)updateTextViewFromEditor
{
    if (editor_) {
        std::string content = editor_->getContent();
        NSString *nsContent = [NSString stringWithUTF8String:content.c_str()];
        [self.textView setString:nsContent];
    }
}

- (void)updateEditorFromTextView
{
    if (editor_) {
        NSString *content = [self.textView string];
        std::string stdContent = [content UTF8String];
        editor_->setContent(stdContent);
        
        if (textChangedCallback_) {
            textChangedCallback_(stdContent);
        }
    }
}

#pragma mark - Menu Actions

- (IBAction)newFile:(id)sender
{
    if (editor_) {
        editor_->clear();
        [self updateTextViewFromEditor];
    }
}

- (IBAction)openFile:(id)sender
{
    NSOpenPanel *openPanel = [NSOpenPanel openPanel];
    [openPanel setCanChooseFiles:YES];
    [openPanel setCanChooseDirectories:NO];
    [openPanel setAllowsMultipleSelection:NO];
    [openPanel setAllowedFileTypes:@[@"txt", @"cpp", @"h", @"hpp", @"c", @"mm", @"m", @"py", @"js", @"html", @"css", @"json", @"md"]];
    
    [openPanel beginWithCompletionHandler:^(NSInteger result) {
        if (result == NSModalResponseOK && self->editor_) {
            NSURL *url = [[openPanel URLs] objectAtIndex:0];
            std::string filePath = [[url path] UTF8String];
            if (self->editor_->openFile(filePath)) {
                [self updateTextViewFromEditor];
            }
        }
    }];
}

- (IBAction)saveFile:(id)sender
{
    [self updateEditorFromTextView];
    
    if (editor_ && editor_->getFilePath().empty()) {
        [self saveAsFile:sender];
    } else if (editor_) {
        editor_->saveFile();
    }
}

- (IBAction)saveAsFile:(id)sender
{
    [self updateEditorFromTextView];
    
    NSSavePanel *savePanel = [NSSavePanel savePanel];
    [savePanel setAllowedFileTypes:@[@"txt", @"cpp", @"h", @"hpp", @"c", @"mm", @"m", @"py", @"js", @"html", @"css", @"json", @"md"]];
    
    [savePanel beginWithCompletionHandler:^(NSInteger result) {
        if (result == NSModalResponseOK && self->editor_) {
            NSURL *url = [savePanel URL];
            std::string filePath = [[url path] UTF8String];
            self->editor_->saveAs(filePath);
        }
    }];
}

- (IBAction)showPluginManager:(id)sender
{
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:@"Plugin Manager"];
    [alert setInformativeText:@"Plugin Manager functionality will be implemented soon."];
    [alert addButtonWithTitle:@"OK"];
    [alert runModal];
}

- (IBAction)showSettings:(id)sender
{
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:@"Settings"];
    [alert setInformativeText:@"Settings dialog will be implemented soon."];
    [alert addButtonWithTitle:@"OK"];
    [alert runModal];
}

- (IBAction)showAbout:(id)sender
{
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:@"About LitePad"];
    [alert setInformativeText:@"LitePad v1.0.0\n\nA lightweight, cross-platform code editor with native GUI support.\n\nBuilt with love using C++ and native platform APIs."];
    [alert addButtonWithTitle:@"OK"];
    [alert runModal];
}

#pragma mark - NSTextViewDelegate

- (void)textDidChange:(NSNotification *)notification
{
    [self updateEditorFromTextView];
    [self.statusLabel setStringValue:[NSString stringWithFormat:@"Lines: %lu, Characters: %lu", 
                                     (unsigned long)[[self.textView string] componentsSeparatedByString:@"\n"].count,
                                     (unsigned long)[[self.textView string] length]]];
}

#pragma mark - NSWindowDelegate

- (BOOL)windowShouldClose:(NSWindow *)window
{
    if (windowCloseCallback_) {
        return windowCloseCallback_();
    }
    
    if (editor_ && editor_->isModified()) {
        NSAlert *alert = [[NSAlert alloc] init];
        [alert setMessageText:@"Save changes?"];
        [alert setInformativeText:@"Your document has unsaved changes. Do you want to save them before closing?"];
        [alert addButtonWithTitle:@"Save"];
        [alert addButtonWithTitle:@"Don't Save"];
        [alert addButtonWithTitle:@"Cancel"];
        
        NSInteger result = [alert runModal];
        if (result == NSAlertFirstButtonReturn) {
            [self saveFile:nil];
            return YES;
        } else if (result == NSAlertThirdButtonReturn) {
            return NO;
        }
    }
    return YES;
}

- (void)windowWillClose:(NSNotification *)notification
{
    [NSApp terminate:nil];
}

@end

// MacOSWindow 实现
class MacOSWindow::Impl {
public:
    LitePadWindowController *windowController;
    std::shared_ptr<Editor> editor;
    std::shared_ptr<PluginManager> pluginManager;
    std::shared_ptr<ConfigManager> configManager;
};

MacOSWindow::MacOSWindow() : pImpl(std::make_unique<Impl>()) {
    pImpl->windowController = [[LitePadWindowController alloc] init];
    [pImpl->windowController setupWindow];
    [pImpl->windowController setupMenuBar];
    [pImpl->windowController setupTextView];
    [pImpl->windowController setupStatusBar];
    [pImpl->windowController bindCallbacks];
    
    // 初始化默认值
    title_ = "LitePad";
    width_ = 1024;
    height_ = 768;
    x_ = 100;
    y_ = 100;
}

MacOSWindow::~MacOSWindow() = default;

void MacOSWindow::show() {
    [pImpl->windowController showWindow:nil];
    [pImpl->windowController.window makeKeyAndOrderFront:nil];
}

void MacOSWindow::hide() {
    [pImpl->windowController.window orderOut:nil];
}

void MacOSWindow::close() {
    [pImpl->windowController.window performClose:nil];
}

void MacOSWindow::setTitle(const std::string& title) {
    title_ = title;
    NSString *nsTitle = [NSString stringWithUTF8String:title.c_str()];
    [pImpl->windowController.window setTitle:nsTitle];
}

std::string MacOSWindow::getTitle() const {
    return title_;
}

void MacOSWindow::setSize(int width, int height) {
    width_ = width;
    height_ = height;
    NSRect frame = [pImpl->windowController.window frame];
    frame.size.width = width;
    frame.size.height = height;
    [pImpl->windowController.window setFrame:frame display:YES];
}

void MacOSWindow::getSize(int& width, int& height) const {
    width = width_;
    height = height_;
}

void MacOSWindow::setPosition(int x, int y) {
    x_ = x;
    y_ = y;
    NSRect frame = [pImpl->windowController.window frame];
    frame.origin.x = x;
    frame.origin.y = y;
    [pImpl->windowController.window setFrame:frame display:YES];
}

void MacOSWindow::getPosition(int& x, int& y) const {
    x = x_;
    y = y_;
}

void MacOSWindow::maximize() {
    [pImpl->windowController.window zoom:nil];
}

void MacOSWindow::minimize() {
    [pImpl->windowController.window miniaturize:nil];
}

void MacOSWindow::restore() {
    if ([pImpl->windowController.window isMiniaturized]) {
        [pImpl->windowController.window deminiaturize:nil];
    }
    if ([pImpl->windowController.window isZoomed]) {
        [pImpl->windowController.window zoom:nil];
    }
}

bool MacOSWindow::isMaximized() const {
    return [pImpl->windowController.window isZoomed];
}

bool MacOSWindow::isMinimized() const {
    return [pImpl->windowController.window isMiniaturized];
}

void MacOSWindow::newFile() {
    [pImpl->windowController newFile:nil];
}

void MacOSWindow::openFile() {
    [pImpl->windowController openFile:nil];
}

void MacOSWindow::saveFile() {
    [pImpl->windowController saveFile:nil];
}

void MacOSWindow::saveAs() {
    [pImpl->windowController saveAsFile:nil];
}

void MacOSWindow::quit() {
    [NSApp terminate:nil];
}

void MacOSWindow::showAbout() {
    [pImpl->windowController showAbout:nil];
}

void MacOSWindow::showPluginManager() {
    [pImpl->windowController showPluginManager:nil];
}

void MacOSWindow::showSettings() {
    [pImpl->windowController showSettings:nil];
}

void MacOSWindow::setEditor(std::shared_ptr<Editor> editor) {
    pImpl->editor = editor;
    [pImpl->windowController setEditor:editor];
}

void MacOSWindow::setPluginManager(std::shared_ptr<PluginManager> pluginManager) {
    pImpl->pluginManager = pluginManager;
    [pImpl->windowController setPluginManager:pluginManager];
}

void MacOSWindow::setConfigManager(std::shared_ptr<ConfigManager> configManager) {
    pImpl->configManager = configManager;
    [pImpl->windowController setConfigManager:configManager];
}

void MacOSWindow::setTextContent(const std::string& content) {
    NSString *nsContent = [NSString stringWithUTF8String:content.c_str()];
    [pImpl->windowController.textView setString:nsContent];
}

std::string MacOSWindow::getTextContent() const {
    NSString *content = [pImpl->windowController.textView string];
    return [content UTF8String];
}

void MacOSWindow::setTextChangedCallback(std::function<void(const std::string&)> callback) {
    [pImpl->windowController setTextChangedCallback:callback];
}

void MacOSWindow::setWindowCloseCallback(std::function<bool()> callback) {
    [pImpl->windowController setWindowCloseCallback:callback];
}

void MacOSWindow::setStatusText(const std::string& text) {
    NSString *nsText = [NSString stringWithUTF8String:text.c_str()];
    [pImpl->windowController.statusLabel setStringValue:nsText];
}

void MacOSWindow::handleFileDrop(const std::string& filePath) {
    if (pImpl->editor) {
        pImpl->editor->openFile(filePath);
        [pImpl->windowController updateTextViewFromEditor];
    }
}

// C++ 函数，用于启动 macOS 事件循环
extern "C" void runMacOSEventLoop() {
    @autoreleasepool {
        // 获取共享应用程序实例
        NSApplication *app = [NSApplication sharedApplication];
        
        // 设置应用程序委托
        LitePadAppDelegate *delegate = [[LitePadAppDelegate alloc] init];
        [app setDelegate:delegate];
        
        // 设置应用程序激活策略
        [app setActivationPolicy:NSApplicationActivationPolicyRegular];
        
        // 激活应用程序
        [app activateIgnoringOtherApps:YES];
        
        // 启动主事件循环
        [app run];
    }
}

#endif // MACOS
