#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "PluginInterface.h"

/**
 * 插件管理器类
 * 负责插件的加载、卸载、启用、禁用等操作
 */
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    /**
     * 加载插件
     * @param pluginPath 插件文件路径
     * @return 是否加载成功
     */
    bool loadPlugin(const std::string& pluginPath);
    
    /**
     * 卸载插件
     * @param pluginName 插件名称
     * @return 是否卸载成功
     */
    bool unloadPlugin(const std::string& pluginName);
    
    /**
     * 启用插件
     * @param pluginName 插件名称
     * @return 是否启用成功
     */
    bool enablePlugin(const std::string& pluginName);
    
    /**
     * 禁用插件
     * @param pluginName 插件名称
     * @return 是否禁用成功
     */
    bool disablePlugin(const std::string& pluginName);
    
    /**
     * 获取插件列表
     * @return 插件名称列表
     */
    std::vector<std::string> getPluginList() const;
    
    /**
     * 获取插件信息
     * @param pluginName 插件名称
     * @return 插件接口指针，如果不存在则返回nullptr
     */
    PluginInterface* getPlugin(const std::string& pluginName) const;
    
    /**
     * 检查插件是否已加载
     * @param pluginName 插件名称
     * @return 是否已加载
     */
    bool isPluginLoaded(const std::string& pluginName) const;
    
    /**
     * 检查插件是否启用
     * @param pluginName 插件名称
     * @return 是否启用
     */
    bool isPluginEnabled(const std::string& pluginName) const;
    
    /**
     * 扫描插件目录
     * @param pluginDir 插件目录路径
     * @return 扫描到的插件数量
     */
    size_t scanPluginDirectory(const std::string& pluginDir);
    
    /**
     * 保存插件配置
     * @param configPath 配置文件路径
     * @return 是否保存成功
     */
    bool savePluginConfig(const std::string& configPath);
    
    /**
     * 加载插件配置
     * @param configPath 配置文件路径
     * @return 是否加载成功
     */
    bool loadPluginConfig(const std::string& configPath);

private:
    struct PluginInfo {
        std::string name;
        std::string path;
        std::shared_ptr<PluginInterface> plugin;
        bool enabled;
        void* libraryHandle;
    };
    
    std::unordered_map<std::string, PluginInfo> plugins_;
    
    /**
     * 获取插件库句柄
     * @param pluginPath 插件文件路径
     * @return 库句柄，失败返回nullptr
     */
    void* loadLibrary(const std::string& pluginPath);
    
    /**
     * 释放插件库
     * @param handle 库句柄
     */
    void unloadLibrary(void* handle);
    
    /**
     * 获取插件创建函数
     * @param handle 库句柄
     * @return 创建函数指针
     */
    PluginCreateFunc getCreateFunction(void* handle);
    
    /**
     * 获取插件销毁函数
     * @param handle 库句柄
     * @return 销毁函数指针
     */
    PluginDestroyFunc getDestroyFunction(void* handle);
};

#endif // PLUGIN_MANAGER_H
