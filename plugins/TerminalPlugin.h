#ifndef TERMINAL_PLUGIN_H
#define TERMINAL_PLUGIN_H

#include "../src/PluginInterface.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>

class Editor;

/**
 * 终端插件类
 * 在编辑器中集成终端功能
 */
class TerminalPlugin : public PluginInterface {
public:
    TerminalPlugin();
    ~TerminalPlugin() override;
    
    // 实现插件接口
    std::string getName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    bool initialize() override;
    void cleanup() override;
    void enable() override;
    void disable() override;
    bool isEnabled() const override;
    
    /**
     * 设置编辑器实例
     * @param editor 编辑器指针
     */
    void setEditor(std::shared_ptr<Editor> editor);
    
    /**
     * 获取编辑器实例
     * @return 编辑器指针
     */
    std::shared_ptr<Editor> getEditor() const;
    
    /**
     * 打开终端
     */
    void openTerminal();
    
    /**
     * 关闭终端
     */
    void closeTerminal();
    
    /**
     * 检查终端是否打开
     * @return 是否打开
     */
    bool isTerminalOpen() const;
    
    /**
     * 执行命令
     * @param command 命令字符串
     * @return 是否执行成功
     */
    bool executeCommand(const std::string& command);
    
    /**
     * 获取命令输出
     * @param command 命令字符串
     * @return 命令输出
     */
    std::string getCommandOutput(const std::string& command);
    
    /**
     * 设置工作目录
     * @param workingDir 工作目录路径
     */
    void setWorkingDirectory(const std::string& workingDir);
    
    /**
     * 获取工作目录
     * @return 工作目录路径
     */
    std::string getWorkingDirectory() const;
    
    /**
     * 设置终端大小
     * @param width 宽度
     * @param height 高度
     */
    void setTerminalSize(int width, int height);
    
    /**
     * 获取终端大小
     * @param width 输出宽度
     * @param height 输出高度
     */
    void getTerminalSize(int& width, int& height) const;
    
    /**
     * 设置终端字体
     * @param fontFamily 字体族
     * @param fontSize 字体大小
     */
    void setTerminalFont(const std::string& fontFamily, int fontSize);
    
    /**
     * 获取终端字体
     * @param fontFamily 输出字体族
     * @param fontSize 输出字体大小
     */
    void getTerminalFont(std::string& fontFamily, int& fontSize) const;
    
    /**
     * 设置终端颜色主题
     * @param theme 主题名称
     */
    void setTerminalTheme(const std::string& theme);
    
    /**
     * 获取终端颜色主题
     * @return 主题名称
     */
    std::string getTerminalTheme() const;
    
    /**
     * 获取可用的主题列表
     * @return 主题列表
     */
    std::vector<std::string> getAvailableThemes() const;
    
    /**
     * 启用/禁用命令历史
     * @param enabled 是否启用
     */
    void setCommandHistoryEnabled(bool enabled);
    
    /**
     * 检查命令历史是否启用
     * @return 是否启用
     */
    bool isCommandHistoryEnabled() const;
    
    /**
     * 获取命令历史
     * @return 命令历史列表
     */
    std::vector<std::string> getCommandHistory() const;
    
    /**
     * 清空命令历史
     */
    void clearCommandHistory();
    
    /**
     * 设置最大历史记录数
     * @param maxHistory 最大记录数
     */
    void setMaxHistorySize(int maxHistory);
    
    /**
     * 获取最大历史记录数
     * @return 最大记录数
     */
    int getMaxHistorySize() const;
    
    /**
     * 设置命令完成功能
     * @param enabled 是否启用
     */
    void setCommandCompletionEnabled(bool enabled);
    
    /**
     * 检查命令完成功能是否启用
     * @return 是否启用
     */
    bool isCommandCompletionEnabled() const;
    
    /**
     * 设置命令建议
     * @param suggestions 建议列表
     */
    void setCommandSuggestions(const std::vector<std::string>& suggestions);
    
    /**
     * 获取命令建议
     * @return 建议列表
     */
    std::vector<std::string> getCommandSuggestions() const;

private:
    std::shared_ptr<Editor> editor_;
    bool enabled_;
    bool terminalOpen_;
    
    // 终端设置
    std::string workingDirectory_;
    int terminalWidth_;
    int terminalHeight_;
    std::string fontFamily_;
    int fontSize_;
    std::string theme_;
    
    // 功能设置
    bool commandHistoryEnabled_;
    bool commandCompletionEnabled_;
    int maxHistorySize_;
    
    // 命令历史和建议
    std::vector<std::string> commandHistory_;
    std::vector<std::string> commandSuggestions_;
    
    /**
     * 初始化终端环境
     */
    void initializeTerminal();
    
    /**
     * 清理终端环境
     */
    void cleanupTerminal();
    
    /**
     * 创建终端进程
     */
    void createTerminalProcess();
    
    /**
     * 销毁终端进程
     */
    void destroyTerminalProcess();
    
    /**
     * 处理终端输入
     * @param input 输入内容
     */
    void handleTerminalInput(const std::string& input);
    
    /**
     * 处理终端输出
     * @param output 输出内容
     */
    void handleTerminalOutput(const std::string& output);
    
    /**
     * 处理终端错误
     * @param error 错误信息
     */
    void handleTerminalError(const std::string& error);
    
    /**
     * 添加命令到历史
     * @param command 命令
     */
    void addToHistory(const std::string& command);
    
    /**
     * 保存命令历史
     */
    void saveCommandHistory();
    
    /**
     * 加载命令历史
     */
    void loadCommandHistory();
    
    /**
     * 加载终端配置
     */
    void loadTerminalConfig();
    
    /**
     * 保存终端配置
     */
    void saveTerminalConfig();
    
    /**
     * 应用终端主题
     * @param theme 主题名称
     */
    void applyTerminalTheme(const std::string& theme);
    
    /**
     * 处理工作目录变化
     */
    void handleWorkingDirectoryChanged();
};

#endif // TERMINAL_PLUGIN_H
