#ifndef AUTO_SAVE_PLUGIN_H
#define AUTO_SAVE_PLUGIN_H

#include "../src/PluginInterface.h"
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>

class Editor;

/**
 * 自动保存插件类
 * 定期自动保存编辑器内容
 */
class AutoSavePlugin : public PluginInterface {
public:
    AutoSavePlugin();
    ~AutoSavePlugin() override;
    
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
     * 设置自动保存间隔
     * @param interval 间隔时间（秒）
     */
    void setAutoSaveInterval(int interval);
    
    /**
     * 获取自动保存间隔
     * @return 间隔时间（秒）
     */
    int getAutoSaveInterval() const;
    
    /**
     * 启用/禁用自动保存
     * @param enabled 是否启用
     */
    void setAutoSaveEnabled(bool enabled);
    
    /**
     * 检查自动保存是否启用
     * @return 是否启用
     */
    bool isAutoSaveEnabled() const;
    
    /**
     * 设置备份文件路径
     * @param backupPath 备份路径
     */
    void setBackupPath(const std::string& backupPath);
    
    /**
     * 获取备份文件路径
     * @return 备份路径
     */
    std::string getBackupPath() const;
    
    /**
     * 设置最大备份文件数量
     * @param maxBackups 最大备份数量
     */
    void setMaxBackups(int maxBackups);
    
    /**
     * 获取最大备份文件数量
     * @return 最大备份数量
     */
    int getMaxBackups() const;
    
    /**
     * 立即执行自动保存
     */
    void performAutoSave();
    
    /**
     * 获取上次自动保存时间
     * @return 上次保存时间
     */
    std::chrono::system_clock::time_point getLastAutoSaveTime() const;
    
    /**
     * 获取下次自动保存时间
     * @return 下次保存时间
     */
    std::chrono::system_clock::time_point getNextAutoSaveTime() const;
    
    /**
     * 设置自动保存提示
     * @param showPrompt 是否显示提示
     */
    void setShowAutoSavePrompt(bool showPrompt);
    
    /**
     * 检查是否显示自动保存提示
     * @return 是否显示提示
     */
    bool isShowAutoSavePrompt() const;
    
    /**
     * 设置自动保存时显示进度条
     * @param showProgress 是否显示进度条
     */
    void setShowAutoSaveProgress(bool showProgress);
    
    /**
     * 检查是否显示自动保存进度条
     * @return 是否显示进度条
     */
    bool isShowAutoSaveProgress() const;

private:
    std::shared_ptr<Editor> editor_;
    bool enabled_;
    bool autoSaveEnabled_;
    
    // 自动保存设置
    int autoSaveInterval_;
    std::string backupPath_;
    int maxBackups_;
    bool showPrompt_;
    bool showProgress_;
    
    // 时间管理
    std::chrono::system_clock::time_point lastAutoSaveTime_;
    std::chrono::system_clock::time_point nextAutoSaveTime_;
    
    // 自动保存线程
    std::thread autoSaveThread_;
    std::atomic<bool> stopAutoSaveThread_;
    
    /**
     * 自动保存线程函数
     */
    void autoSaveThreadFunc();
    
    /**
     * 创建备份文件
     * @return 是否创建成功
     */
    bool createBackup();
    
    /**
     * 清理旧备份文件
     */
    void cleanupOldBackups();
    
    /**
     * 生成备份文件名
     * @return 备份文件名
     */
    std::string generateBackupFileName() const;
    
    /**
     * 检查是否需要自动保存
     * @return 是否需要保存
     */
    bool shouldAutoSave() const;
    
    /**
     * 更新自动保存时间
     */
    void updateAutoSaveTime();
    
    /**
     * 处理内容变化事件
     */
    void handleContentChanged();
    
    /**
     * 处理文件路径变化事件
     */
    void handleFilePathChanged();
    
    /**
     * 显示自动保存提示
     */
    void showAutoSavePrompt();
    
    /**
     * 显示自动保存进度
     */
    void showAutoSaveProgress();
    
    /**
     * 加载插件配置
     */
    void loadPluginConfig();
    
    /**
     * 保存插件配置
     */
    void savePluginConfig();
};

#endif // AUTO_SAVE_PLUGIN_H
